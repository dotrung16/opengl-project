#pragma once

class Renderer {
public:
  Renderer() = default;
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  bool init();

  int version_major() const;
  int version_minor() const;
  const char *device_name() const;

  void draw(int width, int height);

private:
  int version_ = 0;
};
