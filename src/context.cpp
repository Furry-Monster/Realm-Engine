#include "context.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace RealmEngine
{
    void Context::create()
    {
        // initialize logger system
        m_logger = std::make_shared<Logger>();

        // initialize window system
        m_window = std::make_shared<Window>(640, 480, "RealmEngine");
        if (!m_window->initialize())
        {
            m_window.reset();
            return;
        }

        // initialize input system
        m_input = std::make_shared<Input>();
        m_input->initialize();

        // initialize imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui_ImplGlfw_InitForOpenGL(g_context.m_window->getGLFWwindow(), GLFW_TRUE);
        ImGui_ImplOpenGL3_Init();
    }

    void Context::destroy()
    {
        // clean imgui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // reset ptrs
        m_input.reset();
        m_window.reset();
        m_logger.reset();
    }

    Context g_context;
} // namespace RealmEngine