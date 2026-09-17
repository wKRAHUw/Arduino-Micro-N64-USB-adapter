/*
 * N64 data-line pin macros for cycle-accurate AVR bit-banging.
 *
 * The N64 protocol toggles the line by switching the pin between
 * INPUT (idle high via external 2.2k pull-up to 3.3V) and OUTPUT LOW.
 *
 * Change N64_DATA_PIN below if your data wire uses a different digital pin.
 * Only pins on PORTD are supported (pins 2-4 on Leonardo / Micro / Pro Micro).
 */

#ifndef N64_PIN_CONFIG_H
#define N64_PIN_CONFIG_H

#include <pins_arduino.h>

#ifndef N64_DATA_PIN
#define N64_DATA_PIN 2
#endif

#if defined(__AVR_ATmega32U4__)
  #if N64_DATA_PIN == 2
    #define N64_HIGH (DDRD &= ~_BV(PD1))
    #define N64_LOW  (DDRD |= _BV(PD1))
    #define N64_QUERY (PIND & _BV(PD1))
  #elif N64_DATA_PIN == 3
    #define N64_HIGH (DDRD &= ~_BV(PD0))
    #define N64_LOW  (DDRD |= _BV(PD0))
    #define N64_QUERY (PIND & _BV(PD0))
  #elif N64_DATA_PIN == 4
    #define N64_HIGH (DDRD &= ~_BV(PD4))
    #define N64_LOW  (DDRD |= _BV(PD4))
    #define N64_QUERY (PIND & _BV(PD4))
  #else
    #error "N64_DATA_PIN must be 2, 3, or 4 on ATmega32U4 (PORTD pins)."
  #endif

#elif defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
  #if N64_DATA_PIN == 2
    #define N64_HIGH (DDRD &= ~_BV(PD2))
    #define N64_LOW  (DDRD |= _BV(PD2))
    #define N64_QUERY (PIND & _BV(PD2))
  #elif N64_DATA_PIN == 3
    #define N64_HIGH (DDRD &= ~_BV(PD3))
    #define N64_LOW  (DDRD |= _BV(PD3))
    #define N64_QUERY (PIND & _BV(PD3))
  #elif N64_DATA_PIN == 4
    #define N64_HIGH (DDRD &= ~_BV(PD4))
    #define N64_LOW  (DDRD |= _BV(PD4))
    #define N64_QUERY (PIND & _BV(PD4))
  #else
    #error "N64_DATA_PIN must be 2, 3, or 4 on ATmega328P (PORTD pins)."
  #endif

#else
  #error "Unsupported AVR board. Use Arduino Micro / Leonardo / Pro Micro."
#endif

#endif
