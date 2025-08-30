#pragma once

#include <GLFW/glfw3.h>

class Camera;

class Input {
public:
  static void initialize(GLFWwindow *window);
  static void processKeyboard(GLFWwindow *window);
  static void setCamera(Camera *camera);
  static void setDeltaTime(float deltaTime);

  static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
  static void scrollCallback(GLFWwindow *window, double xoffset,
                             double yoffset);
  static void keyboardCallback(GLFWwindow *window, int key, int scancode,
                               int action, int mods);

  static bool isMouseCaptured() { return m_mouse_captured; }
  static void toggleMouseCapture();

private:
  static Camera *m_camera;
  static float m_lastX;
  static float m_lastY;
  static bool m_first_mouse;
  static bool m_mouse_captured;
  static GLFWwindow *m_window;
  static float m_delta_time;

  static void updateMouseCapture();
};