/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TextureObject.cpp
Purpose: Source file for TextureObject
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#include "TextureObject.h"

void TextureObject::SetTextureValues(GLuint textureHandle, GLuint width, GLuint height, GLenum textureType,
                                     GLint textureUnit) {
     mTextureHandle = textureHandle;
     mWidth = width, mHeight = height;
     mTextureType = textureType;
     mTextureUnit = textureUnit;
}

void TextureObject::SetTextureShaderName(const std::string &tName) {
    mShaderName = tName;
}

GLint TextureObject::GetTextureUnit() {
    return mTextureUnit;
}

void TextureObject::SetTextureUniform(std::shared_ptr<Shader> pShader) {
    GLint texSamplerLoc = glGetUniformLocation(pShader->GetProgramID(), mShaderName.c_str());
    if(texSamplerLoc >= 0)//Has uniform
    {
        glUniform1i(texSamplerLoc, mTextureUnit);
    }
}

GLenum TextureObject::GetTextureType() {
    return mTextureType;
}

GLuint TextureObject::GetHandle() {
    return mTextureHandle;
}
