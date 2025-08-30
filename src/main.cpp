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

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "camera.h"
#include "model.h"

#define STR_EQUAL 0
#define cStringIsNullOrEmpty(str)                                              \
  (((str) == nullptr || std::strcmp((str), "") == STR_EQUAL))

static struct GLInfo {
  char *version{nullptr};
  int window_width{0};
  int window_height{0};
  int framebuffer_width{0};
  int framebuffer_height{0};
  char *title{nullptr};
  bool should_close{GLFW_FALSE};
} g_info{nullptr, 640, 480, 0, 0, (char *)"LearnOpenGL", GLFW_FALSE};

static struct RasterState {
  GLenum polygon_mode{0};
  bool enable_depth_test{false};
  bool enable_culling{false};
  GLenum cull_face{0};
  bool enable_wireframe{false};
  float line_width{0};
  float point_size{0};
} g_raster_state{GL_FILL, true, false, GL_BACK, false, 1.0f, 1.0f};

static GLFWwindow *g_window{nullptr};
static Camera g_camera(glm::vec3(0.0f, 0.0f, 5.0f));
static Model *g_model{nullptr};
static float g_delta_time{0.0f};
static float g_last_frame{0.0f};

static float g_lastX = 320.0f;
static float g_lastY = 240.0f;
static bool g_firstMouse = true;
static bool g_mouseCaptured = true;

static void processMovementInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    g_camera.processKeyboard(FORWARD, g_delta_time);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    g_camera.processKeyboard(BACKWARD, g_delta_time);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    g_camera.processKeyboard(LEFT, g_delta_time);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    g_camera.processKeyboard(RIGHT, g_delta_time);
}

static void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
  if (!g_mouseCaptured)
    return;

  if (g_firstMouse) {
    g_lastX = xpos;
    g_lastY = ypos;
    g_firstMouse = false;
  }

  float xoffset = xpos - g_lastX;
  float yoffset = g_lastY - ypos;

  g_lastX = xpos;
  g_lastY = ypos;

  g_camera.processMouseMovement(xoffset, yoffset);
}

static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  g_camera.processMouseScroll(yoffset);
}

static void keyboardCallBack(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
  // esc for closing window
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    g_info.should_close = GLFW_TRUE;
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  // tab for toggling mouse capture
  if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    g_mouseCaptured = !g_mouseCaptured;
    if (g_mouseCaptured) {
      glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    g_firstMouse = true;
  }
}

static void windowSizeCallback(GLFWwindow *window, int width, int height) {
  g_info.window_width = width;
  g_info.window_height = height;
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  g_info.framebuffer_width = width;
  g_info.framebuffer_height = height;

  glViewport(0, 0, g_info.framebuffer_width, g_info.framebuffer_height);
}

inline static std::string loadShaderFrom(std::string path) {
  std::ifstream shaderFile(path);
  std::stringstream shader;
  shader << shaderFile.rdbuf();
  shaderFile.close();
  return shader.str();
}

inline static bool initialize() {
  // init glfw
  if (!glfwInit()) {
    return 0;
  }

  // context creating
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  g_window = glfwCreateWindow(g_info.window_width, g_info.window_height,
                              g_info.title, nullptr, nullptr);
  if (!g_window) {
    glfwTerminate();
    return 0;
  }
  glfwMakeContextCurrent(g_window);

  // bind events
  glfwSetKeyCallback(g_window, keyboardCallBack);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);
  glfwSetCursorPosCallback(g_window, mouseCallback);
  glfwSetScrollCallback(g_window, scrollCallback);

  // capture mouse cursor
  glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // init glad
  if (!gladLoadGL(glfwGetProcAddress)) {
    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 0;
  }
  glfwSwapInterval(1);

  // load info from OpenGL and do some initializations
  glfwGetFramebufferSize(g_window, &g_info.framebuffer_width,
                         &g_info.framebuffer_height);
  glViewport(0, 0, g_info.framebuffer_width, g_info.framebuffer_height);

  g_info.version = (char *)glfwGetVersionString();
  if (!cStringIsNullOrEmpty(g_info.version)) {
    std::string windowTitle =
        std::string(g_info.title) + " - " + std::string(g_info.version);
    glfwSetWindowTitle(g_window, windowTitle.c_str());
  }

  // init imgui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui_ImplGlfw_InitForOpenGL(g_window, GLFW_TRUE);
  ImGui_ImplOpenGL3_Init();

  return 1;
}

