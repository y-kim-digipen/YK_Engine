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
        mCanClose = true;
    }

    std::string GUI_Window::GetTitleName() const {
        return mTitleName;
    }

    void GUI_Window::Render() {
        ImGui::Begin(mTitleName.c_str(), &mIsOpen, mWindowFlags);
        GUI_Object::PreRender();
        for(auto pContent : m_pContents){
            pContent.second->Render();
        }
        GUI_Object::PostRender();
        ImGui::End();
    }

    bool GUI_Window::IsFocused() const {
        return ImGui::IsWindowFocused(ImGuiFocusedFlags_::ImGuiFocusedFlags_RootWindow);
    }

    bool GUI_Window::AddContent(const std::string &name, GUI_Content *content) {
        content->SetParent(this);
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

    void GUI_Window::SetTitle(const std::string &nameStr) {
        mTitleName = nameStr;
    }
}