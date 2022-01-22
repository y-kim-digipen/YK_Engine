/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.cpp
Purpose: Source file for Object
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "Object.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "engine/engine.h"
#include "engine/graphic_misc/Shader.h"
#include "engine/scene/SceneBase.h"
#include "Camera.h"
//#include "CubeCaptureCamera.h"

Object::Object(const std::string& name) : Object(name, std::shared_ptr<Mesh>(), std::shared_ptr<Shader>()){

}

Object::Object(const std::string& name, std::shared_ptr<Mesh> pMesh, std::shared_ptr<Shader> pShader)
        : m_pMesh(pMesh), m_pShader(pShader), mObjectName(name), m_MatrixCacheDirty(true),mToWorldMatrix(1.f),
          m_position(), m_scale(1.f), m_rotation(0.f), mEmissiveColor(Color(0.f)),  mUVType(Mesh::PLANAR_UV) {
    if(m_pShader){
        m_pShader->SetShaderBuffer(mObjectName);
    }
    SetColor(mEmissiveColor);
    mDoVertexNormalDrawing = false;
    mDoFaceNormalDrawing = false;
    mUsingTexture = true;
    mUsingGPUUV = true;
    mFitToBox = true;

    mTextureSlots.resize(10);

//    mTextureSlots[0] = "tex_object0";
//    mTextureSlots[1] = "tex_object1";

}

Object::Object(const std::string& name, const std::string &meshStr, const std::string &shaderStr)
        : Object(name, engine::GetMesh(meshStr), engine::GetShader(shaderStr)) {
    mMeshName = meshStr;
    mShaderName = shaderStr;
}

Object::~Object() {
    std::cout << "[Deleting Object] " << mObjectName << std::endl;
}


void Object::Init() {
    //todo change this after mesh and shader implementation is completed
    if(m_pMesh != nullptr){
//        m_pMesh->SetupBuffer();
    }
    if(m_pShader != nullptr){
        //todo implement this when shader's init is implemented
        //m_pShader->Reload();
    }
}

void Object::PreRender() {
    mDoRender = ((m_pShader != nullptr) & (m_pMesh != nullptr)) | !m_pShader->HasError();

    if(mAdditionalFunction != nullptr && mUpdateAdditionalFunction){
        mAdditionalFunction();
    }
    //todo just for debugging.
    m_MatrixCacheDirty = true;
    TryCalculateMatrix();
    SendMeshDataToShader();
}

