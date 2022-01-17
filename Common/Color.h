/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Color.h
Purpose: Definition of colors
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef SIMPLE_SCENE_COLOR_H
#define SIMPLE_SCENE_COLOR_H
#define MAX_COLOR_BIT 255
struct Color{
public:
    inline Color(float red, float green, float blue, float alpha = 1.f);
    inline Color(int red, int green, int blue, int alpha = 255);
    inline Color(float float_val);
    inline Color(int int_val);
    inline Color();

    inline glm::vec3 AsVec3() const;
    inline glm::vec4 AsVec4() const;

    float r, g, b, a;
};

Color::Color(float red, float green, float blue, float alpha)
    : r(red), g(green), b(blue), a(alpha) {}

Color::Color(int red, int green, int blue, int alpha)
    : Color((float)red/MAX_COLOR_BIT, (float)blue/MAX_COLOR_BIT,
            (float)green/MAX_COLOR_BIT, (float)alpha/MAX_COLOR_BIT) {}

Color::Color(float float_val) : Color(float_val, float_val, float_val) {}

Color::Color(int int_val) : Color((float)int_val/MAX_COLOR_BIT) {}

Color::Color() : Color(0.f) {}

glm::vec3 Color::AsVec3() const {
    return glm::vec3(r, g, b);
}

glm::vec4 Color::AsVec4() const {
    return glm::vec4(r, g, b, a);
}


#endif //SIMPLE_SCENE_COLOR_H
