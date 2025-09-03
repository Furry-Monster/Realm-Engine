#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "render/framebuffer.h"
#include "render/pipeline.h"
#include "render/state.h"

namespace RealmEngine
{
    class Model;
    class Shader;
    class Camera;

    enum class RenderMode : uint8_t
    {
        Forward, // 前向渲染
        Defferd, // 延迟渲染
    };

    class Renderer
    {
    public:
        void initialize();
        void terminate();

        void setRenderMode(RenderMode mode) { m_mode = mode; };

        void addRenderObject(Model* model, const glm::mat4& model_matrix);
        void addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity = 1.0f);
        void addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity = 1.0f);
        void setMainCamera(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position);

        void renderFrame();

    private:
        std::unique_ptr<Pipeline>           m_pipeline;
        std::unique_ptr<StateManager>       m_state_mgr;
        std::unique_ptr<FramebufferManager> m_framebuffer_mgr;
        bool                                m_initialized = false;
        RenderMode                          m_mode {RenderMode::Defferd};

        void beginFrame() const;
        void endFrame() const;
    };
} // namespace RealmEngine