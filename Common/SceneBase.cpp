/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneBase.cpp
Purpose: Source file for SceneBase
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "SceneBase.h"
#include "TestScene.h"
#include "Engine.h"

#include <iostream>

void SceneBase::Init() {
    if(m_pCameras.empty() == false){
        mFocusedCameraIdx = 0;
    }

    FSQ_Object = new Object("Quad", "Quad", "FSQShader");
//    FSQ_Object = new Object("Quad", "Quad", "DebugShader");
    FSQ_Object->Init();

    FSQ_Object->ChangeTexture(0, Engine::gBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT1));
    FSQ_Object->ChangeTexture(1, "");
//    auto pLight = m_pLights.emplace("DefaultLight", new Light("DefaultLight")).first->second;
//    pLight->SetScale(glm::vec3(0.2f));
//    pLight->SetPosition(glm::vec3(1.f, 1.f, 0.f));
//
//    pLight = m_pLights.emplace("DefaultLight2", new Light("DefaultLight2")).first->second;
//    pLight->SetScale(glm::vec3(0.2f));
//    pLight->SetPosition(glm::vec3(-3.f, 0.f, 0.f));
////    pLight->std140_structure.I_Emissive = glm::vec3(0.f, 0.f, 230.f);
//    pLight->std140_structure.position = glm::vec3(0.f, 0.f, 3.f);
////    pLight->std140_structure.I_Emissive = glm::vec3(0.f, 59.f, 9.f);
}

void SceneBase::PreRender() {
    for(auto& cam : m_pCameras){
        cam->Update();
    }
    for(auto& obj : m_pObjects){
        auto& pObject = obj.second;
        pObject->PreRender();
    }
    for(auto& light : m_pLights){
        auto& pLight = light.second;
        pLight->PreRender();
    }
}

void SceneBase::Render() const {
    FBO& gBuffer = Engine::gBuffer;
    UseFBO(gBuffer.GetFBOHandle(), gBuffer.GetFBOSize().first, gBuffer.GetFBOSize().second, true);
    for(auto& obj : m_pObjects){
        auto& pObject = obj.second;
        if(pObject->IsRenderReady()){
            pObject->Render();
        }
    }
    for(auto& light : m_pLights){
        auto& pLight = light.second;
        if(pLight->IsRenderReady()) {
            pLight->Render();
        }
    }
    UseFBO(0, Engine::GetWindowSize().x, Engine::GetWindowSize().y, true);
    FSQ_Object->Render();
}

void SceneBase::PostRender() {
    for(auto& obj : m_pObjects){
        auto& pObject = obj.second;
        pObject->PostRender();
    }
    for(auto& light : m_pLights){
        auto& pLight = light.second;
        if(pLight->IsRenderReady()) {
            pLight->PostRender();
        }
    }

}

void SceneBase::CleanUp() {
    m_pObjects.clear();
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

const std::map<std::string, std::shared_ptr<Object>> &SceneBase::GetObjectList() const {
    return m_pObjects;
}

std::shared_ptr<Object> SceneBase::AddObject(const std::string &objectName, const std::string &usingMesh, const std::string &usingShader) {
    return  m_pObjects.emplace(objectName, std::make_unique<Object>(objectName, usingMesh, usingShader)).first->second;
}

const std::map<std::string, std::shared_ptr<Light>> &SceneBase::GetLightList() const {
    return m_pLights;
}

std::shared_ptr<Light> SceneBase::AddLight(const std::string &objectName, const std::string &usingMesh, const std::string &usingShader) {
    return  m_pLights.emplace(objectName, std::make_unique<Light>(objectName, usingMesh, usingShader)).first->second;
}

Environment &SceneBase::GetEnvironment() {
    return mEnvironment;
}

unsigned SceneBase::GetNumActiveLights() const {
    return m_pLights.size();
}

void SceneBase::RemoveLight(const std::string &lightName) {
    m_pLights.erase(m_pLights.find(lightName));
}

void SceneBase::ClearLights() {
    m_pLights.clear();
}

std::shared_ptr<Object> SceneBase::GetObject(const std::string &objName) {
    return m_pObjects[objName];
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}


