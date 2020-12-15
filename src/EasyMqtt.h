#pragma once

#include <ESP8266WiFi.h>
#include "Entry.h"
#include "Config.h"
#include "Device.h"
#include "WebPortal.h"
#include "NTPClient.h"
#include "Smartthings.h"

class EasyMqtt : public Entry {
  private:
    WiFiClient wifiClient;
    WebPortal webPortal;
    Smartthings *smartthingsHandler = NULL;

    Config* cfg;
    NTPClient* ntpClient;
    Device* deviceList;

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
    EasyMqtt();

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

    /**
       Handle the normal loop
    */
    void loop();
};
