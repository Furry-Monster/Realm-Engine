#include "pipeline.h"
#include "global.h"
#include "logger.h"
#include "render/framebuffer.h"
#include "render/pass/gbuffer_pass.h"
#include "render/pass/lighting_pass.h"
#include "render/state.h"

namespace RealmEngine
{
    void ForwardPipeline::initialize() { LOG_INFO("ForwardPipeline initialized"); }

    void ForwardPipeline::terminate() { LOG_INFO("ForwardPipeline terminated"); }

    void ForwardPipeline::renderShadowMaps()
    {
        // TODO: Implement shadow mapping
    }

    void ForwardPipeline::renderForward()
    {
        // TODO: Implement forward rendering
    }

    void ForwardPipeline::renderPostProcess()
    {
        // TODO: Implement post processing
    }

    void ForwardPipeline::renderUI()
    {
        // TODO: Implement UI rendering
    }

    DeferredPipeline::DeferredPipeline(FramebufferManager* fb_mgr, StateManager* state_mgr) :
        m_framebuffer_mgr(fb_mgr), m_state_mgr(state_mgr)
    {
        m_gbuffer_pass  = std::make_unique<GBufferPass>(fb_mgr, state_mgr);
        m_lighting_pass = std::make_unique<LightingPass>(fb_mgr, state_mgr);
    }

    DeferredPipeline::~DeferredPipeline() = default;

    void DeferredPipeline::initialize()
    {
        if (!m_framebuffer_mgr || !m_state_mgr)
        {
            LOG_ERROR("DeferredPipeline dependencies not set");
            return;
        }

        LOG_INFO("DeferredPipeline initialized");
    }

    void DeferredPipeline::terminate()
    {
        m_gbuffer_pass.reset();
        m_lighting_pass.reset();
        LOG_INFO("DeferredPipeline terminated");
    }

    void DeferredPipeline::setCamera(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position)
    {
        m_view_matrix       = view;
        m_projection_matrix = projection;
        m_camera_position   = position;

        if (m_gbuffer_pass)
        {
            m_gbuffer_pass->setViewMatrix(view);
            m_gbuffer_pass->setProjectionMatrix(projection);
            m_gbuffer_pass->setPrevViewProjectionMatrix(m_prev_view_projection);
        }

        if (m_lighting_pass)
        {
            glm::mat4 inv_vp = glm::inverse(projection * view);
            m_lighting_pass->setInvViewProjectionMatrix(inv_vp);
            m_lighting_pass->setViewPosition(position);
        }

        m_prev_view_projection = projection * view;
    }

    void DeferredPipeline::addRenderObject(Model* model, const glm::mat4& model_matrix)
    {
        if (m_gbuffer_pass && model)
        {
            RenderObject obj;
            obj.model             = model;
            obj.model_matrix      = model_matrix;
            obj.prev_model_matrix = model_matrix; // TODO: Track previous frame transform
            m_gbuffer_pass->addRenderObject(obj);
        }
    }

    void DeferredPipeline::addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity)
    {
        if (m_lighting_pass)
        {
            DirectionalLight light;
            light.direction = direction;
            light.color     = color;
            light.intensity = intensity;
            m_lighting_pass->addDirectionalLight(light);
        }
    }

    void DeferredPipeline::addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity)
    {
        if (m_lighting_pass)
        {
            PointLight light;
            light.position  = position;
            light.color     = color;
            light.intensity = intensity;
            m_lighting_pass->addPointLight(light);
        }
    }

    void DeferredPipeline::clearLights()
    {
        if (m_lighting_pass)
        {
            m_lighting_pass->clearLights();
        }
    }

    void DeferredPipeline::clearRenderObjects()
    {
        if (m_gbuffer_pass)
        {
            m_gbuffer_pass->clearRenderObjects();
        }
    }

    void DeferredPipeline::renderShadowMaps()
    {
        // TODO: Implement shadow mapping
    }

    void DeferredPipeline::renderGBuffer()
    {
        if (m_gbuffer_pass)
        {
            m_gbuffer_pass->draw();
        }
    }

    void DeferredPipeline::renderLighting()
    {
        if (m_lighting_pass)
        {
            m_lighting_pass->draw();
        }
    }

    void DeferredPipeline::renderForwardObjects()
    {
        // TODO: Implement transparent objects rendering
    }

    void DeferredPipeline::renderPostProcess()
    {
        // TODO: Implement post processing
    }

    void DeferredPipeline::renderUI()
    {
        // TODO: Implement UI rendering
    }
} // namespace RealmEngine