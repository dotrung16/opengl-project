#include "app.hpp"

#include "frame_pacer.hpp"
#include "frame_stats.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <memory>

namespace {

void apply_input(Window &window) {
  const InputActions actions = handle_input(window.state());
  if (actions.close)
    window.request_close();
  if (actions.mode_changed) {
    std::printf("Event-driven mode: %s\n",
                window.state().event_driven ? "on" : "off");
  }
}

} // namespace

int run_app() {
#ifdef _WIN32
  std::setvbuf(stdout, nullptr, _IONBF, 0); // Windows has no line buffering
#else
  std::setvbuf(stdout, nullptr, _IOLBF, BUFSIZ);
#endif

  GlfwLibrary glfw;
  if (!glfw.ok())
    return 1;

  std::unique_ptr<Window> window = Window::create({.title = "Window"});
  if (!window)
    return 1;

  // Declared after `window` so it's destroyed while the GL context is alive
  Renderer renderer;
  if (!renderer.init())
    return 1;

  std::printf("GL %d.%d | %s | %s\n", renderer.version_major(),
              renderer.version_minor(), renderer.device_name(),
              platform_name());

  WindowState &state = window->state();
  FramePacer pacer;
  FrameStats stats(glfwGetTime(), state);

  while (!window->should_close()) {
    if (window->is_hidden()) {
      stats.reset(pacer.wait_hidden(), state);
      apply_input(*window);
      continue;
    }

    const double wait_start = glfwGetTime();
    pacer.wait(*window, stats.next_report());
    const double now = glfwGetTime();
    stats.record_wakeup(now - wait_start);

    apply_input(*window);
    if (window->should_close())
      break;
    if (!state.event_driven && !state.needs_redraw && now < pacer.deadline())
      continue;

    pacer.update_swap_interval(state.focused);
    if (const auto report = stats.report_if_due(now, state))
      print_report(*report);

    if (state.event_driven && !state.needs_redraw)
      continue;
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
