#pragma once

#include <ESP8266WebServer.h>
#include "Entry.h"
#include "Config.h"
#include "Device.h"
#include "NTPClient.h"

// http://usemodj.com/2016/08/25/esp8266-arducam-5mp-ov5642-camera-wifi-video-streaming/

class WebPortal {
  private:
    std::unique_ptr<ESP8266WebServer> webServer;
    Entry* mqtt;
    Config* config;
    Device* device;
    NTPClient* ntp;

    String getName(Entry* entry);
    String getName(Entry* root, Entry* entry);

    String getRestPath(Entry* entry);

    String time(long time);
    bool auth();

    void sendSensor(Entry* entry);
    void sendDevices();
    void sendConfigs();
    void sendMqttApi(Entry* entry);
    void sendRestApi(Entry* entry);

    String toJson(Entry* entry);

  public:
    WebPortal();

    void setup(Entry *mqttEntry, Device *device, Config *config, NTPClient *ntp);

    void handleRoot();
    void handleRestAll();
    void handleRest();
    void handleSaveConfig();
    void handleNotFound();

    void loop();
};
