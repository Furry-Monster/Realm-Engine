#include "renderer.h"
#include "context.h"
#include "render/framebuffer.h"
#include "resource/model.h"
#include "shader.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <memory>

namespace RealmEngine
{
    void Renderer::initialize()
    {
        if (m_initialized)
        {
            LOG_WARN("Renderer already initialized");
            return;
        }

        // initialize imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui_ImplGlfw_InitForOpenGL(g_context.m_window->getGLFWwindow(), GLFW_TRUE);
        ImGui_ImplOpenGL3_Init();

        m_state_mgr       = std::make_unique<StateManager>();
        m_framebuffer_mgr = std::make_unique<FramebufferManager>();

        m_state_mgr->initialize();

        // Get window size for framebuffer initialization
        int width  = g_context.m_window->getFramebufferWidth();
        int height = g_context.m_window->getFramebufferHeight();
        m_framebuffer_mgr->initialize(width, height);

        if (m_mode == RenderMode::Defferd)
        {
            m_pipeline = std::make_unique<DeferredPipeline>(m_framebuffer_mgr.get(), m_state_mgr.get());
        }
        else
        {
            m_pipeline = std::make_unique<ForwardPipeline>();
        }

        m_pipeline->initialize();

        m_initialized = true;
        LOG_INFO("Renderer initialized");
    }

    void Renderer::terminate()
    {
        if (!m_initialized)
        {
            LOG_WARN("Renderer not initialized");
            return;
        }

        // clean imgui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        m_initialized = false;
        LOG_INFO("Renderer terminated");
    }

    void Renderer::beginFrame() const
    {
        if (!m_initialized)
        {
            LOG_ERROR("Renderer not initialized");
            return;
        }
    }

    void Renderer::endFrame() const
    {
        if (!m_initialized)
        {
            LOG_ERROR("Renderer not initialized");
            return;
        }
    }

    void Renderer::addRenderObject(Model* model, const glm::mat4& model_matrix)
    {
        if (m_mode == RenderMode::Defferd)
        {
            auto* deferred_pipeline = dynamic_cast<DeferredPipeline*>(m_pipeline.get());
            if (deferred_pipeline)
            {
                deferred_pipeline->addRenderObject(model, model_matrix);
            }
        }
    }

    void Renderer::addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    {
        if (m_mode == RenderMode::Defferd)
        {
            auto* deferred_pipeline = dynamic_cast<DeferredPipeline*>(m_pipeline.get());
            if (deferred_pipeline)
            {
                deferred_pipeline->addDirectionalLight(direction, color, intensity);
            }
        }
    }

    void Renderer::addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity)
    {
        if (m_mode == RenderMode::Defferd)
        {
            auto* deferred_pipeline = dynamic_cast<DeferredPipeline*>(m_pipeline.get());
            if (deferred_pipeline)
            {
                deferred_pipeline->addPointLight(position, color, intensity);
            }
        }
    }

    void Renderer::setCamera(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position)
    {
        if (m_mode == RenderMode::Defferd)
        {
            auto* deferred_pipeline = dynamic_cast<DeferredPipeline*>(m_pipeline.get());
            if (deferred_pipeline)
            {
                deferred_pipeline->setCamera(view, projection, position);
            }
        }
    }

    void Renderer::renderFrame()
    {
        if (!m_initialized || !m_pipeline)
            return;

        beginFrame();

        m_pipeline->render();

        if (m_mode == RenderMode::Defferd)
        {
            auto* deferred_pipeline = dynamic_cast<DeferredPipeline*>(m_pipeline.get());
            if (deferred_pipeline)
            {
                deferred_pipeline->clearRenderObjects();
                deferred_pipeline->clearLights();
            }
        }

        endFrame();
    }
} // namespace RealmEngine