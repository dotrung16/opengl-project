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
    if (state.needs_redraw || window.should_close()) return;
    now = glfwGetTime();
  }
}

}

FramePacer::FramePacer()
    : vsync_(glfwGetPlatform() != GLFW_PLATFORM_WAYLAND),
      min_frame_time_((vsync_ ? kFrameCapSlack : 1.0) / max_refresh_rate()),
      last_frame_(glfwGetTime() - kBackgroundFrameTime),
      swap_interval_(vsync_ ? 1 : 0) {
  glfwSwapInterval(swap_interval_);
}

void FramePacer::wait(const Window& window, double idle_deadline) {
  const WindowState& state = window.state();
  if (state.event_driven) {
    frame_time_ = 0.0;
    const double timeout = idle_deadline - glfwGetTime();
    if (state.needs_redraw || timeout <= 0.0) {
      glfwPollEvents();
    } else {
      glfwWaitEventsTimeout(timeout);
    }
  } else {
    frame_time_ = state.focused ? min_frame_time_ : kBackgroundFrameTime;
    deadline_ = last_frame_ + frame_time_;
    wait_until(window, deadline_);
  }
}

void FramePacer::mark_frame(double now) {
  const bool on_schedule = now >= deadline_ && now - deadline_ < frame_time_;
  last_frame_ = on_schedule ? deadline_ : now;
}

double FramePacer::wait_hidden() {
  glfwWaitEvents();
  last_frame_ = glfwGetTime();
  return last_frame_;
}

void FramePacer::update_swap_interval(bool focused) {
  const int wanted_interval = (vsync_ && focused) ? 1 : 0;
  if (wanted_interval != swap_interval_) {
    swap_interval_ = wanted_interval;
    glfwSwapInterval(swap_interval_);
  }
}
