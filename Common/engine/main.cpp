/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: Main function entry point
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#include "engine.h"

int main(){

    engine::InitWindow(glm::vec2{1920, 1080}, "S22_Assignment1");
    engine::InitEngine();

    do{
        engine::Update();
        } while (engine::IsRunning());
    engine::CleanUp();
    return 0;
}