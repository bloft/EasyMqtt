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

void WebPortal::setup(Entry *mqttEntry, Device *device, Config *config, NTPClient *ntpClient) {
  WebPortal::mqtt = mqttEntry;
  WebPortal::config = config;
  WebPortal::device = device;
  WebPortal::ntp = ntpClient;
  mqtt->debug("Setup Web Portal");
  webServer.reset(new ESP8266WebServer(80));
  webServer->on("/", std::bind(&WebPortal::handleRoot, this));
  webServer->on("/save", std::bind(&WebPortal::handleSaveConfig, this));
  mqtt->each([&](Entry* entry) {
    webServer->on(getRestPath(entry).c_str(), std::bind(&WebPortal::handleRest, this));
  });
  webServer->onNotFound(std::bind(&WebPortal::handleNotFound, this));
  webServer->begin();
}

void WebPortal::handleRoot() {
  if(!auth()) return;

  String page = FPSTR(HTML_MAIN1);
  page.replace("{device_name}", String(mqtt->get("$system")["name"].getValue()));
  page.replace("{device_id}", String(mqtt->get("$system")["deviceId"].getValue()));
  webServer->sendContent(page);

  mqtt->each([&](Entry* entry) {
    if(!entry->isInternal() || webServer->arg("show").equals("all")) {
      sendSensor(entry);
    }
  });

  webServer->sendContent_P(HTML_MAIN2);
  sendDevices();

  webServer->sendContent_P(HTML_MAIN3);
  
  sendConfigs();

  page = FPSTR(HTML_MAIN4);
  page.replace("{device_name}", String(mqtt->get("$system")["name"].getValue()));
  page.replace("{device_id}", String(mqtt->get("$system")["deviceId"].getValue()));
  page.replace("{topic}", mqtt->getTopic());
  webServer->sendContent(page);

  mqtt->each([&](Entry* entry) {
    if(entry->isOut() || entry->isIn()) {
      sendMqttApi(entry);
    }
  });

  webServer->sendContent_P(HTML_MAIN5);
  
  mqtt->each([&](Entry* entry) {
    if(entry->isOut() || entry->isIn()) {
      sendRestApi(entry);
    }
  });

  webServer->sendContent_P(HTML_MAIN6);

  webServer->client().stop();
}

void WebPortal::sendSensor(Entry* entry) {
  String page = FPSTR(HTML_SENSOR);
  page.replace("{color}", entry->isInternal() ? "warning" : "primary");
  page.replace("{name}", getName(entry));
  bool include = false;
  if(entry->isOut()) {
    include = true;
    page.replace("{input}", FPSTR(HTML_SENSOR_INPUT));
  } else {
    page.replace("{input}", "");
  }
  String value = entry->getValue();
  if(value != NULL) {
    include = true;
    page.replace("{output}", FPSTR(HTML_SENSOR_OUTPUT));
    if(getName(entry).endsWith("password")) {
      page.replace("{value}", "***");
    } else {
      page.replace("{value}", value);
    }
  } else {
    page.replace("{output}", "");
  }
  if(include) {
    page.replace("{path}", getRestPath(entry));
    page.replace("{last_updated}", time(entry->getLastUpdate()));
    webServer->sendContent(page);
  }
}

void WebPortal::sendDevices() {
  device->each([&](deviceElem* device) {
    String page = FPSTR(HTML_DEVICES);
    page.replace("{id}", String(device->deviceId));
    page.replace("{name}", String(device->name));
    page.replace("{online}", (device->online ? "<span class=\"label label-success\">ONLINE</span>" : "<span class=\"label label-danger\">OFFLINE</span>"));
    page.replace("{ip}", String(device->ip));
    page.replace("{lastUpdated}", time(device->lastUpdate));
    webServer->sendContent(page);
  });
}

void WebPortal::sendConfigs() {
  webServer->sendContent_P(HTML_CONFIG_HEADER);
  config->each([&](char *key, char *value) {
    String page = FPSTR(HTML_CONFIG_ENTRY);
    String name = String(key);
    page.replace("{key}", key);
    if(name.endsWith("password")) {
      page.replace("{type}", "password");
      page.replace("{value}", "");
    } else {
      page.replace("{type}", "text");
      page.replace("{value}", String(value));
    }
    webServer->sendContent(page);
  });
}

void WebPortal::sendMqttApi(Entry* entry) {
  String page = FPSTR(HTML_API_DOC);
  String path = entry->getTopic();
  if(entry->isOut()) path += "<span class=\"badge\">Set</span>";
  if(entry->isIn()) path += "<span class=\"badge\">Get</span>";
  page.replace("{path}", path);
  webServer->sendContent(page);
}

void WebPortal::sendRestApi(Entry* entry) {
  String page = FPSTR(HTML_API_DOC);
  String path = getRestPath(entry);
  if(entry->isOut()) path += "<span class=\"badge\">POST</span>";
  if(entry->isIn()) path += "<span class=\"badge\">GET</span>";
  page.replace("{path}", path);
  webServer->sendContent(page);
}

void WebPortal::handleRest() {
  if(!auth()) return;
  Entry* entry = &mqtt->get(webServer->uri().substring(6).c_str());
  if(webServer->method() == HTTP_GET) {
    webServer->send(200, "application/json", "{\"value\":\"" + String(entry->getValue()) + "\",\"updated\":\"" + time(entry->getLastUpdate()) + "\"}");
  } else if(webServer->method() == HTTP_POST && entry->isOut()) {
    entry->update(webServer->arg("plain"));
    webServer->send(200, "text/plain", webServer->uri() + " Update");
  } else {
    webServer->send(404, "text/plain", "Unsupported");
  }
}

void WebPortal::handleSaveConfig() {
  if(!auth()) return;
  config->each([&](char* key, char *value) {
    if(!(String(key)).endsWith("password") || strlen(webServer->arg(key).c_str()) > 0) {
      config->set(key, webServer->arg(key).c_str());
    } 
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
  char *pass = config->get("password", "");
  if (strlen(pass) > 0 && !webServer->authenticate("admin", pass)) {
    webServer->requestAuthentication();
    return false;
  }
  return true;
}
