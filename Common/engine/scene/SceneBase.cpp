/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneBase.cpp
Purpose: Source file for SceneBase
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#include "SceneBase.h"
#include "TestScene.h"
#include "engine/engine.h"

#include <iostream>
#include <engine/scene_objects/MeshObject.h>
#include <engine/graphic_misc/BoundingVolume/BasicBoundingVolumes.h>

SceneBase::~SceneBase() {
    for(auto& obj : m_pDeferredObjects){
        delete obj.second;
    }
    for(auto& light : m_pForwardedObjects){
        delete light.second;
    }
    m_pDeferredObjects.clear();
    m_pForwardedObjects.clear();
}

void SceneBase::Init() {
    if(m_pCameras.empty() == false){
        mFocusedCameraIdx = 0;
    }

    Debug_FSQ = new MeshObject("Quad", "Quad", "FSQShader");
    Debug_FSQ->Init();

    Debug_FSQ->ChangeTexture(0, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT0));
    Debug_FSQ->ChangeTexture(1, "");


    Result_FSQ = new MeshObject("Quad", "Quad", "ResultShader2");
    Result_FSQ->Init();
    Result_FSQ->ChangeTexture(0, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT0));
    Result_FSQ->ChangeTexture(1, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT1));
//    Result_FSQ->ChangeTexture(2, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT2));
    Result_FSQ->ChangeTexture(2, "");
    Result_FSQ->ChangeTexture(3, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT3));
    Result_FSQ->ChangeTexture(4, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT4));
    Result_FSQ->ChangeTexture(5, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT5));

//    Reflect_FSQ = new MeshObject("Quad", "Quad", "SSRShader");
//    Reflect_FSQ->ChangeTexture(0, /*engine::renderBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT0)*/"FinalRenderBufferTexture");
//    Reflect_FSQ->ChangeTexture(1, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT6));
//    Reflect_FSQ->ChangeTexture(2, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT7));
//    Reflect_FSQ->ChangeTexture(3, engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT5));
//
//    Combine_FSQ  = new MeshObject("Quad", "Quad", "CombineShader");
//    Combine_FSQ->ChangeTexture(0, engine::renderBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT0));
//    Combine_FSQ->ChangeTexture(1, engine::reflectionBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT1));
}

void SceneBase::PreRender() {
    for(auto& cam : m_pCameras){
        cam->Update();
    }
    for(auto& obj : m_pDeferredObjects){
        auto& pObject = obj.second;
        pObject->PreRender();
    }
    for(auto& light : m_pForwardedObjects){
        auto& pLight = light.second;
        pLight->PreRender();
    }
}

void SceneBase::Render() const {
    FBO& gBuffer = engine::gBuffer;
    UseFBO(gBuffer.GetFBOHandle(), gBuffer.GetFBOSize().first, gBuffer.GetFBOSize().second, true);
    for(auto& obj : m_pDeferredObjects){
        auto& pObject = obj.second;
        if(pObject->IsMeshType())
        {
            if(static_cast<MeshObject*>(pObject)->IsRenderReady()){
                pObject->Render();
            }
            else
            {
                pObject->Render();
            }
        }
    }
}

void SceneBase::PostRender() {
    UseFBO(engine::renderBuffer.GetFBOHandle(), engine::GetWindowSize().x, engine::GetWindowSize().y, true);
    Result_FSQ->Render();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, engine::gBuffer.GetFBOHandle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, engine::renderBuffer.GetFBOHandle());

    glBlitFramebuffer(0, 0, engine::GetWindowSize().x, engine::GetWindowSize().y,
                      0, 0, engine::GetWindowSize().x, engine::GetWindowSize().y,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);



    for(auto& obj : m_pDeferredObjects){
        auto& pObject = obj.second;
        pObject->PostRender();
    }

    for(auto& light : m_pForwardedObjects){
        auto& pLight = light.second;
        if(pLight->IsRenderReady()) {
            pLight->Render();
        }
    }

    for(auto& light : m_pForwardedObjects){
        auto& pLight = light.second;
        if(pLight->IsRenderReady()) {
            pLight->PostRender();
        }
    }

