#include "application.h"

int conductor = 3; // TODO Temp

extern MusicPlayer music_player;
void parse_can_input(App *self, int _) {
    CANMsg msg;
    CAN_RECEIVE(&can0, &msg);
    if (msg.msgId == 8) return;
    print("Can MSG Recieved, msgId: %d ", msg.msgId);
    print("NodeID: %d ", msg.nodeId);
    print("Length: %d ", msg.length);
    print("buff[0]: %d ", (int) (uchar) msg.buff[0]);
    print("buff[1]: %d\n", (int) (uchar) msg.buff[1]);
  
    int num; 
    switch (msg.msgId) {
    case 0:
        ;
        CANMsg respMsg = {1,NODE_ID,0,{}};
        CAN_SEND(&can0, &respMsg);
        break;
    case 1:
        for (int i=0; i<MAX_NETWORK_SIZE; i++) {
            if (self->ranks[i] == msg.nodeId) return;
        }
        self->network_size++;

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
            pending_conductor = msg.nodeId;  // TODO: This will not update the conductor until after 200msec
            AFTER(MSEC(200), &app, switch_conductor, 0);
        }
        break;
    
    case 3: // START
        if (msg.nodeId != conductor) return;
        ASYNC(&music_player, play_music, 0);
        // ASYNC(&music_player, im_alive_ping, 0);  // TODO, Remove this if using the count implementation
        break;
    case 4: // STOP
        if (msg.nodeId != conductor) return;
        ASYNC(&music_player, stop_music, 0);
        break;
    case 6: // SET TEMPO
        if (msg.nodeId != conductor) return;  // TODO: Check if we need to disregard messages from ourselves
        num = (int) msg.buff[1] << 8 | msg.buff[0];
        num = num > 300 ? 300 : (num < 30 ? 30 : num);
        print("Tempo: %d\n", num);
        ASYNC(&music_player, change_tempo, num);
        break;
    case 7:  // SET KEY
        if (msg.nodeId != conductor) return;
        num = (int) msg.buff[0];
        num = num > 10 ? 10 : (num < 0 ? 0 : num);
        num = num - 5;
        print("Set Key: %d\n", num);
        ASYNC(&music_player, change_key, num);
        break;
    case 8: // im alive
        if (self->network_size == 1) {
            CANMsg msg = {9,NODE_ID,0,{}};
            CAN_SEND(&can0, &msg);
            self->network_size += 1;
            // TODO add to known nodes
        }
    case 9: // im new
        break;
    default:
        break;
    }
}