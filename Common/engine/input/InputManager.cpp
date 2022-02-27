/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: InputManager.cpp
Purpose: Source file for InputManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#include <iostream>
#include "InputManager.h"

void InputManager::Init() {
}

void InputManager::Update() {
    mOldKeyBuffer = mNewKeyBuffer;
}

void InputManager::CleanUp() {
//    mOldKeyBuffer.();
//    mNewKeyBuffer.clear();
}

bool InputManager::IsPressed(GLint KeyCode) {
    const auto keyIdx = static_cast<int>(KeyCode);
    return !mOldKeyBuffer[keyIdx] && mNewKeyBuffer[keyIdx];
}

bool InputManager::IsReleased(GLint KeyCode) {
    const auto keyIdx = static_cast<int>(KeyCode);
    return !mNewKeyBuffer[keyIdx] && mOldKeyBuffer[keyIdx];
}

bool InputManager::OnKeyDown(GLint KeyCode) {
    return mNewKeyBuffer[static_cast<int>(KeyCode)];
}

void InputManager::on_key_pressed(GLint KeyCode) {
    mNewKeyBuffer[static_cast<int>(KeyCode)] = true;
}

void InputManager::on_key_released(GLint KeyCode) {
    mNewKeyBuffer[static_cast<int>(KeyCode)] = false;
}

