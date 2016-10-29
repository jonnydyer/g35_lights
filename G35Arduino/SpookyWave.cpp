/*
  G35: An Arduino library for GE Color Effects G-35 holiday lights.
  Copyright © 2011 The G35 Authors. Use, modification, and distribution are
  subject to the BSD license as described in the accompanying LICENSE file.

  By Jonny Dyer

  See README for complete attributions.
*/

#include <SpookyWave.h>
#include "tables.h"
#include <RFM12B.h>

// You will need to initialize the radio by telling it what ID it has and what network it's on
// The NodeID takes values from 1-127, 0 is reserved for sending broadcast messages (send to all nodes)
// The Network ID takes values from 0-255
// By default the SPI-SS line used is D10 on Atmega328. You can change it by calling .SetCS(pin) where pin can be {8,9,10}
#define NODEID          2  //network ID used for this unit
#define NETWORKID       99  //the network ID we are on
#define GATEWAYID     	0  //the node ID we're sending to

char start_payload[] = "A";
char end_payload[] = "B";
RFM12B radio;

uint8_t checkRadio(void)
{
	if (radio.ReceiveComplete())
	{
		if (radio.CRCPass())
	    {
			if((uint8_t)radio.Data[0] == *end_payload)
			{
				return 1;
			}
		}
	}
	return 0;
}

SpookyWave::SpookyWave(G35& g35) : LightProgram(g35) {
  intensities_ = static_cast<uint8_t*>(malloc(light_count_ * sizeof(uint8_t)));
  deltas_ = static_cast<int8_t*>(malloc(light_count_ * sizeof(int8_t)));
  for (uint8_t i = 0; i < light_count_; ++i) {
    intensities_[i] = rand();
    deltas_[i] = rand() % 5 - 2;
  }
  g35_.fill_color(0, light_count_, 255, COLOR_BLACK);
	_state = TWINKLING;
	_next_wave_millis = millis() + random(3000, 20000);
	_wave_position = 0;
	_fade_intensity = 0;
	_ext_trigger = 0;
	radio.Initialize(NODEID, RF12_433MHZ, NETWORKID);
	radio.Sleep(); //sleep right away to save power
}

SpookyWave::~SpookyWave() {
  free(intensities_);
  free(deltas_);
}

uint32_t SpookyWave::Do() {
	static uint8_t i;
	switch(_state)
	{
		case TWINKLING:
			for (i = 0; i < light_count_; ++i) {
				if (i % 2)
			    	g35_.set_color(i, intensities_[i], COLOR_ORANGE);
				else
				    g35_.set_color(i, intensities_[i], COLOR_PURPLE);
			    intensities_[i] += deltas_[i];
			}
			
			if(millis() > _next_wave_millis)
			{
				_state = WAVE;
				_wave_position = 1;
				_ext_trigger=0;
				radio.Wakeup();
				radio.Send(GATEWAYID, start_payload, 1, 0);
				radio.Sleep();
			}
			break;
			
		case WAVE:
			if(_wave_position==(light_count_-1) && _ext_trigger==0)
			{
				radio.Wakeup();
				radio.Send(GATEWAYID, end_payload, 1, 0);
				radio.Sleep();
			}
			for(i=0; i < _wave_position; i++)
			{
					
				if(i >= light_count_)
					break;
					
				if(_ext_trigger == 0)
					g35_.set_color(light_count_ - i - 1, pgm_read_byte(&wave_table[_wave_position - i - 1]), COLOR_GREEN);
				else
					g35_.set_color(i, pgm_read_byte(&wave_table[_wave_position - i - 1]), COLOR_GREEN);
			}
			_wave_position++;
			if(_wave_position > NUM_WAVE_ENTRIES)
			{
				_state = FADING;
				_fade_intensity = 0;
			}
			break;
			
		case FADING:
			for (i = 0; i < light_count_; ++i) {
				if (i % 2)
			    	g35_.set_color(i, ((uint16_t)intensities_[i] * _fade_intensity) >> 8, COLOR_ORANGE);
				else
				    g35_.set_color(i, ((uint16_t)intensities_[i] * _fade_intensity) >> 8, COLOR_PURPLE);
			}
			_fade_intensity += 3;
			if (_fade_intensity >= 210)
			{
				_state = TWINKLING;
				_next_wave_millis = millis() + random(3000, 20000);
			}
			break;
			
		default:
			break;
	}
	if(checkRadio())
	{
		_state = WAVE;
		_wave_position = 1;
		_ext_trigger=1;
	}
  	return 8;
}
