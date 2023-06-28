// DL_client_INIT
// THIS SKETCH IS SOLELY FOR USE OF PROVISIONING THE DOMESTIC LIGHT SENSOR BOARDS
// WITH AWS CERTIFICATES PRIOR TO SHIPPING. 
// THIS SKETCH WILL ERASE ANY PREVIOUSLY STORED PREFERENCES FILES

///////////////////////////////////////////////////////////

// Choose  RTC. Production boards use MAX31343, prototypes use DS3231.
// 
#define DS3231
//#define RTC_MAX31343

#ifdef RTC_MAX31343
#include  "MAX31343.h" //<AnalogRTCLibrary.h> must place ed
#else
#include <RTClib.h>
#endif

#define DL_INIT
#include "secrets.h"

#include <MicrOSCriptESP32.h>
#include <Wire.h>

#include <WiFi.h>
#include <WiFiUDP.h>
#include <WebServer.h>
#include <Preferences.h>
#include <cstdio>
#include <Adafruit_AS7341.h>
#include <time.h>
#include <ArduinoJson.h>
#include <Arduino.h>



#define DL_AWS

#include <WiFiClientSecure.h>
#include <PubSubClient.h>


#define _I2C_WRITE write
#define _I2C_READ  read

// OSE libraries. installed with MicroOSCript
#include "ose_conf.h"
#include "libose/ose_assert.h"
#include "libose/ose_util.h"
#include "libose/ose_context.h"
#include "libose/ose_stackops.h"
#include "libose/ose_vm.h"
#include "libose/ose_print.h"
#include "o.se.stdlib/ose_stdlib.h"

// ADHOC MODE index.html served in ad hoc mode
#include "index.h"
static const char * const adhoc_ssid = "DomesticLight";
static const int adhoc_http_port = 80;


//// AWS TOPICS ///////////////////////
static String cert_ca;
static String cert_device;
static String cert_private;
static String endpoint;
static String THINGNAME;
static const char * clientID;
String dlPublish;
String dlSubscribe;

static String AWS_IOT_PUBLISH_TOPIC;
static String AWS_IOT_SUBSCRIBE_TOPIC; // these stay as strings then converted at time of use with .c_str()


////////////////////////////////////////////////////////////
// pins: Feather format boards currently supported : 
// ESP32-S3 Feather from Adafruit (or w/o PSRAM); Unexpected Maker Feather S3
////////////////////////////////////////////////////////////

//#define ADAFRUIT_FEATHER_ESP32
#define UM_ESP32S3
#if defined(ADAFRUIT_FEATHER_ESP32)
#define DL_PIN_BUTTON1 26 // A0 (ADC2) DAC2 
#define DL_PIN_BUTTON2 25 // A1 (ADC2) DAC1
#define DL_PIN_RTC_SQUARE_WAVE 27

#define DL_PIN_RGB_LED 14 // (ADC2) LED is a WS2812B single data line. Connects to PIN IO14 / A4/ ADC2
#define DL_PIN_BUILTIN_LED 13 // could change to  built in neopixel on pin 33 PIN_NEOPIXEL
#define DL_HWSERIAL Serial2
#elif defined(UM_ESP32S3)
#define DL_PIN_BUTTON1 17 // A0 (ADC2) DAC2  NOTE on artist proof and prototype boards IO17 and IO18 have labels reversed
#define DL_PIN_BUTTON2 18 // A1 (ADC2) DAC1 NOTE on artist proof and prototype boards IO17 and IO18 have labels reversed
#define DL_PIN_RTC_SQUARE_WAVE 3 // REV F and after pin is #3

#define DL_PIN_RGB_LED 14 // A2 (ADC2) (ADC2) LED is a WS2812B single data line. Connects to PIN IO14 / A4
#define DL_PIN_BUILTIN_LED 13
#define DL_HWSERIAL Serial0
#else
#error No pin definitions for this board!
#endif

#ifndef DS3231_ADDRESS
#define DS3231_ADDRESS 0x68
#endif
/// TO DO - ADD MAX31343 RTC DEFINITION

////////////////////////////////////////////////////////////
// classes and globals
////////////////////////////////////////////////////////////


WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
long lastReconnectAttempt = 0; //AWS Reconnect counter


//  check for MAX31343 RTC variant in production model
#ifdef RTC_MAX31343 
MAX31343 rtc(&Wire, MAX31343_I2C_ADDRESS);
MAX31343::reg_status_t g_stat;

