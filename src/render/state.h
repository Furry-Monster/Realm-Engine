#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <array>
#include <stack>

namespace RealmEngine
{
    class StateManager
    {
    public:
        struct State
        {
            // Rasterize
            GLenum polygon_mode = GL_FILL;
            float  line_width   = 1.0f;
            float  point_size   = 1.0f;
            // Depth Testing
            bool   enable_depth_test = true;
            GLenum depth_func        = GL_LESS;
            // Blending
            bool   blending  = false;
            GLenum src_blend = GL_SRC_ALPHA;
            GLenum dst_blend = GL_ONE_MINUS_SRC_ALPHA;
            // Culling
            bool   enable_culling = false;
            GLenum cull_face      = GL_BACK;
            GLenum front_face     = GL_CCW;
            // Misc
            int  v_sync_interval = 1;
            bool enable_msaa     = true;
        };

        void initialize();
        void terminate();

        GLuint getCurrentVAO() const { return m_binding.bound_vao; }
        bool   isUBOBound(GLuint binding_point) const { return m_binding.bound_ubos[binding_point] != 0; }
        bool   isTextureBound(GLuint unit) const { return m_binding.bound_textures[unit] != 0; }

        void pushState(const State& state);
        void popState();

        void bindTexture(int unit, GLuint texture, GLenum target = GL_TEXTURE_2D);
        void bindUBO(GLuint ubo, int binding_point);
        void bindVAO(GLuint vao);

        void unbindAllTexture();
        void unbindAllUBOs();
        void unbindVAO();

    private:
        struct Binding
        {
            GLuint                 bound_vao {0};
            std::array<GLuint, 32> bound_textures {0};
            std::array<GLuint, 16> bound_ubos {0};
        } m_binding;

        struct Limit
        {
            int texture_unit_max {0};
            int uniform_buffer_max {0};
        } m_limit;

        std::stack<State> m_state_stack;
        State             m_current_state;

        void applyState(const State& state);
        void applyRasterState(const State& state);
        void applyDepthState(const State& state);
        void applyBlendState(const State& state);
        void applyCullState(const State& state);
        void applyMiscState(const State& state);
    };
} // namespace RealmEngine