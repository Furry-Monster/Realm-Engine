#pragma once

#include <memory>

class logger;
class input;
class window;

class Context {
public:
  void create();
  void destroy();

  std::shared_ptr<logger> m_logger;
  std::shared_ptr<input> m_input;
  std::shared_ptr<window> m_window;
};

extern Context g_context;