#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "camera.h"

namespace RealmEngine
{
    class Input
    {
    public:
        void initialize(GLFWwindow* window);
        void processKeyboard(GLFWwindow* window);
        void setCamera(Camera* camera);
        void setDeltaTime(float deltaTime);

        void mouseCallback(GLFWwindow* window, double xpos, double ypos);
        void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        bool isMouseCaptured() const { return m_mouse_captured; }
        void toggleMouseCapture();

    private:
        Camera*     m_camera {nullptr};
        GLFWwindow* m_window {nullptr};

        float m_lastX {320.0f};
        float m_lastY {240.0f};
        bool  m_first_mouse {true};
        bool  m_mouse_captured {true};
        float m_delta_time {0.0f};

        void updateMouseCapture();
    };
} // namespace RealmEngine