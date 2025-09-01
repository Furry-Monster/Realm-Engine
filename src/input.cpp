#include "input.h"
#include "engine_context.h"

namespace RealmEngine
{
    Input::Input(Camera* camera) : m_camera(camera) {}

    Input::~Input() { m_camera = nullptr; }

    void Input::initialize()
    {
        // GLFWwindow* window = g_context.m_window->getGLFWwindow();
        // glfwSetWindowUserPointer(window, this);
        // glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
        //     static_cast<Input*>(glfwGetWindowUserPointer(w))->mouseCallback(w, x, y);
        // });
        // glfwSetScrollCallback(window, [](GLFWwindow* w, double x, double y) {
        //     static_cast<Input*>(glfwGetWindowUserPointer(w))->scrollCallback(w, x, y);
        // });
        // glfwSetKeyCallback(window, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        //     static_cast<Input*>(glfwGetWindowUserPointer(w))->keyboardCallback(w, key, scancode, action, mods);
        // });
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        g_context.m_window->registerOnCursorPosFunc([this](double xpos, double ypos) { mouseCallback(xpos, ypos); });
        g_context.m_window->registerOnScrollFunc([this](double /*xpos*/, double ypos) { scrollCallback(ypos); });
        g_context.m_window->registerOnKeyFunc(
            [this](int key, int /*scancode*/, int action, int /*mods*/) { keyboardCallback(key, action); });
    }

    void Input::setCamera(Camera* camera) { m_camera = camera; }

    void Input::setDeltaTime(float deltaTime) { m_delta_time = deltaTime; }

    void Input::processKeyboard()
    {
        if (!m_camera)
            return;

        GLFWwindow* window = g_context.m_window->getGLFWwindow();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_camera->processKeyboard(FORWARD, m_delta_time);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_camera->processKeyboard(BACKWARD, m_delta_time);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_camera->processKeyboard(LEFT, m_delta_time);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_camera->processKeyboard(RIGHT, m_delta_time);
    }

    void Input::mouseCallback(double xpos, double ypos)
    {
        if (!m_mouse_captured || !m_camera)
            return;

        if (m_first_mouse)
        {
            m_lastX       = xpos;
            m_lastY       = ypos;
            m_first_mouse = false;
        }

        float xoffset = xpos - m_lastX;
        float yoffset = m_lastY - ypos;

        m_lastX = xpos;
        m_lastY = ypos;

        m_camera->processMouseMovement(xoffset, yoffset);
    }

    void Input::scrollCallback(double yoffset)
    {
        if (m_camera)
        {
            m_camera->processMouseScroll(yoffset);
        }
    }

    void Input::keyboardCallback(int key, int action)
    {
        GLFWwindow* window = g_context.m_window->getGLFWwindow();

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
            toggleMouseCapture();
        }
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