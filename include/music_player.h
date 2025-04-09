#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include "application.h"
#include "utils.h"

#define MIN_FREQ_INDEX -10
#define MAX_GAP_DURATION 50
#define MIN_GAP_DURATION 25

typedef struct {
  Object super;
  int key;
  int tempo;
  int is_playing;
  int is_led_blinking;
} MusicPlayer;

void play_music(MusicPlayer *music_player, int _);
void stop_music(MusicPlayer *music_player, int _);
void play_next_note(MusicPlayer *music_player, int index);
void change_key(MusicPlayer *music_player, int key);
void change_tempo(MusicPlayer *music_player, int tempo);
void turn_led_off(MusicPlayer *self, int _);
void blink_led(MusicPlayer *self, int _);

#endif
