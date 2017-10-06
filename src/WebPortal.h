#ifndef WebPortal_h
#define WebPortal_h

#include <ESP8266WebServer.h>
#include "EasyMqtt.h"
#include "MqttEntry.h"
#include "html.h"

class WebPortal {
  private:
    std::unique_ptr<ESP8266WebServer> webServer;
    MqttEntry* mqtt;

  String getName(MqttEntry* entry) {
    String path = entry->getTopic();
    path.replace(mqtt->getTopic(), "");
    return path;
  }

  MqttEntry* getEntryByName(MqttEntry* entry, String name) {
    int pos = name.indexOf('/');
    if(pos < 0) {
      return &entry->get(name.c_str());
    } else {
      return getEntryByName(&entry->get(name.substring(0, pos).c_str()), name.substring(pos+1));
    }
  }

  public:
    WebPortal() {
    }

    void setup(MqttEntry& mqttEntry) {
      mqtt = &mqttEntry;
      #ifdef DEBUG
        Serial.println("Setup Web Portal");
      #endif
      webServer.reset(new ESP8266WebServer(80));
      webServer->on("/", std::bind(&WebPortal::handleRoot, this));
      mqtt->each([&](MqttEntry* entry) {
        if(entry->isIn() || entry->isOut()) {
          webServer->on(getName(entry).c_str(), std::bind(&WebPortal::handleRest, this));
        }
      });
      webServer->onNotFound(std::bind(&WebPortal::handleNotFound, this));
      webServer->begin();
	}

  void handleRoot() {
    String page = "";
    page += FPSTR(HTML_HEADER);

	  // Sensors
    mqtt->each([&](MqttEntry* entry) {
      if(entry->isIn()) {

        String value = entry->getValue().toLowerCase();
        if(value == "on" || value == "open" || value == "true") {
          value = FPSTR(HTML_VALUE_ON).replace("{value}", entry->getValue());
        } else if (value == "off" || value == "closed" || value == "false") {
          value = FPSTR(HTML_VALUE_OFF).replace("{value}", entry->getValue());
        } else {
          value = entry->getValue());
        }

        page += FPSTR(HTML_SENSOR);
        page.replace("{name}", getName(entry));
        page.replace("{value}", value);
        page.replace("{last_updated}", String(entry->getLastUpdate() / 1000));
      }
    });

    page += FPSTR(HTML_INPUT_PANEL);

	  // Inputs
    mqtt->each([&](MqttEntry* entry) {
      if(entry->isOut()) {
        page += FPSTR(HTML_INPUT);
        page.replace("{name}", getName(entry));
      }
    });

    page += FPSTR(HTML_FOOTER);
    //page.replace("{device_id}", mqtt->getDeviceId());
    page.replace("{topic}", mqtt->getTopic());
    webServer->send(200, "text/html", page);
  }

  void handleRest() {
    MqttEntry* entry = getEntryByName(mqtt, webServer->uri().substring(1));
    if(webServer->method() == HTTP_GET && entry->isIn()) {
      webServer->send(200, "text/plain", entry->getValue());
    } else if(webServer->method() == HTTP_POST && entry->isOut()) {
      entry->update(webServer->arg("plain"));
      webServer->send(200, "text/plain", webServer->uri() + " Update");
    } else {
      webServer->send(404, "text/plain", "Unsupported");
    }
  }

  void handleNotFound() {
    webServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    webServer->sendHeader("Pragma", "no-cache");
    webServer->sendHeader("Expires", "-1");
    webServer->send(404, "text/plain", "Not Found");
  }

  void loop() {
    webServer->handleClient();
  }
};

#endif
