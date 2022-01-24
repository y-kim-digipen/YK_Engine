/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.h
Purpose: Header file for Light
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#include "engine/scene_objects/Object.h"
namespace GUI{
    class LightDetailContent;
}
class Light : public Object{
    friend class GUI::LightDetailContent;
    friend class LightManager;
public:
    Light(const std::string& lightName) : Object(lightName, "Bulb", "DiffuseShader") {
        SetPosition(glm::vec3(1.f, 1.f, 0.f));
    }

    Light(const std::string& lightName, const std::string& meshName, const std::string& shaderName) : Object(lightName, meshName, shaderName) { }

    void PreRender() override
    {
        std140_structure.position = GetPosition();
        Object::PreRender();
    }

    void* GetSTD140Structure(){
        return &std140_structure.position;
    }

    static GLint GetSTD140StructureSize()
    {
        return sizeof(std140);
    }

public:
    enum LightType{
        POINT_LIGHT = 0,
//        DIRECTIONAL_LIGHT = 1,
        SPOT_LIGHT,
    };

    struct std140{
        glm::vec3 position; /*just update base classes position, it is just for padding*/
        int type = LightType::POINT_LIGHT;
        float Ka = 0.1f;
        float Kd = 0.5f;
        float Ks = 0.8f;
        float ns = 3.f;

        glm::vec3 Ia = glm::vec3(250, 50, 50);

        float c1 = 0.1;
        glm::vec3 Id = glm::vec3(120, 250, 10);
        float c2 = 0.005;
        glm::vec3 Is = glm::vec3(2, 200, 2);
        float c3 = 0.0025;

        glm::vec3 dir = glm::vec3(0.f, -1.f, 0.f);
        float theta = HALF_PI * 0.12f;
        glm::vec3 padding;
        float phi =  HALF_PI * 0.25;
        glm::vec3 DummyPadding;
        float P = 1.f;
    } std140_structure;
};

#endif //ENGINE_LIGHT_H
