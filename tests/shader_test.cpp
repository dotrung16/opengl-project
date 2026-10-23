#include "gl_fixture.hpp"
#include "shader.hpp"
#include "test.hpp"

#include <glad/gl.h>

#include <utility>

namespace {

constexpr const char *kVertex = R"(#version 330 core
layout(location = 0) in vec2 a_position;
void main() { gl_Position = vec4(a_position, 0.0, 1.0); }
)";

constexpr const char *kFragment = R"(#version 330 core
uniform vec4 u_color;
out vec4 frag_color;
void main() { frag_color = u_color; }
)";

constexpr const char *kBroken = R"(#version 330 core
void main() { this_is_not_glsl; }
)";

} // namespace

TEST(shader_builds_valid_program) {
  GlFixture f;
  if (!f.ok())
    return;
  ShaderProgram program;
  CHECK(program.build(kVertex, kFragment));
  CHECK(program.valid());
  CHECK(program.uniform("u_color") != -1);
  CHECK(glGetError() == GL_NO_ERROR);
}

TEST(shader_broken_source_fails) {
  GlFixture f;
  if (!f.ok())
    return;
  ShaderProgram program;
  CHECK(!program.build(kBroken, kFragment));
  CHECK(!program.valid());
  CHECK(glGetError() == GL_NO_ERROR);
}

TEST(shader_failed_rebuild_keeps_old_program) {
  GlFixture f;
  if (!f.ok())
    return;
  ShaderProgram program;
  CHECK(program.build(kVertex, kFragment));
  CHECK(!program.build(kVertex, kBroken));
  CHECK(program.valid());
}

TEST(shader_move_transfers_ownership) {
  GlFixture f;
  if (!f.ok())
    return;
  ShaderProgram first;
  CHECK(first.build(kVertex, kFragment));
  ShaderProgram second = std::move(first);
  CHECK(!first.valid());
  CHECK(second.valid());
}

TEST(shader_move_assign_releases_source) {
  GlFixture f;
  if (!f.ok())
    return;
  ShaderProgram first;
  ShaderProgram second;
  CHECK(first.build(kVertex, kFragment));
  CHECK(second.build(kVertex, kFragment));
  first = std::move(second);
  CHECK(first.valid());
  CHECK(!second.valid());
  CHECK(glGetError() == GL_NO_ERROR);
}

TEST(shader_uniform_setters_raise_no_errors) {
  GlFixture f;
  if (!f.ok())
    return;
  ShaderProgram program;
  CHECK(program.build(kVertex, kFragment));
  program.use();
  program.set_vec4("u_color", 0.25f, 0.5f, 0.75f, 1.0f);
  CHECK(program.uniform("u_color") == program.uniform("u_color"));
  CHECK(program.uniform("u_missing") == -1);
  CHECK(glGetError() == GL_NO_ERROR);
}
