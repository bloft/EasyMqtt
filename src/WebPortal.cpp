#include "WebPortal.h"
#include "html.h"

String WebPortal::getName(Entry* entry) {
  return getName(mqtt, entry);
}

String WebPortal::getName(Entry* root, Entry* entry) {
  String path = entry->getTopic();
  path.replace(root->getTopic(), "");
  return path;
}

String WebPortal::getRestPath(Entry* entry) {
  return "/rest" + getName(entry);
}

WebPortal::WebPortal() {
}

void WebPortal::setup(Entry& mqttEntry, ConfigEntry& config, NTPClient& ntpClient) {
  mqtt = &mqttEntry;
  ntp = &ntpClient;
  config = config;
  mqtt->debug("Setup Web Portal");
  webServer.reset(new ESP8266WebServer(80));
  webServer->on("/", std::bind(&WebPortal::handleRoot, this));
  webServer->on("/save", std::bind(&WebPortal::handleSaveConfig, this));
  mqtt->each([&](Entry* entry) {
      if(entry->isIn() || entry->isOut()) {
      webServer->on(getRestPath(entry).c_str(), std::bind(&WebPortal::handleRest, this));
      }
      });
  webServer->onNotFound(std::bind(&WebPortal::handleNotFound, this));
  webServer->begin();
}

void WebPortal::handleRoot() {
 if(!auth()) return;
  String page = "";
  page += FPSTR(HTML_MAIN1);
  // Sensors
  mqtt->each([&](Entry* entry) {
      if(!entry->isInternal() || webServer->arg("show").equals("all")) {
         String value = entry->getValue();
         if(value != NULL) {
            page += FPSTR(HTML_SENSOR);
            if(getName(entry).endsWith("password")) {
                page.replace("{value}", "***");
            } else {
                value.replace("{value}", entry->getValue());
                page.replace("{value}", value);
            }
            page.replace("{last_updated}", time(entry->getLastUpdate()));
         }
         if(entry->isOut()) {
            page += FPSTR(HTML_INPUT);
         }
         page.replace("{name}", getName(entry));
         page.replace("{path}", getRestPath(entry));
      }
  });

  // Config
  page += FPSTR(HTML_MAIN2);
  page += FPSTR(HTML_CONFIG_HEADER);
  page.replace("{title}", "General");
  config->each([&](Entry* entry) {
      if(entry == config) return;
      page += FPSTR(HTML_CONFIG_ENTRY);
      String name = getName(config, entry).substring(1);
      page.replace("{key}", name);
      if(name.endsWith("password")) {
          page.replace("{type}", "password");
          page.replace("{value}", "");
      } else {
          page.replace("{type}", "text");
          page.replace("{value}", entry->getValue());
      }
  });

  // About
  page += FPSTR(HTML_MAIN3);
  mqtt->each([&](Entry* entry) {
      if(entry->isOut() || entry->isIn()) {
          page += FPSTR(HTML_API_DOC);
          String path = entry->getTopic();
          if(entry->isOut()) path += "<span class=\"badge\">Set</span>";
          if(entry->isIn()) path += "<span class=\"badge\">Get</span>";
          page.replace("{path}", path);
      }
  });

  page += FPSTR(HTML_MAIN4);
  mqtt->each([&](Entry* entry) {
      if(entry->isOut() || entry->isIn()) {
          page += FPSTR(HTML_API_DOC);
          String path = getRestPath(entry);
          if(entry->isOut()) path += "<span class=\"badge\">POST</span>";
          if(entry->isIn()) path += "<span class=\"badge\">GET</span>";
          page.replace("{path}", path);
      }
  });

  page += FPSTR(HTML_MAIN5);
  page.replace("{device_id}", mqtt->get("$system")["deviceId"].getValue());
  page.replace("{topic}", mqtt->getTopic());
  webServer->send(200, "text/html", page);
}

void WebPortal::handleRest() {
  if(!auth()) return;
  Entry* entry = &mqtt->get(webServer->uri().substring(6).c_str());
  if(webServer->method() == HTTP_GET && entry->isIn()) {
    webServer->send(200, "application/json", "{\"value\":\"" + entry->getValue() + "\",\"updated\":\"" + time(entry->getLastUpdate()) + "\"}");
  } else if(webServer->method() == HTTP_POST && entry->isOut()) {
    entry->update(webServer->arg("plain"));
    webServer->send(200, "text/plain", webServer->uri() + " Update");
  } else {
    webServer->send(404, "text/plain", "Unsupported");
  }
}

void WebPortal::handleSaveConfig() {
  if(!auth()) return;
  config->each([&](Entry* entry) {
      String name = getName(entry);
      name = name.substring(9);
      name.replace("/", ".");
      entry->setValue(webServer->arg(name.c_str()));
      Serial.print(name);
      Serial.print(" = ");
      Serial.println(webServer->arg(name.c_str()));
      });
  config->save();
  webServer->sendHeader("Location", String("/"), true);
  webServer->send(302, "text/plain", "");
  ESP.restart();
}

void WebPortal::handleNotFound() {
  if(!auth()) return;
  webServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer->sendHeader("Pragma", "no-cache");
  webServer->sendHeader("Expires", "-1");
  webServer->send(404, "text/plain", "Not Found");
}

void WebPortal::loop() {
  webServer->handleClient();
}

String WebPortal::time(long time) {
  double utcOffset = config->getDouble("time.offset", 2);

  long localTime = round(ntp->getTime(time) + 3600 * utcOffset);

  int seconds = localTime % 60;
  localTime /= 60;
  int minutes = localTime % 60;
  localTime /= 60;
  int hours = localTime % 24;

  char formated[9];
  snprintf(formated, sizeof(formated), "%02d:%02d:%02d", hours, minutes, seconds);

  return String(formated);
}

bool WebPortal::auth() {
    char pass[32];
    config->getCString("web.password", "password", pass);
    if (!webServer->authenticate("admin", pass)) {
        webServer->requestAuthentication();
        return false;
    }
    return true;
}
