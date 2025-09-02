#pragma once

namespace RealmEngine
{
    class RenderPass
    {
    public:
        virtual void prepare() = 0;
        virtual void draw()    = 0;
    };
} // namespace RealmEngine