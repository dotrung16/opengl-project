#include "renderer.hpp"

#include "gl_check.hpp"

#include <glad/gl.h>

#include <cstdio>

namespace {

#if !defined(NDEBUG) && defined(GL_KHR_debug)
void GLAD_API_PTR gl_debug_callback(GLenum, GLenum type, GLuint id,
                                    GLenum severity, GLsizei,
                                    const GLchar *message, const void *) {
  if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;
  std::fprintf(stderr, "GL debug [id %u, type 0x%x]: %s\n", id, type, message);
}
#endif

} // namespace

bool Renderer::init() {
  glGetIntegerv(GL_MAJOR_VERSION, &major_);
  glGetIntegerv(GL_MINOR_VERSION, &minor_);
  if (major_ < 3 || (major_ == 3 && minor_ < 3)) {
    std::fprintf(stderr, "OpenGL 3.3 required, got %d.%d\n", major_, minor_);
    return false;
  }

#if !defined(NDEBUG) && defined(GL_KHR_debug)
  if (GLAD_GL_KHR_debug) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_callback, nullptr);
  }
#endif

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.15f, 0.45f, 0.45f, 1.0f);
  return true;
}

const char *Renderer::device_name() const {
  const GLubyte *name = glGetString(GL_RENDERER);
  return name ? reinterpret_cast<const char *>(name) : "unknown";
}

void Renderer::draw(int width, int height) {
  GL_CHECK(glViewport(0, 0, width, height));
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

#ifndef NDEBUG
  check_gl_errors("Renderer::draw");
#endif
}
