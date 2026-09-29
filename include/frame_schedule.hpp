#pragma once

class FrameSchedule {
 public:
  explicit FrameSchedule(double last_frame) : last_frame_(last_frame) {}

  double begin(double frame_time) {
    frame_time_ = frame_time;
    deadline_ = last_frame_ + frame_time_;
    return deadline_;
  }

  void begin_unpaced() {
    frame_time_ = 0.0;
    deadline_ = 0.0;
  }

  void mark(double now) {
    const bool on_schedule = now >= deadline_ && now - deadline_ < frame_time_;
    last_frame_ = on_schedule ? deadline_ : now;
  }

  void resync(double now) { last_frame_ = now; }

  double last_frame() const { return last_frame_; }

 private:
  double last_frame_;
  double deadline_ = 0.0;
  double frame_time_ = 0.0;
};
