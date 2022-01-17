/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: UBO.h
Purpose: Header file for UBO
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_UBO_H
#define ENGINE_UBO_H


#include <cstddef>
#include <GL/glew.h>

class UBO {

public:
    ~UBO();

    void createUBO(const size_t byteSize, GLenum usageHint = GL_STREAM_DRAW);
    void bindUBO() const;
    void setBufferData(const size_t offset, const void* ptrData, const size_t dataSize);
    void bindBufferBaseToBindingPoint(const GLuint bindingPoint);
    GLuint getBufferID() const;
    void deleteUBO();

private:
    GLuint _bufferID{ 0 };
    size_t _byteSize;

    bool _isBufferCreated = false;
};


#endif //ENGINE_UBO_H
