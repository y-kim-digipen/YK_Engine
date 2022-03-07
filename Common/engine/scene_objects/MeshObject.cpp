/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MeshObject.cpp
Purpose: Source file for MeshObject
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#include "MeshObject.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "engine/engine.h"
#include "engine/graphic_misc/Shader.h"
#include "engine/scene/SceneBase.h"
#include "Camera.h"
#include "engine/graphic_misc/BoundingVolume/BasicBoundingVolumes.h"

MeshObject::MeshObject(const std::string& name) : MeshObject(name, std::shared_ptr<Mesh>(), std::shared_ptr<Shader>()) {}

MeshObject::MeshObject(const std::string& name, std::shared_ptr<Mesh> pMesh, std::shared_ptr<Shader> pShader)
        : BaseObject(name, ObjectTypes::MESH), m_pMesh(pMesh), m_pShader(pShader), m_MatrixCacheDirty(true), mToWorldMatrix(1.f),
          m_position(), m_scale(1.f), m_rotation(0.f), baseColor(Color(0.f)), mUVType(Mesh::PLANAR_UV) {
    if(m_pShader){
        m_pShader->SetShaderBuffer(name);
    }
    SetColor(baseColor);
    mDoVertexNormalDrawing = false;
    mDoFaceNormalDrawing = false;
    mUsingTexture = true;
    mUsingGPUUV = true;
    mFitToBox = true;

    mTextureSlots.resize(10);

//    mTextureSlots[0] = "tex_object0";
//    mTextureSlots[1] = "tex_object1";
}

MeshObject::MeshObject(const std::string& name, const std::string &meshStr, const std::string &shaderStr)
        : MeshObject(name, engine::GetMesh(meshStr), engine::GetShader(shaderStr)) {
    mMeshName = meshStr;
    mShaderName = shaderStr;
}

MeshObject::~MeshObject() {
    std::cout << "[MeshObject deleted] " << mObjectName << std::endl;
}


void MeshObject::Init() {
    assert(m_pMesh != nullptr && m_pShader != nullptr);
}

void MeshObject::PreRender() {
    mDoRender = ((m_pShader != nullptr) & (m_pMesh != nullptr)) | !m_pShader->HasError();

    //todo just for debugging.
    m_MatrixCacheDirty = true;
    TryCalculateMatrix();
    SendMeshDataToShader();
}

