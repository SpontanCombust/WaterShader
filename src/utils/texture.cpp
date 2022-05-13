#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"

#include <cstdio>

Texture::Texture() 
{
    m_handle = 0;
    m_size = {0, 0};
}

Texture::~Texture() 
{
    if(m_handle != 0) {
        glDeleteTextures(1, &m_handle);
    }
}

Texture::Texture(Texture&& other) 
{
    m_handle = other.m_handle;
    other.m_handle = 0;
}

Texture& Texture::operator=(Texture&& other) 
{
    m_handle = other.m_handle;
    other.m_handle = 0;
    return *this;
}

bool Texture::fromBlank(glm::ivec2 size, GLenum format) 
{
    return fromData(nullptr, size, format);
}

bool Texture::fromData(const void *data, glm::ivec2 size, GLenum format) 
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

bool Texture::fromFile(const char *imageFile) 
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

GLuint Texture::getHandle() const
{
    return m_handle;
}

glm::ivec2 Texture::getSize() const
{
    return m_size;
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_handle);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
