#include "model.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>

std::vector<std::shared_ptr<Texture2D>> processMaterials(const aiScene *scene)
{
    std::vector<std::shared_ptr<Texture2D>> textures(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial *mat = scene->mMaterials[i];
        aiString path;
        if(mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) > 0) 
        {
            Texture2D texture;
            if(const aiTexture *embedded = scene->GetEmbeddedTexture(path.C_Str())) 
            {
                if(texture.fromData(embedded->pcData, {embedded->mWidth, embedded->mHeight}, GL_RGBA)) 
                {
                    textures[i] = std::make_shared<Texture2D>(std::move(texture));
                } 
                else 
                {
                    fprintf(stderr, "Failed to process embedded texture %s for material at index %d\n", path.C_Str(), i);
                }
            } 
            else 
            {
                if(texture.fromFile(path.C_Str()))
                {
                    textures[i] = std::make_shared<Texture2D>(std::move(texture));
                }
                else
                {
                    fprintf(stderr, "Failed to process file texture %s for material at index %d\n", path.C_Str(), i);
                }
            }
        } 
        else 
        {
            fprintf(stderr, "No diffuse texture found for material at index %d\n", i);            
        }
    }

    return textures;
}

std::vector<std::shared_ptr<Mesh>> processMeshes(const aiScene *scene)
{
    auto textures = processMaterials(scene);

    std::vector<std::shared_ptr<Mesh>> meshes(scene->mNumMeshes);

    std::vector<glm::vec3> vertices, normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[i];
        const unsigned int numVertices = mesh->mNumVertices;

        vertices.resize(numVertices);
        normals.resize(numVertices);
        uvs.resize(numVertices);
        indices.resize(mesh->mNumFaces * 3);


        std::memcpy(&vertices[0], (void *)mesh->mVertices, numVertices * sizeof(glm::vec3));
        std::memcpy(&normals[0], (void *)mesh->mNormals, numVertices * sizeof(glm::vec3));

        for (size_t i = 0; i < numVertices; i++)
        {
            uvs[i] = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
            indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
            indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
        }


        Mesh convertedMesh;
        if(convertedMesh.load(vertices, normals, uvs, indices))
        {
            convertedMesh.setDiffuseTexture(textures[mesh->mMaterialIndex]);
            convertedMesh.setName(mesh->mName.C_Str());
            meshes[i] = std::make_shared<Mesh>(std::move(convertedMesh));
        }
        else
        {
            fprintf(stderr, "Failed to load geometry for mesh %s at index %d\n", mesh->mName.C_Str(), i);
        }
    } 

    return meshes;
}

std::vector<MeshInstance> processNodes(const aiScene *scene, aiNode *node, const std::vector<std::shared_ptr<Mesh>>& meshes)
{
    // make sure the conversion later on is valid
    assert(sizeof(glm::mat4) == sizeof(aiMatrix4x4));

    std::vector<MeshInstance> instances;

    if(node)
    {
        if(node->mNumMeshes > 0)
        {
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                auto mesh = meshes[node->mMeshes[i]];
                MeshInstance inst(mesh);

                inst.setName(mesh->getName() + "_" + node->mName.C_Str());

                // Assimp's matrices are row-major whereas glm's are column major
                // We can quickly convert it to CM by reinterpreting the data under it
                // (aiMatrix4x4 should also have 16 floats) and then transposing the matrix
                glm::mat4 transform = *(glm::mat4 *)&node->mTransformation;
                inst.setTransform(glm::transpose(transform));

                instances.push_back(inst);
            }
        }

        if(node->mNumChildren > 0)
        {
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                auto childInstances = processNodes(scene, node->mChildren[i], meshes);
                instances.insert(instances.end(), childInstances.begin(), childInstances.end());   
            }
        }
    }

    return instances;
}

bool Model::load(const char *modelPath) 
{
    printf("Load model from %s\n", modelPath);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile( 
        modelPath, 
        aiProcess_Triangulate 
        | aiProcess_FlipUVs 
        | aiProcess_GenNormals 
        | aiProcess_GenUVCoords 
        | aiProcess_OptimizeMeshes 
        | aiProcess_JoinIdenticalVertices
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "Failed to read model from %s\n. Error: %s\n", modelPath, importer.GetErrorString());
        return false;
    }


    m_meshes.clear();
    m_meshInstances.clear();

    m_meshes = std::move(processMeshes(scene));
    m_meshInstances = processNodes(scene, scene->mRootNode, m_meshes);

    return true;
}
