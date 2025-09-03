#include "state.h"
#include "context.h"
#include "logger.h"

namespace RealmEngine
{
    void StateManager::initialize()
    {
        // store default state
        m_current_state = State {};
        applyState(m_current_state);

        // get limits for current OpenGL device
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &m_limit.texture_unit_max);
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &m_limit.uniform_buffer_max);

        LOG_INFO("StateManager initialized");
    }

    void StateManager::terminate()
    {
        while (!m_state_stack.empty())
            m_state_stack.pop();

        unbindAllTexture();
        unbindAllUBOs();
        unbindVAO();

        LOG_INFO("StateManager terminated");
    }

    void StateManager::pushState(const State& state)
    {
        m_state_stack.push(m_current_state);
        m_current_state = state;
        applyState(m_current_state);
    }

    void StateManager::popState()
    {
        if (!m_state_stack.empty())
        {
            m_current_state = m_state_stack.top();
            m_state_stack.pop();
            applyState(m_current_state);
        }
    }

    void StateManager::applyState(const State& state)
    {
        applyRasterState(state);
        applyDepthState(state);
        applyBlendState(state);
        applyCullState(state);
        applyMiscState(state);
    }

    /**
     * @brief bind GPU texture ptr(GLuint) on texture channel(int)
     *
     * @param channel texture channel in current framebuffer
     * @param texture GPU allocated texture ptr
     * @param target texture type , 2D or 1D or else
     */
    void StateManager::bindTexture(int unit, GLuint texture, GLenum target)
    {
        if (unit > m_limit.texture_unit_max - 1)
        {
            LOG_ERROR("Texture unit number out of range in current device!!!");
            return;
        }

        if (unit >= 0 && unit < static_cast<int>(m_binding.bound_textures.size()))
        {
            if (m_binding.bound_textures[unit] != texture)
            {
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(target, texture);
                m_binding.bound_textures[unit] = texture;
            }
        }
    }

    /**
     * @brief bind allocated ubo on shader binding point
     *
     * @param ubo created & allocated ubo ptr
     * @param binding_point shader binding point pos
     */
    void StateManager::bindUBO(GLuint ubo, int binding_point)
    {
        if (binding_point > m_limit.uniform_buffer_max - 1)
        {
            LOG_ERROR("Binding point number out of range in current device!!!");
            return;
        }

        if (binding_point >= 0 && binding_point < static_cast<int>(m_binding.bound_ubos.size()))
        {
            if (m_binding.bound_ubos[binding_point] != ubo)
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
                m_binding.bound_ubos[binding_point] = ubo;
            }
        }
    }

    /**
     * @brief use target vao
     *
     * @param vao
     */
    void StateManager::bindVAO(GLuint vao)
    {
        if (m_binding.bound_vao != vao)
        {
            glBindVertexArray(vao);
            m_binding.bound_vao = vao;
        }
    }

    void StateManager::unbindAllTexture()
    {
        for (int i = 0; i < static_cast<int>(m_binding.bound_textures.size()); ++i)
        {
            if (m_binding.bound_textures[i] != 0)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
                m_binding.bound_textures[i] = 0;
            }
        }
    }

    void StateManager::unbindAllUBOs()
    {
        for (int i = 0; i < static_cast<int>(m_binding.bound_ubos.size()); ++i)
        {
            if (m_binding.bound_ubos[i] != 0)
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, i, 0);
                m_binding.bound_ubos[i] = 0;
            }
        }
    }

    void StateManager::unbindVAO()
    {
        if (m_binding.bound_vao != 0)
        {
            glBindVertexArray(0);
            m_binding.bound_vao = 0;
        }
    }

    void StateManager::applyRasterState(const State& state)
    {
        glPolygonMode(GL_FRONT_AND_BACK, state.polygon_mode);
        glLineWidth(state.line_width);
        glPointSize(state.point_size);
    }

    void StateManager::applyDepthState(const State& state)
    {
        if (state.enable_depth_test)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(state.depth_func);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void StateManager::applyBlendState(const State& state)
    {
        if (state.blending)
        {
            glEnable(GL_BLEND);
            glBlendFunc(state.src_blend, state.dst_blend);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    void StateManager::applyCullState(const State& state)
    {
        if (state.enable_culling)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(state.cull_face);
            glFrontFace(state.front_face);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }

    void StateManager::applyMiscState(const State& state)
    {
        glfwSwapInterval(state.v_sync_interval);

        if (state.enable_msaa)
        {
            glEnable(GL_MULTISAMPLE);
        }
        else
        {
            glDisable(GL_MULTISAMPLE);
        }
    }
} // namespace RealmEngine