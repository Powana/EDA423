#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H
#include "application.h"
#include "canTinyTimber.h"
#include "can_interface.h"
void can_regulator(App *self, int _);
void deliver_next_can_msg_in_q(App *self, int _);

#endif