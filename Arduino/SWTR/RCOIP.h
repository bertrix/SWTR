// RCOIP.h
//
// Definitions for the RCOIP (Remote Control Over IP) protocol
// These definitions are kept in a single header with no expectation
// of programming language so that C, C++ and Objective-C can be supported
//
/// \file RCOIP.h
/// \class RCOIP RCOIP.h 
/// \brief This section describes the RCOIP (Remote Control Over IP) protocol.
///
/// RCOIP protocol is used to
/// carry remote control commands from a Transmitter to a Receiver over an IP
/// transport such as Ethernet or Wi-Fi. 
/// 
/// The RCOIP protocol is a 2-way protocol that defines UDP messages between 
/// an RCOIP Transmitter and Receiver. 
///
/// \par Terminology
///
/// This protocol is intended to carry command and status messages between a pair of devices 
/// that act like a conventional Remote Control or Radio Control (RC) transmitter and receiver.
/// The effect is that commands sent from the transmitter to the receiver cause the reciever to
/// change its behaviour (perhaps by changing the output value on a pin controlling a servo or motor)
/// Unlike conventional RC receivers, an RCOIP receiver can also send useful and interesting telemetry
/// data back to the receiver (albeit at a lower priority).
///
/// So, Commands are sent from the Transmitter to the Receiver over TCP, and periodically in response, 
/// Replies are sent by TCP from the Receiver to the Transmitter.
///
/// This effectively makes the Transmitter a TCP client and the Receiver a TCP server.
/// Messages sent from the Transmitter to the Receiver include commands to set analog outputs,
/// and messages from the Receiver to the Transmitter include Receiver status messages.
///
/// The Receiver is typically a lightweight, low cost device with limitated computation ability.
/// Typical Receiver devices are Arduino microcontrollers with WiShield Wi-Fi support.
///
/// The Transmitter might be heavier and support more features such as interactive user interfaces etc.
/// Typical Transmitters might be iPhone, iPad or Arduino+WiShield devices.
/// The transmitter might look and operate like or simulate a conventional RC transmitter.
///
/// The User is a person (or possibly a program) that uses the Transmitter to send commands to the
/// Receiver in order for the Receiver to do the bidding of the User. 
/// This will typically be a person holding a Transmitter device and manipulating its controls in
/// order to control a vehicle with an embedded Receiver.
///
/// The messages sent by the Transmitter to the Receiver might include Channel data.
/// Each channel is an analog value in the range 0 to 255. Each channel is typically controled by some
/// physical input on the Transmitter.
/// Each Channel 
/// causes some physical effect in the Receiver, and is typically alter by moving some control 
/// on the Transmitter. The physical interpretation of Channels and their values is dependent on the
/// configuration of the Receiver, and the Transmiter and Receiver are expected to be configured 
/// so their interprtation of what each Channel is used for should agree. For example, in one configuration
/// Channel 0 might be throttle, Channel 1 might be aileron and Channel 2 might be undercarriage.
/// And in another configuration, Channel 0 might be steering, Channel 1 might be throttle and 
/// Channel 2 might be horn on/off.
///
/// \par IP Network configurations
///
/// RCOIP is intended to be carried over almost any type of network that can carry IP, 
/// including Wired and Wireless (Wi-Fi) networks. RCOIP does not make any mandatory requirements on
/// networks and address, but recommendations for commonly used conventions are given below.
///
/// The most common and recommended network is a point-to-point ad-hoc Wi-Fi network connection
/// between the Transmitter and the Receiver. Transmitter and Receiver must agree on the SSID 
/// (the network identifier) and
/// whether the ad-hoc network to be used requires encryption (WEP, WPA, WPA2). 
/// Typically, static IP addresses would be used on such a network. 
/// The recommended static IP addresses for such an ad-hoc network connection are 
/// \li Transmitter 169.254.1.1
/// \li Receiver 169.254.1.100
///
/// Another common model is for both Receiver and Transmitter to connect to a mutual infrastructure
/// Wi-Fi network. In this model, either of both Transmitter or Receiver might be configured to get a 
/// dynamic IP address using DHCP. Otherwise they may be configured with static IP addresses compatible with
/// the supporting network.
/// 
/// Any agreed upon UDP port may be used to carry RCOIP, but the recommended ports are 9048 for the Receiver,
/// and any (unspecified) host allocated port number for the Transmitter. Port 9048 is controlled by 
/// Open System Consultants (http://www.open.com.au),
/// and permission is granted by them to use 9048 as the recommnded port for RCOIP
///
/// \par RCOIP Protocol Version 1 (OUT OF DATE)
///
/// Whenever the User alters a control or setting on the Transmitter,
/// the Transmitter sends a RCOIPv1CmdSetAnalogChannels message to the
/// Receiver. RCOIPv1CmdSetAnalogChannels contains the new desired output value for all the defined
/// output channels. 
///
/// Whenever the Receiver receives a RCOIPCmdSetAnalogChannels it might reply with a RCOIPv1ReplyReceiverStatus
/// message. However, in order limit bandwidth and battery power requirements, it does not reply to
/// every RCOIPv1CmdSetAnalogChannels, but only so that it replies at most every ReplyInterval milliseconds.
/// The recommended value for ReplyInterval is 1000 milliseconds.
///
/// RCOIPv1CmdSetAnalogChannels can contain 0 or more analog output values.
/// The maximum number of analog channels is dictatated by the maximum UDP packet size
/// supported by the network, however a minimum of at least 100 channels is expected.
/// Only the number of channels configured into the Transmitter are sent.
///
/// The interpretation of the channel values is dependent on the configuration of the receiver.
/// Each channel value is an 8 bit octet interpreted as an unsigned integer, ranging from 0 to 255 inclusive.
/// 0 is to be interpreted by the Receiver as 'minimum' and 255 as 'maximum', but the exact physical effect 
/// of each channel and the channel values is completely dependent on the configuration of the Receiver.
/// Channel values that are expected by the Receiver but which have not (yet) been received 
/// from the Transmitter are to be interprted as 0. The Receiver is to maintain the output corresponding 
/// to the last received RCOIPv1CmdSetAnalogChannels message until and unless failure detection results in 
/// a failsafe configuration (see below).
///
/// As can be seen from the above discussion, RCOIP is not a 'reliable' protocol in the sense that it does not 
/// use acknowledgements in order to guarantee delivery of messages. 
/// It relies on the 'best efforts' of the network, and the transmission of multiple messages to 
/// deal with network packet loss and delivery problems. Transmitter and Receiver can use absence of received 
/// messages for a period of time as a hint of loss of communications. This is discussed further below.
///
/// \par Keepalive and failure detection
///
/// In order to assist with the detection of communications failure between the Transmitter and he Receiver, 
/// a system of keepalive messages is defined.
///
/// The Transmitter is required to send a message to the receiver at least every KeepaliveInterval milliseconds.
/// The reecommended value for KeepaliveInterval is 1000 milliseconds. Any command may be sent
/// as the keepalive, but RCOIPv1CmdSetAnalogChannels is recommended 
/// (note: at present there is only one command message defined, and it is RCOIPv1CmdSetAnalogChannels)
///
/// When the Receiver receives a keepalive message it will respond in the usual way, 
/// by sending a RCOIPv1ReplyReceiverStatus back to the transmitter, provided it is more than 
/// ReplyInterval milliseconds since the last reply.
///
/// If the Receiver does not receive any Command messages for more than FailInterval milliseconds, the Receiver
/// detects loss of communication with the Transmitter and may adopt a failsafe configuration.
/// 
/// If the Transmitter does not receive any Reply messages for more than FailInterval milliseconds, 
/// the Transmitter
/// detects loss of communication with the receiver and may react in whatever appropriate way, 
/// such as informing the User.
///
/// If the Receiver detects loss of communication with the Transmitter, it is free to respond in whatever 
/// way is appropriate. The recommended and safest way is for the Receiver to adopt a 'failsafe' configuration, 
/// typically by reducing any motor or throttle outputs to 0, off, stopped or safe. If, subsequent to a 
/// communications failure and a failsafe 
/// configuration, the Receiver again detects Commands from the Transmitter, it may recommence 
/// normal operations however, it is recommended that the Receiver remain in the failsafe configuration 
/// until perhaps manually reset.
///
// Copyright (C) 2010 Mike McCauley. 
// Modifications by Joseph East 2011
// $Id: RCOIP.h,v 1.3 2010/06/30 02:48:59 mikem Exp mikem $
#include "Settings.h"
#ifndef RCOIP_h
#define RCOIP_h

