#include "engine_context.h"
#include "logger.h"
#include "window.h"
#include <memory>

void Context::create() {
  m_logger = std::make_shared<Logger>();
  // initialize window system
  m_window = std::make_shared<Window>(640, 480, "RealmEngine");
  if (!m_window->initialize()) {
    m_window.reset();
    return;
  }
}

void Context::destroy() { m_logger.reset(); }

Context g_context;
