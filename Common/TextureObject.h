/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TextureObject.h
Purpose: Header file for TextureObject
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_TEXTUREOBJECT_H
#define ENGINE_TEXTUREOBJECT_H

#include "GL/glew.h"
#include "Shader.h"
#include <string>

class TextureObject {
public:
    void SetTextureValues(GLuint textureHandle, GLuint width, GLuint height, GLenum textureType, GLint textureUnit);
    void SetTextureShaderName(const std::string& tName);
    GLint GetTextureUnit();
    GLenum GetTextureType();
    GLuint GetHandle();
    void SetTextureUniform(std::shared_ptr<Shader> pShader);
private:
    GLuint mTextureHandle;
    GLuint mWidth, mHeight;
    GLenum mTextureType;
    GLint mTextureUnit;

    std::string mShaderName;
};


#endif //ENGINE_TEXTUREOBJECT_H
