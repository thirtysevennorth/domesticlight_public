# DOMESTIC LIGHT: working repository for the Domestic Light Project. 
* Learn more and participate at https://domesticlight.art 

# SUSSEX HACK DAY BRANCH
This branch is set up as a place to pull copies of the sensor source from for the Feb 2024 Hackday at Sussex. For works in progress please fork from this branch and we'll work through merges. Pipe up in discord if you need access to the this branch.

Discord server : invite: https://discord.gg/t8UHbhUXkd 
tag questions around sensor code in the discord server for @strangelfoot (John Macallum - chiming in remotely from Berlin)

# SOFTWARE UPDATE XX Feb 2024
This  version of the project address the following issues:
* updates to accomadate the new production board including stemma QT I2C plugin sensors

# Set flags in code for MAX31343 or DS3231 RTC
* DL_client is built for production and artist proof boards. These are boards with a series # of 200xxx and 300xxx
* DL_Client_DS3231 is built for the artist prototype board. These boards have a series # of 100xxx, and have seperate pcb boards for the light sensor and chip. 

Please use the flagappropriate to your board.

# OVERVIEW OF PROJECT 
The primary working file for sensor code is DL_client
REV date 12 Feb 2024

* Github Repository for code
https://github.com/thirtysevennorth/domesticlight_public

Files are intended to be built using the Arduino IDE v2.21 or later and is for use as part of the Domestic Light project.

# TO UPDATE AN EXISTING BOARD
1) install the required libraries and board package above in the Arduino IDE
2) Connect the sensor board to your computer via a USB-C cable.
3) Open the DL_client.ino file for your board series (prototype series 100xxx or proof/production series 200xxx) in the Arduino IDE
4) Select correct board and port combination in the Arduino IDE window (eg UM Feather S3 and the serial port the board is connected to)  or Adafruit Feather S3). Please be sure to edit the code definition to select the correct board as well.
5) Build and upload the code to your sensor
6) hit the reset button and verify that the sensor is providing output over the serial monitor.
If you encouter an upload error 1 or 2 be sure that you have specified the correct board and serial port, and on some machines ESP-32-S3 boards disconnect after compile and just before upload. Be sure to reselect the board if you see the USB symbol disappear.
NOTE: If you continue to have difficulties, some computers  will require that you put the board into "BOOT" mode before flashing. To do so: press and hold the "boot" button on the esp-32 (the back tiny button on the microcontroller reachable with a paperclip, then press and release the "RESET" button (the right-hand button on the sensor), then release the "boot" button. Then select the board / port in the IDE and flash. 

# INITIAL FLASH OF BOARD
If you are flashing a board not previously configured you will need first use the DL_INIT file to upload the AWS keys and sensor ID to your board. Please contact the Domestic Light project via emial for the required AWS keys and device certificate. 

# REQUIRED BOARD FILES, LIBRARIES, DEPENDENCIES FOR ARDUINO IDE
* PREREQUISITE:	INSTALL THE ARDUINO IDE 2.21 or later located at 
* https://www.arduino.cc/en/software#future-version-of-the-arduino-ide

**Install ESP32S3 BOARD file:
If you have not built a project in Arduino 2.x for ESP32-S3 on your machine please install the following in the Arduino IDE
1) In the Arduino IDE > Preferences > Additional Boards Manager URL add the URL for the 
Espressif Arduino ESP32 boards
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
2) In Boards Manager add "esp32" by Espressif Systems. This should be on V 2.0.14 production release, but has not been tested on v 3.0.0 alpa releases
3) If this is a new Arduino IDE install on a MAC system you may be prompted to install Xcode command line tools. The IDE will prompt you, or you can enter the following command in Terminal "xcode-select --install" and follow the prompts.
4) the UM Feather S3 boards use the USB > Serial driver. You should not need to install any serial driver. 

** ADD THE FOLLOWING LIBRARIES using the Arduino Library Manager:
1) RTClib
2) Adafruit_AS7341
3) pubSubClient by Nick O'leary http://knolleary.net
4) ArduinoJson https://arduinojson.org
5) AnalogRTClib by Analog Devices / Maxim - please see DL_client README as to required and inlcued bugfix edit to the AnalogRTCLib for use with the MAX31343 chip

** Install the following library manually:
* MicroOSCcript. Download zip file "https://github.com/thirtysevennorth/domesticlight_public/blob/main/MicrOSCript.zip" or  clone https://github.com/0Z3/MicrOSCript. Once downloaded add the library to your Arduino Libraries folder. 
* if cloning, be sure the clone is recursive and includes libose, o.se.oscript, and o.se.stdlib if you clone from the web.
   END REQUIRED BOARD FILES, LIBRARIES, DEPENDENCIES 


