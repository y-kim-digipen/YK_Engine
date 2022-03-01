/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIManager.h
Purpose: Header file for GUIManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#ifndef ENGINE_GUIMANAGER_H
#define ENGINE_GUIMANAGER_H
#include <map>
#include <vector>
#include "engine/scene_objects/PrimitiveObject.h"

class GLFWwindow;
namespace GUI{
    class GUI_Window;
    class GUI_Manager{
    public:
        void Init(GLFWwindow* m_pWindow);
        void Update();
        void CleanUp();

    private:

        void PreRender();
        void Render();
        void PostRender();

    private:
        void RenderDockingWindow();

        GLFWwindow* p_glfwWindow;
        bool show_demo_window = false;
        bool show_object_list = false;
        bool show_light_list = false;

        bool show_texture_0 = true;
        bool show_texture_1 = false;
        bool show_texture_2 = false;
        bool show_texture_3 = false;

        bool show_assignment_detail = true;

        std::vector<std::string> textureNames;

        std::vector<std::string> deleting_que;
    private:
        void RenderObjectList();
        void RenderLightList();
        void RenderTextureWindow(const std::string& slotName, std::string& textureName, bool* isOpen);
        void RenderAssignmentDetail();
    };


}
#endif //ENGINE_GUIMANAGER_H
