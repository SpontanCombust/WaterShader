#include "framebuffer.hpp"

#include <cstdio>


Framebuffer::Framebuffer() 
{
    m_fbo = m_rbo = 0;
    m_cbo = nullptr;
    m_ownsCbo = false;
}

Framebuffer::~Framebuffer() 
{
    if(m_cbo && m_ownsCbo) {
        delete m_cbo;
    }
    // FBO and RBO are always created together, so we can only check one of them
    if(m_fbo != 0) {
        glDeleteRenderbuffers(1, &m_rbo);
        glDeleteFramebuffers(1, &m_fbo);
    }
}

Framebuffer::Framebuffer(Framebuffer&& other) 
{
    m_fbo = other.m_fbo;
    m_rbo = other.m_rbo;
    m_cbo = other.m_cbo;
    m_ownsCbo = other.m_ownsCbo;

    other.m_fbo = other.m_rbo = 0;
    other.m_cbo = nullptr;
    other.m_ownsCbo = false;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) 
{
    m_fbo = other.m_fbo;
    m_rbo = other.m_rbo;
    m_cbo = other.m_cbo;
    m_ownsCbo = other.m_ownsCbo;

    other.m_fbo = other.m_rbo = 0;
    other.m_cbo = nullptr;
    other.m_ownsCbo = false;

    return *this;
}

bool Framebuffer::fromTarget(Texture *texture) 
{
    if(!texture || texture->getHandle() == 0) {
        printf("Target given for framebuffer is invalid\n");
        return false;
    }

    if(m_fbo != 0) {
        glDeleteRenderbuffers(1, &m_rbo);
        glDeleteFramebuffers(1, &m_fbo);
    }
    if(m_cbo && m_ownsCbo) {
        delete m_cbo;
    }

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    m_cbo = texture;
    m_ownsCbo = false;

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, texture->getSize().x, texture->getSize().y);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getHandle(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is not complete: %d\n", status);
        glDeleteRenderbuffers(1, &m_rbo);
        glDeleteFramebuffers(1, &m_fbo);
        m_rbo = m_fbo = 0;
        return false;
    }

    return true;
}

bool Framebuffer::fromBlank(glm::ivec2 size, GLenum format) 
{
    Texture *cbo = new Texture();
    if(!cbo->fromBlank(size, format)) {
        printf("Failed to create texture for framebuffer\n");
        delete cbo;
        return false;
    }

    if(!fromTarget(cbo)) {
        printf("Failed to create framebuffer using newly made texture\n");
        delete cbo;
        return false;
    }

    m_ownsCbo = true;
    return true;
}
