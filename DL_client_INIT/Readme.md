#  DL_client_INIT
// THIS SKETCH IS SOLELY FOR USE OF PROVISIONING THE DOMESTIC LIGHT SENSOR BOARDS
// WITH AWS CERTIFICATES PRIOR TO SHIPPING. 
// THIS SKETCH WILL ERASE ANY PREVIOUSLY STORED PREFERENCES FILES


// Prerequisites

## SEE READ ME IN REPO DIRECTORY FOR OVERALL PROJECT INFO AND SETUP DETAILS. 
## The code is for the ESP32-S3 Feather (adafruit and Unexpected Maker) built on the Arduino IDE
## Arduino Libraries

The following libraries can be installed through the Arduino Libraries Manager:

- RTClib
- AnalogRTCLib ***  For Production and Artist Proof boards:
                    the Analog Devices "AnalogRTCLib" has an incorrect
                    directory reference for the MAX31343 header file and must be edited for use. 
                    The corrected files are included in our repository as "MAX31343.h", "MAX31343.cpp", and "MAX31343_registers.h". Once corrected by Analog Devices  "AnalogRTCLib.h" should be used instead.
                    These files must be included in the sketch folder to build. 
- Adafruit_AS7341

## Other Libraries

Install MicrOSCript by John Macallum by cloning the library from the below repository and
placing it in your Arduino libraries folder. Be certain it is cloned recursively. 
(~/Documents/Arduino/libraries):

[https://github.com/0Z3/MicrOSCript](https://github.com/0Z3/MicrOSCript)

__________________

// TO USE:
// 1. CONFIGURE THIS DL_client_INIT.ino WITH
//    * DEVICE SPECIFIC UUID, * CERTIFICATE, * PRIVATE KEY AND * ENDPPOINT IN SECRETS.H
// 2. FLASH THE SKETCH TO THE BOARD
// 3. REBOOT THE BOARD BY PRESSING THE RESET BUTTON.
// 4. CONNECT TO THE SERIAL MONITOR. (if needed hit reset again) 
//    IF SUCCESSFUL YOU WILL SEE A PRINTOUT CONFIRMING THE SAVING OF INFO IN THE SERIAL MONITOR AND THE UUID / MAC ADDR.
// 5. CLOSE THIS SKETCH.
/// // END USE OF DL_client_INIT 

// Move to the DL_cLient.ino sketch
// FLASH BOARD WITH DL_client
// THIS INIT SKETCH ONLY PROVIDES STORAGE INTO PERM MEMORY

// NOTE - IF YOU DID NOT INCLUDE WIFI INFO IN SECRETS.H

// Use the adhoc network SETUP to go on your LOCAL WIFI TO TEST BOARD
// Enter ADHOC mode by pressing and holding Button 1 (Left Button), 
// while pressing and releasing the RESET Button (Right Button). The device will boot into "ADHOC MODE"
// 
// on a computer or phone look for the WIFI Network "DomesticLight". Connect to it via wifi.
// then using a browser go to https://192.168.4.1. 
// Enter your local WIFI network SSID, and its password.
// You can also optionally configure local OSC transmission of the data.
//
// Click submit
//
// 9. Press Reset hardware button to reboot the board. It should connect to the local wifi network
// 10. If successfull you will see bootup messages than copies of messages sent to AWS in the serial monitor.
// 11. Please check the AWS MQTT test client to confirm that the device data is being received.



// LICENSES AND CREDITS 
// the DL_client was created for light sensor client of Domestic Light, a media art project by Ian Winters
// for more info about the project, or to participate please visit https://domesticlight.art
// coding by John Macallum, Ian Winters and Weidong Yang with support from Jeffery Lubow, and Nicolas Seymour-Smith.
// this code is distributed under an MIT License.

// Additional libraries are used under license and include the following
// MicroOSCript, OSE https://github.com/0Z3/ose MIT License (c) 2020 John Macallum
// PubSubClient https://github.com/knolleary/pubsubclient MIT License C(c) 2008-2020 Nicholas O'Leary
// Adafruit AS7341 https://github.com/adafruit/Adafruit_AS7341 Software License Agreement (BSD License) c) 2019 Bryan Siepert for Adafruit Industries
// Adafruit RTCLib https://github.com/adafruit/RTClib MIT License Adafruit 2019
// ArduinoJson  https://github.com/bblanchon/ArduinoJson Beniot BLANCHON MIT 2023
// AnalogRTCLib by Analog Devices Inc.
// ESP32S3 and Arduino libraries by Espressif Systems