struct tm rtc_ctime; // declare global ctime struct for MAX31343

#else
static RTC_DS3231 rtc;  
#endif

static Adafruit_AS7341 as7341;
static MicrOSCriptESP32 o;
static WebServer server(adhoc_http_port);
static Preferences prefs;


// LIGHT SENSOR CONFIGURATION  
static uint8_t ATIME = 29; // sets variables for light sensor config
static uint16_t ASTEP = 599;
static as7341_gain_t GAIN = AS7341_GAIN_64X;
 

// DATA COLLECTION FREQUENCY
static int dataFrequency = 5; // set to a number of seconds to publish data based on square wave ticks. refers to loop ~ line 870
static int sampleCounter = 0;

// used to decide what to do in the loop depending on whether
// we're in config mode or not. this gets set if button 1 is
// pressed during setup().
static bool adhoc_mode;

// RTC variables - keep track of whether the RTC lost power
static int rtc_lost_power;
static volatile int rtc_sqw_fell = 0;
static float rtc_temp = 0;

#define DL_NTP_UPDATE_PERIOD_SEC 86400 // 1 day

// We increment this counter everytime the square wave falls. Once
// it gets to 86400 (one day), we sync with NTP again and reset this
// counter.
static uint32_t ntp_sec_counter = 0;

// keep track of the builtin LED state
static int ledstate = 1;

// NTP server(s) to use for setting the system clock
static const char * const dl_ntp_server1 = "pool.ntp.org";

// vars to keep track of whether we were able to bring the
// different components online
static bool dl_rtc_online = false;
static bool dl_system_clock_set = false;
static bool dl_color_sensor_online = false;
// char time_sync_bundle[16];
// uint32_t unixtime, ntptime;
// int havetime = 0;

static bool light_sensor_online = false;

static IPAddress oscipaddr;
static int oscport;
static bool oscsend;

static unsigned char macaddr[6];
#define MACADDRSTR_SIZE 18
static char macaddrstr[MACADDRSTR_SIZE];

#define UUID_SIZE 1024
static String uuid;
// static String dl_uuid = THINGNAME; // this is to set a variable to allow reprinting the thingname in the OSC / MQTT message as a check against the MAC address


void saveUUID(void) // when flashed DL_INIT mode this includes secrets.h file and saves contents to preferences

{
  prefs.putString("uuid", THING_NAME);
  Serial.println("UUID SAVED ");
  Serial.println(uuid);

  prefs.putString("cert_ca", AWS_CERT_CA);
  Serial.println("AWS CERT CA SAVED");
  Serial.println(cert_ca);

  prefs.putString("endpoint", AWS_IOT_ENDPOINT); 
  Serial.println("IOT ENDPOINT SAVED");
 
  prefs.putString("cert_device", AWS_CERT_CRT); 
  Serial.println("Device Certificate SAVED");
 
  prefs.putString("cert_private", AWS_CERT_PRIVATE);
  Serial.println("Device Private Key SAVED");

  prefs.putString("ssid", WIFI_SSID);
  Serial.printf("TEST NETWORK SSID SAVED");
   
  prefs.putString("pwd", WIFI_PASSWORD);
  Serial.printf("TEST NETWORK WIF PASSWORD SAVED");

Serial.print("SETTINGS SAVED TO MEMORY ");
toggleLED();
delay(100);
toggleLED();
}

void retrieveUUID(void) // when flashed DL_INIT mode this retrieves preferences

{
cert_ca = prefs.getString("cert_ca","");
Serial.println("AWS CERT CA RETRIEVED");
Serial.println(cert_ca);

endpoint = prefs.getString("endpoint", ""); 
Serial.println("IOT Endpoint Retrieved");

cert_device = prefs.getString("cert_device", ""); 
Serial.println("Device Certificate Retrieved");

cert_private = prefs.getString("cert_private", "");
Serial.println("Device Private Key Retrieved");

uuid = prefs.getString("uuid", "");
Serial.println("UUID RETRIEVED");
Serial.println(uuid);

Serial.print("MAC ADDRESS RETRIEVED");
Serial.println(WiFi.macAddress());


// define AWS Topics from variables just retrieved
dlPublish = "DomesticLight/" + uuid + "/data";  // variables for use publish topic  from preferences
AWS_IOT_PUBLISH_TOPIC = dlPublish;
Serial.print("Publish topic defined as: ");
Serial.println(AWS_IOT_PUBLISH_TOPIC);

dlSubscribe = "DomesticLight/" + uuid + "/sub";  // setting subscribe topic from preferences.h
AWS_IOT_SUBSCRIBE_TOPIC = dlSubscribe;
Serial.print("Subscribe topic defined as: ");
Serial.println(AWS_IOT_SUBSCRIBE_TOPIC);

Serial.println("SETTINGS SUCCESSFULLY RETRIEVED FROM MEMORY");
Serial.println("PLEASE REFLASH BOARD WITH DL_client.ino");
toggleLED();
delay(100);
toggleLED();
}