static GLuint loadAndLinkShader(char *vert_path = nullptr,
                                char *frag_path = nullptr) {
  GLint success{0};
  char log[512];

  // load vert shader
  std::string vert_shader_src_raw = loadShaderFrom(
      cStringIsNullOrEmpty(vert_path) ? "../shader/test.vert" : vert_path);
  const char *vert_shader_src = vert_shader_src_raw.c_str();
  GLuint vert_shader{0};
  vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert_shader_src, nullptr);
  glCompileShader(vert_shader);
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vert_shader, 512, nullptr, log);
    std::cerr << "ERROR\n" << log << std::endl;
  }

  // load frag shader
  std::string frag_shader_src_raw = loadShaderFrom(
      cStringIsNullOrEmpty(frag_path) ? "../shader/test.frag" : frag_path);
  const char *frag_shader_src = frag_shader_src_raw.c_str();
  GLuint frag_shader{0};
  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_shader_src, nullptr);
  glCompileShader(frag_shader);
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(frag_shader, 512, nullptr, log);
    std::cerr << "ERROR\n" << log << std::endl;
  }

  // link shader
  GLuint shader_prog{0};
  shader_prog = glCreateProgram();
  glAttachShader(shader_prog, vert_shader);
  glAttachShader(shader_prog, frag_shader);
  glLinkProgram(shader_prog);
  glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_prog, 512, nullptr, log);
    std::cerr << "ERROR\n" << log << std::endl;
  }
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  return shader_prog;
}

static void applyRasterizationState() {
  if (g_raster_state.enable_depth_test) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  if (g_raster_state.enable_culling) {
    glEnable(GL_CULL_FACE);
    glCullFace(g_raster_state.cull_face);
    glFrontFace(GL_CCW);
  } else {
    glDisable(GL_CULL_FACE);
  }

  glPolygonMode(GL_FRONT_AND_BACK, g_raster_state.polygon_mode);
  glLineWidth(g_raster_state.line_width);
  glPointSize(g_raster_state.point_size);
}

