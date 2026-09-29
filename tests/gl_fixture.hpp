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
    if (!glfw.ok()) return;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
    window = Window::create(kWidth, kHeight, "test");
  }

  bool ok() const { return window != nullptr; }
  GLFWwindow* handle() const { return glfwGetCurrentContext(); }
};
