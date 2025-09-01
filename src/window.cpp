#include "window.h"

namespace RealmEngine
{
    Window::Window(int width, int height, const char* title) :
        m_window(nullptr), m_width(width), m_height(height), m_framebuffer_width(0), m_framebuffer_height(0),
        m_title(title)
    {}

    Window::~Window() { terminate(); }

    bool Window::initialize()
    {
        // init glfw
        if (!glfwInit())
        {
            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(m_window);

        // init glad
        if (!gladLoadGL(glfwGetProcAddress))
        {
            glfwDestroyWindow(m_window);
            glfwTerminate();
            return false;
        }
        glfwSwapInterval(1);

        // bind native window callbacks
        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetCharCallback(m_window, charCallback);
        glfwSetCharModsCallback(m_window, charModsCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPosCallback);
        glfwSetCursorEnterCallback(m_window, cursorEnterCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
        glfwSetDropCallback(m_window, dropCallback);
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);

        // set frame buffer size
        glfwGetFramebufferSize(m_window, &m_framebuffer_width, &m_framebuffer_height);
        glViewport(0, 0, m_framebuffer_width, m_framebuffer_height);

        // set title with API version
        const char* version = glfwGetVersionString();
        if (version)
        {
            std::string window_title = m_title + " - " + std::string(version);
            glfwSetWindowTitle(m_window, window_title.c_str());
        }

        return true;
    }

    void Window::terminate()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        glfwTerminate();
    }

} // namespace RealmEngine