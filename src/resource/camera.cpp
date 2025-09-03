#include "camera.h"

namespace RealmEngine
{
    Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) :
        m_position(position), m_world_up(up), m_yaw(yaw), m_pitch(pitch), m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
        m_fov(m_DEFAULT_FOV), m_movement_speed(m_DEFAULT_SPEED), m_mouse_sensitivity(m_DEFAULT_SENSITIVITY)
    {
        updateCameraVecs();
    }

    glm::mat4 Camera::getViewMatrix() const { return glm::lookAt(m_position, m_position + m_front, m_up); }

    glm::mat4 Camera::getProjectionMatrix(float aspect) const
    {
        return glm::perspective(glm::radians(m_fov), aspect, m_DEFAULT_NEAR, m_DEFAULT_FAR);
    }

    void Camera::processKeyboard(Camera::Movement direction, float deltaTime)
    {
        float velocity = m_movement_speed * deltaTime;
        if (direction == Camera::Movement::FORWARD)
            m_position += m_front * velocity;
        if (direction == Camera::Movement::BACKWARD)
            m_position -= m_front * velocity;
        if (direction == Camera::Movement::LEFT)
            m_position -= m_right * velocity;
        if (direction == Camera::Movement::RIGHT)
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
        m_fov -= yoffset;
        if (m_fov < 1.0f)
            m_fov = 1.0f;
        if (m_fov > 45.0f)
            m_fov = 45.0f;
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
} // namespace RealmEngine