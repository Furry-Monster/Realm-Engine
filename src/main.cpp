#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "engine.h"
#include <cmath>
#include <cstring>

int main(int argc, const char **argv) {
  Engine *engine = new Engine();
  engine->boot();
  engine->run();
  engine->terminate();
  return 0;
}