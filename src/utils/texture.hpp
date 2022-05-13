#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture
{
private:
    GLuint m_handle;
    glm::ivec2 m_size;

public:
    Texture();
    ~Texture();

    // no copying, only move
    Texture(Texture&& other);
    Texture& operator=(Texture&& other);

    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;


    bool fromBlank(glm::ivec2 size, GLenum format);
    bool fromData(const void *data, glm::ivec2 size, GLenum format);
    bool fromFile(const char *imageFile);

    GLuint getHandle() const;
    glm::ivec2 getSize() const;

    void bind() const;
    void unbind() const;
};