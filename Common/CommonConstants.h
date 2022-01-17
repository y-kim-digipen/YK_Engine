/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: CommonConstants.h
Purpose: Definition of Common Constants
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_COMMONCONSTANTS_H
#define ENGINE_COMMONCONSTANTS_H
#include <string>
#include <vector>

#include <GL/glew.h>

constexpr float PI = 3.1415926535f;
constexpr float HALF_PI = PI/2;

enum class DataType
{
    Bool = GL_BOOL, Int = GL_INT, Float = GL_FLOAT, Vec2f = GL_FLOAT_VEC2, Vec3f, Vec4f, Mat3f = GL_FLOAT_MAT3, Mat4f, Count,
};

struct AttributeInfo{
    GLint location;
    std::string name;
    GLenum DataType;
    GLint DataSize;

    bool operator < (const AttributeInfo& rhs) const{
        auto hash = std::hash<std::string>{};
        return (location < rhs.location) && (hash(name) < hash(rhs.name)) && (DataType < rhs.DataType) && (DataSize < rhs.DataSize);
    }
};

using AttributeInfoContainer = std::vector<AttributeInfo>;

#endif //ENGINE_COMMONCONSTANTS_H
