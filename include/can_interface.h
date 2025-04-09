#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H
#include "application.h"
#include "canTinyTimber.h"
#include "music_player.h"
void parse_can_input(CANMsg *msg, int conductor);

#endif