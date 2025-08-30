#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
  Window(int width, int height, const char *title);
  ~Window();

  bool initialize();
  void terminate();

  GLFWwindow *getGLFWwindow() { return m_window; }
  bool shouldClose() { return glfwWindowShouldClose(m_window); }
  void swapBuffers() { glfwSwapBuffers(m_window); }
  void pollEvents() { glfwPollEvents(); }

  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
  int getFramebufferWidth() const { return m_framebuffer_width; }
  int getFramebufferHeight() const { return m_framebuffer_height; }

  void setTitle(const std::string &title);

private:
  GLFWwindow *m_window;
  int m_width;
  int m_height;
  int m_framebuffer_width;
  int m_framebuffer_height;
  std::string m_title;

  static void windowSizeCallback(GLFWwindow *window, int width, int height);
  static void framebufferSizeCallback(GLFWwindow *window, int width,
                                      int height);
};