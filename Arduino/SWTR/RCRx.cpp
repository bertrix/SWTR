/// RCRx.cpp
///
/// Remote Control Receiver module for RCOIP protocol
/// \author  Mike McCauley (mikem@open.com.au)
/// \author  Modified for SWAT drone by Joseph East 2012
///
/// Copyright (C) 2010 Mike McCauley
/// $Id: RCRx.cpp,v 1.3 2010/06/28 00:56:10 mikem Exp $

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "RCRx.h"
#include "Setter.h"

#ifdef _SENSOR
#include "Sensor.h"
#endif

#include "g2100.h"
#ifdef _DEBUG
#include "MemoryFree.h"
#endif

// There is precisely one RCRx instance
static RCRx* rcrx;

// Declare the reply packet object globally as it's easier to deal with
static RCOIPv1ReplyReceiverStatus* reply;

// Keeps the size of the current packet
static uint8_t tubes = 0;

/////////////////////////////////////////////////////////////////////
// Glue between the WiShield C side and the RCRx C++ side
// It bears some resemblance to the socketapp.c example in WiShield
// But it's somewhat different
extern "C"
{
	#include "socketapp.h"
	#include "uip.h"

	/*
	* Declaration of the protosocket function that handles the connection
	* (defined at the end of the code).
	*/
	static int handle_connection(struct socket_app_state *s);

	/*---------------------------------------------------------------------------*/

	void socket_app_init(void)
	{
		/*
		* We start to listen for connections on TCP port 9048.
		*/
		uip_listen(HTONS(RCOIP_DEFAULT_RECEIVER_TCP_PORT));
	}
	/*---------------------------------------------------------------------------*/

	void socket_app_appcall(void)
	{

		/*
		* The uip_conn structure has a field called "appstate" that holds
		* the application state of the connection. We make a pointer to
		* this to access it easier.
		*/
		struct socket_app_state *s = &(uip_conn->appstate);

		/*
		* If a new connection was just established, we should initialize
		* the protosocket in our applications' state structure.
		*/
		if(uip_connected()) 
		{
			PSOCK_INIT(&s->p, s->inputbuffer, sizeof(s->inputbuffer));
		}

		/* do some tasks, periodically. Called twice a second IIRC from down the stack */
		
		rcrx->periodicTask();
		
		/*
		* Finally, we run the protosocket function that actually handles
		* the communication. We pass it a pointer to the application state
		* of the current connection.
		*/

		handle_connection(s);

		/*
		* After the socket is handled we do some keepalive checks
		* to ensure that everything is still ok, otherwise kill motors.
		* uip_aborted checks for disgraceful disconnects (yank the cable)
		* uip_closed checks for graceful disconnects (normal socket.close())
		*
		* periodicTask() has a built-in timeout as orginally in RCRx
		* however it only checks during its execution cycle and can't see uip functions
		* so in the cases of graceful / disgraceful disconnect the unit thinks everything is fine
		* which is unacceptible in our real-world scenario.
		* Unless of course the setter library has a 'snap shut' function if it receives no data, in which case this is useless.
		* But the setter library won't easily support L298s from what I can tell.
		*/

		//if(uip_aborted())
		//{
			//rcrx->failsafe();
		//}

	}

	// This thread will handle one UDP 'connection'
	// Caution: RSSI support requires mods to WiShield g2100.c as per 
	// http://asynclabs.com/forums/viewtopic.php?f=10&t=385&start=0
	// Note that if using the user_contrib driver comment out delcaration
	// U16 zg_get_rssi() inside g2100.h, otherwise it will fail to compile
	// This is because we are in a external "C" block which doesn't know about g2100.h explicitly
	// Keeping both in will confuse the linker

	// If you are using this software as part of Common Digital Platform, I've already handled it.
    extern U16 zg_get_rssi();


	/*---------------------------------------------------------------------------*/
	/*
	* This is the protosocket function that handles the communication. A
	* protosocket function must always return an int, but must never
	* explicitly return - all return statements are hidden in the PSOCK
	* macros.
	*/
	static int handle_connection(struct socket_app_state *s)
	{
		// We declare the pointer here for the moment, easier to dispose of later
		PSOCK_BEGIN(&s->p);
		while(!uip_closed())
		{

			/*
			* This is where the major change is.
			* In socketapp it reads in a string and does stuff to it
			but we aren't using strings, nor should we (well I guess we could...)
			* Originally in RCRx the UDP wrapper handled all the nitty gritty
			* but now we have to rely on raw sockets. This has minimal impact
			* on the receiving side of things, but sending packets back changes substantially.
			*/

			rcrx->handleRequest((uint8_t*)uip_appdata, uip_len, zg_get_rssi());
			rcrx->populateReply(reply);

			tubes = rcrx->getSize(); //easier to get size of packet directly rather than recalculate

			PSOCK_SEND(&s->p, (uint8_t*)reply, tubes);

			free(reply);
			reply = 0;
		}

	// The following code executes very rarely
	memset(s->inputbuffer, 0x00, sizeof(s->inputbuffer));
	PSOCK_CLOSE(&s->p);
	PSOCK_END(&s->p);                
	}
}

