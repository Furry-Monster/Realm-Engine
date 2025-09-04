#pragma once

#include "render/pass.h"

#include <glm/glm.hpp>
#include <vector>

namespace RealmEngine
{
    class Model;
    class FramebufferManager;
    class StateManager;

    struct RenderObject
    {
        Model*    model;
        glm::mat4 model_matrix;
        glm::mat4 prev_model_matrix;
    };

    class GBufferPass : public RenderPass
    {
    public:
        GBufferPass(FramebufferManager* fb_mgr, StateManager* state_mgr);

        bool prepare() override;
        void draw() override;
        void clean() override;

        void addRenderObject(const RenderObject& obj);
        void clearRenderObjects();

        void setViewMatrix(const glm::mat4& view) { m_view_matrix = view; }
        void setProjectionMatrix(const glm::mat4& proj) { m_projection_matrix = proj; }
        void setPrevViewProjectionMatrix(const glm::mat4& prev_vp) { m_prev_vp_matrix = prev_vp; }

    private:
        FramebufferManager* m_framebuffer_mgr;
        StateManager*       m_state_mgr;

        std::vector<RenderObject> m_render_objects;

        glm::mat4 m_view_matrix {1.0f};
        glm::mat4 m_projection_matrix {1.0f};
        glm::mat4 m_prev_vp_matrix {1.0f};

        void renderObject(const RenderObject& obj);
    };
} // namespace RealmEngine