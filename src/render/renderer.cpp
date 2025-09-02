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
        int width  = 1280; // TODO: Get from window
        int height = 720;  // TODO: Get from window
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

        applyRendererState();

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

    void Renderer::render(Model*           model,
                          Shader*          shader,
                          const glm::mat4& model_matrix,
                          const glm::mat4& view_matrix,
                          const glm::mat4& projection_matrix)
    {
        if (!model || !shader)
            return;

        shader->use();

        shader->setMat4("projection", projection_matrix);
        shader->setMat4("view", view_matrix);
        shader->setMat4("model", model_matrix);

        model->draw(shader->getShaderProgram());
    }

    void Renderer::clear(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::endFrame() const
    {
        if (!m_initialized)
        {
            LOG_ERROR("Renderer not initialized");
            return;
        }
    }

    void Renderer::applyRendererState() const
    {
        if (m_state.enable_depth_test)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        if (m_state.enable_culling)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(m_state.cull_face);
            glFrontFace(GL_CCW);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        glfwSwapInterval(m_state.v_sync_interval);

        if (m_state.enable_msaa)
        {
            glEnable(GL_MULTISAMPLE);
        }
        else
        {
            glDisable(GL_MULTISAMPLE);
        }

        glPolygonMode(GL_FRONT_AND_BACK, m_state.polygon_mode);
        glLineWidth(m_state.line_width);
        glPointSize(m_state.point_size);
    }

    void Renderer::setLighting(Shader*          shader,
                               const glm::vec3& light_pos,
                               const glm::vec3& light_color,
                               const glm::vec3& view_pos)
    {
        if (!shader)
            return;

        shader->setVec3("lightPos", light_pos);
        shader->setVec3("lightColor", light_color);
        shader->setVec3("viewPos", view_pos);
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
        clear();

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