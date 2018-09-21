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

void WebPortal::setup(Entry& mqttEntry, ConfigEntry& config) {
  mqtt = &mqttEntry;
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
  String page = "";
  page += FPSTR(HTML_MAIN1);
  // Sensors
  mqtt->each([&](Entry* entry) {
      if(!entry->isInternal()) {
      String value = entry->getValue();
      if(value != NULL) {
      value.replace("{value}", entry->getValue());

      page += FPSTR(HTML_SENSOR);
      page.replace("{value}", value);
      page.replace("{last_updated}", String(entry->getLastUpdate() / 1000));
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
  Entry* config = &mqtt->get("$config");
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
  Entry* entry = &mqtt->get(webServer->uri().substring(6).c_str());
  if(webServer->method() == HTTP_GET && entry->isIn()) {
    webServer->send(200, "text/plain", entry->getValue());
  } else if(webServer->method() == HTTP_POST && entry->isOut()) {
    entry->update(webServer->arg("plain"));
    webServer->send(200, "text/plain", webServer->uri() + " Update");
  } else {
    webServer->send(404, "text/plain", "Unsupported");
  }
}

void WebPortal::handleSaveConfig() {
  Serial.println("Save");
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
  webServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  webServer->sendHeader("Pragma", "no-cache");
  webServer->sendHeader("Expires", "-1");
  webServer->send(404, "text/plain", "Not Found");
}

void WebPortal::loop() {
  webServer->handleClient();
}

String WebPortal::time(long time, float utcOffset) {
  long localTime = round(time + 3600 * utcOffset + 86400L) % 86400L;
  int hours = ((localTime  % 86400L) / 3600) % 24;
  int minutes = ((localTime % 3600) / 60);
  int seconds = localTime % 60;
  
  String output = "";
  if(hours < 10) output += "0";
  output += hours + ":";
  if(minutes < 10) output += "0";
  output += minutes + ":";
  if(seconds < 10) output += "0";
  output += seconds;
  return output;
}
