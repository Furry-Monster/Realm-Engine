#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace RealmEngine
{
    class Model;
    class Shader;
    class Camera;

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
        void endFrame() const;

        void clear(const glm::vec4& color = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
        void setViewport(int width, int height) { glViewport(0, 0, width, height); }

        void   setRendererState(const State& state) { m_renderer_state = state; }
        void   applyRendererState() const;
        State& getRendererState() { return m_renderer_state; }

        void renderModel(Model*           model,
                         Shader*          shader,
                         const glm::mat4& model_matrix,
                         const glm::mat4& view_matrix,
                         const glm::mat4& projection_matrix);

        void setLighting(Shader*          shader,
                         const glm::vec3& light_pos,
                         const glm::vec3& light_color,
                         const glm::vec3& view_pos);

    private:
        State m_renderer_state {GL_FILL, 1.0f, 1.0f, true, false, GL_BACK, 1, true};
        bool  m_initialized = false;
    };
} // namespace RealmEngine