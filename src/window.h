#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <functional>
#include <string>
#include <vector>

namespace RealmEngine
{
    class Window
    {
    public:
        Window(int width, int height, const char* title);
        ~Window();

        bool initialize();
        void terminate();

        bool        shouldClose() { return glfwWindowShouldClose(m_window); }
        void        swapBuffers() { glfwSwapBuffers(m_window); }
        static void pollEvents() { glfwPollEvents(); }

        GLFWwindow* getGLFWwindow() { return m_window; }
        int         getWidth() const { return m_width; }
        int         getHeight() const { return m_height; }
        int         getFramebufferWidth() const { return m_framebuffer_width; }
        int         getFramebufferHeight() const { return m_framebuffer_height; }

        using onResetFunc       = std::function<void()>;
        using onKeyFunc         = std::function<void(int, int, int, int)>;
        using onCharFunc        = std::function<void(unsigned int)>;
        using onCharModsFunc    = std::function<void(int, unsigned int)>;
        using onMouseButtonFunc = std::function<void(int, int, int)>;
        using onCursorPosFunc   = std::function<void(double, double)>;
        using onCursorEnterFunc = std::function<void(int)>;
        using onScrollFunc      = std::function<void(double, double)>;
        using onDropFunc        = std::function<void(int, const char**)>;
        using onWindowSizeFunc  = std::function<void(int, int)>;
        using onWindowCloseFunc = std::function<void()>;

        void registerOnResetFunc(onResetFunc func) { m_onResetFunc.push_back(func); }
        void registerOnKeyFunc(onKeyFunc func) { m_onKeyFunc.push_back(func); }
        void registerOnCharFunc(onCharFunc func) { m_onCharFunc.push_back(func); }
        void registerOnCharModsFunc(onCharModsFunc func) { m_onCharModsFunc.push_back(func); }
        void registerOnMouseButtonFunc(onMouseButtonFunc func) { m_onMouseButtonFunc.push_back(func); }
        void registerOnCursorPosFunc(onCursorPosFunc func) { m_onCursorPosFunc.push_back(func); }
        void registerOnCursorEnterFunc(onCursorEnterFunc func) { m_onCursorEnterFunc.push_back(func); }
        void registerOnScrollFunc(onScrollFunc func) { m_onScrollFunc.push_back(func); }
        void registerOnDropFunc(onDropFunc func) { m_onDropFunc.push_back(func); }
        void registerOnWindowSizeFunc(onWindowSizeFunc func) { m_onWindowSizeFunc.push_back(func); }
        void registerOnWindowCloseFunc(onWindowCloseFunc func) { m_onWindowCloseFunc.push_back(func); }

    protected:
        // event handler
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onKey(key, scancode, action, mods);
            }
        }
        static void charCallback(GLFWwindow* window, unsigned int codepoint)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onChar(codepoint);
            }
        }
        static void charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onCharMods(codepoint, mods);
            }
        }
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onMouseButton(button, action, mods);
            }
        }
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onCursorPos(xpos, ypos);
            }
        }
        static void cursorEnterCallback(GLFWwindow* window, int entered)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onCursorEnter(entered);
            }
        }
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onScroll(xoffset, yoffset);
            }
        }
        static void dropCallback(GLFWwindow* window, int count, const char** paths)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->onDrop(count, paths);
            }
        }
        static void windowSizeCallback(GLFWwindow* window, int width, int height)
        {
            Window* app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->m_width  = width;
                app->m_height = height;
            }
        }
        static void windowCloseCallback(GLFWwindow* window) { glfwSetWindowShouldClose(window, true); }

        // event exec
        void onReset()
        {
            for (auto& func : m_onResetFunc)
                func();
        }
        void onKey(int key, int scancode, int action, int mods)
        {
            for (auto& func : m_onKeyFunc)
                func(key, scancode, action, mods);
        }
        void onChar(unsigned int codepoint)
        {
            for (auto& func : m_onCharFunc)
                func(codepoint);
        }
        void onCharMods(int codepoint, unsigned int mods)
        {
            for (auto& func : m_onCharModsFunc)
                func(codepoint, mods);
        }
        void onMouseButton(int button, int action, int mods)
        {
            for (auto& func : m_onMouseButtonFunc)
                func(button, action, mods);
        }
        void onCursorPos(double xpos, double ypos)
        {
            for (auto& func : m_onCursorPosFunc)
                func(xpos, ypos);
        }
        void onCursorEnter(int entered)
        {
            for (auto& func : m_onCursorEnterFunc)
                func(entered);
        }
        void onScroll(double xoffset, double yoffset)
        {
            for (auto& func : m_onScrollFunc)
                func(xoffset, yoffset);
        }
        void onDrop(int count, const char** paths)
        {
            for (auto& func : m_onDropFunc)
                func(count, paths);
        }
        void onWindowSize(int width, int height)
        {
            for (auto& func : m_onWindowSizeFunc)
                func(width, height);
        }

    private:
        // var below are given
        GLFWwindow* m_window {nullptr};
        int         m_width {0};
        int         m_height {0};
        // var below are detected
        int         m_framebuffer_width {0};
        int         m_framebuffer_height {0};
        std::string m_title;

        // events
        std::vector<onResetFunc>       m_onResetFunc;
        std::vector<onKeyFunc>         m_onKeyFunc;
        std::vector<onCharFunc>        m_onCharFunc;
        std::vector<onCharModsFunc>    m_onCharModsFunc;
        std::vector<onMouseButtonFunc> m_onMouseButtonFunc;
        std::vector<onCursorPosFunc>   m_onCursorPosFunc;
        std::vector<onCursorEnterFunc> m_onCursorEnterFunc;
        std::vector<onScrollFunc>      m_onScrollFunc;
        std::vector<onDropFunc>        m_onDropFunc;
        std::vector<onWindowSizeFunc>  m_onWindowSizeFunc;
        std::vector<onWindowCloseFunc> m_onWindowCloseFunc;
    };
} // namespace RealmEngine