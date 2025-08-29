#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/gl.h>

static struct WindowInfo {
  int width{0};
  int height{0};
  char *title{nullptr};
  bool shouldClose{GLFW_FALSE};
} g_info{640, 480, (char *)"LearnOpenGL", GLFW_FALSE};

static GLFWwindow *g_window{nullptr};

static void shouldCloseCallBack(GLFWwindow *window, int key, int scancode,
                                int action, int mods) {
  // esc for closing window
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    g_info.shouldClose = GLFW_TRUE;
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

static void windowSizeCallback(GLFWwindow *window, int width, int height) {
  g_info.width = width;
  g_info.height = height;

  glViewport(0, 0, g_info.width, g_info.height);
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  g_info.width = width;
  g_info.height = height;

  glViewport(0, 0, g_info.width, g_info.height);
}

int main(int argc, const char **argv) {
  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  g_window = glfwCreateWindow(g_info.width, g_info.height, g_info.title,
                              nullptr, nullptr);
  if (!g_window) {
    glfwTerminate();
    return 1;
  }

  // set events
  glfwSetKeyCallback(g_window, shouldCloseCallBack);
  glfwSetWindowSizeCallback(g_window, windowSizeCallback);
  glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);

  glfwMakeContextCurrent(g_window);

  // init glad
  if (!gladLoadGL(glfwGetProcAddress)) {
    glfwDestroyWindow(g_window);
    glfwTerminate();
    return 1;
  }
  glfwSwapInterval(1);

  // main loop
  while (!g_info.shouldClose && !glfwWindowShouldClose(g_window)) {
    // event handler...
    glfwPollEvents();

    // rendering...
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // buffering...
    glfwSwapBuffers(g_window);
  }

  glfwTerminate();
  return 0;
}