//    UseFBO(engine::reflectionBuffer.GetFBOHandle(), engine::GetWindowSize().x, engine::GetWindowSize().y, true);
//    Reflect_FSQ->Render();
//
//    UseFBO(engine::finalRenderBuffer.GetFBOHandle(), engine::GetWindowSize().x, engine::GetWindowSize().y, true);
//    Reflect_FSQ->Render();


    for(auto& obj : m_pDeferredObjects){
        if(obj.second->GetCollider() != nullptr)
        {
            obj.second->GetCollider()->ResetDebuggingColor();
            mColorPicker.Reset();
            obj.second->ResetCollidedObjects();
        }
    }

    for(auto& obj : m_pDeferredObjects){
        auto& pObject = obj.second;
        for(auto& otherObject : m_pDeferredObjects){
            auto& pOtherObject = otherObject.second;
            if(pObject == pOtherObject)
            {
                continue;
            }
            bool res1 = pObject->IsCollidedWith(pOtherObject);
            bool res2 = pObject->IsCollidedWith(pOtherObject);
            if(res1 || res2)
            {
                if(!(res1 && res2))
                {
                    if(res1)
                    {
                        pOtherObject->AddCollided(pObject);
                    }
                    else
                    {
                        pObject->AddCollided(pOtherObject);
                    }
                }
                continue;
            }

            bool collisionRes = pObject->DoColliding(pOtherObject);
            if(collisionRes == true)
            {
                auto collider1 = pObject->GetCollider();
                auto collider2 = pOtherObject->GetCollider();
//                std::cout << pObject->GetName() << "and " << pOtherObject->GetName() << "Collides" << std::endl;
                Color debugColor = mColorPicker.PickOne();
                collider1->SetDebuggingColor(debugColor);
                collider2->SetDebuggingColor(debugColor);
                pOtherObject->AddCollided(pObject);
                pObject->AddCollided(pOtherObject);
            }
        }
    }

    UseFBO(0, engine::GetWindowSize().x, engine::GetWindowSize().y, true);
}

void SceneBase::CleanUp() {
    m_pDeferredObjects.clear();
    m_pForwardedObjects.clear();
    m_pCameras.clear();
}

std::shared_ptr<Camera> SceneBase::GetCurrentCamera() {
    if(mFocusedCameraIdx < 0 || mFocusedCameraIdx >= static_cast<short>(m_pCameras.size())){
        return nullptr;
    }
    return m_pCameras[mFocusedCameraIdx];
}

template<typename... Args>
void SceneBase::AddCamera(Args... arg) {
    return AddCamera(std::make_shared<Camera>(arg...));
}

//void SceneBase::AddCamera(void) {
//    return AddCamera(std::make_shared<Camera>());
//}

void SceneBase::AddCamera(std::shared_ptr<Camera> cam) {
    m_pCameras.emplace_back(cam);
}

const std::map<std::string, BaseObject*> &SceneBase::GetObjectList() const {
    return m_pDeferredObjects;
}

const std::map<std::string, Light*> &SceneBase::GetLightList() const {
    return m_pForwardedObjects;
}

Light* SceneBase::AddLight(const std::string &objectName, const std::string &usingMesh, const std::string &usingShader) {
    return m_pForwardedObjects.emplace(objectName, new Light(objectName, usingMesh, usingShader)).first->second;
}

Environment &SceneBase::GetEnvironment() {
    return mEnvironment;
}

unsigned SceneBase::GetNumActiveLights() const {
    return m_pForwardedObjects.size();
}

void SceneBase::RemoveLight(const std::string &lightName) {
    m_pForwardedObjects.erase(m_pForwardedObjects.find(lightName));
}

void SceneBase::ClearLights() {
    m_pForwardedObjects.clear();
}

BaseObject* SceneBase::GetObject(const std::string &objName) {
    return m_pDeferredObjects[objName];
}

void SceneBase::UseFBO(GLuint FBOHandle, GLuint viewportWidth, GLuint viewportHeight, bool clearBuffer, GLuint viewportStartX,GLuint viewportStartY ) const {

    glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Cannot switch to FBO " << FBOHandle << std::endl;
        std::cerr << "[FBO Error] Frame Buffer Incomplete" << std::endl;
        return;
    }
    glViewport(viewportStartX, viewportStartY, viewportWidth, viewportHeight);
    if(clearBuffer)
    {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void SceneBase::DebugColorPicker::Reset() {
    currentPointer = 0;
}

Color SceneBase::DebugColorPicker::PickOne() {
    Color returnColor = mSelectableColor[currentPointer++];
    if(currentPointer >= mSelectableColor.size())
    {
        currentPointer = 0;
    }
    return returnColor;
}

SceneBase::DebugColorPicker::DebugColorPicker() {
    mSelectableColor.push_back(Color(115,66,34));
    mSelectableColor.push_back(Color(49,127,67));
    mSelectableColor.push_back(Color(228,160,16));
    mSelectableColor.push_back(Color(161,35,18));
    mSelectableColor.push_back(Color(237,118,14));
    mSelectableColor.push_back(Color(30,45,110));
    mSelectableColor.push_back(Color(255,255,000));
    mSelectableColor.push_back(Color(1,93,82));
}

