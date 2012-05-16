/// analogSen.h
///
/// This is a class for reading the A/D converters on the Arduino
/// This is a 10-bit peripheral, so an int will suffice
///
/// \author  Joseph East 2012
/// 

#ifndef analogSen_h
#define analogSen_h

#include "analogSen.h"
#include "Sensor.h"

class analogSen : public Sensor
{
	public:

		analogSen(uint8_t pin); // initialise with user defined pin

		virtual void read(uint8_t** out, int offset); // Sensor inherited read
		virtual void print(); // Sensor inherited print

	protected:

	private:

		uint8_t _pin;
	
		// Sub packet data, do not change order
		struct results
		{
			uint8_t size; // always 2 bytes for this
			int hyun_ae; // int = 2 bytes
		}; 
		struct results _results;
};

#endif