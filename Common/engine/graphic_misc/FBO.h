/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FBO.h
Purpose: Header file for FBO
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_3
Author: Yoonki Kim, y.kim,  180002421
Creation date: Dec 2, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_FBO_H
#define ENGINE_FBO_H

#include <GL/glew.h>
#include <GL/gl.h>

#include <map>
#include <vector>
class TextureObject;
class FBO {
public:
    ~FBO();
    void Init(GLuint width, GLuint height);
    void SetAttachment(GLenum slot, TextureObject* texture, GLuint level = 0);
    void SetDepthAttachment();
    void UseDrawBuffers();

    std::pair<GLuint, GLuint> GetFBOSize();
    GLuint GetFBOHandle();
    TextureObject *GetTextureAttachment(GLenum slot);
    std::string GetAttachmentTextureName(GLenum slot) const;

private:
    GLuint mFBOHandle;
    GLuint mDepthHandle;

    GLuint mFBOWidth, mFBOHeight;

    std::map<GLenum, TextureObject*> mAttachments;
    std::vector<GLenum> mDrawBuffers;
};


#endif //ENGINE_FBO_H