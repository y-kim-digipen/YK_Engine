/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: FBO.h
Purpose: Source file for FBO
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/


#include "FBO.h"
#include "TextureObject.h"
#include "engine/engine.h"

#include <iostream>
void FBO::Init(GLuint width, GLuint height) {
    mFBOWidth = width;
    mFBOHeight = height;
    glGenFramebuffers(1, &mFBOHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBOHandle);
}

void FBO::SetAttachment(GLenum slot, TextureObject *texture, GLuint level) {
    if(texture->GetSize().first != mFBOWidth || texture->GetSize().second != mFBOHeight)
    {
        std::cerr << "Texture and Framebuffer Dimension mismatch, failed to add attachment to FBO" << std::endl;
        return;
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, slot, texture->GetTextureType(), texture->GetHandle(), level);

    if(mAttachments.find(slot) != mAttachments.end()) //if already in
    {
        mAttachments.erase(mAttachments.find(slot));
        mDrawBuffers.erase(std::find(mDrawBuffers.begin(), mDrawBuffers.end(), slot));
    }
    mAttachments[slot] = texture;
    mDrawBuffers.push_back(slot);

//    engine::GetTextureManager().RegisterFromPointer(std::to_string(mFBOHandle) + "_" + std::to_string(slot) + "_" + std::to_string(texture->GetHandle()), texture);
}

void FBO::SetDepthAttachment() {
    glGenRenderbuffers(1, &mDepthHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mFBOWidth, mFBOHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthHandle);
}

void FBO::UseDrawBuffers() {
    glDrawBuffers(mDrawBuffers.size(), mDrawBuffers.data());
}

std::pair<GLuint, GLuint> FBO::GetFBOSize() {
    return std::pair<GLuint, GLuint>(mFBOWidth, mFBOHeight);
}

GLuint FBO::GetFBOHandle() {
    return mFBOHandle;
}

std::string FBO::GetAttachmentTextureName(GLenum slot) const {
//    return std::to_string(mFBOHandle) + "_" + std::to_string(slot) + "_" + std::to_string(mAttachments[slot]->GetHandle());
    return mAttachments.at(slot)->GetName();
}

TextureObject *FBO::GetTextureAttachment(GLenum slot) {
    return mAttachments[slot];
}

FBO::~FBO() {
    std::cout << "[Deleting FBO] " << mFBOHandle << std::endl;
    glDeleteFramebuffers(1, &mFBOHandle);
}