#include "gl_fixture.hpp"
#include "renderer.hpp"

#include "test.hpp"

#include <glad/gl.h>

#include <cmath>
#include <cstring>

namespace {

struct OffscreenTarget {
  GLuint fbo = 0;
  GLuint color = 0;

  OffscreenTarget(int width, int height) {
    glGenRenderbuffers(1, &color);
    glBindRenderbuffer(GL_RENDERBUFFER, color);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color);
  }

  ~OffscreenTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &color);
  }

  bool complete() const {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  }
};

int to_byte(float channel) {
  return static_cast<int>(std::lround(channel * 255.0f));
}

}

TEST(renderer_init_loads_gl_3_3_core) {
  GlFixture f;
  if (!f.ok()) return;
  Renderer renderer;
  CHECK(renderer.init());
  const int major = renderer.version_major();
  const int minor = renderer.version_minor();
  CHECK(major > 3 || (major == 3 && minor >= 3));
  CHECK(glGetError() == GL_NO_ERROR);
}

TEST(renderer_device_name_is_not_empty) {
  GlFixture f;
  if (!f.ok()) return;
  Renderer renderer;
  CHECK(renderer.init());
  const char* name = renderer.device_name();
  CHECK(name != nullptr);
  CHECK(name && std::strlen(name) > 0);
}

TEST(renderer_draw_sets_viewport_to_framebuffer_size) {
  GlFixture f;
  if (!f.ok()) return;
  Renderer renderer;
  CHECK(renderer.init());

  GLint viewport[4] = {};
  renderer.draw(32, 16);
  glGetIntegerv(GL_VIEWPORT, viewport);
  CHECK(viewport[0] == 0 && viewport[1] == 0);
  CHECK(viewport[2] == 32 && viewport[3] == 16);

  renderer.draw(GlFixture::kWidth, GlFixture::kHeight);
  glGetIntegerv(GL_VIEWPORT, viewport);
  CHECK(viewport[2] == GlFixture::kWidth && viewport[3] == GlFixture::kHeight);
  CHECK(glGetError() == GL_NO_ERROR);
}

TEST(renderer_draw_clears_to_background_color) {
  GlFixture f;
  if (!f.ok()) return;
  Renderer renderer;
  CHECK(renderer.init());

  OffscreenTarget target(GlFixture::kWidth, GlFixture::kHeight);
  CHECK(target.complete());
  if (!target.complete()) return;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  CHECK(renderer.init());

  GLfloat clear[4] = {};
  glGetFloatv(GL_COLOR_CLEAR_VALUE, clear);
  CHECK(to_byte(clear[0]) + to_byte(clear[1]) + to_byte(clear[2]) > 0);
  renderer.draw(GlFixture::kWidth, GlFixture::kHeight);

  const int points[][2] = {{0, 0}, {GlFixture::kWidth / 2, GlFixture::kHeight / 2},
                           {GlFixture::kWidth - 1, GlFixture::kHeight - 1}};
  for (const auto& p : points) {
    GLubyte pixel[4] = {};
    glReadPixels(p[0], p[1], 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    for (int c = 0; c < 4; ++c) CHECK(std::abs(pixel[c] - to_byte(clear[c])) <= 1);
  }
  CHECK(to_byte(clear[3]) == 255);
  CHECK(glGetError() == GL_NO_ERROR);
}
