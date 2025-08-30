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
  ((str) != nullptr && std::strcmp((str), "") != STR_EQUAL)

#define glUnbindVertexArray() (glBindVertexArray(0))

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
  return shader.str();
}

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

int main(int argc, const char **argv) {
  // init glfw
  if (!glfwInit()) {
    return 1;
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
    return 1;
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
    return 1;
  }
  glfwSwapInterval(1);

  // load info from OpenGL and do some initializations
  glfwGetFramebufferSize(g_window, &g_info.framebuffer_width,
                         &g_info.framebuffer_height);
  glViewport(0, 0, g_info.framebuffer_width, g_info.framebuffer_height);

  g_info.version = (char *)glfwGetVersionString();
  if (cStringIsNullOrEmpty(g_info.version)) {
    std::string windowTitle =
        std::string(g_info.title) + " - " + std::string(g_info.version);
    glfwSetWindowTitle(g_window, windowTitle.c_str());
  }

  // load vert shader

  GLint success{0};
  char log[512];

  std::string vert_shader_src_raw = loadShaderFrom("../shader/test.vert");
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
  std::string frag_shader_src_raw = loadShaderFrom("../shader/test.frag");
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

  // load & init vert array
  GLuint vao{0}, vbo{0};
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3,
                        (void *)0);
  glEnableVertexAttribArray(0);

  glUnbindVertexArray();

  glUseProgram(shader_prog);
  glBindVertexArray(vao);

  // main loop
  while (!g_info.should_close && !glfwWindowShouldClose(g_window)) {
    // event handler...
    glfwPollEvents();

    // rendering...
    glClearColor(0.2, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // buffering...
    glfwSwapBuffers(g_window);
  }

  glUnbindVertexArray();

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shader_prog);

  glfwTerminate();
  return 0;
}