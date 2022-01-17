/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIWindow.cpp
Purpose: Source file for GUIWindow
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "GUIWindow.h"

#include <iostream>

#include "GUIContent.h"
namespace GUI {
    GUI_Window::GUI_Window(const std::string &titleName) {
        mTitleName = titleName;
        mWindowFlags = ImGuiWindowFlags_::ImGuiWindowFlags_None;
        mIsOpen = true;
    }

    std::string GUI_Window::GetTitleName() const {
        return mTitleName;
    }

    void GUI_Window::PreRender() {
        GUI_Object::PreRender();
        ImGui::SetNextWindowSize(ImVec2(200, 0), ImGuiCond_Once);
        mIsOpen = ImGui::Begin(mTitleName.c_str(), mCanClose ? &mIsOpen : NULL, mWindowFlags);

        //todo add this feature later(only focused menu alpha val is high)
//    if(mIsOpen){
//        if(IsFocused()){
//            ImGui::SetNextWindowBgAlpha(0.3f);
//        }
//        else{
//            ImGui::SetNextWindowBgAlpha(0.7f);
//        }
//    }
    }

    void GUI_Window::Render() {
        for(auto pContent : m_pContents){
            pContent.second->Render();
        }
    }

    void GUI_Window::PostRender() {
        if (mIsOpen) {
            ImGui::End();
            return;
        }
        ImGui::End();
        GUI_Object::PostRender();
    }

    bool GUI_Window::IsFocused() const {
        return ImGui::IsWindowFocused(ImGuiFocusedFlags_::ImGuiFocusedFlags_RootWindow);
    }

    bool GUI_Window::AddContent(const std::string &name, GUI_Content *content) {
        return m_pContents.try_emplace(name, content).second;
    }

    void GUI_Window::AddFlag(ImGuiWindowFlags flag) {
        mWindowFlags |= flag;
    }

    GUI_Window::~GUI_Window() {
//        for(auto& content_itr : m_pContents){
//            delete content_itr.second;
//        }
        m_pContents.clear();
    }

    void GUI_Window::SetCanClose(bool canClose) {
        mCanClose = canClose;
    }
}