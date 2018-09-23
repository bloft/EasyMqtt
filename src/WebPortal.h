#pragma once

#include <ESP8266WebServer.h>
#include "Entry.h"
#include "ConfigEntry.h"

// http://usemodj.com/2016/08/25/esp8266-arducam-5mp-ov5642-camera-wifi-video-streaming/

class WebPortal {
  private:
    std::unique_ptr<ESP8266WebServer> webServer;
    Entry* mqtt;
    ConfigEntry* config;

    String getName(Entry* entry);
    String getName(Entry* root, Entry* entry);

    String getRestPath(Entry* entry);

    String time(long time, float utcOffset);

  public:
    WebPortal();

    void setup(Entry& mqttEntry, ConfigEntry& config);

    void handleRoot();
    void handleRest();
    void handleSaveConfig();
    void handleNotFound();

    void loop();
};
