#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "render/pass.h"
#include <glm/glm.hpp>
#include <vector>

namespace RealmEngine
{
    class FramebufferManager;
    class StateManager;

    struct DirectionalLight
    {
        glm::vec3 direction {0.0f, -1.0f, 0.0f};
        glm::vec3 color {1.0f};
        float     intensity = 1.0f;
    };

    struct PointLight
    {
        glm::vec3 position {0.0f};
        glm::vec3 color {1.0f};
        float     intensity = 1.0f;
        float     constant  = 1.0f;
        float     linear    = 0.09f;
        float     quadratic = 0.032f;
    };

    class LightingPass : public RenderPass
    {
    public:
        LightingPass(FramebufferManager* fb_mgr, StateManager* state_mgr);

        bool prepare() override;
        void draw() override;
        void clean() override;

        void setViewPosition(const glm::vec3& pos) { m_view_pos = pos; }
        void setInvViewProjectionMatrix(const glm::mat4& inv_vp) { m_inv_view_projection = inv_vp; }

        void addDirectionalLight(const DirectionalLight& light);
        void addPointLight(const PointLight& light);
        void clearLights();

    private:
        FramebufferManager* m_framebuffer_mgr;
        StateManager*       m_state_mgr;

        std::vector<DirectionalLight> m_dir_lights;
        std::vector<PointLight>       m_point_lights;

        glm::vec3 m_view_pos {0.0f};
        glm::mat4 m_inv_view_projection {1.0f};

        GLuint m_quad_vao = 0;
        GLuint m_quad_vbo = 0;

        void createFullscreenQuad();
        void setupLightUniforms();
        void renderFullscreenQuad();
    };
} // namespace RealmEngine