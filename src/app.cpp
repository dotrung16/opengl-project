#include "app.hpp"

#include "frame_pacer.hpp"
#include "frame_stats.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>

int run_app() {
  GlfwLibrary glfw;
  if (!glfw.ok()) return 1;

  std::unique_ptr<Window> window = Window::create(800, 600, "OpenGL");
  if (!window) return 1;

  Renderer renderer;
  if (!renderer.init()) return 1;
  window->install_callbacks();

  std::printf("GL %d.%d | %s | %s\n", renderer.version_major(), renderer.version_minor(),
              renderer.device_name(), platform_name());

  WindowState& state = window->state();
  FramePacer pacer;
  FrameStats stats(glfwGetTime(), state);

  while (!window->should_close()) {
    if (window->is_hidden()) {
      stats.reset(pacer.wait_hidden(), state);
      continue;
    }

    const double wait_start = glfwGetTime();
    pacer.wait(*window, stats.next_report());
    const double now = glfwGetTime();
    stats.record_wakeup(now - wait_start);

    pacer.update_swap_interval(state.focused);
    stats.report_if_due(now, state);

    if (state.event_driven && !state.needs_redraw) continue;
    state.needs_redraw = false;
    pacer.mark_frame(now);
    stats.record_frame();

    renderer.draw(state.fb_width, state.fb_height);
    const double swap_start = glfwGetTime();
    window->swap_buffers();
    stats.record_swap(glfwGetTime() - swap_start);
  }

  return 0;
}
