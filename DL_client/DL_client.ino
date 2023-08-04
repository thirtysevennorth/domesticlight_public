// SEE README.MD file for more details, credits, license. Rev 4 Aug 2023

// USE OF THIS SKETCH REQUIRES THAT THE BOARD WAS FLASHED FIRST WITH DL_client_INIT.ino
// AND SERVER credentials were stored in permanent memory.

// INITIAL SETUP NOTES
// For initial set-up boot into ADHOC mode by pressing and holding Button 1 (Left Button), 
// while pressing and releasing the RESET Button (Right Button). The device will boot into "ADHOC MODE"
// on a computer or phone look for the WIFI Network "DomesticLight". Connect to it via wifi.
// then using a browser go to HTTP://192.168.4.1
// Enter your local WIFI network SSID, and its password which is saved locally on the sensor
// You can also optionally configure local OSC transmission of the data.

///////////////////////////////////////////////////////////
// Preferences setting and reset
////////////////////////////////////////////////////////////

// FACTORY RESET - ERASES ALL FLASH PREFERENCES 
// Uncomment this, and the preferences will be overwritten.
// #define DL_FACTORY_RESET
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// DEFINE THE PROCESSOR IN USE
// Processors supported are UM Feather S3 or Adafruit Feather S3. choose 1

//#define ADAFRUIT_FEATHER_ESP32
#define UM_ESP32S3

////////////////////////////////////////////////////////
//// DEFINE THE RTC USED ///////////////////////////////
// Production boards use MAX31343, prototypes use DS3231.
//
// #define DS3231 // for PROTOTYPE SERIES 
#define RTC_MAX31343 //FOR PRODUCTION AND ARTIST PROOF SERIES

// END OF REQUIRED DEFINITIONS///////////////////////////

////////////////////////////////////////////////////////////
// config options
////////////////////////////////////////////////////////////

#include <pgmspace.h>
#include <MicrOSCriptESP32.h>
#include <Wire.h>
#include <FastLED.h> // LED library for color changing WS2812B

#ifdef RTC_MAX31343
#include "MAX31343.h" // this is from the <AnalogRTClib> Arduino library. make the corrections noted to fix the directory error in the header file.
#else
#include <RTClib.h>
#endif

#include <WiFi.h>
#include <WiFiUDP.h>
#include <WebServer.h>
#include <Preferences.h>
#include <cstdio>

#ifdef AS7343
#include <AS7343.h>
#else
#include <Adafruit_AS7341.h>
#endif

#include <time.h>
#include <ArduinoJson.h>

#include <Arduino.h>

#define DL_AWS

#ifdef DL_AWS
// #include "secrets.h" // do not use. left if a future user wants to adapt code to place credentials in sketch.  
// rather than storing in preferences using an INIT sketch. 
// To do so uncomment, and add credentials in the secrets.h file and change the calls to prefs to direct to secrets.h .
#endif

#ifdef DL_AWS
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#else
#include <WiFiClient.h>
#endif

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

// String dlShadowUpdate;  // for use with future device shadow
// String dlShadowGet;
// String dlShadowUpdateAccepted;
// String dlShadowUpdateRejected;
// String dlShadowUpdateDelta;
// String dlShadowGetAccepted;

////////////////////////////////////////////////////////////
// pins: Feather format boards currently supported : 
// ESP32-S3 Feather from Adafruit (or w/o PSRAM); Unexpected Maker Feather S3
// Set def for board above by uncommenting your board, and commenting out others.
////////////////////////////////////////////////////////////

#if defined(ADAFRUIT_FEATHER_ESP32)
#define DL_PIN_BUTTON1 26 // A0 (ADC2) DAC2 
#define DL_PIN_BUTTON2 25 // A1 (ADC2) DAC1
#define DL_PIN_RTC_SQUARE_WAVE 27

#define DL_PIN_RGB_LED 14 // (ADC2) LED is a WS2812B single data line. Connects to PIN IO14 / A4/ ADC2
#define DL_PIN_BUILTIN_LED 13 // could change to  built in neopixel on pin 33 PIN_NEOPIXEL
//#define DL_HWSERIAL Serial2
#elif defined(UM_ESP32S3)
#define DL_PIN_BUTTON1 17 // A0 (ADC2) DAC2  NOTE on artist proof and prototype boards IO17 and IO18 have labels reversed
#define DL_PIN_BUTTON2 18 // A1 (ADC2) DAC1 NOTE on artist proof and prototype boards IO17 and IO18 have labels reversed
#define DL_PIN_RTC_SQUARE_WAVE 3 // REV F and after pin is #3

#define DL_PIN_RGB_LED 14 // IO14 is large ws2812b LED on board, IO40 is UM3 builtin ws2812.
#define DL_PIN_BUILTIN_LED 13
// #define DL_HWSERIAL Serial0
#else
#error No pin definitions for this board!
#endif

#ifndef DS3231_ADDRESS
#define DS3231_ADDRESS 0x68
#endif

// FastLED library definitions for production and prototype board use
#define LED_TYPE WS2812B
#define NUM_LEDS 1
#define BRIGHTNESS 20
#define DATA_PIN DL_PIN_RGB_LED
// Define the array of leds
CRGB leds[NUM_LEDS];

////////////////////////////////////////////////////////////
// classes and globals
////////////////////////////////////////////////////////////

#ifdef DL_AWS
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
long lastReconnectAttempt = 0; //AWS Reconnect counter
#endif

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
uint8_t ATIME = 29; // sets variables for light sensor config
uint16_t ASTEP = 599;
as7341_gain_t GAIN = AS7341_GAIN_1X;
uint16_t averageValue = 4000;

