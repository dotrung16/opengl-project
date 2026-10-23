#pragma once

#include <memory>
#include <vector>

struct GLFWwindow;

struct WindowState {
  int fb_width = 0;
  int fb_height = 0;
  bool focused = true;
  bool iconified = false;
  bool event_driven = true;
  bool needs_redraw = true;
  std::vector<int> pressed_keys;
  double cursor_x = 0.0;
  double cursor_y = 0.0;
  double cursor_dx = 0.0;
  double cursor_dy = 0.0;
  double scroll_dy = 0.0;
  bool has_cursor = false;

  bool has_pending_input() const { return !pressed_keys.empty(); }
};

class GlfwLibrary {
public:
  GlfwLibrary();
  ~GlfwLibrary();
  GlfwLibrary(const GlfwLibrary &) = delete;
  GlfwLibrary &operator=(const GlfwLibrary &) = delete;

  bool ok() const { return ok_; }

private:
  bool ok_ = false;
};

struct WindowConfig {
  int width = 800;
  int height = 600;
  const char *title = "Window";
  bool visible = true;
  bool scale_framebuffer = true;
};

class Window {
public:
  static std::unique_ptr<Window> create(const WindowConfig &config);
  ~Window();
  Window(const Window &) = delete;
  Window &operator=(const Window &) = delete;

  WindowState &state() { return state_; }
  const WindowState &state() const { return state_; }
  bool should_close() const;
  void request_close();
  bool is_hidden() const;
  bool key_down(int key) const;
  void swap_buffers();

private:
  explicit Window(GLFWwindow *handle);

  GLFWwindow *handle_;
  WindowState state_;
};

const char *platform_name();
