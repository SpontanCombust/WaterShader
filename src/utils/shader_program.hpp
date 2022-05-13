#pragma once

#include <GL/glew.h>

class ShaderProgram
{
private:
    GLuint m_handle;

public:
    ShaderProgram();
    ~ShaderProgram();

    // only move allowed
    ShaderProgram(ShaderProgram&& other);
    ShaderProgram& operator=(ShaderProgram&& other);

    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(const ShaderProgram& other) = delete;


    bool fromSources(const char *vertexShaderSource, const char *fragmentShaderSource);
    bool fromFiles(const char *vertexShaderFile, const char *fragmentShaderFile);

    GLuint getHandle() const;

    void bind() const;
    void unbind() const;
};