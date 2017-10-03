#ifndef WebPortal_h
#define WebPortal_h

#include <ESP8266WebServer.h>
#include "MqttMap.h"
#include "heml.h"

class WebPortal {
  private:
	std::unique_ptr<ESP8266WebServer> webServer;

  public:
	void setup() {
	}

	void handleRoot() {
	}

	void handleNotFound() {
	}
}

#endif
