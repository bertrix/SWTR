// ServoSetter.h
//
// Setter class that outputs its value to a Servo
/// \author  Mike McCauley (mikem@open.com.au)
///
// Copyright (C) 2010 Mike McCauley
// $Id: ServoSetter.h,v 1.2 2010/06/21 01:33:53 mikem Exp $

#ifndef ServoSetter_h
#define ServoSetter_h

#include "Setter.h"

class Servo;

/////////////////////////////////////////////////////////////////////
/// \class ServoSetter ServoSetter.h <ServoSetter.h>
/// \brief Setter class that limits its input to between specified min and max values
///
class ServoSetter : public Setter
{   
public:
    /// \param[in] servo The target Servo instance. It is the callers job to attach the Servo to
    /// the servo output pin
    ServoSetter(Servo* servo);

    /// Input the value to be used to set the servo
    /// Servos are controlled within the range 0 to 180 degrees
    /// Input values in the range 0-255 are scaled to 0-180
    /// Values outside that range are limited to 0 and 255
    /// \param[in] value The input value
    virtual void input(int value);

    /// Called when the source of input data is lost, and the Setter is required to fail in a safe way
    virtual void failsafe();

protected:

private:
    Servo* _servo;
};

#endif 
