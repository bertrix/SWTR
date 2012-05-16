// HMC6352.h
//
// Driver class for the Honeywell HMC6352 2-axis I2C compass
// Sparkfun sell a nice breakout board
//

#ifndef HMC6352_h
#define HMC6352_h

#include "Sensor.h"

class HMC6352 : public Sensor
{
	public:

		HMC6352();
		virtual void read(uint8_t** out, int offset);
		virtual void print();
		
	protected:

	private:

		int compassAddress; //I2C compass address, this is hard coded

		// Packet structure
		struct results
		{
			uint8_t size; // always '2 bytes' for this sensor
			int data; // int = 2 bytes
		};
	
		struct results _results;
	
};

#endif