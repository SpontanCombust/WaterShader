#include "texture2d.hpp"

#include <stb_image.h>

#include <cstdio>

Texture2D::Texture2D() 
{
    m_handle = 0;
    m_size = {0, 0};
}

Texture2D::~Texture2D() 
{
    if(m_handle != 0) {
        glDeleteTextures(1, &m_handle);
    }
}

Texture2D::Texture2D(Texture2D&& other) 
{
    m_handle = other.m_handle;
    other.m_handle = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) 
{
    m_handle = other.m_handle;
    other.m_handle = 0;
    return *this;
}

bool Texture2D::fromBlank(glm::ivec2 size, GLenum format) 
{
    return fromData(nullptr, size, format);
}

bool Texture2D::fromData(const void *data, glm::ivec2 size, GLenum format) 
{
    if(m_handle != 0) {
        glDeleteTextures(1, &m_handle);
    }

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, data);

    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        printf("Error when creating texture from data: %s\n", glewGetErrorString(err));
        glDeleteTextures(1, &m_handle);
        m_handle = 0;
        return false;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_size = size;

    return true;
}

bool Texture2D::fromFile(const char *imageFile) 
{
    int channels;
    unsigned char *data = stbi_load(imageFile, &m_size.x, &m_size.y, &channels, 0);
    if(!data) {
        printf("Error when creating texture from file: %s\n", stbi_failure_reason());
        return false;
    }

    GLenum format;
    switch(channels) {
        case 4:
            format = GL_RGBA;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 2:
            format = GL_RG;
            break;
        default:
            format = GL_RED;
    }

    bool result = fromData((const void *)data, m_size, format);
    stbi_image_free(data);
    return result;
}

GLuint Texture2D::getHandle() const
{
    return m_handle;
}

glm::ivec2 Texture2D::getSize() const
{
    return m_size;
}

void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
