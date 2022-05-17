#include "framebuffer.hpp"

#include <cstdio>


Framebuffer::Framebuffer() 
{
    m_fbo = m_rbo = 0;
    m_ownedCbo = nullptr;
    m_size = {0, 0};
}

Framebuffer::~Framebuffer() 
{
    if(m_ownedCbo) {
        delete m_ownedCbo;
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
    m_ownedCbo = other.m_ownedCbo;
    m_size = other.m_size;

    other.m_fbo = other.m_rbo = 0;
    other.m_ownedCbo = nullptr;
    other.m_size = {0, 0};
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) 
{
    m_fbo = other.m_fbo;
    m_rbo = other.m_rbo;
    m_ownedCbo = other.m_ownedCbo;
    m_size = other.m_size;

    other.m_fbo = other.m_rbo = 0;
    other.m_ownedCbo = nullptr;
    other.m_size = {0, 0};

    return *this;
}

bool Framebuffer::fromTarget(Texture2D *texture) 
{
    if(!texture || texture->getHandle() == 0) {
        printf("Target given for framebuffer is invalid\n");
        return false;
    }

    if(m_fbo != 0) {
        glDeleteRenderbuffers(1, &m_rbo);
        glDeleteFramebuffers(1, &m_fbo);
    }
    if(m_ownedCbo) {
        delete m_ownedCbo;
        m_ownedCbo = nullptr;
    }

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

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

    m_size = texture->getSize();

    return true;
}

bool Framebuffer::fromBlank(glm::ivec2 size) 
{
    Texture2D *cbo = new Texture2D();
    if(!cbo->fromBlank(size, GL_RGB)) {
        printf("Failed to create texture for framebuffer\n");
        delete cbo;
        return false;
    }

    if(!fromTarget(cbo)) {
        printf("Failed to create framebuffer using newly made texture\n");
        delete cbo;
        return false;
    }

    m_ownedCbo = cbo;

    return true;
}

bool Framebuffer::fromWindow(GLFWwindow *window) 
{
    if(!window) {
        printf("Window given for framebuffer is invalid\n");
        return false;
    }
    if(m_fbo != 0) {
        glDeleteRenderbuffers(1, &m_rbo);
        glDeleteFramebuffers(1, &m_fbo);
    }
    if(m_ownedCbo) {
        delete m_ownedCbo;
        m_ownedCbo = nullptr;
    }

    m_fbo = m_rbo = 0;
    glfwGetWindowSize(window, &m_size.x, &m_size.y);

    return true;
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_size.x, m_size.y);
}

const Texture2D* Framebuffer::getOwnedTarget() const
{
    return m_ownedCbo;
}

