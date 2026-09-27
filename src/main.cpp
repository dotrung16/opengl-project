#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>

namespace {
constexpr double kBackgroundFrameTime = 1.0 / 10.0;
constexpr double kStatsInterval = 3.0;

struct WindowState {
  int fb_width = 0;
  int fb_height = 0;
  bool focused = true;
  bool iconified = false;
  bool event_driven = false;
  bool needs_redraw = true;
};

struct FrameStats {
  double window_start = 0.0;
  int frames = 0;
  int wakeups = 0;
  double max_wait = 0.0;
  double max_swap = 0.0;

  void reset(double now) {
    window_start = now;
    frames = 0;
    wakeups = 0;
    max_wait = 0.0;
    max_swap = 0.0;
  }
};

WindowState& state_of(GLFWwindow* window) {
  return *static_cast<WindowState*>(glfwGetWindowUserPointer(window));
}

void glfw_error_callback(int code, const char* description) {
  std::fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  WindowState& state = state_of(window);
  state.fb_width = width;
  state.fb_height = height;
  state.needs_redraw = true;
  glViewport(0, 0, width, height);
}

void focus_callback(GLFWwindow* window, int focused) {
  WindowState& state = state_of(window);
  state.focused = (focused == GLFW_TRUE);
  state.needs_redraw = true;
}

void iconify_callback(GLFWwindow* window, int iconified) {
  WindowState& state = state_of(window);
  state.iconified = (iconified == GLFW_TRUE);
  state.needs_redraw = true;
}

void refresh_callback(GLFWwindow* window) {
  state_of(window).needs_redraw = true;
}

void key_callback(GLFWwindow* window, int key, int, int action, int) {
  if (action != GLFW_PRESS) return;
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_E) {
    WindowState& state = state_of(window);
    state.event_driven = !state.event_driven;
    state.needs_redraw = true;
    std::printf("Event-driven mode: %s\n", state.event_driven ? "on" : "off");
  }
}

#if !defined(NDEBUG) && defined(GL_KHR_debug)
void GLAD_API_PTR gl_debug_callback(GLenum, GLenum type, GLuint id, GLenum severity,
                                    GLsizei, const GLchar* message, const void*) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
  std::fprintf(stderr, "GL debug [id %u, type 0x%x]: %s\n", id, type, message);
}
#endif

const char* platform_name(int platform) {
  switch(platform) {
    case GLFW_PLATFORM_WAYLAND: return "Wayland";
    case GLFW_PLATFORM_X11: return "X11";
    case GLFW_PLATFORM_WIN32: return "Win32";
    case GLFW_PLATFORM_COCOA: return "Cocoa";
    default: return "other";
  }
}

void wait_until(GLFWwindow* window, const WindowState& state, double deadline) {
  double now = glfwGetTime();
  if (now >= deadline) {
    glfwPollEvents();
    return;
  }

  while (now < deadline) {
    glfwWaitEventsTimeout(deadline - now);
    if (state.focused || glfwWindowShouldClose(window)) return;
    now = glfwGetTime();
  }
}

}


int main() {
  glfwSetErrorCallback(glfw_error_callback);

  if (!glfwInit()) {
    std::fprintf(stderr, "Failed to init GLFW\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

  glfwWindowHint(GLFW_DEPTH_BITS, 0);
  glfwWindowHint(GLFW_STENCIL_BITS, 0);

#ifndef NDEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

  GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);  if (!window) {
    std::fprintf(stderr, "Failed to create window\n");
    glfwTerminate();
    return 1;
  }

  WindowState state;
  glfwSetWindowUserPointer(window, &state);
  glfwGetFramebufferSize(window, &state.fb_width, &state.fb_height);
  state.focused = (glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0);

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetWindowFocusCallback(window, focus_callback);
  glfwSetWindowIconifyCallback(window, iconify_callback);
  glfwSetWindowRefreshCallback(window, refresh_callback);
  glfwSetKeyCallback(window, key_callback);

  const int version = gladLoadGL(glfwGetProcAddress);
  if (version == 0) {
    std::fprintf(stderr, "Failed to load GL functions\n");
    glfwTerminate();
    return 1;
  }

  std::printf("GL %d.%d | %s | %s\n", GLAD_VERSION_MAJOR(version),
              GLAD_VERSION_MINOR(version),
              reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
              platform_name(glfwGetPlatform()));

#if !defined(NDEBUG) && defined(GL_KHR_debug)
  if (GLAD_GL_KHR_debug) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_callback, nullptr);
  }
#endif

  glClearColor(0.15f, 0.45f, 0.45f, 1.0f);

  int swap_interval = 1;
  glfwSwapInterval(swap_interval);

  double last_time = glfwGetTime() - kBackgroundFrameTime;
  FrameStats stats;
  stats.reset(glfwGetTime());
  while (!glfwWindowShouldClose(window)) {
    if (state.iconified || state.fb_width == 0 || state.fb_height == 0) {
      glfwWaitEvents();
      last_time = glfwGetTime();
      stats.reset(last_time);
      continue;
    }

    const double wait_start = glfwGetTime();
    if (state.event_driven) {
      glfwWaitEventsTimeout(kStatsInterval);
    } else if (state.focused) {
      glfwPollEvents();
    } else {
      wait_until(window, state, last_time + kBackgroundFrameTime);
    }

    const double current_time = glfwGetTime();
    stats.wakeups++;
    stats.max_wait = std::max(stats.max_wait, current_time - wait_start);

    const int wanted_interval = state.focused ? 1 : 0;
    if (wanted_interval != swap_interval) {
      swap_interval = wanted_interval;
      glfwSwapInterval(swap_interval);
    }

    const double elapsed = current_time - stats.window_start;
    if (elapsed >= kStatsInterval) {
      const double avg_frame_ms = stats.frames > 0 ? elapsed * 1000.0 / stats.frames : 0.0;
      std::printf("[Power Stats] FPS: %.1f | Avg frame: %.2f ms | Wakeups/s: %.1f | "
                  "Max wait: %.1f ms | Max swap: %.1f ms | Focused: %s | Mode: %s\n",
                  stats.frames / elapsed, avg_frame_ms, stats.wakeups / elapsed,
                  stats.max_wait * 1000.0, stats.max_swap * 1000.0,
                  state.focused ? "Yes" : "No",
                  state.event_driven ? "event-driven" : "continuous");
      stats.reset(current_time);
    }

    if (state.event_driven && !state.needs_redraw) continue;
    state.needs_redraw = false;
    last_time = current_time;
    stats.frames++;

    glClear(GL_COLOR_BUFFER_BIT);
    const double swap_start = glfwGetTime();
    glfwSwapBuffers(window);
    stats.max_swap = std::max(stats.max_swap, glfwGetTime() - swap_start);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
