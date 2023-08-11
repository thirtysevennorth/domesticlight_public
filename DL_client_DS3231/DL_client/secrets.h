
// IT IS NOT USED OR CALLED DURING NORMAL USE
// PLACEHOLDERS ONLY ARE LEFT ON GITHUB.  

// #include <pgmspace.h>

// #define SECRET
// #define THING_NAME "000000"     // 6 digit serial number CHANGE THIS FOR EACH BOARD

// const char WIFI_SSID[] = "your ssid";           // place to include your setup wifi network // ian winters iphone
// const char WIFI_PASSWORD[] = "your password";           //  for regular use wifi is set via adhoc mode kgcfn1uuq1799

// const char AWS_IOT_ENDPOINT[] = "xxx-xx.xxx.xxxx.amazonaws.com";       //PROJECT AWS ENDPOINT. replace xxx with endppoint 
 
/// Amazon Root CA 1. DO NOT CHANGE. AWS FIXED ROOT CERTIFICATE.  
// static const char AWS_CERT_CA[] PROGMEM = R"EOF(
// -----BEGIN CERTIFICATE-----

//-----END CERTIFICATE-----
// )EOF";
 
// Device Certificate       
// CHANGE TO DEVICE SPECIFIC CERTIFICATE xxxx-certificate.pem.crt     
// static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
// -----BEGIN CERTIFICATE-----
//
//-----END CERTIFICATE-----
//)KEY";
 
// Device Private Key         
//CHANGE TO THE DEVICE SPECIFIC PRIVATE KEY  xxxx.private.pem.key
// static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
// -----BEGIN RSA PRIVATE KEY-----
// 
// -----END RSA PRIVATE KEY-----
// )KEY";