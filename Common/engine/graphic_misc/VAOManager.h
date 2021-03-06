/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VAOManger.h
Purpose: Header file for VAOManger
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/


#ifndef ENGINE_VAOMANAGER_H
#define ENGINE_VAOMANAGER_H
#include <map>
#include <memory>
#include <vector>

#include <GL/gl.h>
#include "engine/common_constants.h"


class Shader;

class VAOManager {
    friend class Shader;
public:
    ~VAOManager();
    void CleanUp();
public:
    GLuint GetVAO(GLuint pAttributeID);
private:
    GLuint GetAttribID(AttributeInfoContainer pAttributes);
    std::map<GLuint, GLuint> mVAOs;
    std::map<AttributeInfoContainer, GLuint> mAttribIDs;
};


#endif //ENGINE_VAOMANAGER_H