/////////////////////////////////////////////////////////////////////
/* RCRX constructor */
RCRx::RCRx()
{

	_analogOutputs = 0;
	#ifdef _SENSOR
	_inputs = 0;
	#endif
	_numInputs = 0;
	_numAnalogOutputs = 0;
	_batteryVoltageAnalogPin = 0;
	_replyInterval = RCOIP_DEFAULT_REPLY_INTERVAL;
	_failInterval = RCOIP_DEFAULT_FAIL_INTERVAL;
	_lastRequestTime = 0;
	_lastReplyTime = 0;
	_connected = 0;
	_rssi = 0;

	// record this as the singleton
	rcrx = this;
}
//////////////////////////////////////////////////////////////////////
void RCRx::setOutputs(Setter** analogOutputs, uint8_t numAnalogOutputs)
{
	_analogOutputs = analogOutputs;
	_numAnalogOutputs = numAnalogOutputs;
}
//////////////////////////////////////////////////////////////////////
#ifdef _SENSOR
void RCRx::setInputs(Sensor** inputs, uint8_t numInputs)
{
	_inputs = inputs;
	_numInputs = numInputs;
	// Print order to the terminal
	for(int z = 0; z < _numInputs; z++)
	{
		_inputs[z]->print();
	}
}
#endif
/////////////////////////////////////////////////////////////////////
void RCRx::init()
{
	WiFi.init();
}

/////////////////////////////////////////////////////////////////////
/*
 * This calls down into the uIP stack to do the thing it does best(?)
 * Put simply, it redirects to socketapp_appcall() (in most cases)
 * That's skipping over a lot of details, but without going into the uIP
 * docs just assume it skips over to that function. If it doesn't
 * then blame the nose.
 */
void RCRx::run()
{
	WiFi.run();
}

/////////////////////////////////////////////////////////////////////
// Sends a Reply message to the transmitter
/*
 * sendReply should actually be more like 'prepare reply'
 * It just updates the global packet struc around line 25
 * No idea about memory impact, this was a solution found in desparation
 */
