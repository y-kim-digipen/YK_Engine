/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TextureManager.h
Purpose: Header file for TextureManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_TEXTUREMANAGER_H
#define ENGINE_TEXTUREMANAGER_H
#include <string>
#include <map>
#include <GL/glew.h>
#include <GL/gl.h>
#include "TextureObject.h"

class TextureManager {
public:
    int CreateTextureFromFile(const std::string& file, const std::string& textureName, GLenum type, GLint textureUnit);
    TextureObject* FindTextureByName(const std::string& name);
    void BindTexture(TextureObject* pTexture);
private:
    TextureObject* CreateTexture(const std::string& textureName, GLint width, GLint height, GLenum textureType, GLint textureUnit);
    void SetSamplerClampingProperties(GLuint samplerID, GLenum clampProp, GLenum mipmapProp);

    //todo change this to shared ptr
    std::map<std::string, TextureObject*> mTextureObjects;
    std::map<TextureObject*, GLuint> mSamplerObjects;
};


#endif //ENGINE_TEXTUREMANAGER_H
