#include "user_interface.h"
#include "application.h"
#include "canTinyTimber.h"
#include "music_player.h"
#include "tone_generator.h"
#include <string.h>

#define USE_CAN_ONLY 0

int num = 0;
int i;

CANMsg msg = {.msgId = -1, .nodeId = NODE_ID, .length=0, .buff={0}}; // TODO Un-hardcode rank 
extern MusicPlayer music_player;
extern Tone_CTRL tone_ctrl;
extern App app;

void parse_user_input(UserInputHandler *self, int inputDigit) {
  switch ((char)inputDigit) {
  case 'o':
    if (app.conductor != app.rank) {
      print("Requesting conductor mode\n", 0);
      if (!(app.evaling_conductor)) {
        AFTER(MSEC(200), &app, switch_conductor, 0);
      }
      pending_conductor = NODE_ID;
      app.evaling_conductor = 1;
      msg.msgId = 2;
      CAN_SEND(&can0, &msg);
    }
    /* TODO: Discuss what happens when the conductor node tries to swap to musician, does another node have to take over?
    if (app.conductor == app.rank)
      print("You are in musician mode", 0);
    break;
    */
    break;
  case 'k':  // CHANGE KEY
    self->in_buffer[self->buf_index] = '\0';
    num = atoi(self->in_buffer);
    num = num > 5 ? 5 : (num < -5 ? -5 : num);
    self->buf_index = 0;

    if (app.conductor == app.rank && !USE_CAN_ONLY)
      ASYNC(&music_player, change_key, num);

    if(app.rank == app.conductor) { // Only send CAN msgs when conductor
      msg.msgId = 6;
      msg.length = 1;
      msg.buff[0] = num + 5;
      print("Send Key: %d\n", num+5);
      CAN_SEND(&can0, &msg);
    }
    break;
  case 't':
    self->in_buffer[self->buf_index] = '\0';
    self->buf_index = 0;
    if (app.conductor != app.rank) return;

    num = atoi(self->in_buffer);
    num = num > 300 ? 300 : (num < 30 ? 30 : num);
    print("New Tempo set to: %d\n", num);
    if (!USE_CAN_ONLY)
      ASYNC(&music_player, change_tempo, num);

    msg.msgId = 5;
    msg.length = 2;
    msg.buff[0] = num & 0x00ff;
    msg.buff[1] = (num >> 8) & 0xff;
    CAN_SEND(&can0, &msg);
    break;
  case 'm':
    ASYNC(&tone_ctrl, toggle_user_mute, 0);
    // msg.length = 1;
    // msg.buff[0] = 'm';
    // CAN_SEND(&can0, &msg);
    break;
  case 'i':
    ASYNC(&tone_ctrl, adjust_volume, 1);
    //msg.length = 1;
    //msg.buff[0] = 'i';
    //CAN_SEND(&can0, &msg);
    break;
  case 'd':
    ASYNC(&tone_ctrl, adjust_volume, -1);
    // msg.length = 1;
    // msg.buff[0] = 'd';
    // CAN_SEND(&can0, &msg);
    break;
  case 'p':
    if (app.conductor != app.rank) return;
    if (!USE_CAN_ONLY) ASYNC(&music_player, play_music, 0);
    msg.msgId = 0;
    msg.length = 0;
    CAN_SEND(&can0, &msg);

    msg.msgId = 3;
    msg.length = 0;
    CAN_SEND(&can0, &msg);
    break;
  case 's':
    if (app.conductor != app.rank) return;
    if (!USE_CAN_ONLY) ASYNC(&music_player, stop_music, 0);
  
    msg.length = 0;
    msg.msgId = 4;
    CAN_SEND(&can0, &msg);
    break;
  default:
    self->in_buffer[self->buf_index++] = inputDigit;
    break;
  }
}

