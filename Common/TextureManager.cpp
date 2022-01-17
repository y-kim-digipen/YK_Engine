/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TextureManager.cpp
Purpose: Source file for TextureManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#include "TextureManager.h"

#include <iostream>

#include "SOIL.h"

int TextureManager::CreateTextureFromFile(const std::string &file, const std::string &textureName, GLenum type,
                                          GLint textureUnit) {
    GLint channel, width, height;
    std::cout << "[ Creating Texture \"" << file << " \"] " <<std::endl;
    GLubyte * img = SOIL_load_image(file.c_str(), &width, &height, &channel, SOIL_LOAD_AUTO);

    if(img == nullptr)
    {
        std::cerr << "Failed to create texture \'" << file << " \'" << std::endl;
        return -1;
    }
    TextureObject* t = CreateTexture(textureName, width, height, type, textureUnit);

    glTextureSubImage2D(t->GetHandle(), 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, img);

    glGenerateMipmap(GL_TEXTURE_2D);

    return 0;
}

TextureObject *
TextureManager::CreateTexture(const std::string &textureName, GLint width, GLint height, GLenum textureType,
                              GLint textureUnit) {
    TextureObject* t = new TextureObject;
    GLuint textureHandle;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    glTextureStorage2D(textureHandle, 6, GL_RGB32F, width, height);

//    glTexImage2D(textureType, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


    GLuint samplerID;
    glCreateSamplers(1, &samplerID);

    glBindSampler(textureUnit, samplerID);
    SetSamplerClampingProperties(samplerID, GL_CLAMP_TO_EDGE, GL_NEAREST);

    t->SetTextureValues(textureHandle, width, height, textureType, textureUnit);
    t->SetTextureShaderName(textureName);

    mTextureObjects.insert(std::make_pair(textureName, t));
    mSamplerObjects.insert(std::make_pair(t, samplerID));

    return t;
}

void TextureManager::SetSamplerClampingProperties(GLuint samplerID, GLenum clampProp, GLenum mipmapProp) {
    glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, clampProp);
    glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, clampProp);
    glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_R, clampProp);
    glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, mipmapProp);
    glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, mipmapProp);

}

TextureObject *TextureManager::FindTextureByName(const std::string &name) {
    return mTextureObjects[name];
}

void TextureManager::BindTexture(TextureObject *pTexture) {
    glActiveTexture(GL_TEXTURE0 + pTexture->GetTextureUnit());
    glBindTexture(pTexture->GetTextureType(), pTexture->GetHandle());
}

