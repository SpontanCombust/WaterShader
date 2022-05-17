#include "mesh_instance.hpp"

MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh) 
: m_mesh(mesh), m_transform()
{

}

void MeshInstance::setTransform(glm::mat4 transform) 
{
    m_transform = transform;
}

const glm::mat4& MeshInstance::getTransform() const
{
    return m_transform;
}

void MeshInstance::setName(const std::string& name) 
{
    m_name = name;
}

const std::string& MeshInstance::getName() const
{
    return m_name;
}

const std::shared_ptr<Mesh>& MeshInstance::getMesh() const
{
    return m_mesh;
}

void MeshInstance::draw() const
{
    m_mesh->draw();
}
