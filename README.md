DOMESTIC LIGHT
working repository for the Domestic Light Project. Learn more and participate at https://domesticlight.art 

//// OVERVIEW OF PROJECT 

The primary working file for sensor code is DL_client

Github Repository for code
https://github.com/thirtysevennorth/domesticlight

Files are intended to be built using the Arduino IDE v2.10 or later and is for use as part of the Domestic Light project.


// REQUIRED BOARD FILES, LIBRARIES, DEPENDENCIES /////////////////////////////////////
INSTALL THE ARDUINO IDE 2.1 or later

CONFIRM YOU HAVE ESP32S3 BOARD INSTALLED
If you have not built a project in Arduino 2.x for ESP32-S3 on your machine please install the following in the Arduino IDE
1) In the Arduino IDE > Preferences > Additional Boards Manager URL add the URL for the 
Espressif Arduino ESP32 boards
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json

2) In Boards Manager add "esp32" by Espressif Systems. This should be on V 2.0.9 or later.

REQUIRES Arduino IDE 2.10 with the following Libraries added :
From Arduino Library Manager install the following libraries:
1) RTClib
2) Adafruit_AS7341
3) pubSubClient by Nick O'leary http://knolleary.net
4) ArduinoJson https://arduinojson.org
5) AnalogRTClib by Analog Devices / Maxim - please see DL_client README as to required bugfix edit to the AnalogRTCLib for use with the MAX31343 chip

Install the following libraries manually:
* MicroOSCcript. To install clone https://github.com/0Z3/MicrOSCript and add the library to your Arduino Libraries folder. 
Be sure the clone is recursive and includes libose, o.se.oscript, and o.se.stdlib if you clone from the web.
//// END REQUIRED BOARD FILES, LIBRARIES, DEPENDENCIES 

//// UPDATE AN EXISTING BOARD
1) install the required libraries and board package above in the Arduino IDE
2) Connect the sensor board to your computer via a USB-C cable.
3) Open the DL_client.ino file in the Arduino IDE
4) Select correct board / port (either Unexpected Maker (UM) Feather S3 or Adafruit Feather S3). Please be sure to edit the code definition to select the correct board as well.
5) Build and upload the code to your sensor
6) hit the reset button and verify that the sensor is providing output over the serial monitor.

//// END UPDATE INSTRUCTIONS

//// TO BUILD AND FLASH A NEW BOARD

//// PROJECT HARDWARE: The project requires the following hardware components to build 
PROCESSOR - ESP32-S3
two processor types:
* Unexpected Maker ESP32-S3 Feather 
* Adafruit ESP-32 S3 Feather - either w/ or W/o PSRAM

RTC
* DS3231 by Analog Devices on an adafruit board if using a bare prototype
* Production boards come with a MAX31343 by Analog Devices installed

