#include "input.h"
#include "global.h"

namespace RealmEngine
{
    Input::Input(Camera* camera) : m_camera(camera) {}

    void Input::initialize()
    {
        g_context.m_window->registerOnCursorPosFunc([this](double xpos, double ypos) { mouseCallback(xpos, ypos); });
        g_context.m_window->registerOnScrollFunc([this](double /*xpos*/, double ypos) { scrollCallback(ypos); });
        g_context.m_window->registerOnKeyFunc(
            [this](int key, int /*scancode*/, int action, int /*mods*/) { stateToggleCallback(key, action); });
        g_context.m_window->registerOnKeyFunc(
            [this](int key, int /*scancode*/, int action, int /*mods*/) { movementCallback(key, action); });

        glfwSetInputMode(g_context.m_window->getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        LOG_INFO("Input System initialized");
    }

    void Input::tick(float deltaTime) { m_delta_time = deltaTime; }

    void Input::terminated()
    {
        LOG_INFO("Input System terminated");

        m_camera = nullptr;
    }

    void Input::setCamera(Camera* camera) { m_camera = camera; }

    void Input::mouseCallback(double xpos, double ypos)
    {
        if (!m_mouse_captured || !m_camera)
            return;

        if (m_first_mouse)
        {
            m_last_mouse_x = xpos;
            m_last_mouse_y = ypos;
            m_first_mouse  = false;
        }

        float xoffset = xpos - m_last_mouse_x;
        float yoffset = m_last_mouse_y - ypos;

        m_last_mouse_x = xpos;
        m_last_mouse_y = ypos;

        m_camera->processMouseMovement(xoffset, yoffset);
    }

    void Input::scrollCallback(double yoffset)
    {
        if (!m_mouse_captured || !m_camera)
            return;

        m_camera->processMouseScroll(yoffset);
    }

    void Input::stateToggleCallback(int key, int action)
    {
        GLFWwindow* window = g_context.m_window->getGLFWwindow();

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
            toggleMouseCapture();
        }
    }

    void Input::movementCallback(int key, int action)
    {
        if (!m_camera)
            return;

        if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
            m_camera->processKeyboard(Camera::Movement::FORWARD, m_delta_time);
        if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
            m_camera->processKeyboard(Camera::Movement::LEFT, m_delta_time);
        if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
            m_camera->processKeyboard(Camera::Movement::BACKWARD, m_delta_time);
        if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
            m_camera->processKeyboard(Camera::Movement::RIGHT, m_delta_time);
    }

    void Input::toggleMouseCapture()
    {
        m_mouse_captured = !m_mouse_captured;

        if (GLFWwindow* window = g_context.m_window->getGLFWwindow())
        {
            glfwSetInputMode(window, GLFW_CURSOR, m_mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }

        m_first_mouse = true;
    }

} // namespace RealmEngine