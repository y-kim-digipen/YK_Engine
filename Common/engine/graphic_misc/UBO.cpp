/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: UBO.cpp
Purpose: Source file for UBO
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#include <GL/glew.h>
#include <iostream>
#include "UBO.h"
UBO::~UBO()
{
    deleteUBO();
}

void UBO::createUBO(const size_t byteSize, GLenum usageHint)
{
    if (_isBufferCreated)
    {
        std::cerr << "This buffer is already created! You need to delete it before re-creating it!" << std::endl;
        return;
    }

    // Generate buffer ID, bind it immediately and reserve space for it
    glGenBuffers(1, &_bufferID);
    glBindBuffer(GL_UNIFORM_BUFFER, _bufferID);
    glBufferData(GL_UNIFORM_BUFFER, byteSize, NULL, usageHint);

    // Mark that the buffer has been created and store its size
    _isBufferCreated = true;
    _byteSize = byteSize;
}

void UBO::bindUBO() const
{
    if (!_isBufferCreated)
    {
        std::cerr << "Uniform buffer object is not created yet! You cannot bind it before you create it!" << std::endl;
        return;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, _bufferID);
}

void UBO::setBufferData(const size_t offset, const void* ptrData, const size_t dataSize)
{
    if (!_isBufferCreated)
    {
        std::cerr << "Could not set data because uniform buffer object is not created yet!" << std::endl;
        return;
    }

    if (offset >= _byteSize)
    {
        std::cerr << "Tried to set data of uniform buffer object at offset " << offset << ", but it's beyond buffer size " << _byteSize << ", will be ignored..." << std::endl;
        return;
    }

    if (offset + dataSize > _byteSize)
    {
        std::cerr << "Could not set data because it would overflow the buffer! Offset: " << offset << ", data size: " << dataSize << std::endl;
        return;
    }

    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, ptrData);
}

void UBO::bindBufferBaseToBindingPoint(const GLuint bindingPoint)
{
    if (!_isBufferCreated)
    {
        std::cerr << "Could not bind buffer base to binding point " << bindingPoint << ", because uniform buffer object is not created yet!" << std::endl;
        return;
    }

    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, _bufferID);
}

GLuint UBO::getBufferID() const
{
    return _bufferID;
}

void UBO::deleteUBO()
{
    if (!_isBufferCreated) {
        return;
    }

    std::cout << "Deleting uniform buffer object with ID " << _bufferID << "..." << std::endl;
    glDeleteBuffers(1, &_bufferID);
    _isBufferCreated = false;
}

