#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/gl.h>

static struct WindowInfo {
  int window_width{0};
  int window_height{0};
  int framebuffer_width{0};
  int framebuffer_height{0};
  char *title{nullptr};
  bool should_close{GLFW_FALSE};
} g_info{640, 480, 0, 0, (char *)"LearnOpenGL", GLFW_FALSE};

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

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

int main(int argc, const char **argv) {
  // init glfw
  if (!glfwInit()) {
    return 1;
  }

  // window creating
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

  // bind events
  glfwSetKeyCallback(g_window, shouldCloseCallBack);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);

  // set context
  glfwMakeContextCurrent(g_window);

  // init glad
  if (!gladLoadGL(glfwGetProcAddress)) {
    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 1;
  }
  glfwSwapInterval(1);

  // set framebuffer size
  glfwGetFramebufferSize(g_window, &g_info.framebuffer_width,
                         &g_info.framebuffer_height);
  glViewport(0, 0, g_info.framebuffer_width, g_info.framebuffer_height);

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // main loop
  while (!g_info.should_close && !glfwWindowShouldClose(g_window)) {
    // event handler...
    glfwPollEvents();

    // rendering...
    glClearColor(0.2, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // buffering...
    glfwSwapBuffers(g_window);
  }

  glfwTerminate();
  return 0;
}