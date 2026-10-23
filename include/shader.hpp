#pragma once

#include <string>
#include <unordered_map>

class ShaderProgram {
public:
  ShaderProgram() = default;
  ~ShaderProgram();
  ShaderProgram(ShaderProgram &&other) noexcept;
  ShaderProgram &operator=(ShaderProgram &&other) noexcept;
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;

  bool build(const char *vertex_source, const char *fragment_source);
  void use() const;
  int uniform(const char *name) const;
  bool valid() const { return id_ != 0; }

  void set_int(const char *name, int value) const;
  void set_float(const char *name, float value) const;
  void set_vec4(const char *name, float x, float y, float z, float w) const;
  void set_mat4(const char *name, const float *matrix) const;

private:
  unsigned int id_ = 0;
  mutable std::unordered_map<std::string, int> uniform_cache_;
};
