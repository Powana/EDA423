#include "can_interface.h"
#include "application.h"

CANMsg can_msg = {0,0,0,{}};

// === Problem 5 ===
void can_regulator(App *self, int _) {
    if (self->can_queue_size == MAX_CAN_QUEUE_SIZE) return;  // Queue is full, discard the message
    self->can_queue_end = (self->can_queue_end + 1) % MAX_CAN_QUEUE_SIZE;
    CAN_RECEIVE(&can0, &can_msg);
    self->can_queue[self->can_queue_end].msgId = can_msg.msgId;
    self->can_queue[self->can_queue_end].arrival_time = T_SAMPLE(&self->app_start_time);
    self->can_queue_size++;

    if (!self->can_cooldown_active && self->can_queue_size == 1) {
        self->can_cooldown_active = 1;
        ASYNC(self, deliver_next_can_msg_in_q, 0);
    }
}

void deliver_next_can_msg_in_q(App *self, int _) {
    if (self->can_queue_size == 0) { // Queue is empty
        self->can_cooldown_active = 0;
        return; 
    }
    MiniMsg msg = self->can_queue[self->can_queue_start];
    self->can_queue_start = (self->can_queue_start + 1) % MAX_CAN_QUEUE_SIZE;
    self->can_queue_size--;
    print("Delivered message with sequence %d; ", msg.msgId);
    print("arrival time: %d.", SEC_OF(msg.arrival_time)); print("%d (s); ", MSEC_OF(msg.arrival_time));
    print("delivery time: %d.", SEC_OF(T_SAMPLE(&self->app_start_time))); print("%d (s);\n", MSEC_OF(T_SAMPLE(&self->app_start_time)));

    SEND(MSEC(self->can_msg_min_interval_ms), 0, self, deliver_next_can_msg_in_q, 0);
}