////////////////////////////////////////////////////////////
// I2C utilities
// 
// these two functions are utilities used to set the frequency of
// the rtc square wave
////////////////////////////////////////////////////////////

static void write_i2c_register(uint8_t addr,
                               uint8_t reg,
                               uint8_t val)
{
    Wire.beginTransmission(addr);
    Wire._I2C_WRITE((byte)reg);
    Wire._I2C_WRITE((byte)val);
    Wire.endTransmission();
}

static uint8_t read_i2c_register(uint8_t addr,
                                 uint8_t reg)
{
    Wire.beginTransmission(addr);
    Wire._I2C_WRITE((byte)reg);
    Wire.endTransmission();

    Wire.requestFrom(addr, (byte)1);
    return Wire._I2C_READ();
}

////////////////////////////////////////////////////////////
// ISRs
////////////////////////////////////////////////////////////

void isr_rtc_sqw()
{
    rtc_sqw_fell = 1;
}

////////////////////////////////////////////////////////////
// LED
////////////////////////////////////////////////////////////

int toggleLED(void)
{
    if(ledstate == HIGH)
    {
        ledstate = LOW;
    }
    else
    {
    	ledstate = HIGH;
    }
    digitalWrite(DL_PIN_BUILTIN_LED, ledstate);
    return ledstate;
}

////////////////////////////////////////////////////////////
// RTC 
////////////////////////////////////////////////////////////



void setup()
{
  
  
    // Set up serial communication
    {
        Serial.begin(115200);
        // DL_HWSERIAL is UART 2, the TX/RX pins broken out
        // on the feather boards
        DL_HWSERIAL.begin(115200);
    }
 
   prefs.begin("dl", false); // false => r/w
   
   prefs.clear();
   Serial.println("Preferences Cleared");
    // Set up pins
    {
        pinMode(DL_PIN_BUTTON1, INPUT);
        pinMode(DL_PIN_BUILTIN_LED, OUTPUT);
    }

    // Initialize builtin LED
    {
        digitalWrite(DL_PIN_BUILTIN_LED, ledstate);
        ledstate = LOW;
    }

    saveUUID();
    retrieveUUID();

    
    // Initialize the light sensor
    {
        Serial.printf("Starting the color sensor...");
        if(as7341.begin())
        {
            as7341.setATIME(ATIME);
            as7341.setASTEP(ASTEP);
            as7341.setGain(GAIN);
            as7341.startReading();
            light_sensor_online = true;
            Serial.printf("LIGHT SENSOR WORKS.\n");
        }
        else
        {
            // Can't initialize the light sensor.  Need to
            // strategize about what to do if this happens.
            Serial.printf("failed.\n");
        }
    }
    Serial.println("SETTINGS SAVED TO MEMORY.");
    delay(100);
    Serial.println("COMPLETE: Please log the UUID and MAC Address.");
    Serial.print("UUID Set:  ");
    Serial.println(uuid);
    Serial.print("MAC ADDRESS:  ");
    Serial.println(WiFi.macAddress());
    Serial.println("THEN REFLASH BOARD WITH DL_client.ino ");
}

void loop()
{
toggleLED();
delay(1000);
Serial.println("SETTINGS SAVED TO MEMORY.");
    delay(100);
    Serial.println("COMPLETE: Please log the UUID and MAC Address.");
    Serial.print("UUID Set:  ");
    Serial.println(uuid);
    Serial.print("MAC ADDRESS:  ");
    Serial.println(WiFi.macAddress());
    Serial.println("THEN REFLASH BOARD WITH DL_client.ino ");
    toggleLED();
delay(3000);
}
