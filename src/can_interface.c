#include "can_interface.h"

extern MusicPlayer music_player;
void parse_can_input(CANMsg *msg, int conductor) {
    int num; 
    switch (msg->msgId) {
    case 0:
    case 1:
    case 2:
        break;
    
    case 3: // START
        if (msg->nodeId != conductor) return;
        ASYNC(&music_player, play_music, 0);
        break;
    case 4: // STOP
        if (msg->nodeId != conductor) return;
        ASYNC(&music_player, stop_music, 0);
        break;
    case 6: // SET TEMPO
        if (msg->nodeId != conductor) return;  // TODO: Check if we need to disregard messages from ourselves
        num = (int) msg->buff[0] << 8 | msg->buff[1];
        num = num > 300 ? 300 : (num < 30 ? 30 : num);
        print("Tempo: %d\n", num);
        ASYNC(&music_player, change_tempo, num);
        break;
    case 7:
        if (msg->nodeId != conductor) return;
        num = (int) msg->buff[0];
        num = num > 10 ? 10 : (num < 0 ? 0 : num);
        num = num - 5;
        print("Key: %d\n", num);
        ASYNC(&music_player, change_key, num);
        break;
    case 8:
    case 9:
        break;
    default:
        break;
    }
}