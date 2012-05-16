/// fakeSen2.h
///
/// This is a class for a fake sensor.
/// Essentially sends characters for loopback testing
///
/// \author  Joseph East
///

#ifndef fakeSen2_h
#define fakeSen2_h

#include "fakeSen2.h"
#include "Sensor.h"

class fakeSen2 : public Sensor
{
	public:

		fakeSen2();

		virtual void read(uint8_t** out, int offset);
		virtual void print();

	protected:

	private:

		// Packet structure
		// This is treated as a flat array by the program
		// As such the order is imperative
		// 'real' size is first, followed by user data
		struct results
		{
			uint8_t size;
			char data[8];
		};

		struct results _results;
};

#endif