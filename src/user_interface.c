#include "user_interface.h"
#include "application.h"
#include "canTinyTimber.h"
#include "music_player.h"
#include "tone_generator.h"
#include <string.h>

int num = 0;
int i;

CANMsg msg = {0, 0, 0};

extern MusicPlayer music_player;
extern Tone_CTRL tone_ctrl;
extern App app;

void parse_user_input(UserInputHandler *self, int inputDigit) {
  switch ((char)inputDigit) {
  case 'o':
    app.mode = !app.mode;
    if (app.mode == 0)
      print("You are in conductor app.mode", 0);
    if (app.mode == 1)
      print("You are in musician app.mode", 0);
    break;
  case 'k':
    self->in_buffer[self->buf_index] = '\0';
    num = atoi(self->in_buffer);
    num = num > 5 ? 5 : (num < -5 ? -5 : num);
    print("Key: %d\n", num);

    if (app.mode == 0)
      ASYNC(&music_player, change_key, num);

    // if(app.mode == 1) Only send CAN msgs in Conductor app.mode
    msg.length = self->buf_index + 1;
    for (i = 0; i < self->buf_index; i++)
      msg.buff[i] = self->in_buffer[i];
    msg.buff[i] = 'k';

    self->buf_index = 0;
    CAN_SEND(&can0, &msg);
    break;
  case 't':
    self->in_buffer[self->buf_index] = '\0';
    num = atoi(self->in_buffer);
    num = num > 300 ? 300 : (num < 30 ? 30 : num);
    print("New Tempo set to: %d\n", num);
    if (app.mode == 0)
      ASYNC(&music_player, change_tempo, num);

    // if(app.mode == 1) Only send CAN msgs in Conductor app.mode
    msg.length = self->buf_index + 1;
    for (i = 0; i < self->buf_index; i++)
      msg.buff[i] = self->in_buffer[i];
    msg.buff[i] = 't';
    self->buf_index = 0;
    CAN_SEND(&can0, &msg);
    break;
  case 'm':
    if (app.mode == 0)
      ASYNC(&tone_ctrl, toggle_user_mute, 0);
    msg.length = 1;
    msg.buff[0] = 'm';
    CAN_SEND(&can0, &msg);
    break;
  case 'i':
    if (app.mode == 0)
      ASYNC(&tone_ctrl, adjust_volume, 1);
    msg.length = 1;
    msg.buff[0] = 'i';
    CAN_SEND(&can0, &msg);
    break;
  case 'd':
    if (app.mode == 0)
      ASYNC(&tone_ctrl, adjust_volume, -1);
    msg.length = 1;
    msg.buff[0] = 'd';
    CAN_SEND(&can0, &msg);
    break;
  case 'p':
    if (app.mode == 0)
      ASYNC(&music_player, play_music, 0);
    msg.length = 1;
    msg.buff[0] = 'p';
    //CAN_SEND(&can0, &msg);
    break;
  case 's':
    if (app.mode == 0)
      ASYNC(&music_player, stop_music, 0);
    msg.length = 1;
    msg.buff[0] = 's';
    CAN_SEND(&can0, &msg);
    break;
  default:
    self->in_buffer[self->buf_index++] = inputDigit;
    break;
  }
}

void parse_can_input(UserInputHandler *self, int inputDigit) {
  switch ((char)inputDigit) {
  case 'k':
    self->in_buffer[self->buf_index] = '\0';
    num = atoi(self->in_buffer);
    self->buf_index = 0;
    num = num > 5 ? 5 : (num < -5 ? -5 : num);
    print("Key: %d\n", num);
    ASYNC(&music_player, change_key, num);
    break;
  case 't':
    self->in_buffer[self->buf_index] = '\0';
    num = atoi(self->in_buffer);
    self->buf_index = 0;
    num = num > 240 ? 240 : (num < 60 ? 60 : num);
    print("Tempo: %d\n", num);
    ASYNC(&music_player, change_tempo, num);
    break;
  case 'm':
    ASYNC(&tone_ctrl, toggle_user_mute, 0);
    break;
  case 'i':
    ASYNC(&tone_ctrl, adjust_volume, 1);
    break;
  case 'd':
    ASYNC(&tone_ctrl, adjust_volume, -1);
    break;
  case 'p':
    ASYNC(&music_player, play_music, 0);
    break;
  case 's':
    ASYNC(&music_player, stop_music, 0);
    break;
  default:
    self->in_buffer[self->buf_index++] = inputDigit;
    break;
  }
}