void RCRx::populateReply(RCOIPv1ReplyReceiverStatus*& _reply)
{	
	// Please note the header
	// This new version includes a 'universal' buffer so you don't have to go re-doing the spec for every sensor change
	// However you'll need to change the clients to correctly parse the new format.
	uint8_t size = 0;

	#ifdef _SENSOR
	uint8_t *mptr; //marshalling pointer
	uint8_t *marshall[_numInputs];

	// How this works:
	// We pass an array of pointers called marshall to each of the sensor objects
	// The sensors return a pointer to a struct which is casted as an array of bytes
	// Due to pointer arithmatic we cannot know the size of this resultant array directly.
	// To counter this, the first element of every result array contains its real size in bytes
	// So we set mptr to browse though every first element of marshall to extract the size we need
	// Then dynamically generate an array and copy the marshall results across based on those sizes
	// We'll also need to separate the results somewhat, maybe a checksum or a 0xFF
	// Check the HMC6352 or fakeSen libraries for sample applications
	// Note that it is imperative that the arrays are declared correctly
	// Any mistake may lead to an overrun which will reset or lockup the micro.
	// The Freemem() function is there to help

	// Obtain all the pointers to the sensor structs and store
	for(int z = 0; z < _numInputs; z++)
	{
		_inputs[z]->read(marshall, z);
	}

	// Look at first element of every array to determine size of resultant packet
	for(int z = 0; z < _numInputs; z++)
	{
		mptr = marshall[z];
		size += *mptr;
	}
	#endif

	// Declare the dynamic array based on the determined size and some spacing bytes
	uint8_t rSize = size + _numInputs;
	uint8_t sensorResult[rSize]; //We allow 1 byte of separation

	// This is the tricky part
	_reply = (RCOIPv1ReplyReceiverStatus*)malloc(sizeof(struct packetStruct) + (rSize * (sizeof(char))));

	#ifdef _DEBUG
	Serial.print("freeMemory() = ");
	Serial.println(freeMemory());
	Serial.println(rSize + sizeof(struct packetStruct));
	#endif

	_reply->version = RC_VERSION;
	_reply->RSSI = _rssi;
	_reply->batteryVoltage = analogRead(_batteryVoltageAnalogPin) >> 2; // Arduino is 10 bits analog and we've only an 8 bit variable
	
	#ifdef _SENSOR
	// Fill 'er up
	int sensorCounter = 0; // a running counter to know where we are in the array
	for(int z = 0; z < _numInputs; z++)
	{
		mptr = marshall[z];
		int h = *mptr; // h stores the size of the current packet
		mptr++;// increment the pointer, we are at the first packet
		for(int j = 0; j < h; j++)
		{
			sensorResult[sensorCounter] = *mptr;
			mptr++;
			sensorCounter++;
		}
	sensorResult[sensorCounter] = 0xFF;
	sensorCounter++;
	}

	for(int z = 0; z < rSize; z++)
	{
		_reply->status[z] = sensorResult[z];
	}
	#endif

	_lastReplyTime = _lastRequestTime;
	setSize(rSize + sizeof(struct packetStruct));
}

void RCRx::setSize(uint8_t in)
{
	packetSize = in;
}

uint8_t RCRx::getSize()
{
	return packetSize;
}
/////////////////////////////////////////////////////////////////////
// This is called when a new message is received in the RC port
// Caution: RSSI support requires mods to WiShield g2100.c as per 
// http://asynclabs.com/forums/viewtopic.php?f=10&t=385&start=0
// You've probably seen this message before further up
void RCRx::handleRequest(uint8_t *msg, uint16_t len, uint16_t rssi)
{
	// Bind the RSSI readings to a local variable
	_rssi = rssi;
	// Bind the incoming packet to the firmware-defined struct, we assume it has been packed properly as there are no checks
	// Structure is also assumed to be correct. All defined in RCOIP.h
	RCOIPCmdSetAnalogChannels* setAnalogChannels = (RCOIPCmdSetAnalogChannels*)msg;
	// Loop through the primary array and extract the critical data
	if (len >= 1 && setAnalogChannels->version == RC_VERSION1)
	{
		int j = 0;
		for(int z = 0; z < _numAnalogOutputs; z++)
		{
			_analogOutputs[z]->input((int)setAnalogChannels->channels[j]);
			j++;
		}

	// Only reply at most once per _replyInterval millis
	// Transmitter will typically interpret gap in replies
	// of 2 sec or more as bad connection
		_lastRequestTime = millis();

		if (_lastRequestTime > _lastReplyTime + _replyInterval)
			Serial.println("Transmit");
	}
}

/////////////////////////////////////////////////////////////////////
// Boolean check as classes outside of UIP can't access the uip debug routines
// Like connection testing
boolean RCRx::connected()
{
	return _connected;
}

/////////////////////////////////////////////////////////////////////
// Call the failsafe functions of all outputs
void RCRx::failsafe()
{
	Serial.print("Disconnect @ ");
	Serial.println(millis());
	for (int i = 0; i < _numAnalogOutputs; i++)
	if(_analogOutputs[i])
		_analogOutputs[i]->failsafe();
}

/////////////////////////////////////////////////////////////////////
// This is called twice per second or so by the UDP code
// do some maintenance like looking for lost connections etc
void RCRx::periodicTask()
{
	
	boolean wasConnected = _connected;
	_connected = millis() < (_lastRequestTime + _failInterval);
	free(reply); //Imperative, on disconnect the packet isn't cleared
	reply = 0; // Force a clear every cycle to prevent leaks
	// If we just lost the connection, fail safe
	if (wasConnected && !_connected)
	{
		failsafe();	
	}
	
}

uint8_t RCRx::getInputs()
{
	return _numInputs;
}
