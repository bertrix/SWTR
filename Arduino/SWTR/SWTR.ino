// Common Digital Platform
// FYP 2011K
// Joseph East
// University of South Australia
//
// This is the Arduino frontend for the Common Digital Platform firmware
// It is based on RCRx from Open Systems Australia with significant modifications
// It compiles to a variety of sizes, minimum 15K. Recommend usage of a Duemilanove or above.
//
// Idea is to use a RCTx compatible application to transmit motor data to the unit.
// The unit will then act on the motor data and send information back
// 
// The backend section can be found in RCRx.cpp and the associated header files RCOIP.h and RXRx.h
// Further information is pending
// Copyright (C) 2010 Mike McCauley
// Modifications by Joseph East 2011

// Default libraries that will almost always be used, so good idea not to touch these
#include "Settings.h"    // Main settings header. Must be the first thing to be included
#include <WiShield.h>    // Wishield TCP stack
#include "RCRx.h"        // RCRx backend
#include "RCOIP.h"       // RCoIP definitions, required for a number of files
#include <g2100.h>       // MRF24WB0M/X WiFi stack
//#include "IL298Setter.h" // Inverted L298 setter interface
//#include <IL298.h>       // Inverted L298 controller (low level)				
#include "ServoSetter.h" // Arduino Servo setter interface
#include <Servo.h>       // Arduino Servo class (low level)

#ifdef _DEBUG
#include "MemoryFree.h"  // Free memory library
#endif

	/****************************************************************************
	* WARNING: Ensure hardware is physically installed else mishaps may occur!*
	**************************************************************************/

// These declarations are for the FreeIMU  low level operations
// They are bound by a define in settings.h, so no need to touch them.
// That actually goes for most of below

// FreeIMU setter interface
// If you're wondering why so many libraries
// FreeIMU is a complete processing platform in and of itself.
// So this firmware can host the FreeIMU platform in addition to its other functions
// The setter provides a shim to interface with the wider program

#ifdef _9DOF 
#include "IMUSetter.h"
#include <ADXL345.h>
#include <HMC58X3.h>
#include <ITG3200.h>
#include <bma180.h>
#include <MS561101BA.h>
#include "DebugUtils.h"
#include "FreeIMU.h"
#include "CommunicationUtils.h"
#endif

#include <Wire.h> // Arduino I2C class (low level)

// These other sensors are direct access hardware
// No pre-processing, we read straight from the wire
// and do everything else on-the-fly

// NMEA parser
#ifdef _NMEA
#include <NMEA.h>
#endif

// HMC6352 I2C compass
#ifdef _HMC6352
#include "HMC6352.h"
#endif

// Fake sensor for testing
#ifdef _fakeSen
#include "fakeSen.h"
#endif

// Fake sensor2 for testing
#ifdef _fakeSen2
#include "fakeSen2.h"
#endif

// Analog sensor

#ifdef _ANALOG
#include "analogSen.h"
#endif

// Declare the receiver object
RCRx rcrx;

// Declare IMU
#ifdef _9DOF
FreeIMU IMU;
#endif

// Declare GPS
#ifdef _NMEA
NMEA GPS;
#endif

// Declare compass
#ifdef _HMC6352
HMC6352 compass;
#endif

#ifdef _ANALOG
analogSen as0(A0);
#endif

// Declare fake sensor
#ifdef _fakeSen
fakeSen virtSen;
#endif

// Declare fake sensor2
#ifdef _fakeSen2
fakeSen2 virtSen;
#endif

// Define numbers of inputs and outputs
#define NUM_OUTPUTS 2
#define NUM_INPUTS 1

// Define brushed motors and servos
//IL298 hMotors[2];
Servo servos[2];

// Setup the setter classes for outputs
//IL298Setter setter0(&hMotors[0], false);
//IL298Setter setter1(&hMotors[0],true);
//IL298Setter setter2(&hMotors[1], false);
//IL298Setter setter3(&hMotors[1], true);
ServoSetter setter0(&servos[0]);
ServoSetter setter1(&servos[1]);

// Setup the setter classes for inputs
#ifdef _9DOF
IMUSetter senset(&IMU);
#endif

// These are the setter interface classes that the sent to the backend
Setter* something[NUM_OUTPUTS] = {&setter0, &setter1};

	/********************************************************
	* Here we map physical pins to the input setter classes*
	********************************************************/
#ifdef _SENSOR
Sensor* peripheral[NUM_INPUTS] = {&virtSen};
#endif

// Macro to define all the wireless settings.
// For information or to change the config consult Settings.h
_WIFI_SETTINGS_AP_
unsigned char ssid_len;
unsigned char security_passphrase_len;

void setup()
{
	Serial.begin(9600);
	Wire.begin();
	delay(5);
	Serial.println("SWTR V1.1 Build 120516 revision.F");
	Serial.println("Loaded modules & order: ");
	#ifdef _9DOF
	IMU.init(); // the parameter enable or disable fast mode
	delay(5);
	#endif

	#ifdef _SENSOR
	Serial.println("_SENSOR");
	// Map sensors to the program
	rcrx.setInputs((Sensor**)&peripheral, NUM_INPUTS);
	#endif
	Serial.println("-- END MODULE LIST -- ");
	Serial.print(NUM_OUTPUTS);
	Serial.println(" registered outputs");
	Serial.print(NUM_INPUTS);
	Serial.println(" registered inputs");

	/********************************************************
	* Here we map physical pins to the output setter classes*
	********************************************************/

	servos[0].attach(3); 
	servos[1].attach(5);
	servos[0].write(75);
	servos[1].write(90);  
	//hMotors[0].pinSetter(4,5);
	//hMotors[1].pinSetter(7,6);

	// Tell the receiver where to send the 3 channels
	rcrx.setOutputs((Setter**)&something, NUM_OUTPUTS);

	// Initialise the receiver
	rcrx.init();
	Serial.println("Connected");
        #ifdef _DEBUG
	Serial.print("free = ");
	Serial.println(freeMemory());
	#endif
}

void loop()
{
	// And do it
	rcrx.run();

	// Failsafe check (add in a check for motor state as well)
	// If the unit drifts out of range or the network connection is severed during a socket operation
	// then the connected flag is never reset. While this happens constantly run the periodic task
	// so the failsafe timer is still active. Otherwise it only runs during a socket appcall which is bad.
	if(rcrx.connected())
	{
		rcrx.periodicTask();
	}
	
}