COMPONENTS TO BUILD PROTOTYPE BOARD FOR WORKSHOPS
* Domestic Light circuit board -  Rev G (Artist Prototype - no SMT assembly, white soldermask) or the ARTIST PROOF / PRODUCTION VERSION (WITH SMT COMPONENTS
* For the production board all that is needed is the board, case, and an ESP32-S3 Feather (either by unexpected maker or Adafruit)
* to build a Rev G artist prototype assembly you will need the following components
	* 2x tactile right angle button switches, E Switch TL1100CF160Q for switch S1 (adhoc mode), switch S2 (RST)
	* Adafruit AS7341 light sensor board https://learn.adafruit.com/adafruit-as7341-10-channel-light-color-sensor-breakout/pinouts
	* Adafruit DS3231 RTC board https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout/pinouts
	* CR1220 coin cell
	* 3x 10k axial resistors OR adding same as SMD 0603 components on pads
	
* USB-C cable to connect to 5v USB power and / or computer

//// END HARDWARE AND SOFTWARE LIST /////////////////////////////////////


// SETUP AND CONFIG: ////////////////////////////////////////////////////

/// INTITIAL SETUP AND FLASHING () 

//// NOTE ON AWS keys, certs and THINGNAME / UUID ////// 
These are assigned by admin and unique to each device 

INITIAL FACTORY SETUP / INIT
* Connect board via USB-C connector on ESP32 to your computer running the Arduino IDE with the DL_client_INIT.ino file open. 
* Edit secrets.h to include:
    * your test network wifi info - be sure to remove when done before shipping the board.
    * the board UUID 
        * // UUID NOTE this is our internal six digit serial - ex. 100001.
        * //// Series 1xxxxx are "prototypes" in general.
        * //// Series 1000xx are testing boards, using boards labeled REV F that are green boards
	* //// Series 1xx2xx are the "artist prototypes" or first publically distributed and use the white circuit "artist prototype, ed of 50"
	* //// Series 2xxxxx are the "artist proof" series or the first SMT assembled series. These include hardware support to attach a VOC and Methane sensor
	* //// Series 3xxxxx are reserved for the future production run of sensors. These will include a VOC and Methane sensor.
    * AWS Endpoint for the device - may vary depending on location of device
    * AWS device private key,  //matching your UUID / THINGNAME
    * AWS device cert to match your assigned UUID.
    
* PUT BOARD INTO BOOT MODE FOR FIRST FLASH: 
   * press and hold "boot" button on the board
   * while holding boot button press and release the "RST" button on the board or the RST button (S2) if correctly connected.
   * in the IDE Tools> Port select the new serial port that appears.
   * In the IDE Tools> Board select the correct board (UM Feather S3 or Adafruit Feather S3 no PSRAM)
   * Build and flash the DL_client.INIT file with the device secrets to the board.
* after the file is fully uploaded and complete, press the RST button to reset the board. Watch the Serial monitor for confimation of success.

LOAD THE CLIENT
* In the Arduino IDE open DL_client.ino
* Flash that to the board
* hit reset when complete and watch the serial monitor for confirmation. If you are on the test newtwork you set wifi info for in the INIT file it should connect to NTP, set the clock, then connect to AWS.
* To set a wifi network put the board into ADHOC or SETUP mode:
   * hold down S1 (the adhoc mode switch) and  press the reset button. 
   * release Button 1 / adhoc mode.
   * The Serial monitor should give you instructions to join the "Domestic Light" adhoc network and go to 192.168.4.1 to enter the local wifi info. 
* Connect to the adhoc mode wifi "Domestic Light" and go to 192.168.4.1
* Enter your local wifi SSID and password, the IP address you want to send OSC to (if desired), and copy the MAC ID shown on the browser into the MAC ID section. Hit submit. 
The wifi will disconnect. Press the RST button if the board doesn't automatically reboot. 


* at this point the board should be fully configured. Watch the serial monitor for output. you should see something like this:

* /////////////  START OF SAMPLE SERIAL MONITOR OUTPUT
* 14:37:27.380 -> done
* 14:37:27.380 -> IP address: xxx.xxx.x.xx // the boards IP address 
* 14:37:27.380 -> Listening for OSC on UDP port 15000 // the UDP port the board is listenting on
* 14:37:27.380 -> Connecting to AWS IOT  // AWS config info 
* 14:37:29.447 -> AWS IoT Connected! - subscribe and publish topics
* 14:37:29.447 -> DomesticLight/100004/sub  // these topics should reflect the boards UUID which shoudl be tied to the MAC ID
* 14:37:29.447 -> DomesticLight/100004/data

* 14:37:29.447 -> Initializing the RTC...done
* 14:37:29.447 -> Setting the system clock...done
* 14:37:32.012 -> Current Time, Date and Temp set to current date and time 
* 14:37:32.012 -> 2023/5/20 (Saturday) 13:37:32  /// THIS should reflect the current time
* 14:37:32.012 ->  since midnight 1/1/1970 = 1684589852s = 19497d
* 14:37:32.012 -> Temperature: 26.25 C
* 14:37:32.012 -> Starting the color sensor...done.
* 14:37:32.143 -> Sent to data and shadow 203 bytes: // these data points are what is going to AWS every x seconds. 
* ////// END SAMPLE SERIAL MONITOR OUTPUT


PDF LINK TO BUILD GUIDE BELOW: https://github.com/thirtysevennorth/domesticlight/blob/main/Domestic%20Light%20Sensor%20Assembly%20Guidelines.pdf


 


