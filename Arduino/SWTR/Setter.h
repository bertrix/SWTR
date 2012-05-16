// Setter.h
//
// Virtual base class for classes that receive a value, maybe transform it and do something with it
// 
/// \author  Mike McCauley (mikem@open.com.au)
///
// Copyright (C) 2010 Mike McCauley
// $Id: Setter.h,v 1.3 2010/06/28 00:56:10 mikem Exp $

#ifndef Setter_h
#define Setter_h

/////////////////////////////////////////////////////////////////////
/// \class Setter Setter.h <Setter.h>
/// \brief Virtual base class for classes that receive a value, maybe transform it and then 
/// do something with it
///
/// Setters (ie different subclasses of Setter) are designed to be chained together.
/// Each Setter in a chain transforms its input value in some way and then maybe passes it on to 
/// its output Setter.
/// Setters can be designed to do almost any transformation with a number, including 
/// changing it, sending it to some device, using the value to set a physical output etc.
/// Each setter can accept as input an int (ie a 16 bit signed number) although some Setters
/// may be more constrained as to which value ranges make physical sense.
class Setter
{
public:
	/// Constructor.
	Setter();

	/// Constructor with setting the target 
	Setter(Setter* target);

	/// This is where incoming values are set.
	// This default implementation merely passes its value to the output
	/// \param[in] value The input value
	virtual void input(int value);

	/// Connects this Setter to a downstream Setter
	/// \param[in] target Pointer to a sublass of Setter, whose input() function will be called
	/// when a new value is available from this Setter.
	virtual void setTarget(Setter* target);

	/// Called when the source of input data is lost, and the Setter is required to fail in a safe way.
	/// Subclasses can override.
	/// Default is to send to next Setter in the chain.
	virtual void failsafe();

	/// Sets the failsafeValue
	void setFailsafeValue(int failsafeValue);

	/// Returns the most recently set failsafe value.
	/// Base class does not use this
	int failsafeValue();

protected:
	/// This is the instance of Setter that will be given the transfotmed output value
	Setter* _target;

	/// This is the failsafe value, which some setters use to set the output when
	/// a failsafe call is made
	int     _failsafeValue;

private:
};

#endif 