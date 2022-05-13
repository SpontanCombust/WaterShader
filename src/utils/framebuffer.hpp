#pragma once

#include "texture.hpp"

#include <GL/glew.h>

class Framebuffer
{
private:
    GLuint m_fbo, m_rbo;
    Texture *m_cbo;
    bool m_ownsCbo;

public:
    Framebuffer();
    ~Framebuffer();

    // no copying, only move
    Framebuffer(Framebuffer&& other);
    Framebuffer& operator=(Framebuffer&& other);

    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer& operator=(const Framebuffer& other) = delete;


    // just remember to use the framebuffer beyond this texture's lifetime
    bool fromTarget(Texture *texture);
    bool fromBlank(glm::ivec2 size, GLenum format);
};