/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VAOManger.cpp
Purpose: Source file for VAOManger
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#include <GL/glew.h>
#include "VAOManager.h"
#include "Shader.h"

VAOManager::~VAOManager() {
    CleanUp();
}

GLuint VAOManager::GetVAO(GLuint pAttributeID) {
    if(mVAOs.find(pAttributeID) == mVAOs.end()){

        GLuint VAOID;
        glGenVertexArrays(1, &VAOID);

        mVAOs.emplace(pAttributeID, VAOID);
    }
    return mVAOs[pAttributeID];
}


GLuint VAOManager::GetAttribID(AttributeInfoContainer pAttributes) {
    if(mAttribIDs.find(pAttributes) == mAttribIDs.end()) {
        static GLuint idGenerator = 0;
        mAttribIDs.emplace(pAttributes, idGenerator++);
    }
    return mAttribIDs[pAttributes];
}


void VAOManager::CleanUp() {
    for(const auto& attrib : mVAOs) {
        glDeleteVertexArrays(1, &attrib.second);
    }
}
