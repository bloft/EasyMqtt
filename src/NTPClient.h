#pragma once

#include <ESP8266WiFi.h>

#define NTP_PACKET_SIZE 48

class NTPClient {
  private:
    long localEpoc = 0;
    long localMillisAtUpdate;
    const char* ntpServerName = "www.google.com";
    const int httpPort = 80;
    byte packetBuffer[ NTP_PACKET_SIZE];

  public:
    NTPClient();
    void update();
    long getTime();
    long getTime(long millis);
};
