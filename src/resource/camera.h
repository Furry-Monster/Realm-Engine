#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace RealmEngine
{

    class Camera
    {
    public:
        enum class Movement : uint8_t
        {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
        };

        static constexpr float m_DEFAULT_YAW         = -90.0f;
        static constexpr float m_DEFAULT_PITCH       = 0.0f;
        static constexpr float m_DEFAULT_SPEED       = 2.5f;
        static constexpr float m_DEFAULT_SENSITIVITY = 0.1f;
        static constexpr float m_DEFAULT_FOV         = 45.0f;
        static constexpr float m_DEFAULT_NEAR        = 0.1f;
        static constexpr float m_DEFAULT_FAR         = 100.0f;

        explicit Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
                        const glm::vec3& up       = glm::vec3(0.0f, 1.0f, 0.0f),
                        float            yaw      = m_DEFAULT_YAW,
                        float            pitch    = m_DEFAULT_PITCH);

        const glm::vec3& getPosition() const { return m_position; }
        const glm::vec3& getFront() const { return m_front; }
        const glm::vec3& getUp() const { return m_up; }
        const glm::vec3& getRight() const { return m_right; }

        float getYaw() const { return m_yaw; }
        float getPitch() const { return m_pitch; }
        float getFOV() const { return m_fov; }
        float getMovementSpeed() const { return m_movement_speed; }
        float getMouseSensitivity() const { return m_mouse_sensitivity; }

        void setPosition(const glm::vec3& position) { m_position = position; }
        void setFOV(float fov) { m_fov = fov; }
        void setMovementSpeed(float speed) { m_movement_speed = speed; }
        void setMouseSensitivity(float sensitivity) { m_mouse_sensitivity = sensitivity; }

        glm::mat4 getViewMatrix() const { return glm::lookAt(m_position, m_position + m_front, m_up); }
        glm::mat4 getProjectionMatrix(float aspect) const
        {
            return glm::perspective(glm::radians(m_fov), aspect, m_DEFAULT_NEAR, m_DEFAULT_FAR);
        }

        void processKeyboard(Movement direction, float deltaTime);
        void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
        void processMouseScroll(float yoffset);

    private:
        glm::vec3 m_position;
        glm::vec3 m_front;
        glm::vec3 m_up;
        glm::vec3 m_right;
        glm::vec3 m_world_up;

        float m_yaw;
        float m_pitch;
        float m_fov;
        float m_movement_speed;
        float m_mouse_sensitivity;

        void updateCameraVecs();
    };
} // namespace RealmEngine