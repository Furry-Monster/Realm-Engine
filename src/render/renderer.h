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

    static struct RasterizationState
    {
        GLenum polygon_mode      = GL_FILL;
        bool   enable_depth_test = true;
        bool   enable_culling    = false;
        GLenum cull_face         = GL_BACK;
        float  line_width        = 1.0f;
        float  point_size        = 1.0f;
    } m_raster_state = {GL_FILL, true, false, GL_BACK, 1.0f, 1.0f};

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void initialize();
        void terminate();

        void beginFrame() const;
        void endFrame() const;

        void clear(const glm::vec4& color = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));
        void setViewport(int width, int height) { glViewport(0, 0, width, height); }

        void setRasterizationState(const RasterizationState& state) { m_raster_state = state; }
        void applyRasterizationState() const;

        void renderModel(Model*           model,
                         Shader*          shader,
                         const glm::mat4& model_matrix,
                         const glm::mat4& view_matrix,
                         const glm::mat4& projection_matrix);

        void setLighting(Shader*          shader,
                         const glm::vec3& light_pos,
                         const glm::vec3& light_color,
                         const glm::vec3& view_pos);

        RasterizationState& getRasterizationState() { return m_raster_state; }

    private:
        RasterizationState m_raster_state;
        bool               m_initialized = false;
    };
} // namespace RealmEngine