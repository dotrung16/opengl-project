#pragma once

struct WindowState;

struct InputActions {
  bool close = false;
  bool mode_changed = false;
};

InputActions handle_input(WindowState& state);
