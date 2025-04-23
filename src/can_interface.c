#include "can_interface.h"
#include "application.h"


int conductor = 3; // TODO Temp

extern MusicPlayer music_player;

// === Problem 5 ===
void can_regulator(App *self, int _) {
    if (self->can_queue_size == MAX_CAN_QUEUE_SIZE) return;  // Queue is full, discard the message
    self->can_queue_end = (self->can_queue_end + 1) % MAX_CAN_QUEUE_SIZE;
    CAN_RECEIVE(&can0, self->can_queue[self->can_queue_end]);

    if (!self->can_cooldown_active && self->can_queue_size == 1)
        self->can_cooldown_active = 1;
        ASYNC(self, deliver_next_can_msg_in_q, 0);
}

void deliver_next_can_msg_in_q(App *self, int _) {
    if (self->can_queue_size == 0) { // Queue is empty
        self->can_cooldown_active = 0;
        return; 
    }
    ASYNC(self, parse_next_can_message, 0);

    SEND(SEC(1), 0, self, deliver_next_can_msg_in_q, 0);
}


void parse_next_can_message(App *self, int _) {
    CANMsg msg = *self->can_queue[self->can_queue_start];
    self->can_queue_start = (self->can_queue_start + 1) % MAX_CAN_QUEUE_SIZE;
    self->can_queue_size--;

    CANMsg respMsg;
    int new_nodeID = 1;
    for (int i=0; i<MAX_NETWORK_SIZE; i++) {
        if (self->ranks[i] == msg.nodeId) new_nodeID = 0;
    }
    if (new_nodeID && msg.nodeId != self->rank) {
        self->ranks[self->network_size-1] = msg.nodeId;
        self->network_size++;
        // if (msg.nodeId < NODE_ID) music_player.cur_note_modulo++; // TODO decrease on node leave
        SYNC(&music_player, update_nth_note_to_play, 0);
    }
    
    if ((msg.msgId != 7) && (msg.msgId != 0)) {
        print("Can MSG Recieved, msgId: %d ", msg.msgId);
        print("NodeID: %d ", msg.nodeId);
        print("Length: %d ", msg.length);
        print("buff[0]: %d ", (int) (uchar) msg.buff[0]);
        print("buff[1]: %d\n", (int) (uchar) msg.buff[1]);
    }

    int num; 
    switch (msg.msgId) {
    case 0:
        respMsg.msgId = 1;
        respMsg.nodeId = self->rank;
        respMsg.length = 0;
        CAN_SEND(&can0, &respMsg);
        break;
        
    case 1:
        if (msg.nodeId == self->rank) return;
        break;

        break;
    case 2: // Start Conducting
        if (self->evaling_conductor) {
            print("conductor conflict\n", 0);
            if (msg.nodeId < pending_conductor) {
                pending_conductor = msg.nodeId;
            }
            break;
        }
        if (!(self->evaling_conductor)) {
            self->evaling_conductor = 1;
            print("starting evaling conductor\n", 0);
            pending_conductor = msg.nodeId;
            AFTER(MSEC(CONDUCTOR_CLASH_MS), &app, switch_conductor, 0);
        }
        break;
    
    case 3: // START
        if (msg.nodeId != self->conductor) return;
        ASYNC(&music_player, play_music, 0);
        // ASYNC(&music_player, im_alive_ping, 0);  // TODO, Remove this if using the count implementation
        break;
        
    case 4: // STOP
        if (msg.nodeId != self->conductor) return;
        ASYNC(&music_player, stop_music, 0);
        break;
    case 5: // SET TEMPO
        if (msg.nodeId != self->conductor) return;  // TODO: Check if we need to disregard messages from ourselves
        num = (int) msg.buff[1] << 8 | msg.buff[0];
        num = num > 300 ? 300 : (num < 30 ? 30 : num);
        print("Tempo: %d\n", num);
        ASYNC(&music_player, change_tempo, num);
        break;
    case 6:  // SET KEY
        if (msg.nodeId != self->conductor) return;
        num = (int) msg.buff[0];
        num = num > 10 ? 10 : (num < 0 ? 0 : num);
        num = num - 5;
        print("Set Key: %d\n", num);
        ASYNC(&music_player, change_key, num);
        break;
    case 7: // im alive
        if (self->network_size == 1) {
            CANMsg msg = {8,self->rank,0,{}};
            CAN_SEND(&can0, &msg);
            self->network_size += 1;
            // TODO add to known nodes
        }
    case 8: // im new
        if(self->rank == msg.nodeId) return;
        break;
    default:
        break;
    }
}