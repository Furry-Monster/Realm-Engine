#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "render/camera.h"
#include "render/window.h"

namespace RealmEngine
{
    class Input
    {
    public:
        explicit Input(Camera* camera = nullptr);
        ~Input();

        void initialize();
        void setCamera(Camera* camera);
        void setDeltaTime(float deltaTime);

        bool isMouseCaptured() const { return m_mouse_captured; }

    protected:
        void toggleMouseCapture();

        void mouseCallback(double xpos, double ypos);
        void scrollCallback(double yoffset);
        void stateToggleCallback(int key, int action);
        void movementCallback(int key, int action);

    private:
        Camera* m_camera {nullptr};

        float m_last_mouse_x {320.0f};
        float m_last_mouse_y {240.0f};

        bool  m_first_mouse {true};
        bool  m_mouse_captured {true};
        float m_delta_time {0.0f};
    };
} // namespace RealmEngine