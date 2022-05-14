#include "textureCube.hpp"

#include <stb_image.h>

TextureCube::TextureCube() 
{
    m_handle = 0;
}

TextureCube::~TextureCube() 
{
    if(m_handle != 0) {
        glDeleteTextures(1, &m_handle);
    }
}

TextureCube::TextureCube(TextureCube&& other) 
{
    m_handle = other.m_handle;
    other.m_handle = 0;
}

TextureCube& TextureCube::operator=(TextureCube&& other) 
{
    m_handle = other.m_handle;
    other.m_handle = 0;
    return *this;
}

bool TextureCube::fromFile(const char * faces[6]) 
{
    if(m_handle != 0) {
        glDeleteTextures(1, &m_handle);
    }

    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);

    int w, h, ch;
    for (int i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(faces[i], &w, &h, &ch, 0);
        if(data) {
            GLenum format = ch == 4 ? GL_RGBA : GL_RGB; // not expecting any other formats
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        } else {
            fprintf(stderr, "Failed to load texture: %s\n", faces[i]);
            glDeleteTextures(1, &m_handle);
            m_handle = 0;
            return false;
        }
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return true;
}

GLuint TextureCube::getHandle() const
{
    return m_handle;
}

void TextureCube::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);
}

void TextureCube::unbind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

