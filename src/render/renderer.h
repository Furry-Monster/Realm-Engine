#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "render/pipeline.h"
#include <memory>

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

        void setViewport(int width, int height) { glViewport(0, 0, width, height); }
        void setLighting(Shader*          shader,
                         const glm::vec3& light_pos,
                         const glm::vec3& light_color,
                         const glm::vec3& view_pos);
        void setRenderMode(RenderMode mode) { m_mode = mode; };

    private:
        std::unique_ptr<Pipeline> m_pipeline;

        bool m_initialized = false;

        RenderMode m_mode {RenderMode::Defferd};
        State      m_state {GL_FILL, 1.0f, 1.0f, true, false, GL_BACK, 1, true};
    };
} // namespace RealmEngine