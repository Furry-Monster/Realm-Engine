#include "renderer.h"
#include "context.h"
#include "resource/model.h"
#include "shader.h"

namespace RealmEngine
{
    void Renderer::initialize()
    {
        if (m_initialized)
        {
            LOG_WARN("Renderer already initialized");
            return;
        }

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
} // namespace RealmEngine