// DATA COLLECTION FREQUENCY
static int dataFrequency = 3; // set to a number of seconds to publish data based on square wave ticks. refers to loop ~ line 870
static int sampleCounter = 0;


// used to decide what to do in the loop depending on whether
// we're in config mode or not. this gets set if button 1 is
// pressed during setup().
static bool adhoc_mode;

// RTC variables - keep track of whether the RTC lost power
static int rtc_lost_power; //currently production boards do not look for power loss
static volatile int rtc_sqw_fell = 0;
float rtc_temp = 0;
long previoussysTime = 0;

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

static bool light_sensor_online = false;

static IPAddress oscipaddr;
static int oscport;
static bool oscsend;

static unsigned char macaddr[6];
#define MACADDRSTR_SIZE 18
static char macaddrstr[MACADDRSTR_SIZE];

#define UUID_SIZE 1024
static String uuid;

#define JSONBUF_SIZE 2048
static char jsonbuf[JSONBUF_SIZE];

// this is so that we can return a list from a function for the color sensor
struct color
{
    uint16_t values[12];
};

uint16_t readings[12]; // duplicate unblocked for autogain

unsigned long readingStartTime; // Variable to store the start time of the reading
const unsigned long readingTimeout = 500; // Timeout duration in milliseconds (adjust this as needed)


////////////////////////////////////////////////////////////
// JSON
////////////////////////////////////////////////////////////

//char *macaddrstr,
// uint8_t ATIME, // change as needed

void formatJSON(struct color color,
                uint32_t time,
                float rtc_temp,
                String uuid,
                char *buf,
                size_t buflen)
{
    StaticJsonDocument<1024> doc;
    
    doc["MAC_ID"] =  macaddrstr;   //macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5];  //remove if UUID will work.
    doc["F1_415"] = color.values[0];
    doc["F2_445"] = color.values[1];
    doc["F3_480"] = color.values[2];
    doc["F4_515"] = color.values[3];
    //doc["A5_VIS1"] = color.values[4];  //ADC 4/5 are duplicate of 10/11 clear NIR reading
    // doc["A6_NIR1"] = color.values[5];  //ADC 4/5 are duplicate of 10/11 clear NIR reading
    doc["F5_555"] = color.values[6];  
    doc["F6_590"] = color.values[7];
    doc["F7_630"] = color.values[8];
    doc["F8_680"] = color.values[9];
    doc["F9_VIS"] = color.values[10]; 
    doc["F10_NIR"] = color.values[11];
    doc["UUID"] = uuid;
    doc["unixtime"] = time;
    doc["RTCTemp"] = rtc_temp; //rtc temp in C dl_rtc_temp();
    doc["ATIME"] = ATIME; // using breaks AWS. causes a MQTT timeout for data and bricks serial communication
    doc["ASTEP"] = ASTEP;  //AS7341 ASTEP
    doc["GAIN"] = GAIN; //AS7341 GAIN
    // TO DO - ADD PLACEHOLDERS FOR FUTURE VOC AND METHANE SENSORS
    serializeJson(doc, buf, buflen); // print to client
}

void getUUID(void) // Use the DL INIT file to save authentication info to preferences
// retrieves authentication credentials stored on board with the INIT setup file. To use outside of DL replace with calls to secrets.h
{
cert_ca = prefs.getString("cert_ca","");
Serial.println("UNIVERSAL AWS CERT CA RETRIEVED ");
// Serial.println(cert_ca);

uuid = prefs.getString("uuid", "");
Serial.print("UUID RETRIEVED: ");
Serial.println(uuid);

endpoint = prefs.getString("endpoint", ""); 
Serial.println("IOT Endpoint Retrieved ");

cert_device = prefs.getString("cert_device", ""); 
Serial.println("Device Certificate Retrieved ");

cert_private = prefs.getString("cert_private", "");
Serial.println("Device Private Key Retrieved ");


// define AWS Topics from authentication and endpoint info just retrieved
dlPublish = "DomesticLight/" + uuid + "/data";  // variables for use publish topic  from preferences
AWS_IOT_PUBLISH_TOPIC = dlPublish;
Serial.print("Publish topic defined as: ");
Serial.println(AWS_IOT_PUBLISH_TOPIC);

dlSubscribe = "DomesticLight/" + uuid + "/sub";  // setting subscribe topic from preferences.h
AWS_IOT_SUBSCRIBE_TOPIC = dlSubscribe;
Serial.print("Subscribe topic defined as: ");
Serial.println(AWS_IOT_SUBSCRIBE_TOPIC);

leds[0] = CRGB(10,10,10); // pale white to show AWS success
FastLED.show();
// TO DO ADD FUTURE DEVICE SHADOW TOPIC SETTING HERE
}

////////////////////////////////////////////////////////////
// AWS
////////////////////////////////////////////////////////////
void publishMessage(char *jsonBuffer)
{
#ifdef DL_AWS
    client.publish(AWS_IOT_PUBLISH_TOPIC.c_str(), jsonBuffer);
    client.loop();
  
#endif
}

// message handler is currently not working
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


