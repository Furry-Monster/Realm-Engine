#pragma once

#include <memory>

#include "input.h"
#include "logger.h"
#include "render/renderer.h"
#include "render/window.h"
#include "resource/resource.h"

namespace RealmEngine
{
    class Logger;
    class Input;
    class Window;
    class Renderer;
    class ResourceManager;

    class Context
    {
    public:
        void create();
        void destroy();

        std::shared_ptr<Logger>   m_logger;
        std::shared_ptr<Window>   m_window;
        std::shared_ptr<Resource> m_resource;
        std::shared_ptr<Renderer> m_renderer;
        std::shared_ptr<Input>    m_input;
    };

    extern Context g_context;
} // namespace RealmEngine