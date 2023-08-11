// This file is the client for the Domestic Light Sensor, part of the larger Domestic Light project.
# REV 4 Aug 2023
# Prerequisites

## SEE READ ME IN REPO DIRECTORY FOR OVERALL PROJECT INFO AND SETUP DETAILS. 
## USE OF THIS SKETCH REQUIRES THAT THE BOARD WAS FLASHED FIRST WITH DL_client_INIT.ino to STORE SERVER CREDENTIALS IN PERM STORAGE.

## The code is for use with ESP32-S3 Feather boards (Adafruit and Unexpected Maker), and intended to be built using the Arduino IDE v 2.1 or later.

## Arduino Libraries
The following libraries can be installed through the Arduino Libraries Manager:

- RTClib
- **** AnalogRTCLib *** NOTE for Production and Artist Proof boards:
                    the Analog Devices "AnalogRTCLib" has an incorrect
                    directory reference for the MAX31343 header file and must be edited for use. 
                    The corrected files are included in our repository as "MAX31343.h", "MAX31343.cpp", and "MAX31343_registers.h". Once corrected by Analog Devices  "AnalogRTCLib.h" should be used instead.
- Adafruit_AS7341

## Other Libraries

MicrOSCript: You can download the zip file for MicroOSCript in the DL Client folder OR for an up to date version clone the library from the MicroOSCrepository.
Place it  in your Arduino libraries folder. Be certain it is cloned recursively. 
(~/Documents/Arduino/libraries):

[https://github.com/0Z3/MicrOSCript](https://github.com/0Z3/MicrOSCript)

__________________

// INITIAL USER SETUP NOTES
/// To use on a new network boot into ADHOC mode and enter your wifi info
//  1) press and holding Button 1 (Left Button), 
//     while pressing and releasing Button 2/  RESET Button (Right Button). 
//  2) release Button 1 - the device will boot into "ADHOC MODE"
//  3) on a computer or phone look for the WIFI Network "DomesticLight". 
//     Connect to it via wifi, and then using a browser go to https://192.168.4.1. 
//  4) Enter the following : 
//     * your local WIFI network SSID, and its password.
//     * You can also optionally configure local OSC transmission of the data.

//    press "submit", wait a few seconds and reset the device 
//    connect back to your local network - you should see data flowing over OSC if you enabled that.
//    alternatively connect to it over serial using a usb-c cable a serial monitor and you will see the JSON data packet that looks something like this
//    "F1_415":67,"F2_445":100,"F3_480":176,"F4_515":207,"F5_555":264, .....
//  for more information about the data packet methods see DL_Data_Schema

// CODE CONFIGURATION NOTES
// To adjust the light sensor gain and integration time, set in the Light Sensor config of the Global variables.
// To set data collection frequency set the data frequency counter  in seconds. Please do not adjust unless asked.

// TO DO / ROADMAP 
// Provide on board approximate rough RGB conversion to print to local OSC.
// Add wifi / power loss buffer code that will store data when wifi is not available. 
// add AWS Device Shadow functionality to aid in buffer data management
// provide support for connection to institutional wifi networks which require an X509 certificate. 
// Provide support for ENS VOC and methane sensors
// 
// KNOWN BUGS 
// Currently the message handler does not correctly recieve or process messages from AWS 
// If AWS connection is interupted, while the device is connected to serial the serial connection is lost and has to be re-enabled.
// The wifi network selection does not reliably connect to wifi networks without a password.
// The OSC message for current Unixtime does not transmit correctly to Isadora and generates an unknown value type

// LICENSES AND CREDITS 
// the DL_client was created for light sensor client of Domestic Light, a media art project by Ian Winters
// for more info about the project, or to participate please visit https://domesticlight.art
// coding by John Macallum, Ian Winters and Weidong Yang with support from Jeffery Lubow, and Nicolas Seymour-Smith.
// this code is distributed under an MIT License.

// Additional libraries are used under license and include the following:
// MicroOSCript, OSE https://github.com/0Z3/ose MIT License (c) 2020 John Macallum
// PubSubClient https://github.com/knolleary/pubsubclient MIT License C(c) 2008-2020 Nicholas O'Leary
// Adafruit AS7341 https://github.com/adafruit/Adafruit_AS7341 Software License Agreement (BSD License) c) 2019 Bryan Siepert for Adafruit Industries
// Adafruit RTCLib https://github.com/adafruit/RTClib MIT License Adafruit 2019
// ArduinoJson  https://github.com/bblanchon/ArduinoJson Beniot BLANCHON MIT 2023
// AnalogRTCLib  Copyright(C) Analog Devices Inc.
// ESP32S3 and Arduino libraries by Espressif Systems.

