#ifndef APPLICATION_H
#define APPLICATION_H

#include <stdio.h>
#include <stdlib.h>

#include "TinyTimber.h"
#include "canTinyTimber.h"
#include "sciTinyTimber.h"

#include "user_interface.h"

// Distrubance freq 384Hz = 1300
#define T_1000_Hz 500
#define T_769_Hz 650
#define T_537_Hz 931
#define T_384_Hz 1300
#define VOLUME 3
#define DEFAULT_TEMPO 120
#define DEFAULT_KEY 0
#define NODE_ID 3
#define MAX_NETWORK_SIZE 3
#define CONDUCTOR_CLASH_MS 500
#define MAX_CAN_QUEUE_SIZE 256

typedef struct {
  uchar msgId;
  Time arrival_time;
} MiniMsg;

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
  // === Problem 5 === 
  int send_can_to_queue;
  MiniMsg can_queue[MAX_CAN_QUEUE_SIZE];
  int can_queue_start;
  int can_queue_end;
  int can_queue_size;
  int can_cooldown_active;
  int can_msg_min_interval_ms;
  Timer app_start_time;
  int print_can_tx;
  // ================
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

extern Serial sci0;

extern Can can0;


#endif
