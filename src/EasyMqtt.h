#ifndef EasyMqtt_h
#define EasyMqtt_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Entry.h"
#include "ConfigEntry.h"
#include "WebPortal.h"
#include "NTPClient.h"

class EasyMqtt : public Entry {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    WebPortal webPortal;

    ConfigEntry* configEntry;

    NTPClient* ntpClient;

    String deviceId = "deviceId";
    long mqttDelay = 0;

    const char* wifi_ssid = "N/A";
    const char* wifi_password = "N/A";

    const char* mqtt_host = "N/A";
    int         mqtt_port = 1883;
    const char* mqtt_username = "N/A";
    const char* mqtt_password = "N/A";

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

    ConfigEntry & config();

    NTPClient & ntp();

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

#endif
