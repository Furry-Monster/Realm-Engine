#pragma once

#include "resource/shader.h"

#include <memory>

namespace RealmEngine
{
    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;

        virtual bool prepare() = 0;
        virtual void draw()    = 0;
        virtual void clean()   = 0;

    protected:
        std::shared_ptr<Shader> m_shader;
    };
} // namespace RealmEngine