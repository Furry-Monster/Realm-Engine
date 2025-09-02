#include "state.h"
#include "context.h"
#include "logger.h"

namespace RealmEngine
{
    void StateManager::initialize()
    {
        m_current_state = State {};
        applyState(m_current_state);
        LOG_INFO("StateManager initialized");
    }

    void StateManager::terminate()
    {
        while (!m_state_stack.empty())
            m_state_stack.pop();

        unbindAllTexture();

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

    void StateManager::bindTexture(int unit, GLuint texture, GLenum target)
    {
        if (unit >= 0 && unit < static_cast<int>(m_binding_cache.bound_textures.size()))
        {
            if (m_binding_cache.bound_textures[unit] != texture)
            {
                glActiveTexture(GL_TEXTURE0 + unit);
                glBindTexture(target, texture);
                m_binding_cache.bound_textures[unit] = texture;
            }
        }
    }

    void StateManager::bindUBO(GLuint buffer, int binding_point)
    {
        if (binding_point >= 0 && binding_point < static_cast<int>(m_binding_cache.bound_ubos.size()))
        {
            if (m_binding_cache.bound_ubos[binding_point] != buffer)
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, buffer);
                m_binding_cache.bound_ubos[binding_point] = buffer;
            }
        }
    }

    void StateManager::bindVAO(GLuint vao)
    {
        if (m_binding_cache.current_vao != vao)
        {
            glBindVertexArray(vao);
            m_binding_cache.current_vao = vao;
        }
    }

    void StateManager::unbindAllTexture()
    {
        for (int i = 0; i < static_cast<int>(m_binding_cache.bound_textures.size()); ++i)
        {
            if (m_binding_cache.bound_textures[i] != 0)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
                m_binding_cache.bound_textures[i] = 0;
            }
        }
    }

    void StateManager::unbindAllUBOs()
    {
        for (int i = 0; i < static_cast<int>(m_binding_cache.bound_ubos.size()); ++i)
        {
            if (m_binding_cache.bound_ubos[i] != 0)
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, i, 0);
                m_binding_cache.bound_ubos[i] = 0;
            }
        }
    }

    void StateManager::unbindVAO()
    {
        if (m_binding_cache.current_vao != 0)
        {
            glBindVertexArray(0);
            m_binding_cache.current_vao = 0;
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