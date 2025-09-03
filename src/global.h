#pragma once

#include <memory>

#include "input.h"
#include "logger.h"
#include "render/renderer.h"
#include "render/window.h"

namespace RealmEngine
{
    class Logger;
    class Input;
    class Window;
    class Renderer;

    class Context
    {
    public:
        void create();
        void destroy();

        std::shared_ptr<Logger>   m_logger;
        std::shared_ptr<Input>    m_input;
        std::shared_ptr<Window>   m_window;
        std::shared_ptr<Renderer> m_renderer;
    };

    extern Context g_context;
} // namespace RealmEngine