#ifdef DL_AWS
void connectAWS()
{   /// based on API for PubSubClient https://pubsubclient.knolleary.net/api#PubSubClient
    //
    // WIFI setup in dl_boot_client() called from setup()
    
    // Configure WiFiClientSecure to use the AWS IoT device credentials
    net.setCACert(cert_ca.c_str());  //AWS_CA_CRT
    net.setCertificate(cert_device.c_str()); // AWS_CERT_CRT
    net.setPrivateKey(cert_private.c_str()); //AWS_CERT_PRIVATE
    client.setKeepAlive(60); //sets timeout in seconds
    client.setBufferSize(2048);
 
    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.setServer(endpoint.c_str(), 8883); //AWS_IOT_ENDPOINT
 
    // Create a message handler
    client.setCallback(messageHandler);
 
    Serial.println("Connecting to AWS IOT");

    int connect_ctr = 0;
    while(!client.connect(THINGNAME.c_str()) && connect_ctr < 30)
    {
        Serial.print(".");
        delay(500);
        ++connect_ctr;
    }
 
    if(!client.connected())
    {
        Serial.println("AWS IoT Timeout!");
        Serial.println(client.state());
        return;
    }
 
    // Subscribe to a topic
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC.c_str());
    client.subscribe(AWS_IOT_PUBLISH_TOPIC.c_str());
    client.publish(AWS_IOT_SUBSCRIBE_TOPIC.c_str(),  "sensor online");

    /// shadow topics remark out if not in use.  
    // client.subscribe(AWS_IOT_SHADOWUPDATE_TOPIC); 
    // client.subscribe(AWS_IOT_SHADOWGET_TOPIC);
    // client.subscribe(AWS_IOT_SHADOWGET_ACCEPT_TOPIC);
    // client.subscribe(AWS_IOT_SHADOWUPDATE_ACCEPT_TOPIC);
    // client.subscribe(AWS_IOT_SHADOWUPDATE_REJECT_TOPIC);
    // client.subscribe(AWS_IOT_SHADOWUPDATE_DELTA_TOPIC);

    
    Serial.println("AWS IoT Connected! - subscribe and publish topics");
    Serial.println(AWS_IOT_SUBSCRIBE_TOPIC.c_str());
    Serial.println(AWS_IOT_PUBLISH_TOPIC.c_str());
    // Serial.println(AWS_IOT_SHADOWUPDATE_TOPIC);
    // Serial.println(AWS_IOT_SHADOWGET_TOPIC);
    leds[0] = CRGB(20,30,0); // brighter reddish green to show AWSconnect
    FastLED.show();
    
}
#endif

// AWS reconnect -- This runs in loop just before publishing a message.

boolean reconnectAWS() {
  if (!client.connected()) {
     Serial.print("Attempting to restore AWS connection...");
     leds[0] = CRGB(20,0,0); // red to show aws connect error
     FastLED.show();
     if (client.connect(THINGNAME.c_str())) {
      Serial.println("reconnected to AWS");
    // Once connected, publish an announcement...
   // client.publish(publishTopic, publishPayload);
  // ... and resubscribe
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC.c_str());
    client.subscribe(AWS_IOT_PUBLISH_TOPIC.c_str());
    client.publish(AWS_IOT_SUBSCRIBE_TOPIC.c_str(), "sensor back online:");
    }
     else {Serial.print("failed, rc=");
     leds[0] = CRGB(100,0,0); // bright red to show aws connect fail
     FastLED.show();
     // Serial.print(client.state());
      }
      
  }
 return client.connected();
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
{if(adhoc_mode == true) {
    if(ledstate == HIGH)
    {
        ledstate = LOW;
        leds[0] = CRGB(20,20,20);
        FastLED.show();
    }
    else
    {
    	ledstate = HIGH;
      leds[0] = CRGB(50,50,50); //white to show adhoc
      FastLED.show();
    }
    digitalWrite(DL_PIN_BUILTIN_LED, ledstate);
    return ledstate;
    }

  else {if(ledstate == HIGH)
    {
        ledstate = LOW;
        leds[0] = CRGB::Black;
        FastLED.show();
        Serial.println("LED OFF");
    }
    else
    {
    	ledstate = HIGH;
       leds[0] = CRGB(0,0,5); //dim flash to show read
       FastLED.show();
       Serial.println("LED ON");
    }
    digitalWrite(DL_PIN_BUILTIN_LED, ledstate);
    return ledstate;
    }

}

////////////////////////////////////////////////////////////
// RTC 
////////////////////////////////////////////////////////////
// There are two versions of the rtc core functions to account for differences between DS3231 and MAX31343 chips
// DS3231 uses the RTCLib library and MAX31343 using the AnalogRTCLib library.
// Set the def in the config section at beginning of file

// DS3231 version //////////////////////////////////////////
#ifdef DS3231

static bool rtc_init(int rtc_square_wave_pin)
{
    // this is the RTC alarm
    pinMode(DL_PIN_RTC_SQUARE_WAVE, INPUT); 
    digitalWrite(DL_PIN_RTC_SQUARE_WAVE, HIGH);

    attachInterrupt(digitalPinToInterrupt(DL_PIN_RTC_SQUARE_WAVE),
                    isr_rtc_sqw, FALLING);
// #endif
    int r = rtc.begin();
    if(r){
        write_i2c_register(DS3231_ADDRESS, 0x0e, 0);
    }
    else
    {
        Serial.printf("couldn't start RTC\n");
        return false;
    }
    rtc_lost_power = rtc.lostPower();
    printDeviceTime();
    return true;
}

// DS3231 version
static uint32_t dl_now_unixtime(void)
{
    if(dl_rtc_online)
    {
        return rtc.now().unixtime();
    }
    else
    {
        // Fall back to the system clock
        return time(NULL);
        Serial.print("DS3231 RTC NOT ONLINE, USING SYSTEM CLOCK: ");
    }
}
//// END DS3231 RTC Version /////////////////////
#endif

// MAX31343 version rtc_init//////////////////////////////////////////
#ifdef RTC_MAX31343
#include "MAX31343.h"

