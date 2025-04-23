#ifndef PART0_H
#define PART0_H

#include "application.h"
#include "utils.h"

extern int num;

typedef struct UserInputHandler {
  Object super;
  char in_buffer[255];
  int buf_index;
  uchar prob5_seq;
  int burst_active;
  Msg burst_msg;
} UserInputHandler;

void parse_user_input(UserInputHandler *userInputHandler, int c);
void burst_msg_sender(UserInputHandler *self, int _);

#endif