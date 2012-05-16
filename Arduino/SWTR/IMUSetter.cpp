/// IMUSetter.cpp
///
/// Implements the FreeIMU shim
///
/// \author  Joseph East
///
#include "IMUSetter.h"
#ifdef _9DOF
#include <FreeIMU.h>

/*
*	WARNING!!
*	THIS MODULE IS CURRENTLY IN DUMMY MODE DUE TO EXCESS MEMORY CONSUMPTION
*	IT WILL COMPILE BUT THE OUTPUT IS HARDCODED AND ESSENTIALLY DOES NOTHING
*
*/

/////////////////////////////////////////////////////////////////////
IMUSetter::IMUSetter(FreeIMU* IMU) : Sensor()
{
	_IMU = IMU;
	//temp.size = 12;
	//temp.tele.f[0] = 0;
	//temp.tele.f[1] = 0;
	//temp.tele.f[2] = 0;
}

/////////////////////////////////////////////////////////////////////
void IMUSetter::read(uint8_t** out, int offset)
{
	// Grab Yaw/Pitch/Roll data and send it to input array
	// 3 floats length = 12 bytes, get your sizes right!
	

	if (_IMU)
	{
		//_IMU->getYawPitchRoll(data);		
	}
	data[0] = 12;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	data[8] = 9;
	data[9] = 0;
	data[10] = 0;
	data[11] = 0;
	data[12] = 0;
	out[offset] = &data[0];
	delay(5);
	
}

void IMUSetter::print()
{
	Serial.print("_9DOF ");
	Serial.println("12 bytes");
}
/////////////////////////////////////////////////////////////////////
#endif