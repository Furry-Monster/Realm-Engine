#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace RealmEngine
{
    class Model;
    class FramebufferManager;
    class StateManager;
    class GBufferPass;
    class LightingPass;

    class Pipeline
    {
    public:
        virtual ~Pipeline() = default;

        virtual void initialize() = 0;
        virtual void render()     = 0;
        virtual void terminate()  = 0;
    };

    class ForwardPipeline : public Pipeline
    {
    public:
        void initialize() override;
        void terminate() override;

        void render() override
        {
            renderShadowMaps();
            renderForward();
            renderPostProcess();
            renderUI();
        }

    protected:
        void renderShadowMaps();
        void renderForward();
        void renderPostProcess();
        void renderUI();
    };

    class DeferredPipeline : public Pipeline
    {
    public:
        DeferredPipeline(FramebufferManager* fb_mgr, StateManager* state_mgr);
        ~DeferredPipeline();

        void initialize() override;
        void terminate() override;

        void render() override
        {
            renderShadowMaps();
            renderGBuffer();
            renderLighting();
            renderForwardObjects();
            renderPostProcess();
            renderUI();
        }

        void setCamera(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position);
        void addRenderObject(Model* model, const glm::mat4& model_matrix);
        void addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);
        void addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity);
        void clearLights();
        void clearRenderObjects();

    protected:
        void renderShadowMaps();
        void renderGBuffer();
        void renderLighting();
        void renderForwardObjects();
        void renderPostProcess();
        void renderUI();

    private:
        FramebufferManager* m_framebuffer_mgr;
        StateManager*       m_state_mgr;

        std::unique_ptr<GBufferPass>  m_gbuffer_pass;
        std::unique_ptr<LightingPass> m_lighting_pass;

        glm::mat4 m_view_matrix {1.0f};
        glm::mat4 m_projection_matrix {1.0f};
        glm::mat4 m_prev_view_projection {1.0f};
        glm::vec3 m_camera_position {0.0f};
    };
} // namespace RealmEngine