void MeshObject::RenderModel() const {
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
    SendMaterialDataToShader();

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
    if( m_pShader->HasUniform("CameraPos_GUIX") ) {
        m_pShader->GetUniformValue<glm::vec3>(GetName(), "CameraPos_GUIX") = pCam->Eye();
    }

    //todo change whole systems only using these matrices
    if(m_pShader->HasUniform("worldMatrix"))
    {
        m_pShader->GetUniformValue<glm::mat4>(GetName(), "worldMatrix") = modelToWorldMatrix;
    }
    if(m_pShader->HasUniform("viewMatrix"))
    {
        m_pShader->GetUniformValue<glm::mat4>(GetName(), "viewMatrix") = viewMatrix;
    }
    if(m_pShader->HasUniform("projectionMatrix"))
    {
        m_pShader->GetUniformValue<glm::mat4>(GetName(), "projectionMatrix") = projectionMatrix;
    }
    if(m_pShader->HasUniform("invertViewMatrix"))
    {
        m_pShader->GetUniformValue<glm::mat4>(GetName(), "invertViewMatrix") = glm::inverse(viewMatrix);
    }
    if(m_pShader->HasUniform("invertProjectionMatrix"))
    {
        m_pShader->GetUniformValue<glm::mat4>(GetName(), "invertProjectionMatrix") = glm::inverse(projectionMatrix);
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

void MeshObject::RenderVertexNormal() const {
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

void MeshObject::RenderFaceNormal() const {
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

void MeshObject::Render() const {
    RenderModel();
}

void MeshObject::PostRender() {
    BaseObject::PostRender();
    if (mDoVertexNormalDrawing) {
        RenderVertexNormal();
    }
    if (mDoFaceNormalDrawing) {
        RenderFaceNormal();
    }

    //todo change this if statement
    if (mCollider != nullptr && mCollider->GetColliderType() <= ColliderTypes::SPHERE) {
        static_cast<BoundingVolume *>(mCollider)->UpdateTransformFromParentObject(this);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2.f);
        mCollider->Draw(GetPosition(), GetScale());
        glLineWidth(1.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void MeshObject::CleanUp() const {}

bool MeshObject::SetShader(const std::string &shaderStr) {
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

bool MeshObject::SetMesh(const std::string &meshStr) {
    auto pMesh = engine::GetMesh(meshStr);
    if(pMesh){
        //todo init mesh here
        m_pMesh = pMesh;
        mMeshName = meshStr;
    }
    return pMesh != nullptr;
}

bool MeshObject::IsRenderReady() const {
    return mDoRender;
}

void MeshObject::TryCalculateMatrix() {
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

        //set the bounding box y height to 1
        tempToWorld = glm::scale(tempToWorld, 1.f / glm::vec3(meshSize.y));
        tempToWorld = glm::translate(tempToWorld, -meshCenter);

        mToWorldMatrix = tempToWorld;
        m_MatrixCacheDirty = false;
    }
}

Collider *MeshObject::GetBoundingVolume() {
    return mCollider;
}

glm::mat4 MeshObject::GetObjectToWorldMatrix() const {
    return mToWorldMatrix;
}

std::string MeshObject::GetUsingMeshName() const {
    return mMeshName;
}

std::string MeshObject::GetUsingShaderName() const {
    return mShaderName;
}

glm::vec3 MeshObject::GetPosition() {
    return m_position;
}

void MeshObject::SetPosition(glm::vec3 position) {
    const glm::vec3 amount = position - m_position;
    AddPosition(amount);
    m_MatrixCacheDirty = true;
}

void MeshObject::AddPosition(glm::vec3 amount) {
    m_position += amount;
    m_MatrixCacheDirty = true;
}

glm::vec3 MeshObject::GetRotation() {
    return m_rotation;
}

void MeshObject::SetRotation(glm::vec3 rotation) {
    const glm::vec3 amount = rotation - m_rotation;
    AddRotation(amount);
    m_MatrixCacheDirty = true;
}

void MeshObject::AddRotation(glm::vec3 amount) {
    m_rotation += amount;
    m_MatrixCacheDirty = true;
}

glm::vec3 MeshObject::GetScale() {
    return m_scale;
}

void MeshObject::SetScale(glm::vec3 scale) {
    const glm::vec3 amount = scale - m_scale;
    AddScale(amount);
    m_MatrixCacheDirty = true;
}

void MeshObject::AddScale(glm::vec3 amount) {
    m_scale += amount;
    m_MatrixCacheDirty = true;
}

void MeshObject::SetColor(Color newColor) {
    baseColor = newColor;
    if(m_pShader->HasUniform("EmissiveColor"))
    {
        m_pShader->GetUniformValue<glm::vec3>(mObjectName, "EmissiveColor") = baseColor.AsVec3();
    }
}

void MeshObject::SendMeshDataToShader()
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

void MeshObject::SetTextureOption(bool usingTexture, bool usingGPUUV) {
    mUsingTexture = usingTexture;
    mUsingGPUUV = usingGPUUV;
}

void MeshObject::ChangeTexture(int slot, const std::string &textureName) {
    if(slot >= mTextureSlots.size())
    {
        throw std::out_of_range("Trying to access out of texture slots");
    }
    mTextureSlots[slot] = textureName;
}

void MeshObject::SendMaterialDataToShader() const {
    if(m_pShader->HasUniform("baseColor"))
    {
        m_pShader->GetUniformValue<glm::vec3>(mObjectName, "baseColor") = baseColor.AsVec3();
    }

    if(m_pShader->HasUniform("material"))
    {
        glm::vec3 materialData = glm::vec3(metallic, roughness, 0.f);
        m_pShader->GetUniformValue<glm::vec3>(mObjectName, "material") = materialData;
    }
}
