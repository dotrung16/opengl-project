#pragma once

#include "window.hpp"

#include <GLFW/glfw3.h>

#include <memory>

struct GlFixture {
  static constexpr int kWidth = 64;
  static constexpr int kHeight = 48;

  GlfwLibrary glfw;
  std::unique_ptr<Window> window;

  GlFixture() {
    if (!glfw.ok())
      return;
    window = Window::create({.width = kWidth,
                             .height = kHeight,
                             .title = "test",
                             .visible = false,
                             .scale_framebuffer = false});
  }

  bool ok() const { return window != nullptr; }
  GLFWwindow *handle() const { return glfwGetCurrentContext(); }
};
