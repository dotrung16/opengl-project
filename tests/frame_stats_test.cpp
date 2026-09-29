#include "frame_stats.hpp"
#include "window.hpp"

#include "test.hpp"

namespace {
constexpr double kEps = 1e-9;
}

TEST(stats_no_report_before_interval) {
  WindowState state;
  FrameStats stats(0.0, state);
  stats.record_frame();
  CHECK(!stats.report_if_due(FrameStats::kInterval - 0.1, state));
}

TEST(stats_report_at_interval) {
  WindowState state;
  state.focused = true;
  state.event_driven = false;
  FrameStats stats(10.0, state);
  for (int i = 0; i < 180; ++i) stats.record_frame();
  for (int i = 0; i < 90; ++i) stats.record_wakeup(0.001);
  stats.record_wakeup(0.004);
  stats.record_swap(0.002);

  const auto report = stats.report_if_due(10.0 + FrameStats::kInterval, state);
  CHECK(report.has_value());
  if (!report) return;
  CHECK_NEAR(report->span, FrameStats::kInterval, kEps);
  CHECK_NEAR(report->fps, 60.0, kEps);
  CHECK_NEAR(report->avg_frame_ms, 1000.0 / 60.0, kEps);
  CHECK_NEAR(report->wakeups_per_sec, 91.0 / FrameStats::kInterval, kEps);
  CHECK_NEAR(report->max_wait_ms, 4.0, kEps);
  CHECK_NEAR(report->max_swap_ms, 2.0, kEps);
  CHECK(report->focused);
  CHECK(!report->event_driven);
}

TEST(stats_report_resets_window) {
  WindowState state;
  FrameStats stats(0.0, state);
  stats.record_frame();
  CHECK(stats.report_if_due(FrameStats::kInterval, state).has_value());
  CHECK_NEAR(stats.next_report(), 2 * FrameStats::kInterval, kEps);
  CHECK(!stats.report_if_due(FrameStats::kInterval + 1.0, state));
}

TEST(stats_mode_change_reports_early_with_previous_mode) {
  WindowState state;
  state.event_driven = true;
  FrameStats stats(0.0, state);
  stats.record_frame();

  state.event_driven = false;
  const auto report = stats.report_if_due(1.0, state);
  CHECK(report.has_value());
  if (!report) return;
  CHECK(report->event_driven);
  CHECK_NEAR(report->fps, 1.0, kEps);
}

TEST(stats_zero_span_change_resets_without_report) {
  WindowState state;
  state.focused = true;
  FrameStats stats(2.0, state);

  state.focused = false;
  CHECK(!stats.report_if_due(2.0, state));
  CHECK(!stats.report_if_due(2.5, state));
}

TEST(stats_no_frames_reports_zero_avg) {
  WindowState state;
  FrameStats stats(0.0, state);
  const auto report = stats.report_if_due(FrameStats::kInterval, state);
  CHECK(report.has_value());
  if (!report) return;
  CHECK_NEAR(report->fps, 0.0, kEps);
  CHECK_NEAR(report->avg_frame_ms, 0.0, kEps);
}
