/// HMC6352.cpp
///
/// Implements the HMC6352 compass
/// Code based from Wiring.org example
/// http://wiring.org.co/learning/libraries/hmc6352sparkfun.html
/// \author  Joseph East (easjy002@mymail.unisa.edu.au)
///

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "HMC6352.h"
#include <Wire.h>
/////////////////////////////////////////////////////////////////////
HMC6352::HMC6352() : Sensor()
{
	compassAddress = 0x42 >> 1; // Wire.h library required 7-bit address
	_results.size = 2; // hard coded, compass will only give 2 byte results
}

/////////////////////////////////////////////////////////////////////

void HMC6352::read(uint8_t** out, int offset)
{
	// step 1: instruct sensor to read echoes
	Wire.beginTransmission(compassAddress);  // transmit to device
	// the address specified in the datasheet is 66 (0x42)
	// but i2c adressing uses the high 7 bits so it's 33
	Wire.write('A');        // command sensor to measure angle
	Wire.endTransmission(); // stop transmitting

	// step 2: wait for readings to happen
	delay(10); // datasheet suggests at least 6000 microseconds
	
	// step 3: request reading from sensor
	Wire.requestFrom(compassAddress, 2); // request 2 bytes from slave device #33

	// step 4: receive reading from sensor
	if (2 <= Wire.available()) // if two bytes were received
	{
		_results.data = Wire.read();  // receive high byte (overwrites previous reading)
		_results.data = _results.data << 8; // shift high byte to be high 8 bits
		_results.data += Wire.read(); // receive low byte as lower 8 bits
		_results.data /= 10;

		out[offset] = (uint8_t*)&_results;
	}
}

void HMC6352::print()
{
	Serial.print("_HMC6352 ");
	Serial.print("I2C ADDR: ");
	Serial.print(compassAddress);
	Serial.println(" 2 bytes");
}