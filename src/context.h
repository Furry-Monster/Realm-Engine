#pragma once

#include <memory>

#include "logger.h"
#include "input.h"
#include "render/window.h"

namespace RealmEngine
{
    class Logger;
    class Input;
    class Window;

    class Context
    {
    public:
        void create();
        void destroy();

        std::shared_ptr<Logger> m_logger;
        std::shared_ptr<Input>  m_input;
        std::shared_ptr<Window> m_window;
    };

    extern Context g_context;
} // namespace RealmEngine