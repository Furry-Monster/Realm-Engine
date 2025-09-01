#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cmath>
#include <cstring>

#include "camera.h"
#include "model.h"
#include "shader.h"

class Engine
{
public:
    void boot();
    void run();
    void terminate();

protected:
    void tick();
    void logicalTick() const;
    void renderTick();

private:
    float m_delta_time {0.0f};
    float m_last_frame {0.0f};

    // temp var , removed latter(added to rendering system)
    Camera* m_camera {nullptr};
    Model*  m_model {nullptr};
    Shader* m_shader {nullptr};

    void drawDebugUI();
};