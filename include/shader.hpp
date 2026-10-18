#pragma once

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

private:
  unsigned int id_ = 0;
};
