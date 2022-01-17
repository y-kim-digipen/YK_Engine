/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectListContent.cpp
Purpose: Source file for ObjectListContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#include "ObjectListContent.h"

#include "Engine.h"
#include "GUIWindow.h"
#include "ColorSet.h"
#include "ObjectDetailContent.h"
#include "SceneBase.h"

namespace GUI{

    void ObjectListContent::Render() {
        constexpr size_t inputBufferSize = 20;

        auto pCurrentScene = Engine::GetCurrentScene();
        if(pCurrentScene == nullptr){
            return;
        }
        auto& ObjList = pCurrentScene->GetObjectList();

        for(auto& obj_itr : ObjList){

            auto& objNameStr = obj_itr.first;
            ImGui::PushID(objNameStr.c_str());

            std::string inputBuffer(inputBufferSize, ' ');
            memcpy(inputBuffer.data(), objNameStr.c_str(), inputBufferSize + 1);
            ImGui::SetNextItemWidth(150);
            ImGui::AlignTextToFramePadding();
            ImGui::InputText(("##" +  objNameStr).c_str(), inputBuffer.data(), inputBufferSize, ImGuiInputTextFlags_AutoSelectAll);
            if(inputBuffer != objNameStr){
                //todo make name changeable
//            objNameStr = inputBuffer;
            }
            //todo fix bug
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            auto& GUIManager = Engine::GetGUIManager();
            if(!GUIManager.HasWindow(objNameStr)){
                if(ImGui::Button("  Detail  ")){
                    auto pWindow = GUIManager.AddWindow(objNameStr);
                    pWindow->AddFlag(ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize );
                    pWindow->AddContent((objNameStr + " detail").c_str(), new ObjectDetailContent(obj_itr.second));
                    pWindow->SetCanClose(false);
                }
            }
            else{
                ImGui::PushStyleColor(ImGuiCol_Button, GUI_Color::CollapseButton_Default);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, GUI_Color::CollapseButton_Default);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GUI_Color::CollapseButton_Hover);

                if(ImGui::Button("Collapse")){
                    GUIManager.RemoveWindow(objNameStr);
                }
                ImGui::PopStyleColor(3);
            }

            ImGui::PopID();
        }
    }
}