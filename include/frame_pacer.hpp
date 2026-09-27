#pragma once

class Window;

class FramePacer {
 public:
  FramePacer();

  void wait(const Window& window, double idle_deadline);
  double wait_hidden();
  void update_swap_interval(bool focused);
  void mark_frame(double now);

 private:
  bool vsync_;
  double min_frame_time_;
  double last_frame_;
  int swap_interval_;
  double deadline_ = 0.0;
  double frame_time_ = 0.0;
};
