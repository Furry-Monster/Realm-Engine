#include "input.h"
#include "camera.h"

Camera *Input::m_camera = nullptr;
float Input::m_lastX = 320.0f;
float Input::m_lastY = 240.0f;
bool Input::m_first_mouse = true;
bool Input::m_mouse_captured = true;
GLFWwindow *Input::m_window = nullptr;
float Input::m_delta_time = 0.0f;

void Input::initialize(GLFWwindow *window) {
  m_window = window;
  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetKeyCallback(window, keyboardCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::setCamera(Camera *camera) { m_camera = camera; }

void Input::setDeltaTime(float deltaTime) { m_delta_time = deltaTime; }

void Input::processKeyboard(GLFWwindow *window) {
  if (!m_camera)
    return;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    m_camera->processKeyboard(FORWARD, m_delta_time);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    m_camera->processKeyboard(BACKWARD, m_delta_time);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    m_camera->processKeyboard(LEFT, m_delta_time);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    m_camera->processKeyboard(RIGHT, m_delta_time);
}

void Input::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
  if (!m_mouse_captured || !m_camera)
    return;

  if (m_first_mouse) {
    m_lastX = xpos;
    m_lastY = ypos;
    m_first_mouse = false;
  }

  float xoffset = xpos - m_lastX;
  float yoffset = m_lastY - ypos;

  m_lastX = xpos;
  m_lastY = ypos;

  m_camera->processMouseMovement(xoffset, yoffset);
}

void Input::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
  if (m_camera) {
    m_camera->processMouseScroll(yoffset);
  }
}

void Input::keyboardCallback(GLFWwindow *window, int key, int scancode,
                             int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    toggleMouseCapture();
  }
}

void Input::toggleMouseCapture() {
  m_mouse_captured = !m_mouse_captured;
  updateMouseCapture();
  m_first_mouse = true;
}

void Input::updateMouseCapture() {
  if (m_window) {
    if (m_mouse_captured) {
      glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
}