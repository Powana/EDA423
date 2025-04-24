#include "tone_generator.h"
#include "application.h"

#define DAC_PORT 0x4000741C
long unsigned int *const dac_ptr = (long unsigned int *)DAC_PORT;

void play_tone(Tone_CTRL *tone_ctrl, int _) {
  tone_ctrl->is_playing = 1;
  ASYNC(tone_ctrl, tone_gen, VOLUME);
}

void stop_tone(Tone_CTRL *tone_ctrl, int _) {
  tone_ctrl->is_playing = 0;
}

void tone_gen(Tone_CTRL *tone_ctrl, int num) {
  if (!tone_ctrl->is_playing) return;

  *dac_ptr = num;
  num = (num != 0 || tone_ctrl->mute || tone_ctrl->user_mute)
            ? 0
            : tone_ctrl->volume;
  SEND(USEC(tone_ctrl->T_half_us), MSEC(500), tone_ctrl, tone_gen, num); 
}

void set_note_duration(Tone_CTRL *tone_ctrl, int duration) {
  tone_ctrl->note_duration = duration;
}

void toggle_user_mute(Tone_CTRL *tone_ctrl, int _) {
  tone_ctrl->user_mute = !tone_ctrl->user_mute;
  tone_ctrl->user_mute == 1 ? print("Muted\n", 0) : print("Unmuted\n", 0);
}

void mute_tone(Tone_CTRL *tone_ctrl, int _) { tone_ctrl->mute = 1; }

void unmute_tone(Tone_CTRL *tone_ctrl, int _) { tone_ctrl->mute = 0; }

void adjust_volume(Tone_CTRL *tone_ctrl, int step) {
  if (tone_ctrl->volume + step < 0 || tone_ctrl->volume + step > 20) {
    print("Volume out of bounds: %d", tone_ctrl->volume + step);
    return;
  }
  tone_ctrl->volume += step;
  print("Volume is now %d\n", tone_ctrl->volume);
}

void set_period(Tone_CTRL *tone_ctrl, int t) {
  /*print("Period changed to %dus", t);*/
  tone_ctrl->T_half_us = t;
}
