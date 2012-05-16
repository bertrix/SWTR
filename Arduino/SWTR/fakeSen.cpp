/// fakeSen.cpp
///
/// This implements the fake sensor
/// A few things to note:
/// The result array is defined inside the fakeSen.h header file
/// Packet structure is device specific which is why they can't be declared at Sensor() level
/// 
/// \author  Joseph East 2012
///

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "fakeSen.h"

/////////////////////////////////////////////////////////////////////
fakeSen::fakeSen() : Sensor()
{

}

/////////////////////////////////////////////////////////////////////
void fakeSen::read(uint8_t** out, int offset)
{
	uint8_t* ptr; // pointer
	ptr = &result[0]; //point to start of array
	ptr++; // increment to where data starts
	result[0] = 8; // first byte stores the real length
	strncpy((char*)ptr, "Hakase\n\0",8); // copy test string into array
	out[offset] = &result[0]; // Send pointer back to program
}
void fakeSen::print()
{
	Serial.print("_fakeSen ");
	Serial.println("8 bytes");
}

/////////////////////////////////////////////////////////////////////