// ServoSetter.cpp
//
// Template for a CPP file
/// \author  Mike McCauley (mikem@open.com.au)
///
// Copyright (C) 2010 Mike McCauley
// $Id: ServoSetter.cpp,v 1.2 2010/06/21 01:33:53 mikem Exp $

#include "ServoSetter.h"
#include <Servo.h>

/////////////////////////////////////////////////////////////////////
ServoSetter::ServoSetter(Servo* servo) : Setter()
{
    _servo = servo;
}

/////////////////////////////////////////////////////////////////////
void ServoSetter::input(int value)
{
  /*  // Clip and scale the value to suit the Servo
    if (value < 0)
	value = 0;
    if (value > 255)
	value = 255;
    value = ((long)value * 180) / 255;
*/
    // Output the value to the servo
    if (_servo)
	_servo->write(value);
}

/////////////////////////////////////////////////////////////////////
void ServoSetter::failsafe()
{
// BE WARY OF THIS SETTING, IT MAY FAIL DEADLY DEPENDING ON SERVO BRAND
// CHANGE APPROPRIATELY
	_servo->write(0);
}

