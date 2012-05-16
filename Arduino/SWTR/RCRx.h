/// RCRx.h
///
/// Suburban Wetland TransceiveR
///
/// \mainpage SWTR library for Arduino
///
/// This is a fork of the Arduino RCKit 1.1 library by Mike McCauley
/// http://www.open.com.au/mikem/arduino/RCKit/
/// 
/// SWTR provides a kit of software objects that make it easy to build an
/// RCOIP  (Remote Control Over IP) receiver on Arduino. RCOIP protocol is used to
/// carry remote control commands from a transmitter to a receiver over an IP
/// transport such as Ethernet or Wi-Fi.
///
/// As such it can be used to build Remote Controlled (RC) vehicles and devices of various kinds, along with
/// matching transmitters. Large numbers of channels can be supported, along with a back-channel 
/// which sends data from the Receiver to the Transmitter. 
/// A compatible iPhone transmitter app is also available.
/// 
/// The transmitter could be a handheld Wi-Fi enabled device such as an iPhone, iPad or Arduino based device. 
/// Or it could be an interactive program running on a desktop computer. The RCTx iPhone transmitter app is 
/// available on the Apple App Store. It provides a simple RCOIP transmitter equipped with 2 joysticks and
/// a number of switches, however it won't work for this port without modification.
/// A sample application written in C# is instead available in the project repository
///
/// The receiver (using this SWTR software) could be a remote controlled car, 
/// plane, helicopter or some other device.
///
/// The RCOIP protocol is a 2-way protocol that defines TCP messages between the RCOIP transmitter and receiver. 
/// This was changed from UDP as in original RCKit due to problems encounterd with the WiShield library, 
/// however those errors may have since been rectified upstream. 
/// Until a compelling reason rises to switch back to UDP this fork will remain on TCP.
///
/// This effectively makes the transmitter a TCP client and the receiver a TCP server. 
/// Messages sent from the transmitter to the receiver include setting analog outputs, 
/// and messages from the receiver to the transmitter include receiver status messages.
///
/// RCTx, RCKit and the RCOIP protocol offer the following advantages over conventional Remote Control:
/// \li Large numbers of channels (> 100) not just 5 or 6, including analog, digital, text, data etc.
/// \li Back channel for telemetry (voltages, signal strengths, GPS position etc)
/// \li Failsafe modes
/// \li Programmable, configurable and extensible
/// \li Works with a variety of types of vehicle and remote devices
/// \li Hackable
/// \li Opportunity to tightly integrate remote control and autonomous vehicle software control
/// \li Works with a variety of IP transports Wi-Fi (ad-hoc or infrastructure), Wired etc.
/// 
/// By default this fork operates on port 9048 which is controller by Open Systems Consultants.
///
/// For further information please consult the wiki
/// https://github.com/bertrix/SWTR/wiki
///

#ifndef RCRx_h
#define RCRx_h

#include "Settings.h"
#include <WiShield.h>
#include <inttypes.h>
#include "RCOIP.h"

#ifdef _9DOF
#include "FreeIMU.h"
#endif

class Setter;
#ifdef _SENSOR
class Sensor;
#endif

/////////////////////////////////////////////////////////////////////
/// \class RCRx RCRx.h <RCRx.h>
/// \brief Remote Control Receiver module for RCOIP protocol on Arduino
///
/// \par Overview
/// This class implements a receiver for RCOIP (Remote Control Over IP). It starts and manages a 
/// WiFi receiver, which receives TCP messages containing RCOIP commands such as remote control channel 
/// values. When channel setting commands are received they are translated into output values which are sent 
/// to Setter objects to control the phycical output devices and pins on the Arduino. 
/// Supports WiShield 1.0 etc.
///
/// \par Outputs
/// SWTR maps RCOIP channels to physical output devices through the analogOutputs array. This is an array 
/// of Setter objects, one for each physical output to be controlled by SWTR. Whenever a RCOIP message 
/// is received with new channel settings, the input() function of each Setter with new data will be 
/// called. This will cause each Setter to set its physical output in response to the remote control
/// data received by an RCOIP.
///
/// \par Inputs
///	In addition, SWTR allows sensors attached to the unit to feedback data to the connected receiver
/// Using a shim class called Sensor you can define how the data can be collected and pre-processed
/// before sending it off to the PC. Data collection occurs everytime a keep-alive packet is sent
///
/// \par Failsafe
/// RCRx supports failsafe behaviour if a connection to the transmitter is lost.
/// RCRx monitors the time of each received RCOIP request. If no request is received for more than 
/// failInterval milliseconds, it will be considered as disconnected, and the failsafe() function of 
/// every Setter will be called, allowing each Setter to adopt its failsafe configuration 
/// (eg throttle to 0 etc). This allows remote control vehicles to fail safe if the transmitter 
/// fails or goes out of range.
///
/// \par WiShield Library Configuration
/// Check the patchlist.txt file inside the source
/// Otherwise for IP configuration and sensors check out Settings.h
///
/// This library has been tested with Arduino Uno, Arduino 1.0 and WiShield 1.0
///
/// \par Installation
///
/// Install in the usual way: unzip the distribution zip file to the libraries
/// sub-folder of your sketchbook. Dont forget the prerequisites too.
class RCRx
{
public:
	/// Constructor. After contruction and initialisation, call the init() and run() functions.
	RCRx();
	
