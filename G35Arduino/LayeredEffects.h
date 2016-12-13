/*
  G35: An Arduino library for GE Color Effects G-35 holiday lights.
  Copyright © 2012 The G35 Authors. Use, modification, and distribution are
  subject to the BSD license as described in the accompanying LICENSE file.
  
  By Mike Tsao <http://github.com/sowbug>.
  
  See README for complete attributions.
*/

#ifndef INCLUDE_G35_PROGRAMS_LAYERED_EFFECTS_H
#define INCLUDE_G35_PROGRAMS_LAYERED_EFFECTS_H

#include <LightProgram.h>

#define N_FLAKES 50
#define MAX_INTENSITY 0xCC

typedef struct {
	color_t color;
	uint8_t intensity;
	uint8_t alpha;
}alpha_color_t;

alpha_color_t blend_colors(alpha_color_t color1, alpha_color_t color2);

class LayeredEffects : public LightProgram {
	public:
		LayeredEffects(G35& g35);
		~LayeredEffects();
  
		uint32_t Do();
 
	private:
		alpha_color_t *layer0_;
		alpha_color_t *layer1_;
		uint32_t _next_front_millis;
		uint8_t _blizzard_position;
		enum {
			SNOWING, BLIZZARD
		};
		struct snow_t {
			uint8_t state;
			float flakes_x[N_FLAKES];
			float flakes_y[N_FLAKES];
			uint8_t flakes_m[N_FLAKES];			 // Q1.7 format
			uint8_t wind_velocity;					 // Q0.8 format
			uint8_t snowiness;
		}_snow;
};

#endif  // INCLUDE_G35_PROGRAMS_LAYERED_EFFECTS_H
