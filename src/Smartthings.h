#pragma once

#include <functional>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include <DNSServer.h>
#include "Config.h"
#include "Entry.h"


class Smartthings {
    private:
        std::unique_ptr<ESP8266WebServer> webServer;
        Entry* entries;
        Config* config;

        bool publishNow = false;
        

        String getPath(Entry* entry);

        String getName(Entry* root, Entry* entry);

        String getAsJson();

        bool sendState();

    public:
        Smartthings(String ns, String deviceHandler);
        void setup(Entry *entries, Config *config, String name);

        void publish();

        void handleState();
        void handleDescription();
        void handleSubscribe();
        void handleUpdate();
        void loop();
};