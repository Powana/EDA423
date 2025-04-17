#include "can_interface.h"
#include "canTinyTimber.h"
#include "application.h"

extern MusicPlayer music_player;
void parse_can_input(CANMsg *msg, int conductor) {
    int num; 
    switch (msg->msgId) {
    case 0:
    case 1:
    case 2: // Start Conducting
        if (evaling_conductor) {
            print("conductor conflict\n", 0);
            if (msg->nodeId < pending_conductor) {
                pending_conductor = msg->nodeId;
            }
            break;
        }
        if (!evaling_conductor) {
            print("starting evaling conductor\n", 0);
            evaling_conductor = 1;
            pending_conductor = msg->nodeId;
            AFTER(MSEC(200), &app, switch_conductor, 0);
        }
        break;
    
    case 3: // START
        if (msg->nodeId != conductor) return;
        ASYNC(&music_player, play_music, 0);
        ASYNC(&music_player, im_alive_ping, 0);

        break;
    case 4: // STOP
        if (msg->nodeId != conductor) return;
        ASYNC(&music_player, stop_music, 0);
        break;
    case 6: // SET TEMPO
        if (msg->nodeId != conductor) return;  // TODO: Check if we need to disregard messages from ourselves
        num = (int) msg->buff[1] << 8 | msg->buff[0];
        num = num > 300 ? 300 : (num < 30 ? 30 : num);
        print("Tempo: %d\n", num);
        ASYNC(&music_player, change_tempo, num);
        break;
    case 7:
        if (msg->nodeId != conductor) return;
        num = (int) msg->buff[0];
        num = num > 10 ? 10 : (num < 0 ? 0 : num);
        num = num - 5;
        print("Set Key: %d\n", num);
        ASYNC(&music_player, change_key, num);
        break;
    case 8: // im alive
        if (network_size == 1) {
            CANMsg msg = {9,3,0,{}};
            CAN_SEND(&can0, &msg);
            network_size += 1;
            // TODO add to known nodes
        }
    case 9: // im new
        break;
    default:
        break;
    }
}