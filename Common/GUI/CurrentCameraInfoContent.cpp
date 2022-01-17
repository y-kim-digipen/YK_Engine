/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: CurrentCameraInfoContent.cpp
Purpose: Source file for CurrentCameraInfoContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "CurrentCameraInfoContent.h"

#include "Engine.h"
#include "SceneBase.h"
#include "Camera.h"

namespace GUI{

    void CurrentCameraInfoContent::Render() {
        auto camera = GetCurrentActiveCamera().get();
        if(camera != nullptr) {
            static float limit = 10000.f;
            static float movementSpeed = 1.f;
            ImGui::BeginChild(ImGui::GetID("Pos"), ImVec2());
            auto camPos = camera->GetEyePosition();
            auto oldCamPos = camPos;
            ImGui::DragFloat3("CamPos", &camPos.x, movementSpeed, -limit, limit);
            ImGui::SameLine();
            ImGui::InputFloat("Speed", &movementSpeed);
            if(oldCamPos != camPos){
                camera->SetEyePosition(camPos);
            }

            ImGui::EndChildFrame();
        }
    }

    std::shared_ptr<Camera> CurrentCameraInfoContent::GetCurrentActiveCamera() const {
        auto pCurrentScene = Engine::GetCurrentScene();
        if(!pCurrentScene){
            return nullptr;
        }
        return pCurrentScene->GetCurrentCamera();
    }
}
