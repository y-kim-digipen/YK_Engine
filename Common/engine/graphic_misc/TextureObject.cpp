/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TextureObject.cpp
Purpose: Source file for TextureObject
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/

#include "TextureObject.h"
#include "engine/engine.h"

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

void TextureObject::SendToShader(std::shared_ptr<Shader> pShader, int slot) {
    const std::string textureName = "tex_object" + std::to_string(slot);
    GLint texSamplerLoc = glGetUniformLocation(pShader->GetProgramID(), textureName.c_str());
    if(texSamplerLoc >= 0)//Has uniform
    {
        glUniform1i(texSamplerLoc, mTextureUnit);
    }
    else{
//        std::cout << GetName() << std::endl;
//        engine::GetTextureManager().BindTextureTo(this, 0);
//        texSamplerLoc = glGetUniformLocation(pShader->GetProgramID(), "tex_object0");
        glUniform1i(texSamplerLoc, mTextureUnit);
    }
}

GLenum TextureObject::GetTextureType() {
    return mTextureType;
}

GLuint TextureObject::GetHandle() {
    return mTextureHandle;
}

std::pair<GLuint, GLuint> TextureObject::GetSize() {
    return std::pair<GLuint, GLuint>(mWidth, mHeight);
}

std::string TextureObject::GetName() const {
    return mShaderName;
}