static bool rtc_init(int rtc_square_wave_pin)
{
    // this is the RTC alarm
    //static sqw_out_freq_t SQW_OUT_FREQ_1HZ = 0;
    pinMode(DL_PIN_RTC_SQUARE_WAVE, INPUT); 
    rtc.begin();
    rtc.set_square_wave_frequency(MAX31343::SQW_OUT_FREQ_1HZ);
   // pinMode(DL_PIN_RTC_SQUARE_WAVE, INPUT); 
    digitalWrite(DL_PIN_RTC_SQUARE_WAVE, HIGH);
    //irq_enable(MAX31343::INTR_ID_PFAIL);
    //irq_enable(MAX31343::SQW_OUT_FREQ_1HZ)
    attachInterrupt(digitalPinToInterrupt(DL_PIN_RTC_SQUARE_WAVE),
                    isr_rtc_sqw, FALLING);

    int r = rtc.get_time(&rtc_ctime);
    printDeviceTime();
    //to do add RTC power fail code
    // /rtc.get_status.stat.bits.pfail(); 
    //rtc.get_status()* @brief  Read status byte  @param[out]   stat: Decoded status byte @returns      0 on success, negative error code on failure.
   return true;            
      
}

// MAX31343 version DLNOW UNIXTIME
static uint32_t dl_now_unixtime(void)
{  // Serial.print("RTC update (1=success): ");  // for testing
   // Serial.println(dl_rtc_online);
    if(dl_rtc_online)
    { 
      // struct tm rtc_ctime;
      int ret = rtc.get_time(&rtc_ctime);
      return mktime(&rtc_ctime);
      Serial.print("RTC _NTP update success: ");
      Serial.println(dl_rtc_online);
    }
    else
    {
        // Fall back to the system clock
        return time(NULL);  // uses NTP time library
        Serial.print("TIME UPDATE FAILED - using system clock");
    }
}
//// END MAX31343 RTC Version /////////////////////
#endif




#define dl_now() dl_now_unixtime() + 2208988800UL

// currently the OSC Time message shows up as unknown data type and value in isadora and max.
// for troubleshooting Would like to have time in human time go to OSC as happen with AWS
// Other OSC desires - confirmation of publication to AWS 

#ifdef RTC_MAX31343
static void pushTimeMsg(ose_bundle vm_s, uint32_t sec, uint32_t fsec) 
{
    ose_pushMessage(vm_s, "/time", strlen("/time"),
                    1, OSETT_TIMETAG, dl_now(), 0);
}

static void now(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    // ose_drop(vm_s);
    // DateTime d = rtc.now();
    // ose_pushTimetag(vm_s, d.unixtime() + 2208988800UL, 0);
    pushTimeMsg(vm_s, dl_now(), 0);
}
#else
static void pushTimeMsg(ose_bundle vm_s, uint32_t sec, uint32_t fsec) 
{
    ose_pushMessage(vm_s, "/time", strlen("/time"),
                    1, OSETT_TIMETAG, dl_now(), 0);
}

static void now(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    // ose_drop(vm_s);
    // DateTime d = rtc.now();
    // ose_pushTimetag(vm_s, d.unixtime() + 2208988800UL, 0);
    pushTimeMsg(vm_s, dl_now(), 0);
}
#endif
////////////////////////////////////////////////////////////
// color sensor
////////////////////////////////////////////////////////////

static struct color getColor(void)
{
    struct color c;
    memset(&c, 0, sizeof(c));
    if(light_sensor_online)
    {
        if (!as7341.readAllChannels(c.values)){
            Serial.printf("error reading color values\n");
        }
    }
    // Serial.printf("%d\n",c.values[10]);
    return c;
}

static void pushColorMsg(ose_bundle vm_s, struct color color)
{
    ose_pushMessage(vm_s, "/color", strlen("/color"), 12,
                    OSETT_INT32, color.values[0],
                    OSETT_INT32, color.values[1],
                    OSETT_INT32, color.values[2],
                    OSETT_INT32, color.values[3],
                    OSETT_INT32, color.values[6],
                    OSETT_INT32, color.values[7],
                    OSETT_INT32, color.values[8],
                    OSETT_INT32, color.values[9],
                    OSETT_INT32, color.values[10],
                    OSETT_INT32, color.values[11],
                    OSETT_INT32, color.values[12]);
}

static void color(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    // ose_drop(vm_s);
    // ose_pushInt32(vm_s, 33);
    struct color color = getColor();
    pushColorMsg(vm_s, color);
}
// send ASTEP
static void pushASTEPMsg(ose_bundle vm_s, uint16_t ASTEP)
{
    ose_pushMessage(vm_s, "/astep", strlen("/astep"), 1,
                    OSETT_INT32, ASTEP);
                 
}

static void ASTEPMSG(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    // ose_drop(vm_s);
    // ose_pushInt32(vm_s, 33);
   ASTEP;
    pushASTEPMsg(vm_s, ASTEP);
}

/// send ATIME
static void pushATIMEMsg(ose_bundle vm_s, uint8_t ATIME)
{
    ose_pushMessage(vm_s, "/atime", strlen("/atime"), 1,
                    OSETT_INT32, ATIME);
                 
}

static void ATIMEMSG(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    // ose_drop(vm_s);
    // ose_pushInt32(vm_s, 33);
   ATIME;
    pushATIMEMsg(vm_s, ATIME);
}

/// send gain
static void pushGAINMsg(ose_bundle vm_s, uint16_t GAIN)
{
    ose_pushMessage(vm_s, "/gain", strlen("/gain"), 1,
                    OSETT_INT32, GAIN);
                 
}

static void GAINMSG(ose_bundle osevm)
{
    ose_bundle vm_s = OSEVM_STACK(osevm);
    // ose_drop(vm_s);
    // ose_pushInt32(vm_s, 33);
   GAIN;
    pushGAINMsg(vm_s, GAIN);
}

