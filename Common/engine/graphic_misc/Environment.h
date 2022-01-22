/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Environment.h
Purpose: Header file for Environment
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_ENVIRONMENT_H
#define ENGINE_ENVIRONMENT_H
class Environment
{
public:
    void* GetSTD140Structure(){
        return &std140_structure.I_Fog.x;
    }

    static GLint GetSTD140StructureSize()
    {
        return sizeof(std140);
    }

public:
    struct std140
    {
        glm::vec3 I_Fog;
        float zNear;
        glm::vec3 GlobalAmbient;
        float zFar;
    } std140_structure;
};
#endif //ENGINE_ENVIRONMENT_H
