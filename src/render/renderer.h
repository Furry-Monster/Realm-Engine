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
        struct State
        {
            GLenum polygon_mode      = GL_FILL;
            float  line_width        = 1.0f;
            float  point_size        = 1.0f;
            bool   enable_depth_test = true;
            bool   enable_culling    = false;
            GLenum cull_face         = GL_BACK;
            int    v_sync_interval   = 1;
            bool   enable_msaa       = true;
        };

        void initialize();
        void terminate();

        void beginFrame() const;
        void render(Model*           model,
                    Shader*          shader,
                    const glm::mat4& model_matrix,
                    const glm::mat4& view_matrix,
                    const glm::mat4& projection_matrix);
        void clear(const glm::vec4& color = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
        void endFrame() const;

        void   setRendererState(const State& state) { m_state = state; }
        void   applyRendererState() const;
        State& getRendererState() { return m_state; }

        void setLighting(Shader*          shader,
                         const glm::vec3& light_pos,
                         const glm::vec3& light_color,
                         const glm::vec3& view_pos);
        void setRenderMode(RenderMode mode) { m_mode = mode; };

        // Deferred rendering interface
        void addRenderObject(Model* model, const glm::mat4& model_matrix);
        void addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity = 1.0f);
        void addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity = 1.0f);
        void setCamera(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& position);
        void renderFrame();

    private:
        std::unique_ptr<Pipeline>           m_pipeline;
        std::unique_ptr<StateManager>       m_state_mgr;
        std::unique_ptr<FramebufferManager> m_framebuffer_mgr;

        bool m_initialized = false;

        RenderMode m_mode {RenderMode::Defferd};
        State      m_state {GL_FILL, 1.0f, 1.0f, true, false, GL_BACK, 1, true};
    };
} // namespace RealmEngine