#pragma once

#if defined(ESP8266)
   #include <ESP8266WiFi.h>
#elif defined(ESP32)
   #include <WiFi.h>
#endif

#include <PubSubClient.h>
#include "Entry.h"
#include "Config.h"
#include "Device.h"
#include "WebPortal.h"
#include "NTPClient.h"

class EasyMqtt : public Entry {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    WebPortal webPortal;

    Config* cfg;
    NTPClient* ntpClient;
    Device* deviceList;

    String deviceId = "deviceId";
    unsigned long mqttDelay = 0;
    unsigned long wifiDelay = 0;

  protected:
    /**
       Handle connections to wifi
    */
    void connectWiFi();
    void disconnectWiFi();

    /**
       Handle connections to mqtt
    */
    void connectMqtt();

  public:
    EasyMqtt();

    void init();

    String getDeviceId();
    
    virtual String getTopic();

    Config & config();

    NTPClient & ntp();

    String name();

    void name(const char* name);

   /*
   * Reset all stored values
   */
   void reset();

    /**
       Configure wifi
       Deprecated
    */
    void wifi(const char* ssid, const char* password);

    /**
       Configure mqtt
       Deprecated
    */
    void mqtt(const char* host, int port, const char* username, const char* password);

    void setDeviceName(const char* name);

    /**
       Handle the normal loop
    */
    void loop();
};