// timeout check for reading
bool readingTimeOutCheck()
{
// Check if the timeout duration has passed since the reading started
  if (millis() - readingStartTime >= readingTimeout) {
    return true; // Timeout occurred
  }
  return false; // Timeout not reached yet
}
// autogain function changes gain and astep and atime in relation to visible light intensity. 
// called from loop where averageValue is set to equal most recent visible light intensity
as7341_gain_t AutoGAIN()
{
      // autogain functionality needs to be redone to index a step at a time. switch statement indexed against gain 
      // then move up / down one index point to 
      // add code to delay LED after reading check LED state - if LEDstate is 1, toggle led, then get color.
            // if led state is 0, get color then toggle led otherwise turn off led
      uint16_t readings[12];
        if (!as7341.readAllChannels(readings)) {Serial.println("Error reading all channels!");
         averageValue = 5000;
         return GAIN;
         }
             
      // Serial.print("ADC4/Clear autogain basis    : ");
      // Serial.println(readings[10]);
      averageValue = readings[10];
    
      switch (GAIN) {
      case AS7341_GAIN_0_5X:
         {if (averageValue >= 3000 && averageValue < 10000) {GAIN = AS7341_GAIN_0_5X; ASTEP = 299; ATIME = 29;
           }
           else if (averageValue >= 10000) {GAIN = AS7341_GAIN_0_5X; ASTEP = 59; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_1X; ASTEP = 299; ATIME = 29;
           }
           break; 
         }
      case AS7341_GAIN_1X:
         {if (averageValue >= 3000 && averageValue < 10000) {GAIN = AS7341_GAIN_1X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue >= 10000) {GAIN = AS7341_GAIN_0_5X; ASTEP = 299; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_2X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
      case AS7341_GAIN_2X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_1X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 3000) {GAIN = AS7341_GAIN_4X; ASTEP = 599; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_2X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
      case AS7341_GAIN_4X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_2X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 3000) {GAIN = AS7341_GAIN_8X; ASTEP = 599; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_4X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
       case AS7341_GAIN_8X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_4X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 3000) {GAIN = AS7341_GAIN_16X; ASTEP = 599; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_8X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
        case AS7341_GAIN_16X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_8X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 2000) {GAIN = AS7341_GAIN_32X; ASTEP = 599; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_16X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
        case AS7341_GAIN_32X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_16X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 2000) {GAIN = AS7341_GAIN_64X; ASTEP = 599; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_32X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
        case AS7341_GAIN_64X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_32X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 3000) {GAIN = AS7341_GAIN_128X; ASTEP = 599; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_64X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
         case AS7341_GAIN_128X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_64X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 3000) {GAIN = AS7341_GAIN_256X; ASTEP = 599; ATIME = 29;
           }
           else {GAIN = AS7341_GAIN_128X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
        case AS7341_GAIN_256X:
         {if (averageValue >= 10000) {GAIN = AS7341_GAIN_128X; ASTEP = 599; ATIME = 29;
           }
           else if (averageValue < 3000) {GAIN = AS7341_GAIN_256X; ASTEP = 999; ATIME = 39;
           }
           else {GAIN = AS7341_GAIN_256X; ASTEP = 599; ATIME = 29;
           }
           break; 
         }
        }
      as7341.setGain(GAIN);
      as7341.setASTEP(ASTEP);
      as7341.setATIME(ATIME);
      // Serial.print("current gain: ");
      // Serial.println(GAIN);
      return GAIN;
}
// // TO DO - ISSUE 18 - RAW COUNTS TO BASIC COUNTS
// add conversion of raw values to basic counts - this factors in gain and integration

// static struct counts getCounts(void)
// {
//    struct counts count;
//    memset(&count, 0, sizeof(count));
//    if(light_sensor_online)
//    {
//        if (!as7341.readAllChannels(c.values)){
//            Serial.printf("error reading color values\n");
//      }
//   }
// return count;
// }

// float counts[12];
// for(uint8_t i = 0; i < 12; i++) {
 //   if(i == 4 || i == 5) continue;
    // we skip the first set of duplicate clear/NIR readings
    // (indices 4 and 5)
 ////   counts[i] = as7341.toBasicCounts(readings[i]); //NB BasicCounts = RawSensorValue / ( Gain x IntegrationTime ) * (correction factor per channel from AMS)
//  }




////////////////////////////////////////////////////////////
// ad hoc network callbacks
////////////////////////////////////////////////////////////

void handleRoot()
{   
    // String s = index_html;
    int len = strlen(index_html) + 512;
    char buf[len];
    snprintf(buf, len, index_html,
             macaddrstr,
             prefs.getString("uuid", "").c_str(),
             prefs.getString("ssid", "").c_str(),
             prefs.getString("pwd", "").c_str(),
             prefs.getString("oscipaddr", "").c_str(),
             prefs.getInt("oscport", 20000));
          
    String s = buf;
    server.send(200, "text/html", s);
    leds[0] = CRGB(100,100,100); // bright white to show entered adhoc mode
    FastLED.show();
}

void handleGetColor()
{   leds[0] = CRGB(80,80,80); // slightly dimmer white to show adhoc color read
    FastLED.show(); 
    struct color c = getColor();
    char buf[512];
    static int32_t counter;
    snprintf(buf, 256, "%d %d %d %d %d %d %d %d %d %d %d %d",
             c.values[0], c.values[1], c.values[2],
             c.values[3], c.values[4], c.values[5],
             c.values[6], c.values[7], c.values[8],
             c.values[9], c.values[10], c.values[11]);
    String s = buf;
    server.send(200, "text/plain", s);
    leds[0] = CRGB(100,100,100); // bright white to show entered adhoc mode
    FastLED.show(); 
}

void handleGet()
{   // uuid being set at time of config now
    if(server.hasArg("uuid") == true)
    {
        prefs.putString("uuid", server.arg("uuid"));
        Serial.printf("%s: uuid: %s\n", __func__, server.arg("uuid").c_str());
   }
    if(server.hasArg("network") == true)
    {
        prefs.putString("ssid", server.arg("network"));
        Serial.printf("%s: ssid: %s\n", __func__, server.arg("network").c_str());
    }
    if(server.hasArg("password") == true)
    {
        prefs.putString("pwd", server.arg("password"));
        Serial.printf("%s: pwd: %s\n", __func__, server.arg("password").c_str());
    }
    if(server.hasArg("oscipaddr") == true)
    {
        prefs.putString("oscipaddr", server.arg("oscipaddr"));
        Serial.printf("%s: IP Addr: %s\n", __func__, server.arg("oscipaddr").c_str());
    }
    if(server.hasArg("oscport") == true)
    {
        prefs.putInt("oscport", server.arg("oscport").toInt());
        Serial.printf("%s: Port: %s\n", __func__, server.arg("oscport").c_str());
    }
    if(server.hasArg("oscsend") == true)
    {
        prefs.putBool("oscsend", true);
        Serial.printf("%s: Send OSC: true\n", __func__);
    }
    else
    {
        prefs.putBool("oscsend", false);
        Serial.printf("%s: Send OSC: false\n", __func__);
    }
    prefs.end();
    leds[0] = CRGB(0,100,0); // bright green flash to show adhoc mode success
    FastLED.show();
    delay(1000);
    ESP.restart();
}


// print device time at startup
#ifdef RTC_MAX31343
//MAX31343 version of print device time

void printDeviceTime()
{char buf[40];
int now = rtc.get_time(&rtc_ctime); //new MAX31343 variable ...
int ret = rtc.get_time(&rtc_ctime);
    if (ret) {
        Serial.println("get_time failed!");
        return;
    }
    
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &rtc_ctime); // prints time
    Serial.print("current UTC time per RTC: ");
    Serial.println(buf);

    Serial.print("Current RTC Temperature: ");
    Serial.print(rtc_temp, 2);
    Serial.println(" C");
}
#else
//DS3231 version print device time
void printDeviceTime()
{
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime now = rtc.now();
    Serial.println("current UTC date and time ");
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");

    Serial.print("Current RTC Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

}

#endif


void dl_bind_OSC_functions(void)
{
    o.bindfn("/o/rtc/now", now);
    o.bindfn("/o/color", color);
}

void dl_boot_adhoc(void) // called from setup if adhoc button is pressed
{
    Serial.printf("Entering ad hoc mode...");
    o.init();
    dl_bind_OSC_functions();
    adhoc_mode = true;
    prefs.begin("dl", false); // false => r/w
#ifdef DL_FACTORY_RESET
    prefs.clear();
#endif
    getUUID();
    WiFi.softAP(adhoc_ssid, NULL);
    IPAddress adhoc_ipaddr = WiFi.softAPIP();
    server.on("/", handleRoot);
    server.on("/color", handleGetColor);
    server.on("/get", handleGet);
    server.begin();
    Serial.printf("ready.\n");
    Serial.printf("Visit %s with a browser "
                  "to configure this device.\n",
                  adhoc_ipaddr.toString().c_str());
    leds[0] = CRGB(100,100,100); // white to show adhoc mode
    FastLED.show();
}

///// DL BOOT CLIENT. CALLED FROM SETUP
void dl_boot_client(void)
{
    // In normal client mode, we retrieve the ssid and
    // password for the WiFi network from the
    // preferences.
    Serial.printf("Starting up...\n");
    adhoc_mode = false;
    leds[0] = CRGB(20,0,0); // red startup
    FastLED.show();
    delay(200);

#ifdef DL_INIT    
    prefs.begin("dl", false); // false => r/w
#else
    prefs.begin("dl", true); // true => read-only
#endif

#ifdef DL_FACTORY_RESET
    prefs.clear();
#endif
    getUUID();
    
    String ssid, pwd;
    if((ssid = prefs.getString("ssid", "")) != ""
       && (pwd = prefs.getString("pwd", "")) != "")
    {
        Serial.printf("Connecting to (%s)...",
                      ssid.c_str());
        o.init(ssid.c_str(), pwd.c_str(), (unsigned int)15000); // need some type of error catch to start logging data locally if wifi fails
        Serial.printf("done\n");
        Serial.printf("IP address: %s\n",
                      o.wifiIPAddress().c_str());
        Serial.printf("Listening for OSC on UDP port %d\n",
                      o.udpPort());
        dl_bind_OSC_functions();
        leds[0] = CRGB(20,20,0); // red-green to show wifi connect
        FastLED.show();
    }
    else
    {
        Serial.printf("No WiFi credentials specified.\n");
        Serial.printf("Hold the button down while rebooting "
                      "in order to configure this device to "
                      "connect to your WiFi network.\n");
        Serial.printf("Rebooting in 5 sec...\n");
        leds[0] = CRGB(50,0,0); // red error to show no connection
        FastLED.show();
        delay(5000);
        ESP.restart();
    }
    if((oscsend = prefs.getBool("oscsend", false)))
    {
        oscipaddr.fromString(prefs.getString("oscipaddr", ""));
        oscport = prefs.getInt("oscport", 20000);
    }
    // uuid = prefs.getString("uuid", "");

    
#ifdef DL_AWS
    connectAWS();
#endif
}

void setup()
{
    bool adhoc_mode = false;
    // Lookup MAC address and format it
    {
        WiFi.macAddress(macaddr);
        snprintf(macaddrstr, MACADDRSTR_SIZE,
                 "%02X:%02X:%02X:%02X:%02X:%02X",
                 macaddr[0], macaddr[1],
                 macaddr[2], macaddr[3],
                 macaddr[4], macaddr[5]);
    }
    
    // Set up serial communication
    {
        Serial.begin(115200);
        // DL_HWSERIAL is UART 2, the TX/RX pins broken out
        // on the feather boards
       // DL_HWSERIAL.begin(115200);
    }

    // Set up pins
    {
        pinMode(DL_PIN_BUTTON1, INPUT);
        pinMode(DL_PIN_BUILTIN_LED, OUTPUT);
    }

    // Initialize builtin LED
    {
        digitalWrite(DL_PIN_BUILTIN_LED, ledstate);
        ledstate = LOW;
        FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
        leds[0] = CRGB(5,0,0);
        FastLED.show();
    }

    // If the button is pressed, boot into ad hoc mode and
    // serve a webpage with configuration options,
    // otherwise, boot into normal client mode.
    if(digitalRead(DL_PIN_BUTTON1) == LOW)
    {   adhoc_mode = true;
        dl_boot_adhoc();
        adhoc_mode = true;
        leds[0] = CRGB(50,50,50);
        FastLED.show();
        delay(1000);
    }
    else
    {
        dl_boot_client(); // this is where wifi, then AWS UUID config starts if adhoc mode not present DL Boot client in 720. Needs error catching for no WIFI connection
    }

    // Initialize the RTC
    Serial.printf("Initializing the RTC...");
    if(!rtc_init(DL_PIN_RTC_SQUARE_WAVE))
    {
        // Couldn't start the RTC. This is pretty bad, but
        // let's hope for now that we'll be able to set the
        // system clock with an NTP server.
    }
    else
    {
        dl_rtc_online = true;
        Serial.printf("done\n");
    }

    // Set system clock. The system clock starts counting
    // from 0, so we wait until we get an NTP value. Once
    // our clock is set, we spin until its value changes,
    // and then immediately set the RTC to the new
    // value. This should synchronize the RTC with the
    // system clock reasonably well.
    if(adhoc_mode == false)
    {
        Serial.printf("Setting the system clock...");
        const int timeout_sec = 20;
        // We don't care about the timezone or DST--all
        // devices should use UTC.
        configTime(0, 0, dl_ntp_server1);
        // Wait until we get a time from the NTP server.
        #ifdef RTC_MAX31343
        struct tm timeinfo;
        int tick = 0;
        while(tick < timeout_sec*10){
          tick++;
          delay(100);
          if(getLocalTime(&timeinfo))
            break;
        }
        if(tick >= timeout_sec*10)
        {
          Serial.printf("failed: ");
          Serial.printf("couldn't get the time from %s\n", dl_ntp_server1);
        }
        else {
          Serial.printf("done\n");
          rtc.set_time(&timeinfo);
          dl_system_clock_set = true;
        }
#else
        time_t t1 = time(NULL);
        while(t1 < timeout_sec)
        {
            delay(100);
            t1 = time(NULL);
        }
        if(t1 == timeout_sec)
        {
            Serial.printf("failed: ");
            Serial.printf("couldn't get the time from %s\n",
                          dl_ntp_server1);
            if(dl_rtc_online == true)
            {
                // This is good--we use the RTC as the main
                // clock anyway. If the time looks
                // reasonable, we could use it to set the
                // system clock.
            }
        }
        else
        {
            Serial.printf("done\n");
            dl_system_clock_set = true;
            // Now spin until we get a transition.
            time_t t2 = t1;
            while(t1 == t2)
            {
                t2 = time(NULL);
            }
            #ifdef RTC_MAX31343
            // Set the RTC for a MAX31343
            tm dt = tm(t2);  /// this part needs help adjusting. 
            rtc.set_time(dt); // See time example on setting


            #else
             // Set the RTC for DS3231
            DateTime dt(t2);
            rtc.adjust(dt);
            #endif
          // show greenish blue flash to indicate clock set success
          leds[0] = CRGB(0,20,30); 
          FastLED.show();
          delay(1000);

        }
#endif
    }

     Serial.printf("Current Time, Date and Temp set to "); printDeviceTime(); //adds current time in to diagnostic printout. Send to AWS also?

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
            Serial.printf("done.\n");
            leds[0] = CRGB(0,0,40); // blue flash to show light sensor online 
            FastLED.show();
            delay(1000);
            leds[0] = CRGB(0,0,0);
            FastLED.show();
            readingStartTime = millis();
        }
        else
        {
            // Can't initialize the light sensor.  Need to
            // strategize about what to do if this happens.
            Serial.printf("failed.\n");
            leds[0] = CRGB(100,0,40); // red flash to show light sensor is not working 
            FastLED.show();
            delay(1000);
        }
    }
}

