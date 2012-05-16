/// Sensor.cpp
///
/// Implemetation of Sensor class
///
/// \author  Joseph East 2011
///

#include <inttypes.h>
#include "RCOIP.h"
#include "Sensor.h"

/////////////////////////////////////////////////////////////////////
Sensor::Sensor()
{
	_target = 0;
}

/////////////////////////////////////////////////////////////////////
Sensor::Sensor(Sensor* target)
{
	_target = target;

}

/////////////////////////////////////////////////////////////////////
void Sensor::read(uint8_t** out, int offset)
{
	if (_target)
		_target->read(out, offset);
}

/////////////////////////////////////////////////////////////////////
/*void Sensor::setTarget(Sensor* target)
{
    _target = target;
}*/

void Sensor::print()
{
	_target->print();
}