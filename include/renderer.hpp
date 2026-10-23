#pragma once

class Renderer {
public:
  Renderer() = default;
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  bool init();

  int version_major() const { return major_; }
  int version_minor() const { return minor_; }
  const char *device_name() const;

  void draw(int width, int height);

private:
  int major_ = 0;
  int minor_ = 0;
};
