/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIWindow.h
Purpose: Header file for GUIWindow
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_GUIWINDOW_H
#define ENGINE_GUIWINDOW_H

#include "GUIObject.h"

#include <string>
#include <map>
namespace GUI{
    class GUI_Content;
    class GUI_Window : public GUI_Object{
        friend class GUI_Manager;
    public:
        virtual ~GUI_Window();
        [[nodiscard]] std::string GetTitleName() const;
        bool AddContent(const std::string& name, GUI_Content* content);
        void AddFlag(ImGuiWindowFlags flag);

        void SetCanClose(bool canClose);
    protected:
        bool IsFocused() const;
        void PreRender() override;
        void Render() override;
        void PostRender() override;
    protected:
        explicit GUI_Window(const std::string &titleName);
        std::string mTitleName;
        ImGuiWindowFlags mWindowFlags;
        std::map<std::string, GUI_Content*> m_pContents;

        bool mCanClose = true;
    };
}


#endif //ENGINE_GUIWINDOW_H
