#if !defined(ESP8266)
#error This file is for ESP8266 only
#endif

#define COMPDATE __DATE__ __TIME__
#define APPNAME "Hackaday.io api"
#define VERSION "V0.0.1"

#include <Arduino.h>
#include <hackadayio_api.h>
#include <ESP8266WiFi.h>      // Arduino library - https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h
#include <WiFiClientSecure.h> // Arduino library - https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiClientSecure.h
#include <time.h>             // Arduino library - Secure connections need the right time to know validity of certificate.

#include "ca_certificate.h"
#include "secrets.h" // Uncomment this line and add secrets.h to your project
#ifndef SECRETS_H
#define SECRETS_H
const char WIFI_SSID[] = "ssid";
const char WIFI_PASSWORD[] = "password";
#endif

// ***********************************  CONSTANTS  *****************************

// time
static const int TIME_TIMEZONE_NL = 3600;
static const int TIME_DAYLIGHTOFFSET_SEC_NL = 3600;
static const char TIME_NTPSERVER_1[] = "nl.pool.ntp.org";
static const char TIME_NTPSERVER_2[] = "pool.ntp.org";

// site
const char HACKADAYIO_HOST[] = "api.hackaday.io";
const uint16_t HACKADAYIO_PORT = 443;

// ***********************************  VARIABLES  *****************************

int minutes_startloop;

// ***********************************  FUNCTIONS  *****************************

// ***********************************  WIFI  **********************************

// More events: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h

void onSTAConnected(WiFiEventStationModeConnected e /*String ssid, uint8 bssid[6], uint8 channel*/)
{
  Serial.printf("WiFi Connected: SSID %s @ BSSID %.2X:%.2X:%.2X:%.2X:%.2X:%.2X Channel %d\n",
                e.ssid.c_str(), e.bssid[0], e.bssid[1], e.bssid[2], e.bssid[3], e.bssid[4], e.bssid[5], e.channel);
  Serial.print("Get IP from ");
  Serial.println(e.ssid.c_str());
}

void onSTADisconnected(WiFiEventStationModeDisconnected e /*String ssid, uint8 bssid[6], WiFiDisconnectReason reason*/)
{
  Serial.printf("WiFi Disconnected: SSID %s BSSID %.2X:%.2X:%.2X:%.2X:%.2X:%.2X Reason %d\n",
                e.ssid.c_str(), e.bssid[0], e.bssid[1], e.bssid[2], e.bssid[3], e.bssid[4], e.bssid[5], e.reason);
  // Reason: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiType.h
  Serial.print("ERROR ");
  Serial.println(e.ssid.c_str());
}

void onSTAGotIP(WiFiEventStationModeGotIP e /*IPAddress ip, IPAddress mask, IPAddress gw*/)
{
  Serial.printf("WiFi GotIP: localIP %s SubnetMask %s GatewayIP %s\n",
                e.ip.toString().c_str(), e.mask.toString().c_str(), e.gw.toString().c_str());
  Serial.print("Got IP");
  Serial.println(e.ip.toString().c_str());
}

// ***********************************  SETUP  *********************************

void setup()
{

  Serial.begin(115200);
  Serial.println();

  // WiFi
  static WiFiEventHandler e1, e2, e4;
  // WiFi events
  e1 = WiFi.onStationModeConnected(onSTAConnected);
  e2 = WiFi.onStationModeDisconnected(onSTADisconnected);
  e4 = WiFi.onStationModeGotIP(onSTAGotIP);
  // WiFi start
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(false);  // do not automatically connect on power on to the last used access point
  WiFi.setAutoReconnect(true); // attempt to reconnect to an access point in case it is disconnected
  WiFi.persistent(false);      // Store no SSID/PASSWORD in flash
  WiFi.begin(WIFI_SSID2, WIFI_PASSWORD2);

  // timezone CET = 3600, daylight CET->CEST = 3600
  // void configTime(int timezone, int daylightOffset_sec, const char* server1, const char* server2, const char* server3)
  configTime(TIME_TIMEZONE_NL, TIME_DAYLIGHTOFFSET_SEC_NL, TIME_NTPSERVER_1, TIME_NTPSERVER_2);

  minutes_startloop = 99;
}

// ********************  LOOP  ********************

void loop()
{
  // get time
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  // time set and not same <hour> as last time
  if ((now > 24 * 3600) & (minutes_startloop != timeinfo.tm_hour))
  {
    minutes_startloop = timeinfo.tm_hour;

    BearSSL::WiFiClientSecure client;
    HackadayioApi api(client, HACKADAYIO_API_KEY);
    HackadayioApi::HProject hproject = api.GetProject(163680);
    if (!hproject.api_error) {
      Serial.printf("description: %s\n", hproject.description);
      Serial.printf("views: %d\n", hproject.views);
    } else {
      Serial.printf("Error : %d\n", hproject.api_error);
    }

  }
}