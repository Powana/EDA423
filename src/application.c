#include "application.h"

#define PRESS_MOMENTARY 0
#define PRESS_AND_HOLD 1

App app = {initObject(), initTimer(), initTimer(), .user_button_mode=0, .trigger_mode=0, .inter_arrival_times={}, .tap_count=0, .bounce_flag=0, .rank=NODE_ID, .ranks={}, .network_size=1, .conductor=-1, .evaling_conductor=0, .simulate_silent_fail=0, .can_connected=0, .recvd_heartbeats={}};

MusicPlayer music_player = {initObject(), DEFAULT_KEY, DEFAULT_TEMPO, .note_idx=-1, .is_playing=0, .is_led_blinking=0, .nth_note_to_play=0, .current_note_segment=0, .force_mute = 0};
UserInputHandler userInputHandler = {initObject(), {}, 0};
Tone_CTRL tone_ctrl = {initObject(), VOLUME, 0, 1, T_1000_Hz, 0, 0, 0};
Distortion distortion = {initObject(), 1000, 0};
Serial sci0 = initSerial(SCI_PORT0, &app, reader);
Can can0 = initCan(CAN_PORT0, &app, parse_can_input);
SysIO sio0 = initSysIO(SIO_PORT0, &app, sio_receive);
int pending_conductor = 0;
int base_freq_indices[32] = {0, 2, 4, 0, 0, 2, 4, 0, 4, 5, 7, 4,  5, 7, 7, 9,
                             7, 5, 4, 0, 7, 9, 7, 5, 4, 0, 0, -5, 0, 0, -5, 0};
// freq indices corresponding to -10 to 14
int periods[25] = {2025, 1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276,
                   1204, 1136, 1073, 1012, 956,  902,  851,  804,  758,
                   716,  676,  638,  602,  568,  536,  506};
char note_lengths[32] = {'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'b',
                         'a', 'a', 'b', 'c', 'c', 'c', 'c', 'a', 'a', 'c', 'c',
                         'c', 'c', 'a', 'a', 'a', 'a', 'b', 'a', 'a', 'b'};
int min_index = -10;
int max_index = 14;

Time hold_time;
Msg message;

void sio_receive(App *self, int val) {
  // First time. Ignore contact bounce test
  if (self->trigger_mode == 0) {
    if (self->bounce_flag == 0) {
      self->bounce_flag = 1;
      AFTER(MSEC(100), self, button_press_logic, 0);
      T_RESET(&self->hold_timer);
    } else { // Detect contact bounces
      print("Bounce detected\n", 0);
    }
  } else if (self->trigger_mode == 1 && self->bounce_flag == 0) {
    ASYNC(self, button_release_logic, 0);
  }
  self->trigger_mode = !self->trigger_mode;
  SIO_TRIG(&sio0, self->trigger_mode);
}

void button_release_logic(App *self, int _) {
  hold_time = T_SAMPLE(&self->hold_timer);
  print("Hold time: %ds ", SEC_OF(hold_time));
  print("%dms\n", MSEC_OF(hold_time));

  if (self->user_button_mode ==
      PRESS_AND_HOLD) { // Button was in press-and-hold mode
    print("Button held for %d seconds\n", SEC_OF(hold_time));
    if (SEC_OF(hold_time) >= 2) {
      ASYNC(&music_player, change_tempo, DEFAULT_TEMPO);
      print("Tempo reset to %d BPM\n", DEFAULT_TEMPO);
      self->tap_count = 0;
    }
  } else { // Button was in momentary mode
    if (hold_time < SEC(1)) {
      ABORT(message);
    }
  }

  self->user_button_mode = PRESS_MOMENTARY;
  print("Switched to momentary mode\n", 0);
}

