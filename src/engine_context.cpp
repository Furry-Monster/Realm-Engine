#include "engine_context.h"
#include "logger.h"
#include <memory>

void Context::create() { m_logger = std::make_shared<Logger>(); }

void Context::destroy() { m_logger.reset(); }

Context g_context;
