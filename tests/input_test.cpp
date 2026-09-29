#include "input.hpp"
#include "window.hpp"

#include "test.hpp"

#include <GLFW/glfw3.h>

TEST(input_empty_queue_does_nothing) {
  WindowState state;
  state.needs_redraw = false;
  const InputActions actions = handle_input(state);
  CHECK(!actions.close);
  CHECK(!actions.mode_changed);
  CHECK(!state.needs_redraw);
}

TEST(input_escape_requests_close) {
  WindowState state;
  state.pressed_keys = {GLFW_KEY_ESCAPE};
  const InputActions actions = handle_input(state);
  CHECK(actions.close);
  CHECK(!actions.mode_changed);
}

TEST(input_e_toggles_mode_and_requests_redraw) {
  WindowState state;
  state.event_driven = true;
  state.needs_redraw = false;
  state.pressed_keys = {GLFW_KEY_E};
  const InputActions actions = handle_input(state);
  CHECK(actions.mode_changed);
  CHECK(!actions.close);
  CHECK(!state.event_driven);
  CHECK(state.needs_redraw);
}

TEST(input_double_toggle_restores_mode) {
  WindowState state;
  state.event_driven = false;
  state.pressed_keys = {GLFW_KEY_E, GLFW_KEY_E};
  handle_input(state);
  CHECK(!state.event_driven);
}

TEST(input_unbound_keys_are_ignored) {
  WindowState state;
  state.event_driven = true;
  state.needs_redraw = false;
  state.pressed_keys = {GLFW_KEY_A, GLFW_KEY_SPACE, GLFW_KEY_F1};
  const InputActions actions = handle_input(state);
  CHECK(!actions.close);
  CHECK(!actions.mode_changed);
  CHECK(state.event_driven);
  CHECK(!state.needs_redraw);
}

TEST(input_queue_is_drained) {
  WindowState state;
  state.pressed_keys = {GLFW_KEY_A, GLFW_KEY_E, GLFW_KEY_ESCAPE};
  const InputActions actions = handle_input(state);
  CHECK(state.pressed_keys.empty());
  CHECK(!state.has_pending_input());
  CHECK(actions.close);
  CHECK(actions.mode_changed);
}
