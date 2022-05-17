#include "mesh.hpp"

Mesh::Mesh()
{
    glCreateBuffers(1, &m_vboVertices);
    glCreateBuffers(1, &m_vboNormals);
    glCreateBuffers(1, &m_vboUVs);
    glCreateBuffers(1, &m_ibo);
    glCreateVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboUVs);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
    glBindVertexArray(0);

    m_iboSize = m_iboCapacity = 0;

    m_diffuseTexture = nullptr;
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &m_vboVertices);
    glDeleteBuffers(1, &m_vboNormals);
    glDeleteBuffers(1, &m_vboUVs);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
}

Mesh::Mesh(Mesh&& other) 
{
    m_vboVertices = other.m_vboVertices;
    m_vboNormals = other.m_vboNormals;
    m_vboUVs = other.m_vboUVs;
    m_ibo = other.m_ibo;
    m_vao = other.m_vao;

    m_iboSize = other.m_iboSize;
    m_iboCapacity = other.m_iboCapacity;

    m_name = std::move(other.m_name);
    m_diffuseTexture = std::move(other.m_diffuseTexture);


    other.m_vboVertices = 0;
    other.m_vboNormals = 0;
    other.m_vboUVs = 0;
    other.m_ibo = 0;
    other.m_vao = 0;

    other.m_iboSize = other.m_iboCapacity = 0;
    other.m_diffuseTexture = nullptr;
}

Mesh& Mesh::operator=(Mesh&& other) 
{
    m_vboVertices = other.m_vboVertices;
    m_vboNormals = other.m_vboNormals;
    m_vboUVs = other.m_vboUVs;
    m_ibo = other.m_ibo;
    m_vao = other.m_vao;

    m_iboSize = other.m_iboSize;
    m_iboCapacity = other.m_iboCapacity;

    m_name = std::move(other.m_name);
    m_diffuseTexture = std::move(other.m_diffuseTexture);


    other.m_vboVertices = 0;
    other.m_vboNormals = 0;
    other.m_vboUVs = 0;
    other.m_ibo = 0;
    other.m_vao = 0;

    other.m_iboSize = other.m_iboCapacity = 0;
    other.m_diffuseTexture = nullptr;

    return *this;
}



bool Mesh::load(const std::vector<glm::vec3>& vertices, 
                const std::vector<glm::vec3>& normals,
                const std::vector<glm::vec2>& uvs,
                const std::vector<unsigned int>& indices)
{
    if(vertices.size() != normals.size() || vertices.size() != uvs.size())
    {
        printf("Mesh vertex counts don't match for normals or uvs\n");
        return false;
    }

    m_iboSize = indices.size();

    if(m_iboSize > m_iboCapacity)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboUVs);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        m_iboCapacity = m_iboSize;
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), vertices.data());

        glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normals.size() * sizeof(glm::vec3), normals.data());

        glBindBuffer(GL_ARRAY_BUFFER, m_vboUVs);
        glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size() * sizeof(glm::vec2), uvs.data());

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());
    }

    return true;
}

void Mesh::setName(const std::string& name) 
{
    m_name = name;
}

const std::string& Mesh::getName() const
{
    return m_name;
}

void Mesh::setDiffuseTexture(std::shared_ptr<Texture2D> texture) 
{
    m_diffuseTexture = texture;
}

void Mesh::draw() const
{
    glActiveTexture(GL_TEXTURE0);
    if(m_diffuseTexture) {
        m_diffuseTexture->bind();
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_iboSize, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
