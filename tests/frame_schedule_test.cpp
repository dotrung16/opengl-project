#include "frame_schedule.hpp"

#include "test.hpp"

namespace {
constexpr double kEps = 1e-9;
constexpr double kFrame = 0.016;
}

TEST(schedule_deadline_is_last_frame_plus_frame_time) {
  FrameSchedule schedule(1.0);
  CHECK_NEAR(schedule.begin(kFrame), 1.0 + kFrame, kEps);
}

TEST(schedule_on_time_frames_do_not_drift) {
  FrameSchedule schedule(0.0);
  const double deadline = schedule.begin(kFrame);
  schedule.mark(deadline + 0.002);
  CHECK_NEAR(schedule.last_frame(), deadline, kEps);
  CHECK_NEAR(schedule.begin(kFrame), 2 * kFrame, kEps);
}

TEST(schedule_resyncs_after_stall) {
  FrameSchedule schedule(0.0);
  const double deadline = schedule.begin(kFrame);
  const double late = deadline + 1.5 * kFrame;
  schedule.mark(late);
  CHECK_NEAR(schedule.last_frame(), late, kEps);
}

TEST(schedule_early_frame_uses_actual_time) {
  FrameSchedule schedule(0.0);
  const double deadline = schedule.begin(kFrame);
  const double early = deadline - 0.005;
  schedule.mark(early);
  CHECK_NEAR(schedule.last_frame(), early, kEps);
}

TEST(schedule_unpaced_frames_track_actual_time) {
  FrameSchedule schedule(0.0);
  schedule.begin(kFrame);
  schedule.begin_unpaced();
  schedule.mark(kFrame);
  CHECK_NEAR(schedule.last_frame(), kFrame, kEps);
  schedule.mark(5.0);
  CHECK_NEAR(schedule.last_frame(), 5.0, kEps);
}

TEST(schedule_resync_moves_next_deadline) {
  FrameSchedule schedule(0.0);
  schedule.resync(10.0);
  CHECK_NEAR(schedule.begin(kFrame), 10.0 + kFrame, kEps);
}
