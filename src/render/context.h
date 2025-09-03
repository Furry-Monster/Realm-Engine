#pragma once

#include "render/framebuffer.h"
#include "render/pipeline.h"
#include "render/state.h"

#include <memory>

namespace RealmEngine
{
    class Pipeline;
    class StateManager;
    class FramebufferManager;

    class RenderContext
    {
    public:
    private:
        std::unique_ptr<Pipeline>           m_pipeline;
        std::unique_ptr<StateManager>       m_state_mgr;
        std::unique_ptr<FramebufferManager> m_framebuffer_mgr;
    };
} // namespace RealmEngine