#ifndef WebPortal_h
#define WebPortal_h

#include <ESP8266WebServer.h>
#include "MqttEntry.h"
#include "html.h"

class WebPortal {
  private:
	std::unique_ptr<ESP8266WebServer> webServer;
  MqttEntry* mqttEntry;

  public:
  WebPortal() {
  }

	void setup(MqttEntry& mqttEntry) {
      webServer.reset(new ESP8266WebServer(80));
      webServer->on("/", std::bind(&WebPortal::handleWebRoot, this));
      webServer->onNotFound(std::bind(&WebPortal::handleNotFound, this));
      webServer->begin();
      WebPortal::mqttEntry = &mqttEntry;
	}

  void handleWebRoot() {
    String page = "";
    page += FPSTR(HTML_HEADER);

	  // Sensors
    mqttEntry->each([&page](MqttEntry* entry){
      if(entry->isOut()) {
        page += FPSTR(HTML_SENSOR);
        page.replace("{name}", entry->getTopic());
        page.replace("{value}", entry->getValue());
        page.replace("{last_updated}", "123");
      }
    });

    page += FPSTR(HTML_INPUT_PANEL);

	  // Inputs
    mqttEntry->each([&page](MqttEntry* entry){
      if(entry->isIn()) {
        page += FPSTR(HTML_INPUT);
        page.replace("{name}", entry->getTopic());
      }
    });

    page += FPSTR(HTML_FOOTER);
    page.replace("{topic}", mqttEntry->getTopic());
    webServer->send(200, "text/html", page);
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
