#include "frame_stats.hpp"

#include "window.hpp"

#include <algorithm>
#include <cstdio>

void FrameStats::reset(double now, const WindowState& state) {
  window_start_ = now;
  frames_ = 0;
  wakeups_ = 0;
  max_wait_ = 0.0;
  max_swap_ = 0.0;
  focused_ = state.focused;
  event_driven_ = state.event_driven;
}

void FrameStats::record_wakeup(double wait_time) {
  ++wakeups_;
  max_wait_ = std::max(max_wait_, wait_time);
}

void FrameStats::record_swap(double swap_time) {
  max_swap_ = std::max(max_swap_, swap_time);
}

void FrameStats::report_if_due(double now, const WindowState& state) {
  const double elapsed = now - window_start_;
  const bool changed = state.focused != focused_ || state.event_driven != event_driven_;
  if (!changed && elapsed < kStatsInterval) return;

  if (elapsed > 0.0) {
    const double avg_frame_ms = frames_ > 0 ? elapsed * 1000.0 / frames_ : 0.0;
    std::printf("[Power Stats] Span: %.1f s | FPS: %.1f | Avg frame: %.2f ms | "
                "Wakeups/s: %.1f | Max wait: %.1f ms | Max swap: %.1f ms | "
                "Focused: %s | Mode: %s\n",
                elapsed, frames_ / elapsed, avg_frame_ms, wakeups_ / elapsed,
                max_wait_ * 1000.0, max_swap_ * 1000.0,
                focused_ ? "Yes" : "No",
                event_driven_ ? "event-driven" : "continuous");
  }
  reset(now, state);
}
