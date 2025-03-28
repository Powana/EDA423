#ifndef PART1_H
#define PART1_H
#include "application.h"
#include "utils.h"

typedef struct {
  Object super;
  int volume;
  int user_mute;
  int mute;
  int T_half_us;
  int use_deadline;
  int note_duration;
  int is_playing;
} Tone_CTRL;

typedef struct {
  Object super;
  int background_loop_range;
  int use_deadline;
} Distortion;

void set_period(Tone_CTRL *, int);
void tone_gen(Tone_CTRL *, int);
void play_tone(Tone_CTRL *tone_ctrl, int _);
void stop_tone(Tone_CTRL *tone_ctrl, int _);

void toggle_user_mute(Tone_CTRL *, int);
void mute_tone(Tone_CTRL *tone_ctrl, int _);
void unmute_tone(Tone_CTRL *tone_ctrl, int _);
void adjust_volume(Tone_CTRL *tone_ctrl, int step);

void toggle_deadline(Tone_CTRL *, int);

void distort(Distortion *, int);
void load_control(Distortion *, int);
void toggle_distort_deadline(Distortion *, int);

#endif