void button_press_logic(App *self, int _) {
  int button_state = SIO_READ(&sio0);
  self->bounce_flag = 0;

  if (button_state == 1) // Button not pressed at 100ms
    return;

  message = AFTER(MSEC(900), self, switch_to_held_mode, 0);
  print("Tapped! Tap count: %d\n", self->tap_count + 1);
  if (self->tap_count == 0) {
    T_RESET(&self->arrival_timer);
    self->tap_count++;
  } else {
    Time current_interval = T_SAMPLE(&self->arrival_timer);
    T_RESET(&self->arrival_timer);

    self->inter_arrival_times[self->tap_count - 1] = current_interval;
    print("Inter-arrival time: %ds ", SEC_OF(current_interval));
    print("%dms\n", MSEC_OF(current_interval));

    self->tap_count++;
    if (self->tap_count == 4) {
      Time max_diff = 0;
      Time time_diff;

      for (int i = 0; i < 2; i++) {
        for (int j = 1; j < 3; j++) {
          time_diff =
              abs(self->inter_arrival_times[i] - self->inter_arrival_times[j]);

          if (time_diff > max_diff)
            max_diff = time_diff;
        }
      }

      print("4 taps arrived with max difference of %dms\n", MSEC_OF(max_diff));
      if (SEC_OF(max_diff) == 0 && MSEC_OF(max_diff) < 100) {
        Time avg_time =
            (self->inter_arrival_times[0] + self->inter_arrival_times[1] +
             self->inter_arrival_times[2]) /
            3;
        int new_tempo = 60000 / (SEC_OF(avg_time) * 1000 + MSEC_OF(avg_time));
        if (new_tempo < 30)
          new_tempo = 30;
        if (new_tempo > 300)
          new_tempo = 300;
        print("Taps are rythmic. Changing tempo to %d\n", new_tempo);
        ASYNC(&music_player, change_tempo, new_tempo);
      } else {
        print("Taps are not rythmic\n", 0);
      }
      self->tap_count = 0;
    }
  }
}

void switch_to_held_mode(App *self, int _) {
  print("switched to held mode\n", 0);
  self->user_button_mode = PRESS_AND_HOLD;
}

void switch_conductor(App* self, int _) {
  print("switched conductor to %d\n", pending_conductor);
  self->conductor = pending_conductor;
  self->evaling_conductor = 0;
  pending_conductor = 0;
  
}

void print_every_T(App *self, int _) {
  if(self->conductor == NODE_ID) {
    print("Tempo: %d\n", music_player.tempo);
  } else {
    if(tone_ctrl.mute) print("MUTED\n", 0);
  }
  AFTER(SEC(5), self, print_every_T, 0);
}

void heartbeat(App *self, int _) {
  if (self->simulate_silent_fail) {
    AFTER(MSEC(HEARTBEAT_INTERVAL_MS), self, heartbeat, 0);
    return;
  }
  
  // Send heartbeat
  CANMsg h_msg = {10, NODE_ID, 0, {}};
  int can_res = CAN_SEND(&can0, &h_msg);
  
  // Check for CAN failure
  if (can_res == 1 && self->can_connected) {  // Fail, no connection, alone in network, F3 Requirement
    print("Silent Failure (F3)\n", 0);
    self->can_connected = 0;
    self->network_size = 1;
    if (self->conductor != NODE_ID) {
      ASYNC(&music_player, stop_music, 0 );
    }
    else {
      ASYNC(&music_player, update_nth_note_to_play, 0);
    }
  }
  else if (can_res == 0) {
    if (self->can_connected == 0) {
      print("Can connected (F3).\n", 0);
    }
    self->can_connected = 1;
  }

  // == Check others heartbeat
  //print("Check heartbeats. Network size %d. ", self->network_size);
  for (int i=0; i<self->network_size-1; i++) {
    //print("i: %d. ", i);
    if (self->recvd_heartbeats[self->ranks[i]] <= -1)  { // Node died
      print("Node death detected with rank %d.\n", self->ranks[i]);

       // Shift elements to the left
      for (int j = i; j < self->network_size - 1; j++) {
        self->ranks[j] = self->ranks[j + 1];
      }
      self->network_size--;
      ASYNC(&music_player, update_nth_note_to_play, 0);

      if (self->network_size == 1 && self->conductor != NODE_ID) { // Left as a single musician, stop the music
        ASYNC(&music_player, stop_music, 0 );
      } 
    }
    else {
      //print("No node death at that i. The recv_heartbeats val is: %d.\n", self->recvd_heartbeats[self->ranks[i]]);
      self->recvd_heartbeats[self->ranks[i]] -= 1; // Decrease every counter by 1 every INTERVAL seconds
    }
  }

  AFTER(MSEC(HEARTBEAT_INTERVAL_MS), self, heartbeat, 0);
}


void reader(App *self, int c) {
  ASYNC(&userInputHandler, parse_user_input, c);
}

void startApp(App *self, int arg) {
  CAN_INIT(&can0);
  SCI_INIT(&sci0);
  SIO_INIT(&sio0);
  SCI_WRITE(&sci0, "Ready.\n");
  ASYNC(self, heartbeat, 0);
  AFTER(SEC(5), self, print_every_T, 0);
}

int main() {
  INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
  INSTALL(&can0, can_interrupt, CAN_IRQ0);
  INSTALL(&sio0, sio_interrupt, SIO_IRQ0);

  TINYTIMBER(&app, startApp, 0);
  return 0;
}
