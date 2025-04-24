#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <stdlib.h>

#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"
#include "sioTinyTimber.h"

#include "music_player.h"
#include "tone_generator.h"
#include "user_interface.h"

// Distrubance freq 384Hz = 1300
#define T_1000_Hz 500
#define T_769_Hz 650
#define T_537_Hz 931
#define T_384_Hz 1300
#define VOLUME 8
#define DEFAULT_TEMPO 120
#define DEFAULT_KEY 0
#define NODE_ID 3
#define MAX_NETWORK_SIZE 3
#define CONDUCTOR_CLASH_MS 200
#define MAX_NODE_RANK 15
#define MIN_MISSED_CONS_HEARTBEATS 3
#define HEARTBEAT_INTERVAL_MS 25

typedef struct {
  Object super;
  Timer arrival_timer;
  Timer hold_timer;
  int user_button_mode;
  int trigger_mode;
  int inter_arrival_times[3];
  int tap_count;
  int bounce_flag;
  int rank;
  int ranks[MAX_NETWORK_SIZE];
  int network_size;
  int conductor;
  int evaling_conductor;
  int simulate_silent_fail;
  int can_connected;
  int recvd_heartbeats[MAX_NODE_RANK];
} App;

extern App app;

#include "can_interface.h"

extern int base_freq_indices[32];
extern int periods[25];
extern char note_lengths[32];
extern int min_index;
extern int max_index;
extern int mode;
extern int pending_conductor;

void reader(App *, int);
void receiver(App *, int);
void sio_receive(App *self, int val);
void switch_to_held_mode(App *self, int _);
void button_press_logic(App *self, int _);
void button_release_logic(App *self, int _);
void switch_conductor(App *self, int _);
void heartbeat(App *self, int _);

extern Serial sci0;

extern Can can0;

extern SysIO sio0;

#endif
