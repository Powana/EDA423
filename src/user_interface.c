#include "user_interface.h"
#include "application.h"
#include "canTinyTimber.h"
#include "music_player.h"
#include "tone_generator.h"
#include <string.h>

#define USE_CAN_ONLY 0

int num = 0;
int i;

extern MusicPlayer music_player;
extern Tone_CTRL tone_ctrl;
extern App app;

void burst_msg_sender(UserInputHandler *self, int _) {
  if (!self->burst_active) return;
  CANMsg burst_msg;
  burst_msg.msgId = self->prob5_seq;
  if (app.print_can_tx) print("Transmitting A: %d\n", self->prob5_seq);
  self->prob5_seq = self->prob5_seq == 127 ? 0 : self->prob5_seq + 1;

  CAN_SEND(&can0, &burst_msg);
  if (app.print_can_tx) print("Transmitting CAN msg with ID: %d\n", burst_msg.msgId);

  self->burst_msg = SEND(MSEC(500), 0, self, burst_msg_sender, 0);
}

void parse_user_input(UserInputHandler *self, int inputDigit) {
  switch ((char)inputDigit) {
  case 'z':
    print("Our node: %d ", NODE_ID);
    print("is cond: %d\n", app.conductor == NODE_ID);
    for (int i=0; i<app.network_size-1; i++) {
      print("Node: %d ", app.ranks[i]);
      print("is cond: %d\n", app.conductor == app.ranks[i]);
    }
    break;
  
  case 'B':  // Prob 5, Burst
    print("Starting Burst\n", 0);
    self->burst_active = 1;
    self->burst_msg = ASYNC(self, burst_msg_sender, 0);
    break;
  
  case 'X': // Stop burst
    print("Stopping Burst\n", 0);
    self->burst_active = 0;
    ABORT(self->burst_msg);
    break;
  
  case 'I': // Set can msg interval, in s, min 1.
    self->in_buffer[self->buf_index] = '\0';
    num = atoi(self->in_buffer);
    num = num ? num : 1; // Set num to 1 if is zero
    self->buf_index = 0;
    print("Setting CAN interval to %d (s)\n", num);
    app.can_msg_min_interval_ms = num*1000; 
    break;

  case 'P':
    app.print_can_tx = !app.print_can_tx;
    break;
  default:
    self->in_buffer[self->buf_index++] = inputDigit;
    break;
  }
}