// RCOIP (Remote Control Over IP) versions we know about

/// \def RC_VERSION1
/// Indicates Version 1 of RCOIP protocol
#define RC_VERSION1 1

/// \def RC_VERSION
/// This is the default (latest) version of the RCOIP protocol
#define RC_VERSION RC_VERSION1

/// \def RCOIP_DEFAULT_REPLY_INTERVAL
/// Defines the default maximum time in milliseconds between Reply messages
/// sent by the receiver
#define RCOIP_DEFAULT_REPLY_INTERVAL 1000

/// \def RCOIP_DEFAULT_FAIL_INTERVAL
/// Defines the default maximum time in milliseconds between that will be regarded as a loss of connection
/// by either the receiver or the transmitter
#define RCOIP_DEFAULT_FAIL_INTERVAL 5000

/// \def RCOIP_DEFAULT_KEEPALIVE_INTERVAL
/// Defines the default time in milliseconds between keepalaive Command messages
/// sent by the receiver. If no messages are transmitted for more than this period of time
/// a keepalive Command message will be sent to the receiver.
#define RCOIP_DEFAULT_KEEPALIVE_INTERVAL 1000

/// \def RCOIP_DEFAULT_RECEIVER_TCP_PORT
/// Defines the default RCOIP receiver UDP port number
/// 9048 is controlled by Open System Consultants
#define RCOIP_DEFAULT_RECEIVER_TCP_PORT 9048