static void drawDebugInfoWidget() {
  ImGui::Begin("Debug Info");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("Window size: %d x %d", g_info.window_width,
              g_info.window_height);
  ImGui::Text("Framebuffer size: %d x %d", g_info.framebuffer_width,
              g_info.framebuffer_height);
  ImGui::Text("OpenGL Version: %s", g_info.version);

  ImGui::Separator();
  ImGui::Text("Rasterization Controls:");

  const char *polygonModes[] = {"Fill", "Line", "Point"};
  int currentMode = (g_raster_state.polygon_mode == GL_FILL)   ? 0
                    : (g_raster_state.polygon_mode == GL_LINE) ? 1
                                                               : 2;
  if (ImGui::Combo("Polygon Mode", &currentMode, polygonModes, 3)) {
    g_raster_state.polygon_mode = (currentMode == 0)   ? GL_FILL
                                  : (currentMode == 1) ? GL_LINE
                                                       : GL_POINT;
  }

  ImGui::Checkbox("Enable Depth Test", &g_raster_state.enable_depth_test);
  ImGui::Checkbox("Enable Face Culling", &g_raster_state.enable_culling);

  if (g_raster_state.enable_culling) {
    const char *cullModes[] = {"Back", "Front", "Front and Back"};
    int currentCull = (g_raster_state.cull_face == GL_BACK)    ? 0
                      : (g_raster_state.cull_face == GL_FRONT) ? 1
                                                               : 2;
    if (ImGui::Combo("Cull Face", &currentCull, cullModes, 3)) {
      g_raster_state.cull_face = (currentCull == 0)   ? GL_BACK
                                 : (currentCull == 1) ? GL_FRONT
                                                      : GL_FRONT_AND_BACK;
    }
  }

  ImGui::SliderFloat("Line Width", &g_raster_state.line_width, 0.1f, 10.0f);
  ImGui::SliderFloat("Point Size", &g_raster_state.point_size, 1.0f, 20.0f);

  ImGui::Separator();
  ImGui::Text("Camera Controls:");
  ImGui::Text("Position: (%.2f, %.2f, %.2f)", g_camera.m_position.x,
              g_camera.m_position.y, g_camera.m_position.z);
  ImGui::Text("Yaw: %.2f, Pitch: %.2f", g_camera.m_yaw, g_camera.m_pitch);
  ImGui::SliderFloat("FOV", &g_camera.m_zoom, 1.0f, 45.0f);
  ImGui::SliderFloat("Speed", &g_camera.m_movement_speed, 0.1f, 10.0f);
  ImGui::Text("Mouse: %s (Tab to toggle)",
              g_mouseCaptured ? "Captured" : "Free");
  ImGui::Text("Controls: WASD to move, Mouse to look, Scroll to zoom");

  static char modelPath[256] = "../assets/model/";
  ImGui::InputText("Model Path", modelPath, sizeof(modelPath));
  if (ImGui::Button("Load Model")) {
    if (g_model)
      delete g_model;
    try {
      g_model = new Model(modelPath);
    } catch (...) {
      g_model = nullptr;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear Model")) {
    if (g_model) {
      delete g_model;
      g_model = nullptr;
    }
  }

  ImGui::Separator();
  ImGui::Text("Model Info:");
  if (g_model) {
    ImGui::Text("Meshes: %zu", g_model->m_meshes.size());
    ImGui::Text("Textures: %zu", g_model->m_textures.size());
  } else {
    ImGui::Text("No model loaded");
  }
  ImGui::End();
}

inline static void terminate() {
  // clean imgui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // terminate openGL
  glfwDestroyWindow(g_window);
  glfwTerminate();
}

int main(int argc, const char **argv) {
  // initalize glfw , glad and imgui
  if (!initialize()) {
    std::cerr << "ERROR\nInitialize failed..." << std::endl;
    return 1;
  }

  // load shader
  GLuint shader_prog = loadAndLinkShader();

  // load model
  g_model = new Model("../assets/model/backpack/backpack.obj");

  // enable depth testing
  glEnable(GL_DEPTH_TEST);

  // main loop
  while (!g_info.should_close && !glfwWindowShouldClose(g_window)) {
    // update timing
    float currentFrame = glfwGetTime();
    g_delta_time = currentFrame - g_last_frame;
    g_last_frame = currentFrame;

    // update imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    drawDebugInfoWidget();

    // event handler...
    glfwPollEvents();
    processMovementInput(g_window);

    // rendering...
    // background
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // apply rasterization state
    applyRasterizationState();

    glUseProgram(shader_prog);

    // set matrices
    glm::mat4 projection = g_camera.getProjectionMatrix(
        (float)g_info.framebuffer_width / (float)g_info.framebuffer_height);
    glm::mat4 view = g_camera.getViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader_prog, "projection"), 1,
                       GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shader_prog, "view"), 1, GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader_prog, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));

    // set lighting uniforms
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(shader_prog, "lightPos"), 1,
                 glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader_prog, "lightColor"), 1,
                 glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shader_prog, "viewPos"), 1,
                 glm::value_ptr(g_camera.m_position));

    // render model or fallback triangle
    if (g_model) {
      g_model->draw(shader_prog);
    }

    // ui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // buffering...
    glfwSwapBuffers(g_window);
  }

  // clean assets
  if (g_model) {
    delete g_model;
  }
  glDeleteProgram(shader_prog);

  terminate();
  return 0;
}