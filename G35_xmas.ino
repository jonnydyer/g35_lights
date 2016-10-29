/*
  G35: An Arduino library for GE Color Effects G-35 holiday lights.
  Copyright © 2011 The G35 Authors. Use, modification, and distribution are
  subject to the BSD license as described in the accompanying LICENSE file.

  See README for complete attributions.
*/

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
#include <ProgramRunner.h>
#include <RFM12B.h>



// Total # of lights on string (usually 50, 48, or 36). Maximum is 63, because
// the protocol uses 6-bit addressing and bulb #63 is reserved for broadcast
// messages.
#define LIGHT_COUNT (50)

// Arduino pin number. Pin 13 will blink the on-board LED.
#define G35_PIN (16)

#define PRINT_MILLIS 500

uint32_t last_print_millis = 0;

G35String lights(G35_PIN, LIGHT_COUNT);

LightProgram* CreateProgram(uint8_t program_index) {
  //return new Creepers(lights);
  //return new SpookyFlicker(lights);
  //return new PumpkinChase(lights);
  //return new Meteorite(lights);
  //return new Orbit(lights);
  //return new Eyes(lights);
  //return new Inchworm(lights);
  //return new Pulse(lights);
  //return new Cylon(lights);
  return new LayeredEffects(lights);
}

ProgramRunner runner(CreateProgram, 1, 5);

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

void setup() {
  uint32_t seed = seedOut(32);
  randomSeed(seed);
  seed &= 0xff;
  // random() isn't very random. But this seed generator works quite well.
  while (seed--) {
    random();
  }
  lights.enumerate();
  runner.disable_time_based_switching();
  Serial.begin(115200);
}

void loop() {
  runner.loop();
}
