#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <stdlib.h>

#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "sioTinyTimber.h"

#include "music_player.h"
#include "can_interface.h"
#include "tone_generator.h"
#include "user_interface.h"

// Distrubance freq 384Hz = 1300
#define T_1000_Hz 500
#define T_769_Hz 650
#define T_537_Hz 931
#define T_384_Hz 1300
#define VOLUME 12
#define DEFAULT_TEMPO 120
#define DEFAULT_KEY 0

typedef struct {
  Object super;
  Timer arrival_timer;
  Timer hold_timer;
  int mode;
  int user_button_mode;
  int trigger_mode;
  int inter_arrival_times[3];
  int tap_count;
  int bounce_flag;
  int rank;
  int ranks[2];
  int conductor;
} App;

extern App app;

extern int base_freq_indices[32];
extern int periods[25];
extern char note_lengths[32];
extern int min_index;
extern int max_index;
extern int mode;
extern int network_size;
extern int pending_conductor;
extern int evaling_conductor;

void reader(App *, int);
void receiver(App *, int);
void sio_receive(App *self, int val);
void switch_to_held_mode(App *self, int _);
void button_press_logic(App *self, int _);
void button_release_logic(App *self, int _);
void switch_conductor(App *self, int _);

extern Serial sci0;

extern Can can0;

extern SysIO sio0;

#endif
