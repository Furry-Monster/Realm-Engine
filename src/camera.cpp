#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    m_front(glm::vec3(0.0f, 0.0f, -1.0f)), m_movement_speed(g_camera_info.speed),
    m_mouse_sensitivity(g_camera_info.sensitivity), m_zoom(g_camera_info.zoom)
{
    m_position = position;
    m_world_up = up;
    m_yaw      = yaw;
    m_pitch    = pitch;
    updateCameraVecs();
}

glm::mat4 Camera::getViewMatrix() const { return glm::lookAt(m_position, m_position + m_front, m_up); }

glm::mat4 Camera::getProjectionMatrix(float aspect) const
{
    return glm::perspective(glm::radians(m_zoom), aspect, 0.1f, 100.0f);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = m_movement_speed * deltaTime;
    if (direction == FORWARD)
        m_position += m_front * velocity;
    if (direction == BACKWARD)
        m_position -= m_front * velocity;
    if (direction == LEFT)
        m_position -= m_right * velocity;
    if (direction == RIGHT)
        m_position += m_right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= m_mouse_sensitivity;
    yoffset *= m_mouse_sensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    updateCameraVecs();
}

void Camera::processMouseScroll(float yoffset)
{
    m_zoom -= yoffset;
    if (m_zoom < 1.0f)
        m_zoom = 1.0f;
    if (m_zoom > 45.0f)
        m_zoom = 45.0f;
}

void Camera::updateCameraVecs()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_world_up));
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}