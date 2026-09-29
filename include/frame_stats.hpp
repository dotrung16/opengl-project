#pragma once

#include <optional>

struct WindowState;

struct FrameReport {
  double span = 0.0;
  double fps = 0.0;
  double avg_frame_ms = 0.0;
  double wakeups_per_sec = 0.0;
  double max_wait_ms = 0.0;
  double max_swap_ms = 0.0;
  bool focused = false;
  bool event_driven = false;
};

void print_report(const FrameReport& report);

class FrameStats {
 public:
  static constexpr double kInterval = 3.0;

  FrameStats(double now, const WindowState& state) { reset(now, state); }

  void reset(double now, const WindowState& state);
  void record_wakeup(double wait_time);
  void record_frame() { ++frames_; }
  void record_swap(double swap_time);
  std::optional<FrameReport> report_if_due(double now, const WindowState& state);
  double next_report() const { return window_start_ + kInterval; }

 private:
  double window_start_ = 0.0;
  int frames_ = 0;
  int wakeups_ = 0;
  double max_wait_ = 0.0;
  double max_swap_ = 0.0;
  bool focused_ = false;
  bool event_driven_ = false;
};
