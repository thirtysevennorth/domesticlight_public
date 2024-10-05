# This file is the client for the Domestic Light Sensor.
# REV 5 OCT 2024 - NEW OSC Branch
This update adds new OSC library, ESP 3.0 support, and updated JSON support for new data types. 

## SEE READ ME IN REPO DIRECTORY FOR OVERALL PROJECT INFO AND SETUP DETAILS. 
USE OF THIS SKETCH REQUIRES THAT THE BOARD WAS FLASHED FIRST WITH DL_client_INIT.ino to STORE SERVER CREDENTIALS IN PERM STORAGE.
The code is for use with Production (series 200xxx) and (300xxx) version of the sensor using ESP32-S3 Feather boards (Unexpected Maker) and the MAX31343 RTC, and intended to be built using the Arduino IDE v 2.32 or later.
__________________

# INITIAL USER SETUP NOTES
# ADHOC MODE / WIFI Config. 
  1) press and hold Button 1 (Left Button), while pressing and releasing Button 2/  RESET Button (Right Button).
  2)  release Button 1 - the device will boot into "ADHOC MODE"
  3)  on a computer or phone look for the WIFI Network "DomesticLight".
  4)  Connect to it via wifi, and then using a browser go to http://192.168.4.1.
  5)  Enter the following : 
     * your local WIFI network SSID, and its password.
     * You can also optionally configure local OSC transmission of the data.
    press "submit", wait a few seconds and reset the device

To test:     
    6) connect back to your local network - you should see data flowing over OSC if you enabled that, or alternatively connect to the sensor over serial using a usb-c cable a serial monitor and you will see the JSON data packet that looks something like this
 "F1_415":67,"F2_445":100,"F3_480":176,"F4_515":207,"F5_555":264, .....
  for more information about the data packet methods see DL_Data_Schema

 # CODE CONFIGURATION NOTES
 * adjust board processor and RTC enabling / disabling the respective definitions.
   * RTC: DS3231 ro MAX31343, Processor: UM3 or ADA
 * To enable/disable AWS connections include #define AWS
 * To adjust the light sensor gain and integration time, set in the Light Sensor config of the Global variables.
 * To set data collection frequency set the data frequency counter  in seconds. Please do not adjust unless asked.
 * To factory reset uncomment #define DL_FACTORY_RESET. This will require the board to be reflashed with the DL_client_INIT sketch.

# TO DO / ROADMAP 
  * Provide on board RGB conversion to print to local OSC.
  * Add wifi / power loss buffer code that will store data when wifi is not available. 
  * Provide support for connection to institutional wifi networks which require an X509 certificate. 
  * Provide support for additional I2C sensors
 
#  KNOWN BUGS 
 * The wifi network selection does not reliably connect to wifi networks without a password.
 * The OSC message for current Unixtime does not transmit correctly to Isadora and generates an unknown value type
 * If the device enters Adhoc mode and no connection is made within 1 to 3 minutes the device restarts

# LICENSES AND CREDITS 
 * the DL_client was created for light sensor client of Domestic Light, a media art project by Ian Winters
 * for more info about the project, or to participate please visit https://domesticlight.art
 * coding by John Macallum, Ian Winters and Weidong Yang with support from Jeffery Lubow, and Nicolas Seymour-Smith.
 * this code is distributed under an MIT License.

# Additional libraries used include the following and incorporate their respective license terms:
* PubSubClient https://github.com/knolleary/pubsubclient MIT License C(c) 2008-2020 Nicholas O'Leary
* Adafruit AS7341 https://github.com/adafruit/Adafruit_AS7341 Software License Agreement (BSD License) c) 2019 Bryan Siepert for Adafruit Industries
* Adafruit RTCLib https://github.com/adafruit/RTClib MIT License Adafruit 2019
* ArduinoJson  https://github.com/bblanchon/ArduinoJson Beniot BLANCHON MIT 2024
* ESP WebOTA  https://github.com/scottchiefbaker/ESP-WebOTA MIT License Scott Baker 2019
* AnalogRTCLib  Copyright(C) Analog Devices Inc.
* ESP32S3 and Arduino libraries by Espressif Systems 2024
