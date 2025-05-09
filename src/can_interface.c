#include "application.h"

int conductor = 3; // TODO Temp

extern MusicPlayer music_player;
void parse_can_input(App *self, int _) {
    CANMsg msg;
    CANMsg respMsg;
    CAN_RECEIVE(&can0, &msg);
    
    int new_nodeID = 1;
    for (int i=0; i<app.network_size-1; i++) {  // TODO: Change to use app.network_size
        if (self->ranks[i] == msg.nodeId) new_nodeID = 0;
    }
    if (new_nodeID && msg.nodeId != self->rank) {
        self->ranks[self->network_size-1] = msg.nodeId;
        self->network_size++;
        if (msg.nodeId < NODE_ID) music_player.cur_note_modulo++; // TODO decrease on node leave
        SYNC(&music_player, update_nth_note_to_play, 0);
    }
    
    if (msg.msgId != 7) {
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
        if (msg.nodeId == app.rank) return;
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
            pending_conductor = msg.nodeId;  // TODO: This will not update the conductor until after 200msec
            AFTER(MSEC(200), &app, switch_conductor, 0);
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
        if (self->network_size == 1) { // TODO Fix, this wont be called anymore as the network size will already be updated.
            CANMsg msg = {8,self->rank,0,{}};
            CAN_SEND(&can0, &msg);
            self->network_size += 1;
            // TODO add to known nodes
        }
        for (int i=0; i < app.network_size-1; i++) {
            if (app.ranks[i] == msg.nodeId) {
                app.still_alive[i] = 0;
            }
        }
        /*
        for (int i=0; i < app.network_size-1; i++) {
            if (app.ranks[i] == msg.nodeId) {           // Node sending Im Alive is in current network
                print("Recieved Im alive from %d\n", msg.nodeId);
                app.still_alive[i] = msg.nodeId;        // todo temp line
                if (msg.length == 2) {                  // Sender is currently playing
                    if (msg.buff[0] >= music_player.note_idx && msg.buff[1] >= music_player.current_note_segment) { // Is the message up to date with our melody position?
                        app.still_alive[i] = msg.nodeId;
                        // TODO: If the player is ahead of us, catch up my updating the idxs
                    }
                    else {} // The Im alive ping is old / behind us in the melody
                }
                else { // Sender is not playing
                    app.still_alive[i] = msg.nodeId;
                } 
            }
            else {} // Sender is not in current network, this should never be reached
        }
        */
        break;
    case 8: // im new
        if(self->rank == msg.nodeId) return;
        break;
    default:
        break;
    }
}