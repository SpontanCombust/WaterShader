#pragma once

#include "mesh.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>


class MeshInstance
{
private:
    std::shared_ptr<Mesh> m_mesh;
    glm::mat4 m_transform;
    std::string m_name;

public:
    MeshInstance(std::shared_ptr<Mesh> mesh);

    void setName(const std::string& name);
    const std::string& getName() const;

    void setTransform(glm::mat4 transform);

    void draw() const;
};