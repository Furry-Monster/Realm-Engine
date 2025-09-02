#include "lighting_pass.h"
#include "context.h"
#include "logger.h"
#include "render/framebuffer.h"
#include "render/shader.h"
#include "render/state.h"

namespace RealmEngine
{
    LightingPass::LightingPass(FramebufferManager* fb_mgr, StateManager* state_mgr) :
        m_framebuffer_mgr(fb_mgr), m_state_mgr(state_mgr)
    {
        m_shader = std::make_shared<Shader>("../shader/lighting.vert", "../shader/lighting.frag");
        createFullscreenQuad();
    }

    bool LightingPass::prepare()
    {
        if (!m_shader || !m_framebuffer_mgr || !m_state_mgr)
        {
            LOG_ERROR("LightingPass not properly initialized");
            return false;
        }

        m_framebuffer_mgr->bindFrameBuffer(FramebufferType::PostProcess_A);
        m_framebuffer_mgr->clearFrameBuffer(FramebufferType::PostProcess_A);

        StateManager::State lighting_state;
        lighting_state.enable_depth_test = false;
        lighting_state.enable_culling    = false;
        lighting_state.blending          = false;

        m_state_mgr->pushState(lighting_state);

        m_shader->use();

        m_framebuffer_mgr->bindAttachment(AttachmentType::GBuffer_Albedo, 0);
        m_framebuffer_mgr->bindAttachment(AttachmentType::GBuffer_Normal, 1);
        m_framebuffer_mgr->bindAttachment(AttachmentType::GBuffer_Motion, 2);
        m_framebuffer_mgr->bindAttachment(AttachmentType::GBuffer_Depth, 3);

        m_shader->setInt("gAlbedoMetallic", 0);
        m_shader->setInt("gNormalRoughness", 1);
        m_shader->setInt("gMotionShadingModel", 2);
        m_shader->setInt("gDepth", 3);

        m_shader->setMat4("invViewProjection", m_inv_view_projection);
        m_shader->setVec3("viewPos", m_view_pos);

        setupLightUniforms();

        return true;
    }

    void LightingPass::draw()
    {
        if (!prepare())
            return;

        renderFullscreenQuad();
        clean();
    }

    void LightingPass::clean()
    {
        m_state_mgr->popState();
        m_state_mgr->unbindVAO();
        m_state_mgr->unbindAllTexture();
    }

    void LightingPass::addDirectionalLight(const DirectionalLight& light) { m_dir_lights.push_back(light); }

    void LightingPass::addPointLight(const PointLight& light) { m_point_lights.push_back(light); }

    void LightingPass::clearLights()
    {
        m_dir_lights.clear();
        m_point_lights.clear();
    }

    void LightingPass::createFullscreenQuad()
    {
        float quad_vertices[] = {// positions   // texCoords
                                 -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

                                 -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

        glGenVertexArrays(1, &m_quad_vao);
        glGenBuffers(1, &m_quad_vbo);

        glBindVertexArray(m_quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>((2 * sizeof(float))));

        glBindVertexArray(0);
    }

    void LightingPass::setupLightUniforms()
    {
        int num_dir_lights   = static_cast<int>(m_dir_lights.size());
        int num_point_lights = static_cast<int>(m_point_lights.size());

        m_shader->setInt("numDirLights", num_dir_lights);
        m_shader->setInt("numPointLights", num_point_lights);

        for (int i = 0; i < num_dir_lights && i < 4; ++i)
        {
            std::string base = "dirLights[" + std::to_string(i) + "]";
            m_shader->setVec3(base + ".direction", m_dir_lights[i].direction);
            m_shader->setVec3(base + ".color", m_dir_lights[i].color);
            m_shader->setFloat(base + ".intensity", m_dir_lights[i].intensity);
        }

        for (int i = 0; i < num_point_lights && i < 32; ++i)
        {
            std::string base = "pointLights[" + std::to_string(i) + "]";
            m_shader->setVec3(base + ".position", m_point_lights[i].position);
            m_shader->setVec3(base + ".color", m_point_lights[i].color);
            m_shader->setFloat(base + ".intensity", m_point_lights[i].intensity);
            m_shader->setFloat(base + ".constant", m_point_lights[i].constant);
            m_shader->setFloat(base + ".linear", m_point_lights[i].linear);
            m_shader->setFloat(base + ".quadratic", m_point_lights[i].quadratic);
        }
    }

    void LightingPass::renderFullscreenQuad()
    {
        m_state_mgr->bindVAO(m_quad_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
} // namespace RealmEngine