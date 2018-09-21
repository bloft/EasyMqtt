#pragma once

#include <ESP8266WiFi.h>

#define NTP_PACKET_SIZE 48

class NTPClient {
  private:
    float utcOffset = 0;
    long localEpoc = 0;
    long localMillisAtUpdate;
    const char* ntpServerName = "www.google.com";
    const int httpPort = 80;
    byte packetBuffer[ NTP_PACKET_SIZE];

  public:
    NTPClient(float utcOffset);
    void update();

    long getUtcTime();
    long getTime();

    String toString();

    String getHours();
    String getMinutes();
    String getSeconds();
};
