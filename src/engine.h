#pragma once

#include "camera.h"
#include "model.h"
#include "shader.h"
#include "window.h"

class Engine {
public:
  void boot();
  void run();
  void tick();
  void terminate();

private:
  Window *m_window{nullptr};
  Camera *m_camera{nullptr};
  float m_delta_time{0.0f};
  float m_last_frame{0.0f};

  // temp var
  Model *m_model{nullptr};
  Shader *m_shader{nullptr};

  void logicalTick();
  void renderTick();

  void drawDebugUI();
};