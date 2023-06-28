// secrets.h CONTAINS DEVICE CERTIFICATE INFO SET ON EACH DEVICE AT TIME OF "FACTORY" SETUP
// IT IS NOT USED, INCLUDED OR CALLED DURING NORMAL USE.

// ADD CERTIFICATE AND WIFI DETAILS IN LOCALLY NOT ON GITHUB

// NOTE TO FUTURE USERS: To repurpose the DL_Client for other uses outside of Domestic Light include "secrets.h" in the main sketch,
// then replace calls to prefs with calls to this secrets.file

// "FACTORY SETUP" USE NOTES
// Fill in the following info for the boards being flashed

// 1) AWS_IOT_ENDPOINT replacing the xxxx with the project AWS endpoint

// PER DEVICE CHANGE: 
// 2) THING_NAME (the DL UUID)
// 3) AWS_CERT_CRT (just the characters inside the certificate descript)
// 4) AWS_CERT_PRIVATE (just the characters inside the certificate descript)

// BE CERTAIN THAT THE CERTIFICATES MATCH THE THING_NAME / DL UUID AS THEY ARE KEYED TO THAT DEVICE.

// SEE README IN DL_client_INIT FOR MORE DETAILS on purpose of this "Factory setup" mode.

#include <pgmspace.h>

#define SECRET
#define THING_NAME "000000"     // 6 digit serial number CHANGE THIS FOR EACH BOARD. 

const char WIFI_SSID[] = "SSID";                   // place to include the setup wifi network
const char WIFI_PASSWORD[] = "PASSWORD";           // this info will be erased by user at user setup 

const char AWS_IOT_ENDPOINT[] = "ENDPOINT.com";       //PROJECT AWS ENDPOINT. 
 
/// Amazon Root CA 1. DO NOT CHANGE. AWS FIXED ROOT CERTIFICATE.  
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----

)EOF";
 
// Device Certificate       
// CHANGE TO DEVICE SPECIFIC CERTIFICATE xxxx-certificate.pem.crt     CHANGE THIS PER SENSOR
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----

)KEY";
 
// Device Private Key         
//CHANGE TO THE DEVICE SPECIFIC PRIVATE KEY  xxxx.private.pem.key  CHANGE THIS PER SENSOR
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";