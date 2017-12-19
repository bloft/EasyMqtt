#ifndef WebPortal_h
#define WebPortal_h

#include <ESP8266WebServer.h>
#include "EasyMqtt.h"
#include "MqttEntry.h"
#include "html.h"

// http://usemodj.com/2016/08/25/esp8266-arducam-5mp-ov5642-camera-wifi-video-streaming/

class WebPortal {
  private:
    std::unique_ptr<ESP8266WebServer> webServer;
    MqttEntry* mqtt;

  String getName(MqttEntry* entry) {
    String path = entry->getTopic();
    path.replace(mqtt->getTopic(), "");
    return path;
  }

  String getRestPath(MqttEntry* entry) {
    return "/rest" + getName(entry);
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
          webServer->on(getRestPath(entry).c_str(), std::bind(&WebPortal::handleRest, this));
        }
      });
      webServer->onNotFound(std::bind(&WebPortal::handleNotFound, this));
      webServer->begin();
	}

  void handleRoot() {
    String page = "";
    page += FPSTR(HTML_MAIN1);
	  // Sensors
    mqtt->each([&](MqttEntry* entry) {
      if(!entry->isInternal()) {
        String value = entry->getValue();
        if(value != NULL) {
          value.toLowerCase();
          if(value == "on" || value == "open" || value == "true") {
            value = FPSTR(HTML_VALUE_ON);
          } else if (value == "off" || value == "closed" || value == "false") {
            value = FPSTR(HTML_VALUE_OFF);
          } else {
            value = "{value}";
          }
          value.replace("{value}", entry->getValue());

          page += FPSTR(HTML_SENSOR);
          page.replace("{name}", getName(entry));
          page.replace("{path}", getRestPath(entry));
          page.replace("{value}", value);
          page.replace("{last_updated}", String(entry->getLastUpdate() / 1000));
        }
      }
    });

    page += FPSTR(HTML_MAIN2);
	  // Inputs
    mqtt->each([&](MqttEntry* entry) {
      if(entry->isOut() && !entry->isInternal()) {
        page += FPSTR(HTML_INPUT);
        page.replace("{name}", getName(entry));
      }
    });

    page += FPSTR(HTML_MAIN3);
    mqtt->each([&](MqttEntry* entry) {
      if(entry->isOut() || entry->isIn()) {
        page += FPSTR(HTML_API_DOC);
        page.replace("{path}", entry->getTopic());
      }
    });

    page += FPSTR(HTML_MAIN4);
    mqtt->each([&](MqttEntry* entry) {
      if(entry->isOut() || entry->isIn()) {
        page += FPSTR(HTML_API_DOC);
        page.replace("{path}", getRestPath(entry));
      }
    });
    page += FPSTR(HTML_MAIN5);
    // About
    page.replace("{device_id}", mqtt->get("system/deviceId").getValue());
    page.replace("{topic}", mqtt->getTopic());
    webServer->send(200, "text/html", page);
  }

  void handleRest() {
    MqttEntry* entry = &mqtt->get(webServer->uri().substring(6).c_str());
    if(webServer->method() == HTTP_GET && entry->isIn()) {
      webServer->send(200, "text/plain", entry->getValue());
    } else if(webServer->method() == HTTP_POST && entry->isOut()) {
      entry->update(webServer->arg("plain"));
      webServer->send(200, "text/plain", webServer->uri() + " Update");
    } else {
      webServer->send(404, "text/plain", "Unsupported");
    }
  }

  void handleDebug() {
    WiFiClient client = webServer->client();
    webServer->sendContent("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    while(client.connected()) {
        client.write("this is a test\n");
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
