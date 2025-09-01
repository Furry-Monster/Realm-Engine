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

        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

        if (!gladLoadGL(glfwGetProcAddress))
        {
            glfwDestroyWindow(m_window);
            glfwTerminate();
            return false;
        }
        glfwSwapInterval(1);

        glfwGetFramebufferSize(m_window, &m_framebuffer_width, &m_framebuffer_height);
        glViewport(0, 0, m_framebuffer_width, m_framebuffer_height);

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

    void Window::setTitle(const std::string& title)
    {
        m_title = title;
        if (m_window)
        {
            glfwSetWindowTitle(m_window, title.c_str());
        }
    }

    void Window::windowSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (win)
        {
            win->m_width  = width;
            win->m_height = height;
        }
    }

    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (win)
        {
            win->m_framebuffer_width  = width;
            win->m_framebuffer_height = height;
        }
        glViewport(0, 0, width, height);
    }
} // namespace RealmEngine