#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class TextureCube
{
private:
    GLuint m_handle;

public:
    TextureCube();
    ~TextureCube();

    // no copying, only move
    TextureCube(TextureCube&& other);
    TextureCube& operator=(TextureCube&& other);

    TextureCube(const TextureCube& other) = delete;
    TextureCube& operator=(const TextureCube& other) = delete;


    // should be given in order: right, left, top, bottom, front, back
    bool fromFile(const char * faces[6]);

    GLuint getHandle() const;

    void bind() const;
    void unbind() const;
};