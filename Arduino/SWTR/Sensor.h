/// Sensor.h
///
/// Virtual base class for classes that receive a value, maybe transform it and do something with it
///
/// \author  Joseph East 2011
///

#ifndef Sensor_h
#define Sensor_h
#include <inttypes.h>
#include "RCOIP.h"

/////////////////////////////////////////////////////////////////////
/// \class Sensor Sensor.h <Sensor.h>
/// \brief Virtual base class for classes that receive a value, maybe transform it and then 
/// do something with it
///
/// Sensors (ie different subclasses of Sensor) are designed to be chained together.
/// Each Sensor in a chain grabs input from hardware, maybe processes it then passes 
/// a pointer to its results to the calling function
/// While sensors may output any size value, in the end it is split into byte chunks
/// 
class Sensor
{   
	public:
		/// Constructor.
		Sensor();

		/// Constructor with setting the target 
		Sensor(Sensor* target);


		/// This default implementation merely passes its value to the output
		virtual void read(uint8_t** out, int offset);

		/// Connects this Sensor to a downstream Sensor
		/// \param[in] target Pointer to a sublass of Sensor, whose input() function will be called
		/// when a new value is available from this Sensor.
		//virtual void setTarget(Sensor* target);
		virtual void print();
    
	protected:
		/// This is the instance of Sensor that will be given the transfotmed output value
		Sensor* _target;

	private:
};

#endif 