/////////////////////////////////////////////////////////////////////
/// \struct RCOIPv1CmdSetAnalogChannels
/// \brief SetAnalogChannels command message structure
///
/// Defines the structure over-the-wire of 
/// a Version 1 SetAnalogChannels command
typedef struct
{
    /// The RCOIP version number.
    /// Must be set to RC_VERSION1
	uint8_t version;

	/// 0 or more analog channel values, one per octet.
	/// Channel values range from 0 (minimum) to 255 (maximum) inclusive.
	/// The physical meaning of channel values and how they map to output devices and pins
	/// depends on how the channel is interpreted at the Receiver.
	/// Channels which represent simple on-off channels are expected to be 0 for 'off' and any other
	/// value for 'on'.
	uint8_t channels[0];

} RCOIPv1CmdSetAnalogChannels;


/////////////////////////////////////////////////////////////////////
/// \struct RCOIPv1ReplyReceiverStatus
/// \brief ReceiverStatus reply message structure
///
/// Defines the structure over-the-wire of 
/// a Version 1 ReceiverStatus reply
typedef struct packetStruct
{
	/// The RCOIP version number. 
	/// Must be set to RC_VERSION1
	uint8_t version;

	/// The value of the RSSI (Receiver Signal Strength Indicator) as measured by the receiver
	/// in the last received message. Arbitrary units. If not available, set to 0.
	uint8_t RSSI;

	/// The current value of the Receiver's battery voltage.
	/// 0 means 0V. 255 means 15.0V
	uint8_t batteryVoltage;

	// This is a 'universal' status array, to be interpreted by the implementor
	// 
	uint8_t status[];

} RCOIPv1ReplyReceiverStatus;

/// \typedef RCOIPCmdSetAnalogChannels
/// This is the preferred type to use to refer to the latest version of 
/// the RCOIPCmdSetAnalogChannels message
typedef RCOIPv1CmdSetAnalogChannels RCOIPCmdSetAnalogChannels;

/// \typedef RCOIPReplyReceiverStatus
/// This is the preferred type to use to refer to the latest version of 
/// the RCOIPReplyReceiverStatus message
typedef RCOIPv1ReplyReceiverStatus RCOIPReplyReceiverStatus;

#endif
