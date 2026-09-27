#pragma once

#include <memory>

struct GLFWwindow;

struct WindowState {
  int fb_width = 0;
  int fb_height = 0;
  bool focused = true;
  bool iconified = false;
  bool event_driven = true;
  bool needs_redraw = true;
};

class GlfwLibrary {
 public:
  GlfwLibrary();
  ~GlfwLibrary();
  GlfwLibrary(const GlfwLibrary&) = delete;
  GlfwLibrary& operator=(const GlfwLibrary&) = delete;

  bool ok() const { return ok_; }

 private:
  bool ok_ = false;
};

class Window {
 public:
  static std::unique_ptr<Window> create(int width, int height, const char* title);
  ~Window();
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  WindowState& state() { return state_; }
  const WindowState& state() const { return state_; }
  bool should_close() const;
  bool is_hidden() const;
  void install_callbacks();
  void swap_buffers();

 private:
  explicit Window(GLFWwindow* handle);

  GLFWwindow* handle_;
  WindowState state_;
};

const char* platform_name();
