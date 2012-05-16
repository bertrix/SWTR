/// fakeSen2.cpp
///
/// This implements the virtual fake sensor2
/// A few things to note:
///
/// This is functionally the same as fakeSen.h
/// The only difference is that the array is replaced with a casted structure.
/// In terms of memory consumption it is the same but it is easier to understand.
/// The string at the bottom makes it *that* much larger
/// 
/// \author  Joseph East 
///

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "fakeSen2.h"

/////////////////////////////////////////////////////////////////////
fakeSen2::fakeSen2() : Sensor()
{
	_results.size = 8;
}

/////////////////////////////////////////////////////////////////////
void fakeSen2::read(uint8_t** out, int offset)
{
	strncpy(&_results.data[0], "Hakase\n\0",8);	
	out[offset] = (uint8_t*)&_results;
}
void fakeSen2::print()
{
	Serial.print("_fakeSen2 ");
	Serial.println("8 bytes");
}

//////////////////////////////////////////////////////////////////////