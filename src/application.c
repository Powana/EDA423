#include "application.h"

#define PRESS_MOMENTARY 0
#define PRESS_AND_HOLD 1

App app = {initObject(), initTimer(), initTimer(), .user_button_mode=0, .trigger_mode=0, .inter_arrival_times={}, .tap_count=0, .bounce_flag=0, .rank=NODE_ID, .ranks={}, .network_size=1, .conductor=-1, .evaling_conductor=0, .can_queue={}, .can_queue_start=0, .can_queue_end=MAX_CAN_QUEUE_SIZE-1, .can_queue_size=0, .can_cooldown_active=0, .can_msg_min_interval_ms=1000, .print_can_tx=0};
UserInputHandler userInputHandler = {initObject(), {}, 0, 0, 0};

Can can0 = initCan(CAN_PORT0, &app, can_regulator);
Serial sci0 = initSerial(SCI_PORT0, &app, reader);
Msg message;
void reader(App *self, int c) {
  ASYNC(&userInputHandler, parse_user_input, c);
}

void startApp(App *self, int arg) {
  CAN_INIT(&can0);
  SCI_INIT(&sci0);
  SCI_WRITE(&sci0, "Ready.\n");
  T_RESET(&self->app_start_time);
}

int main() {
  INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
  INSTALL(&can0, can_interrupt, CAN_IRQ0);

  TINYTIMBER(&app, startApp, 0);
  return 0;
}
