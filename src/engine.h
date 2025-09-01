#pragma once

#include "camera.h"
#include "model.h"
#include "shader.h"

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
  float m_delta_time{0.0f};
  float m_last_frame{0.0f};

  // temp var , removed latter(added to rendering system)
  Camera *m_camera{nullptr};
  Model *m_model{nullptr};
  Shader *m_shader{nullptr};

  void drawDebugUI();
};