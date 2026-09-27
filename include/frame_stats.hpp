#pragma once

struct WindowState;

inline constexpr double kStatsInterval = 3.0;

class FrameStats {
 public:
  FrameStats(double now, const WindowState& state) { reset(now, state); }

  void reset(double now, const WindowState& state);
  void record_wakeup(double wait_time);
  void record_frame() { ++frames_; }
  void record_swap(double swap_time);
  void report_if_due(double now, const WindowState& state);
  double next_report() const { return window_start_ + kStatsInterval; }

 private:
  double window_start_ = 0.0;
  int frames_ = 0;
  int wakeups_ = 0;
  double max_wait_ = 0.0;
  double max_swap_ = 0.0;
  bool focused_ = false;
  bool event_driven_ = false;
};
