/*
  G35: An Arduino library for GE Color Effects G-35 holiday lights.
  Copyright © 2011 The G35 Authors. Use, modification, and distribution are
  subject to the BSD license as described in the accompanying LICENSE file.

  See README for complete attributions.
*/

#include <avr/wdt.h>

#include <G35String.h>
//#include <Eyes.h>
//#include <Orbit.h>
//#include <Meteorite.h>
//#include <Creepers.h>
//#include <Pulse.h>
//#include <Cylon.h>
//#include <SpookyWave.h>
//#include <SpookyFlicker.h>
//#include <SpookySlow.h>
//#include <PumpkinChase.h>
//#include <Inchworm.h>
#include <LayeredEffects.h>
#include <LightProgram.h>
/*---NOTE!  RFM12B library is only required for SpookyWave.h */
//#include <RFM12B.h>


// Total # of lights on string (usually 50, 48, or 36). Maximum is 63, because
// the protocol uses 6-bit addressing and bulb #63 is reserved for broadcast
// messages.
#define LIGHT_COUNT (50)

// Arduino pin number. Pin 13 will blink the on-board LED.
#define G35_PIN (16)

G35String lights(G35_PIN, LIGHT_COUNT);

LightProgram *program;

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

// http://www.utopiamechanicus.com/77/better-arduino-random-numbers/
// We assume A0 and A1 are disconnected.
uint32_t seedOut(unsigned int noOfBits) {
  uint32_t seed = 0, limit = 99;
  int bit0 = 0, bit1 = 0;
  while (noOfBits--) {
    for (int i = 0; i < limit; ++i) {
      bit0 = analogRead(0) & 1;
      bit1 = analogRead(1) & 1;
      if (bit1 != bit0)
        break;
    }
    seed = (seed << 1) | bit1;
  }
  return seed;
}

void watchdogSetup(void)
{
  cli();       // disable all interrupts
  wdt_reset(); // reset the WDT timer
  /*
  WDTCSR configuration:
  WDIE = 1: Interrupt Enable
  WDE = 1 :Reset Enable
  WDP3 = 0 :For 125ms Time-out
  WDP2 = 0 :For 125ms Time-out
  WDP1 = 1 :For 125ms Time-out
  WDP0 = 1 :For 125ms Time-out
  */
  // Enter Watchdog Configuration mode:
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  // Set Watchdog settings:
  WDTCSR = (1<<WDIE) | (1<<WDE) | (0<<WDP3) | (0<<WDP2) | (1<<WDP1) | (1<<WDP0);
  sei();
}

ISR(WDT_vect) // Watchdog timer interrupt.
{
//  Serial.print("WDT! MCUCR: 0x");
//  Serial.println(MCUCR, HEX);
//  Serial.print("Free Ram: ");
//  Serial.println(freeRam());
}

void setup() {
  uint32_t seed = seedOut(32);
  uint8_t reg;
  pinMode(LED_BUILTIN, OUTPUT);
  randomSeed(seed);
  seed &= 0xff;
  // random() isn't very random. But this seed generator works quite well.
  while (seed--) {
    random();
  }
  lights.enumerate();
  program = new LayeredEffects(lights);
  //program = new SpookyFlicker(lights);
  Serial.begin(115200);
  watchdogSetup();
}

void loop() {
  uint32_t now;
  static uint32_t next_do_millis = millis();
  static uint8_t blink_status = 0;
  now = millis();
  if (now >= next_do_millis) {
      digitalWrite(LED_BUILTIN, blink_status ^= 0x01);
      next_do_millis = now + program->Do();
      //next_do_millis = now + 50;
  }
  wdt_reset();
}
