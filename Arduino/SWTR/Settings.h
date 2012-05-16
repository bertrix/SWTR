/************************************************************************************
* Settings.h
* Joseph East 2011
* SWTR
*
* This file is basically the config file for the program
* Contains the WiFi settings and some peripheral options
* 
* The wireless profile that is chosen is set in the main *.ino file
* This simply has settings for the (by default) 2 profiles.
*************************************************************************************/

#ifndef SETTINGS_H_
#define SETTINGS_H_

/************************************************************************************
* Peripheral options, make sure these are also reflected in the client!
* Uncomment to activate
************************************************************************************/
#define _DEBUG // Debug mode, messages are sent over serial but otherwise 100% functional. Compiles larger and consumes more RAM.
#define _SENSOR // Activates sensor module for data input and transmission over WiFi. No sensors with work without this.
//#define _9DOF // Activates the FreeIMU based 9DOF module and related functionality. Warning, consumes 10K prog mem!!
//#define _NMEA // Activates the NEMA scraper on the serial port
//#define _HMC6352 // Activates the Honeywell HMC6352 I2C compass 
//#define _fakeSen // Activates the fake sensor module for loopback testing
//#define _ANALOG // Activates the 10-bit A/D module
#define _fakeSen2 // Same as fake sensor but uses casted struct pointers instead of sane arrays. Just a different way of doing things.

/************************************************************************************
* Wireless Profiles
************************************************************************************/

#define _WIFI_SETTINGS_AP_															\
unsigned char local_ip[] = {192,168,1,46};											\
unsigned char gateway_ip[] = {192,168,1,1};											\
unsigned char subnet_mask[] = {255,255,255,0};										\
char ssid[] = {"ASYNCLABS"};														\
unsigned char security_type = 0;													\
const prog_char security_data[] PROGMEM = {};										\
unsigned char wireless_mode = WIRELESS_MODE_INFRA;								 	
//const prog_char security_passphrase[] PROGMEM = {"donot8me"}; 					\
/*prog_uchar wep_keys[] PROGMEM = 													\
{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 	\
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	\
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	\
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 		\
};*/

/***********************************************************************************/

#define _WIFI_SETTINGS_AD_HOC_														\
unsigned char local_ip[] = {169,254,1,100};											\
unsigned char gateway_ip[] = {169,254,1,1};											\
unsigned char subnet_mask[] = {255,255,0,0};										\
char ssid[] PROGMEM = {"RCArduino"};												\
unsigned char security_type = ZG_SECURITY_TYPE_NONE;								\
unsigned char wireless_mode = WIRELESS_MODE_ADHOC;									\
const prog_char security_passphrase[] PROGMEM = {"donot8me"}; 						\
prog_uchar wep_keys[] PROGMEM = 													\
{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 	\
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	\
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	\
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 		\
}; 

/***********************************************************************************/


#endif