void Object::RenderModel() const {
    const GLint shaderPID = m_pShader->GetProgramID();

    //setting&binding buffer
    auto& attributeInfos = m_pShader->GetAttribInfos();
    const GLuint VAO_ID = engine::GetVAOManager().GetVAO(m_pShader->GetAttributeID());

    auto& VBOInfo = engine::GetVBOManager().GetVBOInfo(m_pMesh);

    if(mUsingTexture){
        const int textureSlotSize = mTextureSlots.size();
        for(int i = 0; i < textureSlotSize; ++i)
        {
            const std::string usingTextureString = mTextureSlots[i];
            if(usingTextureString.length() == 0)
            {
                continue;
            }
            TextureObject* pTextureObj = engine::GetTextureManager().FindTextureByName(usingTextureString);
            if(pTextureObj == nullptr)
            {
                continue;
            }
            engine::GetTextureManager().BindTexture(pTextureObj);
            pTextureObj->SendToShader(m_pShader, i);
        }
    }

    glBindVertexArray(VAO_ID);
    for(auto& attribute : attributeInfos){
        glEnableVertexAttribArray(attribute.location);

        glBindBuffer(GL_ARRAY_BUFFER, VBOInfo.first[attribute.name]);
        glVertexAttribPointer( attribute.location,
                               attribute.DataSize,
                               attribute.DataType,
                               GL_FALSE,
                               0,
                               (void *) 0 );
    }
    //setup EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOInfo.second);

    //Drawing Logic
    glUseProgram(shaderPID);
    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    GLint vNormalTransformLoc = glGetUniformLocation(shaderPID, "vertexNormalTransform");


    const auto& pCam = engine::GetCurrentScene()->GetCurrentCamera();

    //Get matricies
    glm::mat4 modelToWorldMatrix = GetObjectToWorldMatrix();
    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();

    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelToWorldMatrix;
    glm::mat4 normalMatrix = modelToWorldMatrix;

    if(m_pShader->HasUniform("modelToWorldTransform")) {
        m_pShader->GetUniformValue<glm::mat4>(GetName(), "modelToWorldTransform")
                = modelToWorldMatrix;
    }
    if(m_pShader->HasUniform("perspectiveMatrix")) {
        m_pShader->GetUniformValue<glm::mat4>(GetName(), "perspectiveMatrix")
                = projectionMatrix * viewMatrix;
    }
    if( m_pShader->HasUniform("CameraPos_GUIX") ) /*might using phong things...*/{
        m_pShader->GetUniformValue<glm::vec3>(GetName(), "CameraPos_GUIX") = pCam->Eye();
    }

    m_pShader->SetAllUniforms(mObjectName);

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
    glUniformMatrix4fv(vNormalTransformLoc, 1, GL_FALSE, &normalMatrix[0][0]);

    glDrawElements(GL_TRIANGLES, m_pMesh->getVertexIndicesCount(), GL_UNSIGNED_INT, (void*) 0);

    for(auto& attribute : attributeInfos){
        glDisableVertexAttribArray(attribute.location);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void Object::RenderVertexNormal() const {
    auto pNormalDrawShader = engine::GetShader("NormalDrawShader");
    const GLint shaderPID = pNormalDrawShader->GetProgramID();

    //setting&binding buffer
    auto& attributeInfos = pNormalDrawShader->GetAttribInfos();
    const GLuint VAOID = engine::GetVAOManager().GetVAO(engine::GetShader("NormalDrawShader")->GetAttributeID());

    auto& VBOInfo = engine::GetVBOManager().GetVBOInfo(m_pMesh);

    glBindVertexArray(VAOID);
    for(auto& attribute : attributeInfos){
        glEnableVertexAttribArray(attribute.location);

        glBindBuffer(GL_ARRAY_BUFFER, VBOInfo.first[attribute.name]);
        glVertexAttribPointer( attribute.location,
                               attribute.DataSize,
                               attribute.DataType,
                               GL_FALSE,
                               0,
                               (void *) 0 );
    }
    //Drawing Logic
    glUseProgram(shaderPID);
    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    if(vTransformLoc < 0){
        std::cerr << "Unable to find uniform variable!" << std::endl;
    }
    const auto& pCam = engine::GetCurrentScene()->GetCurrentCamera();

    //Get matricies
    glm::mat4 modelToWorldMatrix = GetObjectToWorldMatrix();
    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();

    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelToWorldMatrix;

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

    glDrawArrays(GL_LINES, 0, m_pMesh->getVertexNormalCount());

    for(auto& attribute : attributeInfos){
        glDisableVertexAttribArray(attribute.location);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void Object::RenderFaceNormal() const {
    auto pNormalDrawShader = engine::GetShader("FaceNormalDrawShader");
    const GLint shaderPID = pNormalDrawShader->GetProgramID();

    //setting&binding buffer
    auto& attributeInfos = pNormalDrawShader->GetAttribInfos();
    const GLuint VAOID = engine::GetVAOManager().GetVAO(engine::GetShader("FaceNormalDrawShader")->GetAttributeID());

    auto& VBOInfo = engine::GetVBOManager().GetVBOInfo(m_pMesh);

    glBindVertexArray(VAOID);
    for(auto& attribute : attributeInfos){
        glEnableVertexAttribArray(attribute.location);

        glBindBuffer(GL_ARRAY_BUFFER, VBOInfo.first[attribute.name]);
        glVertexAttribPointer( attribute.location,

                               attribute.DataSize,
                               attribute.DataType,
                               GL_FALSE,
                               0,
                               (void *) 0 );
    }

    //Drawing Logic
    glUseProgram(shaderPID);
    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    const auto& pCam = engine::GetCurrentScene()->GetCurrentCamera();

    //Get matricies
    glm::mat4 modelToWorldMatrix = GetObjectToWorldMatrix();
    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();


    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelToWorldMatrix;

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

    glDrawArrays(GL_LINES, 0, m_pMesh->getFaceNormalDisplayCount());

    for(auto& attribute : attributeInfos){
        glDisableVertexAttribArray(attribute.location);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void Object::Render() const {
    RenderModel();
    if(mDoVertexNormalDrawing){
        RenderVertexNormal();
    }
    if(mDoFaceNormalDrawing){
        RenderFaceNormal();
    }
}

void Object::PostRender() {

}

void Object::CleanUp() const {

}

bool Object::SetShader(const std::string &shaderStr) {
    auto pShader = engine::GetShader(shaderStr);
    if(pShader){
        m_pShader->DeleteShaderBuffer(mObjectName);
        m_pShader = pShader;
        m_pShader->SetShaderBuffer(mObjectName);
        mShaderName = shaderStr;
        //todo init shader here
        if(m_pShader->HasError())
        {
            std::cerr << m_pShader->GetName() << "has error, try to load again!" << std::endl;
        }

    }
    return pShader != nullptr;
}

bool Object::SetMesh(const std::string &meshStr) {
    auto pMesh = engine::GetMesh(meshStr);
    if(pMesh){
        //todo init mesh here
        m_pMesh = pMesh;
        mMeshName = meshStr;
    }
    return pMesh != nullptr;
}

bool Object::IsRenderReady() const {
    return mDoRender;
}

void Object::TryCalculateMatrix() {
    if(m_MatrixCacheDirty){
        glm::mat4 tempToWorld{1.f};

        const auto meshBoundingBox = m_pMesh->GetBoundingBox();
        glm::vec3 meshSize = (meshBoundingBox.second - meshBoundingBox.first);
        glm::vec3 meshCenter = (meshBoundingBox.second + meshBoundingBox.first) * 0.5f;

        tempToWorld = glm::translate(tempToWorld, m_position);

        tempToWorld = glm::rotate(tempToWorld, m_rotation.x, glm::vec3{ 1.f, 0.f, 0.f });
        tempToWorld = glm::rotate(tempToWorld, m_rotation.y, glm::vec3{ 0.f, 1.f, 0.f });
        tempToWorld = glm::rotate(tempToWorld, m_rotation.z, glm::vec3{ 0.f, 0.f, 1.f });
        tempToWorld = glm::scale(tempToWorld, m_scale);

        //set to the bounding box height
        tempToWorld = glm::scale(tempToWorld, 1.f / glm::vec3(meshSize.y));

        tempToWorld = glm::translate(tempToWorld, -meshCenter);

        mToWorldMatrix = tempToWorld;
        m_MatrixCacheDirty = false;
    }
}

glm::mat4 Object::GetObjectToWorldMatrix() const {
    return mToWorldMatrix;
}

std::string Object::GetUsingMeshName() const {
    return mMeshName;
}

std::string Object::GetUsingShaderName() const {
    return mShaderName;
}

glm::vec3 Object::GetPosition() {
    return m_position;
}

void Object::SetPosition(glm::vec3 position) {
    const glm::vec3 amount = position - m_position;
    AddPosition(amount);
    m_MatrixCacheDirty = true;
}

void Object::AddPosition(glm::vec3 amount) {
    m_position += amount;
    m_MatrixCacheDirty = true;
}

glm::vec3 Object::GetRotation() {
    return m_rotation;
}

void Object::SetRotation(glm::vec3 rotation) {
    const glm::vec3 amount = rotation - m_rotation;
    AddRotation(amount);
    m_MatrixCacheDirty = true;
}

void Object::AddRotation(glm::vec3 amount) {
    m_rotation += amount;
    m_MatrixCacheDirty = true;
}

glm::vec3 Object::GetScale() {
    return m_scale;
}

void Object::SetScale(glm::vec3 scale) {
    const glm::vec3 amount = scale - m_scale;
    AddScale(amount);
    m_MatrixCacheDirty = true;
}

void Object::AddScale(glm::vec3 amount) {
    m_scale += amount;
    m_MatrixCacheDirty = true;
}

void Object::BindFunction(std::function<void(Object *)> func) {
    mAdditionalFunction = std::bind(func, this);
    mUpdateAdditionalFunction = true;
}

std::string Object::GetName() const {
    return mObjectName;
}

void Object::SetColor(Color newColor) {
    mEmissiveColor = newColor;
    if(m_pShader->HasUniform("EmissiveColor"))
    {
        m_pShader->GetUniformValue<glm::vec3>(mObjectName, "EmissiveColor") = mEmissiveColor.AsVec3();
    }
}

void Object::SendMeshDataToShader()
{
    if(mUsingGPUUV)
    {
        if(!IsRenderReady())
        {
            return;
        }
        if(m_pShader->HasUniform("ModelScale_GUIX"))
        {
            m_pShader->GetUniformValue<glm::vec3>(mObjectName, "ModelScale_GUIX") = m_pMesh->getModelScale();
        }
        if(m_pShader->HasUniform("MinBoundingBox_GUIX"))
        {
            const auto& boundingBox = m_pMesh->GetBoundingBox();
            m_pShader->GetUniformValue<glm::vec3>(mObjectName, "MinBoundingBox_GUIX") = boundingBox.first;
            m_pShader->GetUniformValue<glm::vec3>(mObjectName, "MaxBoundingBox_GUIX") = boundingBox.second;
        }
        if(m_pShader->HasUniform("UVType_GUIX"))
        {
            m_pShader->GetUniformValue<int>(mObjectName, "UVType_GUIX") = static_cast<int>(mUVType);
        }
    }
    else
    {
        if(mUVType != m_pMesh->GetCurrentUsingCPUMeshUV())
        {
            m_pMesh->ChangeUVType(mUVType);
        }
    }
    if(m_pShader->HasUniform("UsingTexture_GUIX"))
    {
        m_pShader->GetUniformValue<GLboolean>(mObjectName, "UsingTexture_GUIX") = mUsingTexture;
    }
    if(m_pShader->HasUniform("UsingGPUUV_GUIX"))
    {
        m_pShader->GetUniformValue<GLboolean>(mObjectName, "UsingGPUUV_GUIX") = mUsingGPUUV;
    }

}

void Object::RemoveFunction() {
    mAdditionalFunction = nullptr;
}

void Object::SetFunctionUpdate(bool updateStatus) {
    mUpdateAdditionalFunction = updateStatus;
}

void Object::SetTextureOption(bool usingTexture, bool usingGPUUV) {
    mUsingTexture = usingTexture;
    mUsingGPUUV = usingGPUUV;
}

void Object::ChangeTexture(int slot, const std::string &textureName) {
    if(slot >= mTextureSlots.size())
    {
        throw std::out_of_range("Trying to access out of texture slots");
    }
    mTextureSlots[slot] = textureName;
}
