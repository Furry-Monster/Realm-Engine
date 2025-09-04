#include "window.h"
#include "global.h"

namespace RealmEngine
{
    Window::Window(const int width, const int height, const char* title) :
        m_width(width), m_height(height), m_title(title)
    {}

    bool Window::initialize()
    {
        // init glfw
        if (!glfwInit())
        {
            LOG_ERROR("Failed to initialize glfw in Window System");

            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);              // use opengl
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                 // use ver 3.x
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                 // use ver 3.3
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // use core profile
        glfwWindowHint(GLFW_SAMPLES, 4);                               // set MSAA sample nums

        // create window
        m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            LOG_ERROR("Failed to create window in Window System");

            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(m_window);
        setVisible(false);

        // init glad
        if (!gladLoadGL(glfwGetProcAddress))
        {
            LOG_ERROR("Failed to initialize glad in Window System");

            glfwDestroyWindow(m_window);
            glfwTerminate();
            return false;
        }
        glfwSwapInterval(1);      // use v-sync
        glEnable(GL_MULTISAMPLE); // use MSAA

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
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);

        // set frame buffer size
        glfwGetFramebufferSize(m_window, &m_framebuffer_width, &m_framebuffer_height);
        glViewport(0, 0, m_framebuffer_width, m_framebuffer_height);
        registerOnFramebufferSizeFunc([this](int width, int height) { glViewport(0, 0, width, height); });

        // set title with API version
        const char* version = glfwGetVersionString();
        if (version)
        {
            std::string window_title = m_title + " - " + std::string(version);
            glfwSetWindowTitle(m_window, window_title.c_str());
        }

        LOG_INFO("Window System initialized");

        return true;
    }

    void Window::terminate()
    {
        LOG_INFO("Window System terminated");

        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        glfwTerminate();
    }

} // namespace RealmEngine