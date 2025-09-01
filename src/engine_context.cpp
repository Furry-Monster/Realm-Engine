#include "engine_context.h"

namespace RealmEngine
{
    void Context::create()
    {
        // initialize logger system
        m_logger = std::make_shared<Logger>();

        // initialize window system
        m_window = std::make_shared<Window>(640, 480, "RealmEngine");
        if (!m_window->initialize())
        {
            m_window.reset();
            return;
        }

        // initialize input system
        m_input = std::make_shared<Input>();
        m_input->initialize();
    }

    void Context::destroy()
    {
        m_input.reset();
        m_window.reset();
        m_logger.reset();
    }

    Context g_context;
} // namespace RealmEngine