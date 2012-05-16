/// analogSen.cpp
///
/// This implements the analog sensor
/// 
/// \author  Joseph East 2012
///

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "analogSen.h"

/////////////////////////////////////////////////////////////////////
analogSen::analogSen(uint8_t pin) : Sensor()
{
	_pin = pin;
	_results.size = 2;
}

/////////////////////////////////////////////////////////////////////
void analogSen::read(uint8_t** out, int offset)
{
	_results.hyun_ae = analogRead(_pin);
	out[offset] = (uint8_t*)&_results;
}

void analogSen::print()
{
	Serial.print("_ANALOG ");
	Serial.print("Pin ");
	Serial.print(_pin);
	Serial.println(" 2 bytes");
}

/////////////////////////////////////////////////////////////////////

