#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>


#define CAMERA_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define CAMERA_FOV 60.f
#define CAMERA_ASPECT_RATIO 16.0f / 9.0f
#define CAMERA_MOVE_SPEED 5.f
#define CAMERA_ROTATE_SPEED 10.f


Camera::Camera()
{
    m_position = glm::vec3(0.0f);
    m_yaw = -90.0f;
    m_pitch = 0.0f;

    updateViewUsingRotation();
    m_projection = glm::perspective(glm::radians(CAMERA_FOV), CAMERA_ASPECT_RATIO, 0.1f, 100.0f);

    m_isRotationActive = false;
}

Camera::~Camera()
{

}

void Camera::updateViewUsingRotation()
{
    const float sinYaw = sin(glm::radians(m_yaw));
    const float cosYaw = cos(glm::radians(m_yaw));
    const float sinPitch = sin(glm::radians(m_pitch));
    const float cosPitch = cos(glm::radians(m_pitch));

    m_forward = glm::normalize( glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch) );

    m_view = glm::lookAt(m_position, m_position + m_forward, CAMERA_UP);
    m_right = glm::vec3(m_view[0][0], m_view[1][0], m_view[2][0]);
}

// moved this into seperate function so it can initialize a static variable
glm::dvec2 getMousePosition() {
    glm::dvec2 pos;
    glfwGetCursorPos(glfwGetCurrentContext(), &pos.x, &pos.y);
    return pos;
}

void Camera::handleInput(GLFWwindow *window, float deltaTime)
{
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        m_position += m_forward * CAMERA_MOVE_SPEED * deltaTime;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        m_position -= m_forward * CAMERA_MOVE_SPEED * deltaTime;
    } 
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        m_position -= m_right * CAMERA_MOVE_SPEED * deltaTime;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        m_position += m_right * CAMERA_MOVE_SPEED * deltaTime;
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        m_isRotationActive = !m_isRotationActive;
        glfwSetInputMode(window, GLFW_CURSOR, m_isRotationActive ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);   
    }

    static glm::dvec2 lastMousePos = getMousePosition(); // this initialization will be done only once
    glm::dvec2 curMousePos = getMousePosition();
    
    if(m_isRotationActive) {
        glm::dvec2 motion = curMousePos - lastMousePos;
        m_yaw += motion.x * deltaTime * CAMERA_ROTATE_SPEED;
        m_pitch -= motion.y * deltaTime * CAMERA_ROTATE_SPEED;

        m_pitch = glm::clamp(m_pitch, -89.f, 89.f);
    }
    
    lastMousePos = curMousePos;
}

void Camera::update()
{
    updateViewUsingRotation();
}

void Camera::setPosition(glm::vec3 position)
{
    m_position = position;
}

const glm::vec3& Camera::getPosition() const
{
    return m_position;
}

void Camera::setRotation(float yaw, float pitch)
{
    m_yaw = yaw;
    m_pitch = pitch;
}

float Camera::getYaw() const
{
    return m_yaw;
}

float Camera::getPitch() const
{
    return m_pitch;
}

const glm::mat4& Camera::getView() const
{
    return m_view;
}

const glm::mat4& Camera::getProjection() const
{
    // currently no plans for modifying the projection matrix
    return m_projection;
}