void loop()
{
  if(adhoc_mode)
  {
      server.handleClient();
      yield();
  }
    else
    {  
    /// test of code for a timeout flag for autogain adjustment outside of the blocking loop
    // bool timeOutFlag = readingTimeOutCheck();  // setting timeout flag to adjust autotgain
    //if(as7341.checkReadingProgress() || timeOutFlag ) {
    // if (timeOutFlag) {
    //  loop();
    //  } //Recover/restart/retc.
    // else {
     //IMPORTANT: make sure readings is a uint16_t array of size 12, otherwise strange things may happen
    // as7341.getAllChannels(readings);  //Calling this any other time may give you old data
    // if (!as7341.readAllChannels(readings))
    //  Serial.println("AS7341 Error reading all channels!");
    // else
    // Serial.println(readings[10]);
    // averageValue = readings[10]; 
    // AutoGAIN();
   //  as7341.startReading();
     //   }
     //  }
     if(readingTimeOutCheck) {
       uint32_t sys_now_unixtime = time(NULL);
       AutoGAIN();
       // Serial.print("sqw wave fell: ");
      //  Serial.println(rtc_sqw_fell); //rtc_sqw_fell is called by isr_rtc_sqw() which is the RTC interrrupt
       int sqwstate = digitalRead(DL_PIN_RTC_SQUARE_WAVE);
      // Serial.println(sqwstate);
       // Serial.println(sys_now_unixtime); // set new sqw fell 
       if (millis() - readingStartTime >= (1010)) {rtc_sqw_fell = 1; Serial.println("using millis counter");}
     }
    
        if(rtc_sqw_fell) // this is the main action loop called on every falling square wave.
        {   
          ose_bundle vm_s = o.stack();
            
          if (ledstate = 1) 
            {toggleLED();}
          Serial.println("starting color read");
          //AutoGAIN(); // adjust gain to avoid saturation
          struct color color = getColor(); // actual sensor reading
          Serial.println("color read complete");
          uint32_t rtc_now_unixtime = dl_now_unixtime();
          uint32_t sys_now_unixtime = time(NULL);
          #ifdef RTC_MAX31343
           rtc.get_temp(rtc_temp);
          #else
          float rtc_temp = rtc.getTemperature();
          #endif
          //averageValue = color.values[10]; // vis light value for auto gain
         // AutoGAIN();  // autogain: still gets stuck on intense brightness changes that cause sensor saturation
         uint32_t ntp_sec_counter = (ntp_sec_counter + 1) % DL_NTP_UPDATE_PERIOD_SEC;
         int should_perform = sampleCounter == 0;
         sampleCounter = (sampleCounter + 1) % dataFrequency;
            Serial.printf("sampleCounter: %d, dataFrequency: %d, should_perform: %d\n",
                          sampleCounter, dataFrequency, should_perform);
         client.loop(); // check for any incoming message
         if(ntp_sec_counter == 0)
            {
                // A day has gone by, time to do an NTP update
                Serial.printf("NTP Sync\n");
                configTime(0, 0, dl_ntp_server1);
            }
         rtc_sqw_fell = 0; 
         readingStartTime = millis(); // resets SQW interrupt flag
            // Serial.printf("0x%x, 0x%x\n",
            //               dl_now_unixtime(),
            //               sys_now_unixtime);
         Serial.print("current gain: "); //prints current gain every second
         Serial.println(GAIN);

         if(should_perform) // this is transmit section that sends the current data to AWS. This happens based on the interval frequency
            {
                    // send data to AWS here
                    // the 12 individual values from the color sensor
                    // can be accessed with color.values[0], ...,
                    // color.values[11].
                    // makejson(jsonbuf, uuid, color, rtc_now_unixtime);

                    // first check to see if we are connected and then run the AWS reconnection function
                if (!client.connected()) {
                  long now = millis();
                  if (now - lastReconnectAttempt > 5000) {
                        lastReconnectAttempt = now;
                         // Attempt to reconnect using AWS connect function
                         if (reconnectAWS()) {
                             lastReconnectAttempt = 0;
                            }
                        }
                  } else
                   // sends data to AWS assuming AWS connection
                   {Serial.println("starting publication");  
                    char jsonbuf[2048];
                    formatJSON(color, rtc_now_unixtime, rtc_temp, uuid, jsonbuf, 2048);
                    publishMessage(jsonbuf); //sends to data topic as defined by getUUID
                    Serial.printf("data sent %d bytes:\n%s\n", strlen(jsonbuf), jsonbuf);
                    printDeviceTime(); // prints UTC time and temp to serial
                    }
                    toggleLED(); // data read and transmit complete. turn on LED
                
            
              if(oscsend)
                {
                    // if the Send OSC checkbox was ticked, and we have
                    // an address and a port, send a bundle to them.
                    // see microOSC library for details
                    
                    pushColorMsg(vm_s, color);
                    pushASTEPMsg(vm_s, ASTEP);
                    pushATIMEMsg(vm_s, ATIME);
                    pushGAINMsg(vm_s, GAIN);
                    pushTimeMsg(vm_s, rtc_now_unixtime, 0);
                    ose_bundleAll(vm_s);
                    o.udpSendElemTo(oscipaddr, oscport, vm_s);
                    ose_clear(o.stack());
                    Serial.println("sent OSC");
                }
            } // end of if should perform
        }
        if(o.serviceInterrupts())
        {
            o.eval();
            ose_clear(o.stack());
        }
    
        if(o.slipSerialRead())
        {
            o.eval();
            ose_clear(o.stack());
        }
        // HARDWARE SERIAL TIME SYNC TO SYNC DEVICES USING THE RX/TX PINS. ONLY FOR INITIAL CONFIG USE
        // also uncomment HWSERIAL
        // if(DL_HWSERIAL.available() > 0)
        // {
        //     Serial.printf("Reading from DL_HWSERIAL\n");
        //     if(havetime)
        //     {
        //         Serial.printf("havetime, setting clock to 0x%x\n",
        //                       unixtime);
        //         DateTime dt(unixtime + 1);
        //         rtc.adjust(dt);
        //         havetime = 0;
        //         unixtime = 0;
        //         memset(time_sync_bundle, 0, 16);
        //     }
        //     int i = 0;
        //     while(DL_HWSERIAL.available())
        //     {
        //         char c = DL_HWSERIAL.read();
        //         time_sync_bundle[i++] = c;
        //         if(i >= 16)
        //         {
        //             DL_HWSERIAL.read();
        //         }
        //     }
        //     if(!strcmp(time_sync_bundle, "#bundle\0"))
        //     {
        //         unixtime = ntohl(*((uint32_t *)(time_sync_bundle + 8)));
        //         havetime = 1;
            
        //     }
        // }
    
        if(o.udpRead())
        {
            Serial.printf("udp\n");
            o.eval();
            ose_clear(o.stack());
        }
// Serial.println("end of loop");
        yield();
    }
}
