#include "frame_pacer.hpp"

#include "window.hpp"

#include <GLFW/glfw3.h>

#include <algorithm>

namespace {

constexpr double kBackgroundFrameTime = 1.0 / 10.0;
constexpr double kFrameCapSlack = 0.9;
constexpr int kFallbackRefreshRate = 60;

int max_refresh_rate() {
  int count = 0;
  GLFWmonitor** monitors = glfwGetMonitors(&count);
  int rate = 0;
  for (int i = 0; i < count; ++i) {
    if (const GLFWvidmode* mode = glfwGetVideoMode(monitors[i])) {
      rate = std::max(rate, mode->refreshRate);
    }
  }
  return rate > 0 ? rate : kFallbackRefreshRate;
}

void wait_until(const Window& window, double deadline) {
  const WindowState& state = window.state();
  double now = glfwGetTime();
  if (now >= deadline || state.needs_redraw) {
    glfwPollEvents();
    return;
  }

  while (now < deadline) {
    glfwWaitEventsTimeout(deadline - now);
    if (state.needs_redraw || state.has_pending_input() || window.should_close()) return;
    now = glfwGetTime();
  }
}

}

FramePacer::FramePacer()
    : vsync_(glfwGetPlatform() != GLFW_PLATFORM_WAYLAND),
      min_frame_time_((vsync_ ? kFrameCapSlack : 1.0) / max_refresh_rate()),
      swap_interval_(vsync_ ? 1 : 0),
      schedule_(glfwGetTime() - kBackgroundFrameTime) {
  glfwSwapInterval(swap_interval_);
}

void FramePacer::wait(const Window& window, double idle_deadline) {
  const WindowState& state = window.state();
  if (state.event_driven) {
    schedule_.begin_unpaced();
    const double timeout = idle_deadline - glfwGetTime();
    if (state.needs_redraw || timeout <= 0.0) {
      glfwPollEvents();
    } else {
      glfwWaitEventsTimeout(timeout);
    }
  } else {
    const double frame_time = state.focused ? min_frame_time_ : kBackgroundFrameTime;
    wait_until(window, schedule_.begin(frame_time));
  }
}

double FramePacer::wait_hidden() {
  glfwWaitEvents();
  const double now = glfwGetTime();
  schedule_.resync(now);
  return now;
}

void FramePacer::update_swap_interval(bool focused) {
  const int wanted_interval = (vsync_ && focused) ? 1 : 0;
  if (wanted_interval != swap_interval_) {
    swap_interval_ = wanted_interval;
    glfwSwapInterval(swap_interval_);
  }
}
