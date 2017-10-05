#ifndef WebPortal_h
#define WebPortal_h

#include <ESP8266WebServer.h>
#include "EasyMqtt.h"
#include "html.h"

class WebPortal {
  private:
	std::unique_ptr<ESP8266WebServer> webServer;
  EasyMqtt* mqtt;

  public:
  WebPortal() {
  }

	void setup(EasyMqtt& mqtt) {
      webServer.reset(new ESP8266WebServer(80));
      webServer->on("/", std::bind(&WebPortal::handleRoot, this));
      /*
      mqtt->each([&](MqttEntry* entry) {
        String path = entry->getTopic();
        path.replace(mqtt->getTopic(), "");
        webServer->on(path, std::bind(&WebPortal::handleRest, this));
      }
      */
      webServer->onNotFound(std::bind(&WebPortal::handleNotFound, this));
      webServer->begin();
      WebPortal::mqtt = &mqtt;
	}

  void handleRoot() {
    String page = "";
    page += FPSTR(HTML_HEADER);

	  // Sensors
    mqtt->each([&](MqttEntry* entry) {
      if(entry->isIn()) {
        page += FPSTR(HTML_SENSOR);
        page.replace("{name}", entry->getTopic());
        page.replace("{value}", entry->getValue());
        page.replace("{last_updated}", new String(entry->getLastUpdate() / 1000));
      }
    });

    page += FPSTR(HTML_INPUT_PANEL);

	  // Inputs
    mqtt->each([&](MqttEntry* entry) {
      if(entry->isOut()) {
        page += FPSTR(HTML_INPUT);
        page.replace("{name}", entry->getTopic());
      }
    });

    page += FPSTR(HTML_FOOTER);
    page.replace("{device_id}", mqtt->getDeviceId());
    page.replace("{topic}", mqtt->getTopic());
    webServer->send(200, "text/html", page);
  }

  void handleRest() {
    if(webServer->method() == HTTP_GET) {
      webServer->send(200, "text/plain", "Unsupported");
    } else if(webServer->method() == HTTP_POST) {
      Serial.println(webServer.arg("plain"));
      webServer->send(200, "text/plain", "Unsupported");
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
