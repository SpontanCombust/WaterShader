#pragma once

#include "mesh.hpp"
#include "mesh_instance.hpp"

#include <memory>
#include <vector>


class Model
{
public:
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::vector<MeshInstance> m_meshInstances;

public:
    // Loads model using Assimp
    // Formats supported by Assimp can be found here
    // https://github.com/assimp/assimp/blob/master/doc/Fileformats.md
    bool load(const char *modelPath);
};