	/// Assign the setters to the program
	void setOutputs(Setter** analogOutputs, uint8_t numAnalogOutputs);
	
	/// Assign the sensors to the progam
	#ifdef _SENSOR
	void setInputs(Sensor** inputs, uint8_t numInputs);
	#endif

	/// Initialises the wireless WiFi receiver
	/// Call once at startup time after addresses etc have been configured.
	void init();

	/// Call this to process pending Wireless events. Call this as often as possible in your
	/// main loop. Runs the wireless driver stack.
	void run();

	/// Call to handle an incoming UDP message containing an RCOIP command message.
	/// This is usually only called from within RCRx, but could be called externally 
	/// for testing purposes etc.
	/// \param[in] msg Pointer to the UDP message
	/// \param[in] len Length of the UDP mesage in bytes
	/// \param[in] rssi Receiver Signal Strength as reported by the WiFi receiver 
	/// when the message was received.
	void handleRequest(uint8_t *msg, uint16_t len, uint16_t rssi);

	/// Called by RCRx when no RCOIP message has been received for more than failInterval miliseconds.
	/// Calls the failsafe function for all configured output Setters.
	void failsafe();

	/// Called by RCRx periodically (typically twice per second) to do period processing such as 
	/// detecting loss of messages
	void periodicTask();

	/// Returns whether the RCRx considers itself to be connected to the transmitter.
	/// Initialsed to false. Whenever an RCOIP request is receved, set to true. If no  RCOIP request 
	/// is receved for more than  failInterval miliseconds, set to false.
	/// \return true if the RCRx is still receiving messages from the transmitter.
	boolean connected();	

	/// Populates the reply packet with data
	/// Requires a pointer to the RCOIPv1ReplyReceiverStatus structure
	void populateReply(RCOIPv1ReplyReceiverStatus*& _reply);	

	/// As packet sizes are calcualted dynamically we need a global means of finding them out
	/// Set and Get packet sizes obviously go this. There's a maximum size of 255 bytes per packet
	/// but that should be able to be expanded with a change of units.
	void setSize(uint8_t in);	
	uint8_t getSize();
	uint8_t getInputs();

protected:

private:
	/// Array of output Setters
	Setter**      _analogOutputs;

	/// Array of input Setters
	#ifdef _SENSOR
		Sensor** _inputs;
	#endif

	/// Number of Setters in _analogOutputs
	uint8_t       _numAnalogOutputs;
	
	/// Number of Sensors in _inputs	
	uint8_t _numInputs;

	/// Arduino analog input pin which yields the arduino battery voltage
	uint8_t _batteryVoltageAnalogPin;

	/// Max time in milliseconds between RCOIP replies
	unsigned int _replyInterval; 

	/// Max time in milliseconds between received request before RCRx is considered to be disconnected
	unsigned int  _failInterval; 

	/// The time we last got a RCOIP request from the transmitter
	unsigned long _lastRequestTime;

	/// The last time we sent a RCOIP reply to the transmitter
	unsigned long _lastReplyTime;

	/// Whether RCRx is considered to be connected to the transmitter
	boolean _connected;

	/// The value of the RSSI (receiver signal strength indicator)
	/// in the last request received
	uint16_t _rssi;
	
	/// Packet size of the data we are sending back to the PC
	uint8_t packetSize;

	/// The IMU (if any) attached to the unit
	#ifdef _9DOF
	FreeIMU* IMU;
	#endif
	
};

#endif 