/*
  G35: An Arduino library for GE Color Effects G-35 holiday lights.
  Copyright © 2012 The G35 Authors. Use, modification, and distribution are
  subject to the BSD license as described in the accompanying LICENSE file.
  
  By Mike Tsao <http://github.com/sowbug>.
  
  See README for complete attributions.
*/

#ifndef INCLUDE_G35_PROGRAMS_SPOOKY_WAVE_H
#define INCLUDE_G35_PROGRAMS_SPOOKY_WAVE_H

#include <LightProgram.h>

uint8_t checkRadio(void);

class SpookyWave : public LightProgram {
	public:
		SpookyWave(G35& g35);
		~SpookyWave();
  
		uint32_t Do();
 
	private:
		uint8_t *intensities_;
		int8_t *deltas_;
		uint8_t _state;
		uint32_t _next_wave_millis;
		uint8_t _wave_position, _fade_intensity, _ext_trigger;
		enum {
			TWINKLING, WAVE, FADING
		};
};

#endif  // INCLUDE_G35_PROGRAMS_SPOOKY_WAVE_H
