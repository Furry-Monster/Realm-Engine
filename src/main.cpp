#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#define STR_EQUAL 0
#define cStringIsNullOrEmpty(str)                                              \
  (((str) == nullptr || std::strcmp((str), "") == STR_EQUAL))

#define glUnbindVertexArray() (glBindVertexArray(0))

static unsigned int indices[] = {
    0, 1, 2, // 第一个三角形
};

static float vertices[] = {
    // 位置              // 颜色
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 右下
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 左下
    0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // 顶部
};

static struct GLInfo {
  char *version{nullptr};
  int window_width{0};
  int window_height{0};
  int framebuffer_width{0};
  int framebuffer_height{0};
  char *title{nullptr};
  bool should_close{GLFW_FALSE};
} g_info{nullptr, 640, 480, 0, 0, (char *)"LearnOpenGL", GLFW_FALSE};

static GLFWwindow *g_window{nullptr};

static void shouldCloseCallBack(GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
  // esc for closing window
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    g_info.should_close = GLFW_TRUE;
    glfwSetWindowShouldClose(window, GLFW_TRUE);
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
  glfwSetKeyCallback(g_window, shouldCloseCallBack);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);

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
  ImGui::Text("Vertex Info:");
  for (int i = 0; i < 3; ++i) {
    ImGui::Text(
        "Vertex %d:\tPos(%5.2f, %5.2f, %5.2f) Color(%5.2f, %5.2f, %5.2f)", i,
        vertices[i * 6 + 0], vertices[i * 6 + 1], vertices[i * 6 + 2],
        vertices[i * 6 + 3], vertices[i * 6 + 4], vertices[i * 6 + 5]);
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

  // load & init vert array
  GLuint vao{0}, vbo{0}, ebo{0};
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1u, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6,
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glUnbindVertexArray();

  // use config
  glUseProgram(shader_prog);
  glBindVertexArray(vao);

  // main loop
  while (!g_info.should_close && !glfwWindowShouldClose(g_window)) {
    // update imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    drawDebugInfoWidget();

    // event handler...
    glfwPollEvents();

    // rendering...
    float cur_time = glfwGetTime();
    float green_val = std::sin(cur_time) / 2.0f + 0.5f;
    glClearColor(0.2, green_val, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // buffering...
    glfwSwapBuffers(g_window);
  }

  glUnbindVertexArray();

  // clean assets
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shader_prog);

  terminate();
  return 0;
}