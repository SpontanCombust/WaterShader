#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Camera
{
private:
    glm::vec3 m_position;
    float m_yaw;
    float m_pitch;

    glm::vec3 m_forward;
    glm::vec3 m_right;

    glm::mat4 m_view;
    glm::mat4 m_projection;

    bool m_isRotationActive;


public:
    Camera();
    ~Camera();

    void handleInput(GLFWwindow *window, float deltaTime);
    void update();

    void setPosition(glm::vec3 position);
    const glm::vec3& getPosition() const;

    void setRotation(float yaw, float pitch);
    float getYaw() const;
    float getPitch() const;

    const glm::mat4& getView() const;
    const glm::mat4& getProjection() const;

private:
    void updateViewUsingRotation();
};