//
// Created by yoonki on 9/11/21.
//

#ifndef ENGINE_KEYCODES_H
#define ENGINE_KEYCODES_H
#include <GLFW/glfw3.h>

enum class Key
{
    SPACE = GLFW_KEY_SPACE,
    A = GLFW_KEY_A,
    B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    L_CTRL = GLFW_KEY_LEFT_CONTROL, L_ALT = GLFW_KEY_LEFT_ALT,
    F1 = GLFW_KEY_F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    ESC = GLFW_KEY_ESCAPE,
    ENTER = GLFW_KEY_ENTER,
    RIGHT_ARROW = GLFW_KEY_RIGHT, LEFT_ARROW, DOWN_ARROW, UP_ARROW,
    COUNT = 48,
};

enum class M_button
{
    LEFT = GLFW_MOUSE_BUTTON_1, RIGHT = GLFW_MOUSE_BUTTON_2, WHEEL = GLFW_MOUSE_BUTTON_3,
    COUNT,
};

inline int ConvertKeyEnumToInt(Key key){
    if (key == Key::SPACE)
    {
        return 0;
    }
    if (key == Key::ESC)
    {
        return 42;
    }
    if(key == Key::ENTER)
    {
        return 43;
    }
    if (key == Key::L_CTRL)
    {
        return 27;
    }
    if (key == Key::L_ALT)
    {
        return 28;
    }
    if (key >= Key::A && key <= Key::Z)
    {
        return 1 + static_cast<int>(key) - static_cast<int>(Key::A);
    }
    if (key >= Key::F1 && key <= Key::F12)
    {
        return 28 + static_cast<int>(key) - static_cast<int>(Key::F1);
    }
    if (key >= Key::RIGHT_ARROW && key <= Key::UP_ARROW)
    {
        return 44 + static_cast<int>(key) - static_cast<int>(Key::RIGHT_ARROW);
    }
    return -1;
}

#endif //ENGINE_KEYCODES_H
