#include "gbuffer_pass.h"
#include "global.h"
#include "logger.h"
#include "render/framebuffer.h"
#include "render/state.h"
#include "resource/model.h"
#include "resource/shader.h"

namespace RealmEngine
{
    GBufferPass::GBufferPass(FramebufferManager* fb_mgr, StateManager* state_mgr) :
        m_framebuffer_mgr(fb_mgr), m_state_mgr(state_mgr)
    {
        m_shader = std::make_shared<Shader>("../shader/gbuffer.vert", "../shader/gbuffer.frag");
    }

    bool GBufferPass::prepare()
    {
        if (!m_shader || !m_framebuffer_mgr || !m_state_mgr)
        {
            LOG_ERROR("GBufferPass not properly initialized");
            return false;
        }

        m_framebuffer_mgr->bindFrameBuffer(FramebufferType::GBuffer);
        m_framebuffer_mgr->clearFrameBuffer(FramebufferType::GBuffer);

        StateManager::State gbuffer_state;
        gbuffer_state.enable_depth_test = true;
        gbuffer_state.depth_func        = GL_LESS;
        gbuffer_state.enable_culling    = true;
        gbuffer_state.cull_face         = GL_BACK;
        gbuffer_state.blending          = false;

        m_state_mgr->pushState(gbuffer_state);

        m_shader->use();
        m_shader->setMat4("view", m_view_matrix);
        m_shader->setMat4("projection", m_projection_matrix);

        return true;
    }

    void GBufferPass::draw()
    {
        if (!prepare())
            return;

        for (const auto& obj : m_render_objects)
        {
            renderObject(obj);
        }

        clean();
    }

    void GBufferPass::clean()
    {
        m_state_mgr->popState();
        m_state_mgr->unbindVAO();
    }

    void GBufferPass::addRenderObject(const RenderObject& obj) { m_render_objects.push_back(obj); }

    void GBufferPass::clearRenderObjects() { m_render_objects.clear(); }

    void GBufferPass::renderObject(const RenderObject& obj)
    {
        if (!obj.model)
            return;

        m_shader->setMat4("model", obj.model_matrix);

        glm::mat4 prev_mvp = m_prev_vp_matrix * obj.prev_model_matrix;
        m_shader->setMat4("prevMVP", prev_mvp);

        m_shader->setFloat("metallic", 0.0f);
        m_shader->setFloat("roughness", 0.5f);
        m_shader->setInt("shadingModel", 0);

        obj.model->draw(m_shader->getShaderProgram());
    }
} // namespace RealmEngine