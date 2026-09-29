#include "gl_fixture.hpp"
#include "window.hpp"

#include "test.hpp"

#include <GLFW/glfw3.h>

namespace {

GLFWkeyfun installed_key_callback(GLFWwindow* h) {
  GLFWkeyfun cb = glfwSetKeyCallback(h, nullptr);
  glfwSetKeyCallback(h, cb);
  return cb;
}

GLFWframebuffersizefun installed_framebuffer_callback(GLFWwindow* h) {
  GLFWframebuffersizefun cb = glfwSetFramebufferSizeCallback(h, nullptr);
  glfwSetFramebufferSizeCallback(h, cb);
  return cb;
}

GLFWwindowfocusfun installed_focus_callback(GLFWwindow* h) {
  GLFWwindowfocusfun cb = glfwSetWindowFocusCallback(h, nullptr);
  glfwSetWindowFocusCallback(h, cb);
  return cb;
}

GLFWwindowiconifyfun installed_iconify_callback(GLFWwindow* h) {
  GLFWwindowiconifyfun cb = glfwSetWindowIconifyCallback(h, nullptr);
  glfwSetWindowIconifyCallback(h, cb);
  return cb;
}

GLFWwindowrefreshfun installed_refresh_callback(GLFWwindow* h) {
  GLFWwindowrefreshfun cb = glfwSetWindowRefreshCallback(h, nullptr);
  glfwSetWindowRefreshCallback(h, cb);
  return cb;
}

}

TEST(window_create_makes_context_current) {
  GlFixture f;
  CHECK(f.ok());
  CHECK(f.handle() != nullptr);
}

TEST(window_initial_state) {
  GlFixture f;
  if (!f.ok()) return;
  const WindowState& state = f.window->state();
  CHECK(state.fb_width > 0);
  CHECK(state.fb_height > 0);
  CHECK(state.needs_redraw);
  CHECK(!state.iconified);
  CHECK(!state.has_pending_input());
  CHECK(!f.window->is_hidden());
  CHECK(!f.window->should_close());
}

TEST(window_request_close) {
  GlFixture f;
  if (!f.ok()) return;
  f.window->request_close();
  CHECK(f.window->should_close());
}

TEST(window_installs_all_callbacks) {
  GlFixture f;
  if (!f.ok()) return;
  CHECK(installed_key_callback(f.handle()) != nullptr);
  CHECK(installed_framebuffer_callback(f.handle()) != nullptr);
  CHECK(installed_focus_callback(f.handle()) != nullptr);
  CHECK(installed_iconify_callback(f.handle()) != nullptr);
  CHECK(installed_refresh_callback(f.handle()) != nullptr);
}

TEST(window_key_press_is_queued) {
  GlFixture f;
  if (!f.ok()) return;
  const GLFWkeyfun key = installed_key_callback(f.handle());
  key(f.handle(), GLFW_KEY_E, 0, GLFW_PRESS, 0);
  key(f.handle(), GLFW_KEY_ESCAPE, 0, GLFW_PRESS, 0);
  const WindowState& state = f.window->state();
  CHECK(state.has_pending_input());
  CHECK(state.pressed_keys.size() == 2);
  if (state.pressed_keys.size() != 2) return;
  CHECK(state.pressed_keys[0] == GLFW_KEY_E);
  CHECK(state.pressed_keys[1] == GLFW_KEY_ESCAPE);
  CHECK(!f.window->should_close());
}

TEST(window_key_release_repeat_and_unknown_are_ignored) {
  GlFixture f;
  if (!f.ok()) return;
  const GLFWkeyfun key = installed_key_callback(f.handle());
  key(f.handle(), GLFW_KEY_E, 0, GLFW_RELEASE, 0);
  key(f.handle(), GLFW_KEY_E, 0, GLFW_REPEAT, 0);
  key(f.handle(), GLFW_KEY_UNKNOWN, 0, GLFW_PRESS, 0);
  CHECK(!f.window->state().has_pending_input());
}

TEST(window_framebuffer_resize_updates_state) {
  GlFixture f;
  if (!f.ok()) return;
  WindowState& state = f.window->state();
  state.needs_redraw = false;
  installed_framebuffer_callback(f.handle())(f.handle(), 200, 100);
  CHECK(state.fb_width == 200);
  CHECK(state.fb_height == 100);
  CHECK(state.needs_redraw);
}

TEST(window_zero_size_framebuffer_is_hidden) {
  GlFixture f;
  if (!f.ok()) return;
  const GLFWframebuffersizefun resize = installed_framebuffer_callback(f.handle());
  resize(f.handle(), 0, 0);
  CHECK(f.window->is_hidden());
  resize(f.handle(), 10, 0);
  CHECK(f.window->is_hidden());
  resize(f.handle(), 10, 10);
  CHECK(!f.window->is_hidden());
}

TEST(window_iconify_hides_and_restores) {
  GlFixture f;
  if (!f.ok()) return;
  WindowState& state = f.window->state();
  const GLFWwindowiconifyfun iconify = installed_iconify_callback(f.handle());
  state.needs_redraw = false;
  iconify(f.handle(), GLFW_TRUE);
  CHECK(state.iconified);
  CHECK(f.window->is_hidden());
  CHECK(state.needs_redraw);
  iconify(f.handle(), GLFW_FALSE);
  CHECK(!state.iconified);
  CHECK(!f.window->is_hidden());
}

TEST(window_focus_updates_state) {
  GlFixture f;
  if (!f.ok()) return;
  WindowState& state = f.window->state();
  const GLFWwindowfocusfun focus = installed_focus_callback(f.handle());
  state.needs_redraw = false;
  focus(f.handle(), GLFW_FALSE);
  CHECK(!state.focused);
  CHECK(state.needs_redraw);
  focus(f.handle(), GLFW_TRUE);
  CHECK(state.focused);
}

TEST(window_refresh_requests_redraw) {
  GlFixture f;
  if (!f.ok()) return;
  WindowState& state = f.window->state();
  state.needs_redraw = false;
  installed_refresh_callback(f.handle())(f.handle());
  CHECK(state.needs_redraw);
}
