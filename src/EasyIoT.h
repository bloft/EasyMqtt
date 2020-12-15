#pragma once

#include <ESP8266WiFi.h>
#include "Entry.h"
#include "Config.h"
#include "WebPortal.h"
#include "NTPClient.h"
#include "Smartthings.h"
#include "Mqtt.h"

class EasyIoT : public Entry {
  private:
    WiFiClient wifiClient;
    WebPortal webPortal;
    Smartthings *smartthingsHandler = NULL;
    Mqtt *mqttHandler = NULL;

    Config* cfg;
    NTPClient* ntpClient;

    String deviceId = "deviceId";
    unsigned long wifiDelay = 0;

  protected:
    /**
       Handle connections to wifi
    */
    void connectWiFi();
    void disconnectWiFi();

    void publish(Entry* entry, String message);

  public:
    EasyIoT();

    String getDeviceId();
    
    virtual String getTopic();

    virtual String getName();

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

    void smartthings(String ns, String deviceHandler);

    void mqtt();
    void mqtt(const char* host, int port, const char* username, const char* password);

    /**
       Handle the normal loop
    */
    void loop();
};
