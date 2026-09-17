/*
 * Nintendo 64 controller protocol (host side).
 * Timing-critical AVR assembly based on Andrew Brown / Peter Den Hartog / Jarno Lehtinen.
 */

#include "N64Protocol.h"
#include "N64PinConfig.h"

#include <string.h>

namespace {

struct RawN64Status {
  uint8_t data1;
  uint8_t data2;
  int8_t stick_x;
  int8_t stick_y;
};

char rawDump[32];

void translateRawData(RawN64Status &status) {
  memset(&status, 0, sizeof(status));

  for (uint8_t i = 0; i < 8; i++) {
    status.data1 |= rawDump[i] ? (0x80 >> i) : 0;
  }
  for (uint8_t i = 0; i < 8; i++) {
    status.data2 |= rawDump[8 + i] ? (0x80 >> i) : 0;
  }
  for (uint8_t i = 0; i < 8; i++) {
    status.stick_x |= rawDump[16 + i] ? (0x80 >> i) : 0;
  }
  for (uint8_t i = 0; i < 8; i++) {
    status.stick_y |= rawDump[24 + i] ? (0x80 >> i) : 0;
  }
}

void n64Send(uint8_t *buffer, char length) {
  char bits;

  asm volatile(";Starting outer for loop");
outer_loop:
  {
    asm volatile(";Starting inner for loop");
    bits = 8;
inner_loop:
    {
      asm volatile(";Setting line to low");
      N64_LOW;

      asm volatile(";branching");
      if (*buffer >> 7) {
        asm volatile(";Bit is a 1");
        asm volatile("nop\nnop\nnop\nnop\nnop\n");

        asm volatile(";Setting line to high");
        N64_HIGH;

        asm volatile("nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n");
      } else {
        asm volatile(";Bit is a 0");
        asm volatile("nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\nnop\n"
                     "nop\n");

        asm volatile(";Setting line to high");
        N64_HIGH;
      }

      asm volatile(";finishing inner loop body");
      --bits;
      if (bits != 0) {
        asm volatile("nop\nnop\nnop\nnop\nnop\n"
                     "nop\nnop\nnop\nnop\n");
        asm volatile(";rotating out bits");
        *buffer <<= 1;
        goto inner_loop;
      }
    }

    asm volatile(";continuing outer loop");
    --length;
    if (length != 0) {
      ++buffer;
      goto outer_loop;
    }
  }

  asm volatile("nop\nnop\nnop\nnop\n");
  N64_LOW;
  asm volatile("nop\nnop\nnop\nnop\nnop\n"
               "nop\nnop\nnop\nnop\nnop\n"
               "nop\nnop\nnop\nnop\n");
  N64_HIGH;
}

bool n64Receive() {
  unsigned char timeout;
  char bitcount = 32;
  char *bitbin = rawDump;

read_loop:
  timeout = 0x3f;
  while (N64_QUERY) {
    if (!--timeout) {
      return false;
    }
  }

  asm volatile("nop\nnop\nnop\nnop\nnop\n"
               "nop\nnop\nnop\nnop\nnop\n"
               "nop\nnop\nnop\nnop\nnop\n"
               "nop\nnop\nnop\nnop\nnop\n"
               "nop\nnop\nnop\nnop\nnop\n"
               "nop\nnop\nnop\nnop\nnop\n");
  *bitbin = N64_QUERY;
  ++bitbin;
  --bitcount;
  if (bitcount == 0) {
    return true;
  }

  timeout = 0x3f;
  while (!N64_QUERY) {
    if (!--timeout) {
      return false;
    }
  }
  goto read_loop;
}

void waitForIdle() {
  for (int idle = 0; idle < 64; idle++) {
    if (!N64_QUERY) {
      idle = -1;
    }
  }
}

}  // namespace

void n64Init() {
  digitalWrite(N64_DATA_PIN, LOW);
  pinMode(N64_DATA_PIN, INPUT);

  uint8_t initByte = 0x00;
  noInterrupts();
  n64Send(&initByte, 1);
  waitForIdle();
  interrupts();
}

bool n64Poll(N64ControllerState &state) {
  uint8_t command = 0x01;
  RawN64Status raw;

  noInterrupts();
  n64Send(&command, 1);
  const bool ok = n64Receive();
  interrupts();

  if (!ok) {
    return false;
  }

  translateRawData(raw);
  state.buttons1 = raw.data1;
  state.buttons2 = raw.data2;
  state.stickX = raw.stick_x;
  state.stickY = raw.stick_y;
  return true;
}
