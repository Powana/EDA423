#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include "application.h"

#define MIN_FREQ_INDEX -10
#define MAX_GAP_DURATION 50
#define MIN_GAP_DURATION 25

typedef struct {
  Object super;
  int key;
  int tempo;
  int note_idx;
  int is_playing;
  int is_led_blinking;
  // int cur_note_modulo;
  int nth_note_to_play;
  int current_note_segment;
  int force_mute;
} MusicPlayer;

void play_music(MusicPlayer *music_player, int _);
void stop_music(MusicPlayer *music_player, int _);
void play_next_note(MusicPlayer *music_player, int index);
void check_segment(MusicPlayer *MusicPlayer, int send_data);
void change_key(MusicPlayer *music_player, int key);
void change_tempo(MusicPlayer *music_player, int tempo);
void turn_led_off(MusicPlayer *self, int _);
void blink_led(MusicPlayer *self, int _);
void im_alive_ping(MusicPlayer *self, int _);
void update_nth_note_to_play(MusicPlayer *self, int _);
void fmute(MusicPlayer *music_player, int _);
void funmute(MusicPlayer *music_player, int _);



#endif
