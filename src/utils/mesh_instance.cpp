#include "mesh_instance.hpp"

MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh) 
: m_mesh(mesh), m_transform()
{

}

void MeshInstance::setTransform(glm::mat4 transform) 
{
    m_transform = transform;
}

void MeshInstance::setName(const std::string& name) 
{
    m_name = name;
}

const std::string& MeshInstance::getName() const
{
    return m_name;
}

void MeshInstance::draw() const
{
    m_mesh->draw();
}
