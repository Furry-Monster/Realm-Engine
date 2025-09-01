#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

static struct CameraInfo
{
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float zoom;
} g_camera_info {-90.0f, 0.0f, 2.5f, 0.1f, 45.0f};

class Camera
{
public:
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_world_up;

    float m_yaw;
    float m_pitch;

    float m_movement_speed;
    float m_mouse_sensitivity;
    float m_zoom;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f),
                    float     yaw      = g_camera_info.yaw,
                    float     pitch    = g_camera_info.pitch);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;

    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

private:
    void updateCameraVecs();
};