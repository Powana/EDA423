#ifndef PART0_H
#define PART0_H

#include "application.h"
#include "utils.h"

extern int num;

typedef struct UserInputHandler {
  Object super;
  char in_buffer[16];
  int buf_index;
} UserInputHandler;

void parse_user_input(UserInputHandler *userInputHandler, int c);
void parse_can_input(UserInputHandler *self, int inputDigit);

#endif