/// fakeSen.h
///
/// This is a class for a virtual sensor.
/// Essentially sends characters for loopback testing
///
/// \author  Joseph East 2012
/// 

#ifndef fakeSen_h
#define fakeSen_h

#include "fakeSen.h"
#include "Sensor.h"


class fakeSen : public Sensor
{
	public:
 
		fakeSen();

		virtual void read(uint8_t** out, int offset);
		virtual void print();
	
	protected:

	private:

		uint8_t result[9];
};

#endif