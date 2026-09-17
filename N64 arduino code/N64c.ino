/*
 * N64 Controller -> USB Gamepad Adapter (Arduino Micro)
 *
 * Wiring:
 *   N64 GND  -> Arduino GND
 *   N64 3.3V -> Arduino 3.3V  (NOT 5V)
 *   N64 DATA -> digital pin 2, with 2.2k pull-up to 3.3V
 *
 * Library (Arduino Library Manager):
 *   "Joystick" by Matthew Heironimus
 *
 * Leave the stick untouched for ~1 second after power-up so rest
 * calibration can zero hardware drift.
 */

#include <Joystick.h>
#include "N64Protocol.h"

// Raise this if rest still drifts in joy.cpl (typical worn N64: 12..24).
static const int8_t STICK_DEADZONE = 18;

// Sample count for rest-position origin. Keep stick still at boot.
static const uint8_t CALIBRATION_SAMPLES = 48;

/*
 * USB buttons (joy.cpl is 1-based: Arduino 0 = Windows Button 1)
 *
 *  1 A     2 B     3 Z      4 Start
 *  5 L     6 R
 *  7 C-Up  8 C-Down  9 C-Left  10 C-Right
 * 11 D-Up 12 D-Down 13 D-Left  14 D-Right
 *
 * D-pad is also the POV hat, so emulators that bind a hat still work.
 *
 * N64 status bits (MSB first):
 *   byte0: A B Z Start D-Up D-Down D-Left D-Right
 *   byte1: 0 0 L R     C-Up C-Down C-Left C-Right
 */
static const uint8_t BTN_A = 0;
static const uint8_t BTN_B = 1;
static const uint8_t BTN_Z = 2;
static const uint8_t BTN_START = 3;
static const uint8_t BTN_L = 4;
static const uint8_t BTN_R = 5;
static const uint8_t BTN_C_UP = 6;
static const uint8_t BTN_C_DOWN = 7;
static const uint8_t BTN_C_LEFT = 8;
static const uint8_t BTN_C_RIGHT = 9;
static const uint8_t BTN_D_UP = 10;
static const uint8_t BTN_D_DOWN = 11;
static const uint8_t BTN_D_LEFT = 12;
static const uint8_t BTN_D_RIGHT = 13;
static const uint8_t USB_BUTTON_COUNT = 14;

static const uint8_t N64_A = 0x80;
static const uint8_t N64_B = 0x40;
static const uint8_t N64_Z = 0x20;
static const uint8_t N64_START = 0x10;
static const uint8_t N64_D_UP = 0x08;
static const uint8_t N64_D_DOWN = 0x04;
static const uint8_t N64_D_LEFT = 0x02;
static const uint8_t N64_D_RIGHT = 0x01;
static const uint8_t N64_L = 0x20;
static const uint8_t N64_R = 0x10;
static const uint8_t N64_C_UP = 0x08;
static const uint8_t N64_C_DOWN = 0x04;
static const uint8_t N64_C_LEFT = 0x02;
static const uint8_t N64_C_RIGHT = 0x01;

Joystick_ Joystick(
    JOYSTICK_DEFAULT_REPORT_ID,
    JOYSTICK_TYPE_JOYSTICK,
    USB_BUTTON_COUNT, 1,
    true, true, false,
    false, false, false,
    false, false,
    false, false, false);

N64ControllerState pad;
int8_t originX = 0;
int8_t originY = 0;
bool calibrated = false;

static int8_t clampInt8(int16_t value) {
  if (value > 127) {
    return 127;
  }
  if (value < -128) {
    return -128;
  }
  return static_cast<int8_t>(value);
}

// N64 reports signed stick already centered at 0. Do NOT subtract 128.
static void applyStick(int8_t rawX, int8_t rawY, int8_t &outX, int8_t &outY) {
  int16_t x = static_cast<int16_t>(rawX) - originX;
  int16_t y = static_cast<int16_t>(rawY) - originY;

  const int32_t magSq = static_cast<int32_t>(x) * x + static_cast<int32_t>(y) * y;
  const int32_t dzSq = static_cast<int32_t>(STICK_DEADZONE) * STICK_DEADZONE;
  if (magSq <= dzSq) {
    outX = 0;
    outY = 0;
    return;
  }

 static const int16_t N64_MAX = 85;

outX = clampInt8((x * 127L) / N64_MAX);
outY = clampInt8((-y * 127L) / N64_MAX);
}

