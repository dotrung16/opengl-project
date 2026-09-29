#pragma once

#include "frame_schedule.hpp"

class Window;

class FramePacer {
 public:
  FramePacer();

  void wait(const Window& window, double idle_deadline);
  double wait_hidden();
  void update_swap_interval(bool focused);
  void mark_frame(double now) { schedule_.mark(now); }

 private:
  bool vsync_;
  double min_frame_time_;
  int swap_interval_;
  FrameSchedule schedule_;
};
