// A demonstration of multiple independent strings on a single controller.
//
// By Mike Tsao <http://github.com/sowbug>

#include <G35String.h>
#include <ProgramRunner.h>
#include <StockPrograms.h>
#include <PlusPrograms.h>

// How long each program should run.
#define PROGRAM_DURATION_SECONDS (30)

#define LIGHT_COUNT (50)

#define USING_TEENSY_AS_LEONARDO (0)

#if USING_TEENSY_AS_LEONARDO
// A Teensy 2.0 that's been flashed with the Arduino Leonardo bootloader (thus
// permanently destroying the HalfKay bootloader that ships with the Teensy).
// The first string is connected to D6, and the second to B4. 
G35String lights_1(12, LIGHT_COUNT);
G35String lights_2(8, LIGHT_COUNT);
#else
// Standard Arduino, string 1 on Pin 13, string 2 on Pin 12.
G35String lights_1(13, LIGHT_COUNT);
G35String lights_2(12, LIGHT_COUNT);
#endif

const int PROGRAM_COUNT = StockProgramGroup::ProgramCount +
  PlusProgramGroup::ProgramCount;

StockProgramGroup stock_programs;
PlusProgramGroup plus_programs;

LightProgram* CreateProgram(G35& lights, uint8_t program_index) {
  randomSeed(rand() + analogRead(0));

  if (program_index < StockProgramGroup::ProgramCount) {
    return stock_programs.CreateProgram(lights, program_index);
  }
  program_index -= StockProgramGroup::ProgramCount;

  if (program_index < PlusProgramGroup::ProgramCount) {
    return plus_programs.CreateProgram(lights, program_index);
  }
  program_index -= PlusProgramGroup::ProgramCount;

  return NULL;
}

LightProgram* CreateProgram_1(uint8_t program_index) {
  return CreateProgram(lights_1, program_index);
}

LightProgram* CreateProgram_2(uint8_t program_index) {
  // If you'd prefer both strings to simultaneously run individual instances of
  // the same program, remove the + 1 offset.
  return CreateProgram(lights_2, program_index + 1);
}

ProgramRunner runner_1(CreateProgram_1, PROGRAM_COUNT,
                       PROGRAM_DURATION_SECONDS);
ProgramRunner runner_2(CreateProgram_2, PROGRAM_COUNT,
                       PROGRAM_DURATION_SECONDS);

void setup() {
  randomSeed(analogRead(0));

  delay(50);
  lights_1.enumerate();
  lights_2.enumerate();
  delay(50);

  lights_1.do_test_patterns();
  lights_2.do_test_patterns();
}

void loop() {
  runner_1.loop();
  runner_2.loop();
}
