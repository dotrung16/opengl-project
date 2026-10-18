#include "shader.hpp"

#include <glad/gl.h>

#include <cstdio>
#include <utility>

namespace {

GLuint compile(GLenum type, const char* source) {
  const GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint ok = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (ok != GL_TRUE) {
    char log[1024] = {};
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    std::fprintf(stderr, "%s shader error:\n%s\n",
                 type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", log);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

} // namespace

ShaderProgram::~ShaderProgram() {
  if (id_ != 0) glDeleteProgram(id_);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
  : id_(std::exchange(other.id_, 0)) {}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
  std::swap(id_, other.id_);
  return *this;
}

bool ShaderProgram::build(const char* vertex_source, const char* fragment_source) {
  const GLuint vertex = compile(GL_VERTEX_SHADER, vertex_source);
  const GLuint fragment = compile(GL_FRAGMENT_SHADER, fragment_source);

  if (vertex == 0 || fragment == 0) {
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return false;
  }

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);
  glDeleteShader(vertex);
  glDeleteShader(fragment);

  GLint ok = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &ok);
  if (ok != GL_TRUE) {
    char log[1024] = {};
    glGetProgramInfoLog(program, sizeof(log), nullptr, log);
    std::fprintf(stderr, "Shader link error:\n%s\n", log);
    glDeleteProgram(program);
    return false;
  }

  if (id_ != 0) glDeleteProgram(id_);
  id_ = program;
  return true;
}

void ShaderProgram::use() const {
  glUseProgram(id_);
}

int ShaderProgram::uniform(const char* name) const {
  return glGetUniformLocation(id_, name);
}