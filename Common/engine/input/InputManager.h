/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: InputManager.h
Purpose: Header file for InputManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#ifndef ENGINE_INPUTMANAGER_H
#define ENGINE_INPUTMANAGER_H
#include <utility>
#include <array>
#include "KeyCodes.h"

class InputManager{
public:
    static void Init();
    static void Update();
    static void CleanUp();

    static bool IsPressed(GLint KeyCode);
    static bool IsReleased(GLint KeyCode);
    static bool OnKeyDown(GLint KeyCode);

public:
    //Implementation for GLFW callback from engine
    static void on_key_pressed(GLint KeyCode);
    static void on_key_released(GLint KeyCode);
//    static void on_mouse_pressed(int button);
//    static void on_mouse_released(int button);
//    static void on_mouse_scrolled(double offset);

private:
    inline static std::array<bool, GLFW_KEY_LAST> mNewKeyBuffer, mOldKeyBuffer;
//    static bool IsAnyKeyPressed, IsAnyKeyReleased, IsAnyKeyDown;
};
#endif //ENGINE_INPUTMANAGER_H
