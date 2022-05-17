#pragma once

#include "texture2d.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>


class Framebuffer
{
private:
    GLuint m_fbo, m_rbo;
    std::shared_ptr<Texture2D> m_ownedCbo;
    // cbo may also have the size, but we won't necessairly be using cbo 
    // for example when using this framebuffer as the default GL framebuffer
    glm::ivec2 m_size; 

public:
    Framebuffer();
    ~Framebuffer();

    // no copying, only move
    Framebuffer(Framebuffer&& other);
    Framebuffer& operator=(Framebuffer&& other);

    Framebuffer(const Framebuffer& other) = delete;
    Framebuffer& operator=(const Framebuffer& other) = delete;


    // just remember to use the framebuffer beyond this texture's lifetime
    bool fromTarget(Texture2D *texture);
    bool fromBlank(glm::ivec2 size);
    bool fromWindow(GLFWwindow *window); // to make a default GL framebuffer

    void bind() const;
    // no unbind function as that would require remembering the previous FBO size

    const std::shared_ptr<Texture2D>& getOwnedTarget() const;
};