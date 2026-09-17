#ifndef N64_PROTOCOL_H
#define N64_PROTOCOL_H

#include <Arduino.h>

struct N64ControllerState {
  uint8_t buttons1;  // A, B, Z, Start, D-Up, D-Down, D-Left, D-Right
  uint8_t buttons2;  // unused, unused, L, R, C-Up, C-Down, C-Left, C-Right
  int8_t stickX;     // already signed, 0 = electrical center
  int8_t stickY;
};

void n64Init();
bool n64Poll(N64ControllerState &state);

#endif
