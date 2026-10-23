#include "window.hpp"

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <cstdio>

namespace {

WindowState &state_of(GLFWwindow *window) {
  return *static_cast<WindowState *>(glfwGetWindowUserPointer(window));
}

void glfw_error_callback(int code, const char *description) {
  std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  WindowState &state = state_of(window);
  state.fb_width = width;
  state.fb_height = height;
  state.needs_redraw = true;
}

void focus_callback(GLFWwindow *window, int focused) {
  WindowState &state = state_of(window);
  state.focused = (focused == GLFW_TRUE);
  state.needs_redraw = true;
}

void iconify_callback(GLFWwindow *window, int iconified) {
  WindowState &state = state_of(window);
  state.iconified = (iconified == GLFW_TRUE);
  state.needs_redraw = true;
}

void refresh_callback(GLFWwindow *window) {
  state_of(window).needs_redraw = true;
}

void key_callback(GLFWwindow *window, int key, int, int action, int) {
  if (action != GLFW_PRESS || key == GLFW_KEY_UNKNOWN)
    return;
  state_of(window).pressed_keys.push_back(key);
}

void cursor_pos_callback(GLFWwindow *window, double x, double y) {
  WindowState &state = state_of(window);
  if (state.has_cursor) {
    state.cursor_dx += x - state.cursor_x;
    state.cursor_dy += y - state.cursor_y;
  }
  state.cursor_x = x;
  state.cursor_y = y;
  state.has_cursor = true;
}

void scroll_callback(GLFWwindow *window, double, double y) {
  state_of(window).scroll_dy += y;
}

} // namespace

GlfwLibrary::GlfwLibrary() {
  glfwSetErrorCallback(glfw_error_callback);
  ok_ = (glfwInit() == GLFW_TRUE);
  if (!ok_)
    std::fprintf(stderr, "Failed to init GLFW\n");
}

GlfwLibrary::~GlfwLibrary() {
  if (ok_)
    glfwTerminate();
}

std::unique_ptr<Window> Window::create(const WindowConfig &config) {
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

  glfwWindowHint(GLFW_DEPTH_BITS, 24);
  glfwWindowHint(GLFW_STENCIL_BITS, 0);
  glfwWindowHint(GLFW_VISIBLE, config.visible ? GLFW_TRUE : GLFW_FALSE);
  glfwWindowHint(GLFW_SCALE_FRAMEBUFFER,
                 config.scale_framebuffer ? GLFW_TRUE : GLFW_FALSE);

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

  GLFWwindow *handle = glfwCreateWindow(config.width, config.height,
                                        config.title, nullptr, nullptr);
  if (!handle) {
    std::fprintf(stderr, "Failed to create window\n");
    return nullptr;
  }
  std::unique_ptr<Window> window(new Window(handle));
  if (gladLoadGL(glfwGetProcAddress) == 0) {
    std::fprintf(stderr, "Failed to load GL functions\n");
    return nullptr;
  }
  return window;
}

Window::Window(GLFWwindow *handle) : handle_(handle) {
  glfwSetWindowUserPointer(handle_, &state_);
  glfwGetFramebufferSize(handle_, &state_.fb_width, &state_.fb_height);
  state_.focused = (glfwGetWindowAttrib(handle_, GLFW_FOCUSED) != 0);
  glfwMakeContextCurrent(handle_);

  glfwSetFramebufferSizeCallback(handle_, framebuffer_size_callback);
  glfwSetWindowFocusCallback(handle_, focus_callback);
  glfwSetWindowIconifyCallback(handle_, iconify_callback);
  glfwSetWindowRefreshCallback(handle_, refresh_callback);
  glfwSetKeyCallback(handle_, key_callback);
  glfwSetCursorPosCallback(handle_, cursor_pos_callback);
  glfwSetScrollCallback(handle_, scroll_callback);
}

Window::~Window() { glfwDestroyWindow(handle_); }

bool Window::should_close() const {
  return glfwWindowShouldClose(handle_) != 0;
}

void Window::request_close() { glfwSetWindowShouldClose(handle_, GLFW_TRUE); }

bool Window::is_hidden() const {
  return state_.iconified || state_.fb_width == 0 || state_.fb_height == 0;
}

bool Window::key_down(int key) const {
  return glfwGetKey(handle_, key) == GLFW_PRESS;
}

void Window::swap_buffers() { glfwSwapBuffers(handle_); }

const char *platform_name() {
  switch (glfwGetPlatform()) {
  case GLFW_PLATFORM_WAYLAND:
    return "Wayland";
  case GLFW_PLATFORM_X11:
    return "X11";
  case GLFW_PLATFORM_WIN32:
    return "Win32";
  case GLFW_PLATFORM_COCOA:
    return "Cocoa";
  case GLFW_PLATFORM_NULL:
    return "Null";
  default:
    return "other";
  }
}
