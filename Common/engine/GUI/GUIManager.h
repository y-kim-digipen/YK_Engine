/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIManager.h
Purpose: Header file for GUIManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_GUIMANAGER_H
#define ENGINE_GUIMANAGER_H
#include <map>
#include <vector>
class GLFWwindow;
namespace GUI{
    class GUI_Window;
    class GUI_Manager{
    public:
        void Init(GLFWwindow* m_pWindow);
        void Update();
        void CleanUp();

        GUI_Window* AddWindow(const std::string& name);
        bool HasWindow(const std::string& windowNameStr);
        void RemoveWindow(const std::string& windowNameStr);
    private:
        void InitWindows();
        void PreRender();
        void Render();
        void PostRender();

    private:
        void RenderDockingWindow();

        bool show_demo_window = false;
        std::map<std::string, GUI_Window*> m_pWindows;
        GLFWwindow* p_glfwWindow;

        bool show_object_list = false;
        bool show_light_list = false;

        std::vector<std::string> deleting_que;
    private:
        void RenderObjectList();
        void RenderLightList();


    };
}
#endif //ENGINE_GUIMANAGER_H
