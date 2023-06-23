// secrets.h CONTAINS DEVICE CERTIFICATE INFO SET ON EACH DEVICE AT TIME OF "FACTORY" SETUP
// IT IS NOT USED, INCLUDED OR CALLED DURING NORMAL USE.

// ADD CERTIFICATE AND WIF DETAILS IN LOCALLY NOT ON GITHUB

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
#define THING_NAME "000000"     // 6 digit serial number CHANGE THIS FOR EACH BOARD

const char WIFI_SSID[] = "YOUR_SSID";           //place to include the setup wifi network
const char WIFI_PASSWORD[] = "YOUR_PASSWORD";           // this info will be erased by user at user setup 

const char AWS_IOT_ENDPOINT[] = "xxx.xxx.xxx.xxx";       //PROJECT AWS ENDPOINT. 
 
/// Amazon Root CA 1. DO NOT CHANGE. AWS FIXED ROOT CERTIFICATE.  
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUGD6rO5Wi6mqAFc3+Eb1nW84jj7MwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDYxNTIzNTc1
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANAxWsEj1xQJ7qtVLJFH
RbGzBPT2seVrOUVQae5Y372nnn/XgpBMWQi9yECu/SnWRfIEMWkKnfOeNEilPnOx
hpfj14a0rK8jKEf4aB7YQEEQqtPpvIgn/ZYZhnoGg73B/ayqKz90B+nKj4VaJz3R
DuOi+8ZSm2/qbDPFSoMCtBBrj+6lRbSXKSrCHbmG85IsT1uE+/qjjUoB3BTNVZ6i
gugJ9IVODDcAUAcP7QL9Tpqng20lhfEWep9P/Y/o76x69ZMjXSEDpmHqgohGvQWC
KqeXm1ufbS6sAmpT1Tl2NBvFLUFL7+sm/4BJirYnLqhvzwy5zW1qbZ9WR3IQqIkO
SJMCAwEAAaNgMF4wHwYDVR0jBBgwFoAUKUOT8LQk9eReUKTj0nnLsH4Ziy0wHQYD
VR0OBBYEFE0sDxhUe55X8Y3hG/IdTSzDwYvHMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQChjK3iqsciWEjWwBXEVxOMLOiL
MJqiYLoy7LZkuHx7dC+YXUZetQhbfp39LBCYCV8q9x4/e2PCvNLEPmSV6O8PVhds
CVfsOlq1j1441diY1RuawAH+uEDjQggO1QGyyTYLTTwjvwI9/aBEBYJmi78MRdje
JqTD6oQYxht1jyEr5nwpqix80fx6YSW/a63gJdpLV9agelU6wKPyRMvlTD9pVDi/
tTHSOyf0dMm7BV/qYJeDavM9YFHOODmnxNKKm703eUGE5l5a48Gu+wAqvHiKqVOO
j76LmJVX8p43rNNI3tew8VMPeOkzlkTmzzFbxiepqw6lw1Gg6lacaPHfo1cD
-----END CERTIFICATE-----

)EOF";
 
// Device Certificate       
// CHANGE TO DEVICE SPECIFIC CERTIFICATE xxxx-certificate.pem.crt     
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----

)KEY";
 
// Device Private Key         
//CHANGE TO THE DEVICE SPECIFIC PRIVATE KEY  xxxx.private.pem.key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----

)KEY";