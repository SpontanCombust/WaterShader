#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture2D
{
private:
    GLuint m_handle;
    glm::ivec2 m_size;

public:
    Texture2D();
    ~Texture2D();

    // no copying, only move
    Texture2D(Texture2D&& other);
    Texture2D& operator=(Texture2D&& other);

    Texture2D(const Texture2D& other) = delete;
    Texture2D& operator=(const Texture2D& other) = delete;


    bool fromBlank(glm::ivec2 size, GLenum format);
    bool fromData(const void *data, glm::ivec2 size, GLenum format);
    bool fromFile(const char *imageFile);

    GLuint getHandle() const;
    glm::ivec2 getSize() const;

    void bind() const;
    void unbind() const;
};