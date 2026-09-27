#pragma once

class Renderer {
 public:
  bool init();

  int version_major() const;
  int version_minor() const;
  const char* device_name() const;

  void draw(int width, int height);

 private:
  int version_ = 0;
  int viewport_width_ = 0;
  int viewport_height_ = 0;
};
