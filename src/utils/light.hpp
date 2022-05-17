#pragma once

#include <glm/glm.hpp>

struct Light {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};