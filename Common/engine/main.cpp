/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: main.cpp
Purpose: Main function entry point
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "engine.h"

int main(){

    engine::InitWindow(glm::vec2{1600, 900}, "Assignment 2");
    engine::InitEngine();

    do{
        engine::Update();
        } while (engine::IsRunning());
    engine::CleanUp();
    return 0;
}