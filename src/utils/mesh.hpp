#pragma once

#include "texture2d.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>


class Mesh
{
private:
    GLuint m_vboVertices;
    GLuint m_vboNormals;
    GLuint m_vboUVs;
    GLuint m_ibo;
    GLuint m_vao;

    size_t m_iboSize;
    size_t m_iboCapacity;

    std::string m_name;
    std::shared_ptr<Texture2D> m_diffuseTexture;


public:
    Mesh();
    ~Mesh();

    // no copying (for now)
    Mesh(const Mesh& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;

    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);


    bool load(const std::vector<glm::vec3>& vertices, 
              const std::vector<glm::vec3>& normals,
              const std::vector<glm::vec2>& uvs,
              const std::vector<unsigned int>& indices);

    void setName(const std::string& name);
    const std::string& getName() const;
    
    void setDiffuseTexture(std::shared_ptr<Texture2D> texture);

    void draw() const;
};