static uint8_t isPressed(uint8_t bits, uint8_t mask) {
  return (bits & mask) ? 1 : 0;
}

static void applyHatSwitch(uint8_t buttons1) {
  const bool up = buttons1 & N64_D_UP;
  const bool down = buttons1 & N64_D_DOWN;
  const bool left = buttons1 & N64_D_LEFT;
  const bool right = buttons1 & N64_D_RIGHT;

  int hat = -1;
  if (up && right) {
    hat = 45;
  } else if (right && down) {
    hat = 135;
  } else if (down && left) {
    hat = 225;
  } else if (left && up) {
    hat = 315;
  } else if (up) {
    hat = 0;
  } else if (right) {
    hat = 90;
  } else if (down) {
    hat = 180;
  } else if (left) {
    hat = 270;
  }

  Joystick.setHatSwitch(0, hat);
}

static void sendGamepadState(const N64ControllerState &state) {
  int8_t x = 0;
  int8_t y = 0;
  applyStick(state.stickX, state.stickY, x, y);

  Joystick.setXAxis(x);
  Joystick.setYAxis(y);

  Joystick.setButton(BTN_A, isPressed(state.buttons1, N64_A));
  Joystick.setButton(BTN_B, isPressed(state.buttons1, N64_B));
  Joystick.setButton(BTN_Z, isPressed(state.buttons1, N64_Z));
  Joystick.setButton(BTN_START, isPressed(state.buttons1, N64_START));
  Joystick.setButton(BTN_L, isPressed(state.buttons2, N64_L));
  Joystick.setButton(BTN_R, isPressed(state.buttons2, N64_R));
  Joystick.setButton(BTN_C_UP, isPressed(state.buttons2, N64_C_UP));
  Joystick.setButton(BTN_C_DOWN, isPressed(state.buttons2, N64_C_DOWN));
  Joystick.setButton(BTN_C_LEFT, isPressed(state.buttons2, N64_C_LEFT));
  Joystick.setButton(BTN_C_RIGHT, isPressed(state.buttons2, N64_C_RIGHT));
  Joystick.setButton(BTN_D_UP, isPressed(state.buttons1, N64_D_UP));
  Joystick.setButton(BTN_D_DOWN, isPressed(state.buttons1, N64_D_DOWN));
  Joystick.setButton(BTN_D_LEFT, isPressed(state.buttons1, N64_D_LEFT));
  Joystick.setButton(BTN_D_RIGHT, isPressed(state.buttons1, N64_D_RIGHT));

  applyHatSwitch(state.buttons1);
  Joystick.sendState();
}

static bool calibrateRestPosition() {
  int32_t sumX = 0;
  int32_t sumY = 0;
  uint8_t got = 0;

  const unsigned long deadline = millis() + 1500;
  while (got < CALIBRATION_SAMPLES && millis() < deadline) {
    if (n64Poll(pad)) {
      sumX += pad.stickX;
      sumY += pad.stickY;
      got++;
    }
    delay(8);
  }

  if (got == 0) {
    return false;
  }

  originX = clampInt8(sumX / got);
  originY = clampInt8(sumY / got);
  return true;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Joystick.setXAxisRange(-128, 127);
  Joystick.setYAxisRange(-128, 127);
  Joystick.begin(false);

  n64Init();
  delay(50);

  calibrated = calibrateRestPosition();
  digitalWrite(LED_BUILTIN, calibrated ? LOW : HIGH);

  sendGamepadState(pad);
}

void loop() {
  if (!n64Poll(pad)) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(4);
    digitalWrite(LED_BUILTIN, calibrated ? LOW : HIGH);
    return;
  }

  sendGamepadState(pad);
  delay(8);  // ~125 Hz is enough; faster polling can stall USB on 32U4
}
