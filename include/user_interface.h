#ifndef PART0_H
#define PART0_H

#include "application.h"
#include "utils.h"

extern int num;

typedef struct UserInputHandler {
  Object super;
  char in_buffer[255];
  int buf_index;
} UserInputHandler;

void parse_user_input(UserInputHandler *userInputHandler, int c);
void unset_failure(UserInputHandler *self, int _);

#endif