#include "shader.hpp"

#include <glad/gl.h>

#include <cstddef>
#include <cstdio>
#include <utility>

namespace {

const char *stage_name(GLenum type) {
  switch (type) {
  case GL_VERTEX_SHADER:
    return "Vertex";
  case GL_FRAGMENT_SHADER:
    return "Fragment";
  case GL_GEOMETRY_SHADER:
    return "Geometry";
  default:
    return "Unknown";
  }
}

std::string shader_log(GLuint shader) {
  GLint length = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
  if (length <= 0)
    return {};

  std::string log(static_cast<std::size_t>(length), '\0');
  GLsizei written = 0;
  glGetShaderInfoLog(shader, length, &written, log.data());
  log.resize(static_cast<std::size_t>(written));

  return log;
}

std::string program_log(GLuint program) {
  GLint length = 0;
  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
  if (length <= 0)
    return {};

  std::string log(static_cast<std::size_t>(length), '\0');
  GLsizei written = 0;
  glGetProgramInfoLog(program, length, &written, log.data());
  log.resize(static_cast<std::size_t>(written));

  return log;
}

GLuint compile(GLenum type, const char *source) {
  const GLuint shader = glCreateShader(type);
  if (shader == 0)
    return 0;
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  GLint ok = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);

  if (ok != GL_TRUE) {
    std::fprintf(stderr, "%s shader error:\n%s\n", stage_name(type),
                 shader_log(shader).c_str());
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

} // namespace

ShaderProgram::~ShaderProgram() {
  if (id_ != 0)
    glDeleteProgram(id_);
}

ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept
    : id_(std::exchange(other.id_, 0)),
      uniform_cache_(std::move(other.uniform_cache_)) {
  other.uniform_cache_.clear();
}

ShaderProgram &ShaderProgram::operator=(ShaderProgram &&other) noexcept {
  if (this != &other) {
    if (id_ != 0) {
      glDeleteProgram(id_);
    }
    id_ = std::exchange(other.id_, 0);
    uniform_cache_ = std::move(other.uniform_cache_);
    other.uniform_cache_.clear();
  }
  return *this;
}

bool ShaderProgram::build(const char *vertex_source,
                          const char *fragment_source) {
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
    std::fprintf(stderr, "Shader link error:\n%s\n",
                 program_log(program).c_str());
    glDeleteProgram(program);
    return false;
  }

  if (id_ != 0)
    glDeleteProgram(id_);
  id_ = program;
  uniform_cache_.clear();

  return true;
}

void ShaderProgram::use() const { glUseProgram(id_); }

int ShaderProgram::uniform(const char *name) const {
  if (const auto it = uniform_cache_.find(name); it != uniform_cache_.end())
    return it->second;
  const int location = glGetUniformLocation(id_, name);
  uniform_cache_.emplace(name, location);

  return location;
}

void ShaderProgram::set_int(const char *name, int value) const {
  glUniform1i(uniform(name), value);
}

void ShaderProgram::set_float(const char *name, float value) const {
  glUniform1f(uniform(name), value);
}

void ShaderProgram::set_vec4(const char *name, float x, float y, float z,
                             float w) const {
  glUniform4f(uniform(name), x, y, z, w);
}

void ShaderProgram::set_mat4(const char *name, const float *matrix) const {
  glUniformMatrix4fv(uniform(name), 1, GL_FALSE, matrix);
}
