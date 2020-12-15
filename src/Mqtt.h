#pragma once

#include <functional>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#include "Entry.h"
#include "Config.h"
#include "Device.h"

class Mqtt {
    private:
        PubSubClient mqttClient;
        WiFiClient *wifiClient = NULL;
        Config *config;
        unsigned long mqttDelay = 0;
        Entry *entries;
        Device *deviceList;

        void connect();

    public:
        Mqtt();

        Device *getDeviceList();
        
        void setup(Entry *entries, Config *config, WiFiClient *wifiClient);

        void publish(Entry* entry, String message);

        void loop();
};