#include "renderer.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdio>

namespace {

#if !defined(NDEBUG) && defined(GL_KHR_debug)
void GLAD_API_PTR gl_debug_callback(GLenum, GLenum type, GLuint id, GLenum severity,
                                    GLsizei, const GLchar* message, const void*) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
  std::fprintf(stderr, "GL debug [id %u, type 0x%x]: %s\n", id, type, message);
}
#endif

}

bool Renderer::init() {
  version_ = gladLoadGL(glfwGetProcAddress);
  if (version_ == 0) {
    std::fprintf(stderr, "Failed to load GL functions\n");
    return false;
  }

#if !defined(NDEBUG) && defined(GL_KHR_debug)
  if (GLAD_GL_KHR_debug) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_callback, nullptr);
  }
#endif

  glClearColor(0.15f, 0.45f, 0.45f, 1.0f);
  return true;
}

int Renderer::version_major() const {
  return GLAD_VERSION_MAJOR(version_);
}

int Renderer::version_minor() const {
  return GLAD_VERSION_MINOR(version_);
}

const char* Renderer::device_name() const {
  return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

void Renderer::draw(int width, int height) {
  if (width != viewport_width_ || height != viewport_height_) {
    glViewport(0, 0, width, height);
    viewport_width_ = width;
    viewport_height_ = height;
  }
  glClear(GL_COLOR_BUFFER_BIT);
}
