# DOMESTIC LIGHT: working repository for the Domestic Light Project. 
* Learn more and participate at https://domesticlight.art 

# SOFTWARE UPDATE March 15 2024
This March 15th, 2024 version of the project address the following issues:
1) provides status monitoring and automatic reconnect for AWS to address wifi and connection issues
2) preps devices for use of additional I2C connection for add-on sensor
3) adds WebOTA update capability to allow for updates of the device from a webpage without an IDE

4) A reference binary release for use reflashing boards with OTA functionaly is release 1.0

# The update has two builds
* DL_client is built for production and artist proof boards. These are boards with a series # of 300xxx and 200xxx respectively.
* DL_Client_DS3231 is built for the artist prototype board. These boards have a series # of 100xxx, and have seperate pcb boards for the light sensor and chip. 

Please use the build appropriate to your board.

# OVERVIEW OF PROJECT 
The primary working file for sensor code is DL_client
REV date 15 MAR 2024

* Github Repository for code
https://github.com/thirtysevennorth/domesticlight_public
* Website
https://domesticlight.art

Files are intended to be built using the Arduino IDE v2.32 or later and is for use as part of the Domestic Light project.

# TO UPDATE AN EXISTING BOARD
1) install the required libraries and board package above in the Arduino IDE
2) Connect the sensor board to your computer via a USB-C cable.
3) Open the DL_client.ino file for your board series (prototype series 100xxx or proof/production series 200xxx) in the Arduino IDE
4) Select correct board and port combination in the Arduino IDE window (eg UM Feather S3 and the serial port the board is connected to) or Adafruit Feather S3). Please be sure to edit the code definition to select the correct board as well.
5) Build and upload the code to your sensor
6) hit the reset button and verify that the sensor is providing output over the serial monitor.
If you encouter an upload error 1 or 2 be sure that you have specified the correct board and serial port, and on some machines ESP-32-S3 boards disconnect after compile and just before upload. Be sure to reselect the board if you see the USB symbol disappear.
NOTE: If you continue to have difficulties, some computers  will require that you put the board into "BOOT" mode before flashing. To do so: press and hold the "boot" button on the esp-32 (the back tiny button on the microcontroller reachable with a paperclip, then press and release the "RESET" button (the right-hand button on the sensor), then release the "boot" button. Then select the board / port in the IDE and flash. 


# REQUIRED BOARD FILES, LIBRARIES, DEPENDENCIES FOR ARDUINO IDE
* PREREQUISITE:	INSTALL THE ARDUINO IDE 2.32 or later located at 
* https://www.arduino.cc/en/software#future-version-of-the-arduino-ide

**Install ESP32S3 BOARD file:
If you have not built a project in Arduino 2.x for ESP32-S3 on your machine please install the following in the Arduino IDE
1) In the Arduino IDE > Preferences > Additional Boards Manager URL add the URL for the 
Espressif Arduino ESP32 boards
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
2) In Boards Manager add "esp32" by Espressif Systems. This should be on V 2.0.14. **Do not use the V.3.0 alpha release** of the esp32 board file.
3) If this is a new Arduino IDE install on a MAC system you may be prompted to install Xcode command line tools. The IDE will prompt you, or you can enter the following command in Terminal "xcode-select --install" and follow the prompts.
4) the UM Feather S3 boards use the USB > Serial driver. You should not need to install any serial driver. 

** ADD THE FOLLOWING LIBRARIES using the Arduino Library Manager:
1) RTClib
2) Adafruit_AS7341
3) pubSubClient by Nick O'leary http://knolleary.net
4) ArduinoJson https://arduinojson.org
5) AnalogRTClib by Analog Devices / Maxim - please see readme and use version included with this library that includes bugfix edit to the AnalogRTCLib for use with the MAX31343 chip
6) ESP32-OTA by Scott Baker

** Install the following library manually:
* MicroOSCcript. Download zip file "https://github.com/thirtysevennorth/domesticlight_public/blob/main/MicrOSCript.zip". Once downloaded add the library to your Arduino Libraries folder. 

   END REQUIRED BOARD FILES, LIBRARIES, DEPENDENCIES 

# CREDITS, LICENSE, AND CONTRIBUTORS
Key contributors to the Domestic Light Sensor Client are [John Macallum](https://github.com/maccallum) and Ian Winters. To learn more about [Domestic Light](https://domesticlight.art). 
Funding for the Domestic Light project is provided by numerous supporters including the Creative Work Fund, the Rainin Foundation and the support of our partners Leonardo/ISAST, University of Sussex Digital Humanities Lab, and San Francisco Arts Education Project.

The client software is released under an MIT license.

Additional  libraries used under the terms of their license include:
* [FastLED](https://github.com/FastLED/FastLED) under an MIT License
* [ArduinoJSON](https://github.com/bblanchon/ArduinoJson) under an MIT License
* [Adafruit AS7341](https://github.com/adafruit/Adafruit_AS7341) under a BSD License
* [PubSubClient](https://github.com/knolleary/pubsubclient) under an MIT License
* [AnalogRTCLIB](https://github.com/analogdevicesinc/AnalogRTCLibrary/blob/7f996e41772edafeb7a04c1ae92abd2bdc994f63/src/MAX31343/MAX31343.h) under an MIT License
* [ESP Web-OTA](https://github.com/scottchiefbaker/ESP-WebOTA) MIT License 
* MicroOSCript under a license from John Macallum for use in this client software.
