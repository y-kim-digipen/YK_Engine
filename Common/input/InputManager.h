//
// Created by yoonki on 9/10/21.
//
#ifndef ENGINE_INPUTMANAGER_H
#define ENGINE_INPUTMANAGER_H
#include <utility>
#include <array>
#include "KeyCodes.h"

class InputManager{
friend class Engine;
public:
    static void Init();
    static void Update();
    static void CleanUp();

    static bool IsPressed(GLint KeyCode);
    static bool IsReleased(GLint KeyCode);
    static bool OnKeyDown(GLint KeyCode);

private:
    //Implementation for GLFW callback from Engine
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
