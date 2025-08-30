#pragma once

#include "camera.h"
#include "model.h"
#include "shader.h"
#include "window.h"

class Engine {
public:
  void boot();
  void run();
  void terminate();

protected:
  void tick();
  void logicalTick();
  void renderTick();

private:
  Window *m_window{nullptr};
  Camera *m_camera{nullptr};
  float m_delta_time{0.0f};
  float m_last_frame{0.0f};

  // temp var , removed latter
  Model *m_model{nullptr};
  Shader *m_shader{nullptr};

  void drawDebugUI();
};