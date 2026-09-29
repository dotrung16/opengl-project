#include "input.hpp"

#include "window.hpp"

#include <GLFW/glfw3.h>

namespace {

void handle_key(WindowState& state, InputActions& actions, int key) {
  switch (key) {
    case GLFW_KEY_ESCAPE:
      actions.close = true;
      break;
    case GLFW_KEY_E:
      state.event_driven = !state.event_driven;
      state.needs_redraw = true;
      actions.mode_changed = true;
      break;
    default:
      break;
  }
}

}

InputActions handle_input(WindowState& state) {
  InputActions actions;
  for (const int key : state.pressed_keys) handle_key(state, actions, key);
  state.pressed_keys.clear();
  return actions;
}
