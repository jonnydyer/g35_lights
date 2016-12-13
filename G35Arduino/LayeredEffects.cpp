/*
  G35: An Arduino library for GE Color Effects G-35 holiday lights.
  Copyright © 2011 The G35 Authors. Use, modification, and distribution are
  subject to the BSD license as described in the accompanying LICENSE file.

  By Jonny Dyer

  See README for complete attributions.
*/

#include <LayeredEffects.h>


/*
* alpha_color_t blend_colors(alpha_color_t color1, alpha_color_t color2)
* Description: blends colors using alpha value as best as can be done
* with some simple fixed point math
*/
alpha_color_t blend_colors(alpha_color_t color1, alpha_color_t color2)
{
	uint16_t alpha_temp;
	alpha_color_t color3;
	
	color3.color = (((0x00FF - color2.alpha) * (color1.color & 0x000F) + color2.alpha * (color2.color & 0x000F)) >> 8) +
								((((0x00FF - color2.alpha) * ((color1.color >> 4) & 0x000F) + color2.alpha * ((color2.color >> 4) & 0x000F)) >> 4) & 0x00F0) + 
								(((0x00FF - color2.alpha) * ((color1.color >> 8) & 0x000F) + color2.alpha * ((color2.color >> 8) & 0x000F)) & 0x0F00 );
	color3.intensity = ((0x00FF - color2.alpha) * color1.intensity + (uint16_t)color2.alpha * color2.intensity) >> 8;
	alpha_temp = (uint16_t)color1.alpha + color2.alpha;
	if (alpha_temp > 255)
		color3.alpha = 255;
	else
		color3.alpha = alpha_temp;
	return color3;
}

LayeredEffects::LayeredEffects(G35& g35) : LightProgram(g35) {
  layer0_ = static_cast<alpha_color_t*>(malloc(light_count_ * sizeof(alpha_color_t)));
  layer1_ = static_cast<alpha_color_t*>(malloc(light_count_ * sizeof(alpha_color_t)));
  for (uint8_t i = 0; i < light_count_; i++) {
		if (i % 2)
		{
			layer0_[i] = (alpha_color_t){COLOR_RED, MAX_INTENSITY, 255};
			g35_.set_color(i, MAX_INTENSITY, COLOR_RED);
		}
    else
		{
			layer0_[i] = (alpha_color_t){COLOR_GREEN, MAX_INTENSITY, 255};
			g35_.set_color(i, MAX_INTENSITY, COLOR_GREEN);
		}
		layer1_[i] = (alpha_color_t){COLOR_WHITE, MAX_INTENSITY, 0};
	}
	_snow.state = SNOWING;
	_snow.wind_velocity = 25;
	_snow.snowiness = 5;//random(5, N_FLAKES);
	for(uint8_t i = 0; i<_snow.snowiness; i++)
	{
		_snow.flakes_x[i] = random(0, light_count_);
		_snow.flakes_m[i] = random(64, 200);
		_snow.flakes_y[i] = 0.0 - (float)_snow.flakes_m[i] / 256.0;
	}
	_next_front_millis = millis();
	_blizzard_position = 0;
}

LayeredEffects::~LayeredEffects() {
  free(layer0_);
  free(layer1_);
}

uint32_t LayeredEffects::Do() {
	static uint8_t i, ind;
	static float xdist, overlap, m;
	static int8_t j;
	alpha_color_t led_color;
	switch(_snow.state)
	{
		case SNOWING:
			for (i = 0; i < light_count_; ++i) {
				// Clear the layer
				layer1_[i].alpha = 0;
			}
		
			for (i = 0; i < _snow.snowiness; i++)
			{
				m = (float)_snow.flakes_m[i] *.004;
				if (_snow.flakes_y[i] > m)
				{
					_snow.flakes_x[i] = random(0, light_count_);
					_snow.flakes_m[i] = random(64, 200);
					_snow.flakes_y[i] = 0.0 - (float)_snow.flakes_m[i] *.004;
				}
				else
				{
					_snow.flakes_y[i] += (0.9 - m) * 0.10 + 0.04;
					_snow.flakes_x[i] += ((float)_snow.wind_velocity - 25.0) * 0.01;
					ind = floor(_snow.flakes_x[i]);
					for (j = ind-1; j <= ind + 1; j++)
					{
						if(j < light_count_ && j >= 0)
						{
							xdist = _snow.flakes_x[i] - j;
							overlap = (2.5 - _snow.flakes_y[i]*_snow.flakes_y[i] - (_snow.flakes_x[i] - j)*(_snow.flakes_x[i] - j)) * 64.0;
							if(overlap > 0)
							{
								if(overlap + (float)layer1_[j].alpha > 255)
									layer1_[j].alpha = 255;
								else
									layer1_[j].alpha += overlap;
							}
						}
					}
				}
			}
			for (i = 0; i < light_count_; ++i) {
				led_color = blend_colors(layer0_[i], layer1_[i]);
				g35_.set_color(i, led_color.intensity, led_color.color);
			}
			if(millis() - _next_front_millis > 1000)
			{
				_snow.snowiness += random(0, 7) - 3;
				_snow.wind_velocity += random(0, 7) - 3;
				_next_front_millis = millis();
				if (_snow.snowiness >= N_FLAKES)
					_snow.snowiness = 1;
				if (_snow.wind_velocity & 0x80)
					_snow.wind_velocity = 0;
				else if (_snow.wind_velocity > 50)
					_snow.wind_velocity = 50;
			}
			break;
			
		default:
			break;
	}
  	return 10;
}