/// IMUSetter.h
///
/// Shim for the FreeIMU library
///
/// \author  Joseph East
///

#ifndef IMUSetter_h
#define IMUSetter_h

#include "Sensor.h"

/*
*	WARNING!!
*	THIS MODULE IS CURRENTLY IN DUMMY MODE DUE TO EXCESS MEMORY CONSUMPTION
*	IT WILL COMPILE BUT THE OUTPUT IS HARDCODED AND ESSENTIALLY DOES NOTHING
*
*/

class FreeIMU;

class IMUSetter : public Sensor
{
	public:

		IMUSetter(FreeIMU* IMU); // FreeIMU instance

		virtual void read(uint8_t** out, int offset); // Inherited read
		virtual void print(); // Inherited print

	protected:

	private:

		FreeIMU* _IMU;

		// TODO: update the library to use struct format per fakeSen.h

		/*struct IMUStruct
		{
			uint8_t size;
			union u_tag
			{
				uint8_t b[12];
				float f[3];
			} tele;
		}; 
		float data[3];
		IMUStruct temp;*/

		uint8_t data[13];
};

#endif