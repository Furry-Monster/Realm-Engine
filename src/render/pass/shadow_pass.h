#pragma once

#include "render/pass.h"
#include "render/shader.h"
#include <memory>

namespace RealmEngine
{
    class ShadownPass : public RenderPass
    {
    public:
        bool prepare() override
        {
            m_shader = std::make_unique<Shader>("../shader/shadow.vert", "../shader/shadow.frag");
            return m_shader == nullptr;
        }

        void draw() override {}
    };
} // namespace RealmEngine