//
// Created by yoonki on 9/11/21.
//
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

