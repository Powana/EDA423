#include "music_player.h"
#include "tone_generator.h"

extern Tone_CTRL tone_ctrl;

void play_music(MusicPlayer *music_player, int _) {
  if (music_player->is_playing) {
    print("Tone is already playing", 0);
    return;
  }
  music_player->is_playing = 1;

  ASYNC(&tone_ctrl, play_tone, 0);
  ASYNC(music_player, play_next_note, 0);

  blink_led(music_player, 0);
}

void stop_music(MusicPlayer *music_player, int _) {
  music_player->is_playing = 0;
  ASYNC(&tone_ctrl, turn_led_off, 0);
  ASYNC(&tone_ctrl, stop_tone, 0);
}

void play_next_note(MusicPlayer *music_player, int index) {
  if (!music_player->is_playing) {
    return;
  }

  if (index > 31)
    index = 0;
  music_player->note_idx = index;
  int half_period =
      periods[base_freq_indices[index] - MIN_FREQ_INDEX + music_player->key];

  float note_length;
  switch (note_lengths[index]) {
  case 'a':
    note_length = 1.0;
    break;
  case 'b':
    note_length = 2.0;
    break;
  case 'c':
    note_length = 0.5;
    break;
  }

  int note_duration_ms = (60000.0 / music_player->tempo) * note_length;
  SYNC(&tone_ctrl, set_period, half_period);
  

  // UNMUTE
  if (music_player->cur_note_modulo == music_player->nth_note_to_play) {
    SEND(0, 0, &tone_ctrl, unmute_tone, 0);
    SEND(0,0, music_player, im_alive_ping, 8*note_length);
  } else {
    SEND(0,0, music_player, im_alive_ping, -8*note_length);
  }

  // MUTE
  SEND(MSEC(note_duration_ms - 75),
    MSEC(note_duration_ms - 75), &tone_ctrl, mute_tone, 0);


  // RECURSIVE CALL
  SEND(MSEC(note_duration_ms), 0, music_player, play_next_note, index + 1);
  music_player->cur_note_modulo = (music_player->cur_note_modulo + 1) % network_size;
  music_player->current_note_segment = 0;
}

void update_note(MusicPlayer *music_player, int _) {
  music_player->cur_note_modulo = (music_player->cur_note_modulo + 1) % network_size;
  music_player->current_note_segment = 0;
}
void change_key(MusicPlayer *music_player, int key) { music_player->key = key; }

void change_tempo(MusicPlayer *music_player, int tempo) { music_player->tempo = tempo; }

void im_alive_ping(MusicPlayer *music_player, int count) {
  if (!music_player->is_playing || count == 0) {
    return;
  }
  unsigned char note = music_player->note_idx;
  CANMsg msg = {8, 3, 0, {0,0,0,0}};
  int new_count = count;
  if (count > 0) {
    
    msg.length = 2;
    msg.buff[0] = note;
    msg.buff[1] = music_player->current_note_segment; 
    new_count--;
  } else {
    new_count++;
  }
  
  music_player->current_note_segment++;
  
  CAN_SEND(&can0, &msg); 
  
  SEND(SEC(60.0 / music_player->tempo)/ 8, 0, music_player, im_alive_ping, new_count);
}


void blink_led(MusicPlayer *self, int _) {
  if (!self->is_playing) {
    SIO_WRITE(&sio0, 1);
    return;
  }

  SIO_WRITE(&sio0, 0); // 0 turns the LED on
  
  // Schedule turning the LED off in the middle of the beat
  Time beat_length = MSEC(60000 / self->tempo);
  AFTER(beat_length / 2, self, turn_led_off, 0);
  
  // Schedule the next blink at the beginning of the next beat
  AFTER(beat_length, self, blink_led, 0);
}

void turn_led_off(MusicPlayer *self, int _) {
  SIO_WRITE(&sio0, 1);
}
