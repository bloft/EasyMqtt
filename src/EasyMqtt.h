#pragma once

#include <ESP8266WiFi.h>
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

    String getDeviceId();
    
    virtual String getTopic();

    Config & config();

    NTPClient & ntp();

    String name();

    void name(const char* name);

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

    /**
       Handle the normal loop
    */
    void loop();
};
