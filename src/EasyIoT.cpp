#include "EasyIoT.h"

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <FS.h>

/**
  * Handle connections to wifi
  */
void EasyIoT::connectWiFi() {
  if(WiFi.getMode() != WIFI_STA && wifiDelay < millis()) {
    disconnectWiFi();
    WiFi.mode(WIFI_STA);
  }
  if(WiFi.getMode() == WIFI_STA) {
    if(WiFi.status() != WL_CONNECTED) {
      if(wifiDelay == 0) {
        debug("Connecting to wifi");
        WiFi.begin(config().get("wifi.ssid", ""), config().get("wifi.password", ""));
        wifiDelay = millis() + 10000; // 10 sec
      } else if(wifiDelay < millis()) {
        debug("WiFi connection timeout - Setup AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("EasyIoT", "123456");
        debug("IP address", WiFi.softAPIP().toString());
        wifiDelay = millis() + 300000; // 5 min
      }
    } else if (wifiDelay != 0) {
      wifiDelay = 0;
      debug("WiFi connected");
      debug("IP address", WiFi.localIP().toString());
      debug("devideId", deviceId);

      if(smartthingsHandler) {
        smartthingsHandler->setup(this, &config(), getName());
      }

      if(mqttHandler) {
        mqttHandler->setup(this, &config(), &wifiClient);
      }

      webPortal.setup(this, NULL, &config(), &ntp());

      
    }
  }
}

void EasyIoT::disconnectWiFi() {
  if(WiFi.status() == WL_CONNECTED) {
    WiFi.softAPdisconnect();
    WiFi.disconnect();
    int timeout = 0;
    while(WiFi.status() != WL_DISCONNECTED && timeout < 200){
      delay(10);
      timeout++;
    }
    wifiDelay = 0;
  }
}

EasyIoT::EasyIoT() : Entry("easyMqtt") {
  // Add config entry
  cfg = new Config();
  cfg->load();

  // Add time(NTP)
  ntpClient = new NTPClient();

  deviceId = config().get("device.id", String(ESP.getChipId()).c_str());

  char * password = config().get("password", "");
  if(strlen(password) > 0) {
    ArduinoOTA.setPassword(password);
  }
  ArduinoOTA.setHostname(deviceId.c_str());
  ArduinoOTA.begin();

  setInterval(30, 10); // Check every 30 Sec and force update after 10 (5 min) unchanged

  get("$system").setInterval(300, 3); // every 5 min and force after 3 unchanged
  get("$system")["uptime"] << []() {
    return millis() / 1000;
  };
  get("$system")["deviceId"] << [this]() {
    return deviceId;
  };
  get("$system")["name"] << [this]() {
    return name();
  };
  get("$system")["wifi"]["rssi"] << []() {
    return WiFi.RSSI();
  };
  get("$system")["wifi"]["quality"] << []() {
    int quality = (WiFi.RSSI()+100)*1.6667;
    if(quality < 0) quality = 0;
    if(quality > 100) quality = 100;
    return quality;
  };
  get("$system")["wifi"]["ssid"] << []() {
    return WiFi.SSID();
  };
  get("$system")["wifi"]["ip"] << []() {
    return WiFi.localIP().toString();
  };
  get("$system")["wifi"]["mac"] << []() {
    return WiFi.macAddress();
  };

  get("$system")["mem"]["heap"] << []() {
    return ESP.getFreeHeap();
  };
  get("$system")["fs"]["usedBytes"] << []() {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    return fs_info.usedBytes;
  };
  get("$system")["fs"]["totalBytes"] << []() {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    return fs_info.totalBytes;
  };
  get("$system")["fs"]["usage"] << []() {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    return (fs_info.usedBytes / fs_info.totalBytes) * 100;
  };

  get("$system")["online"] << []() {
    return "ON";
  };

  get("$system")["time"].setInterval(900); // every 15 min
  get("$system")["time"] << [this]() {
    ntp().update();
    return ntp().getTime();
  };

  get("$system")["restart"] >> [this](String value) {
    if(strcmp(config().get("password", ""), value.c_str()) == 0) {
      debug("Restart");
      ESP.restart();
    }
  };

  // Auto discover
  get("$system")["openhab"] << [this]() {
    String json = "{"; 
    json += "id:'";
    json += getDeviceId();
    json += "',label:'";
    json += name();
    json += "',payloadAvailabl:'ON',payloadNotAvailable:'OFF',availabilityTopic:'";
    json += get("$system").get("online").getTopic();
    json += "',properties:{},channels:[";

    each([&](Entry* entry) {
      if(!entry->isInternal() && (entry->isIn() || entry->isOut()) && !entry->isSetter()) {
        String name = entry->getTopic();
        name.replace(getTopic(), "");
        String id = name;
        if(id.startsWith("/")) {
          id = id.substring(1);
        }
        id.replace("/", "-");
        
        json += "{type:'";
        json += toString(entry->getType());
        json += "',id:'";
        json += id;
        json += "',label:'";
        json += name;
        json += "'";

        if(entry->isIn()) {
          json += ",stateTopic:'";
          json += entry->getTopic();
          json += "'";
        }
        if(entry->isOut()) {
          json += ",commandTopic:'";
          json += entry->getTopic();
          json += "'";
        } else if (entry->contains("set") && entry->get("set").isSetter()) {
          json += ",commandTopic:'";
          json += entry->get("set").getTopic();
          json += "'";
        }

        json += "},";
      }
    });
    json += "]}";
    return json;
  };

  get("$system")["reset"] >> [this](String value) {
    if(strcmp(config().get("password", ""), value.c_str()) == 0) {
      reset();
    }
  };

  get("$system")["reset"]["reason"] << [this]() {
    return ESP.getResetReason();
  };

  setPublishFunction([&](Entry* entry, String message){
    publish(entry, message);
  });
}

String EasyIoT::getDeviceId() {
  return deviceId;
}

String EasyIoT::getTopic() {
  return String("easyMqtt/") + deviceId;
}

String EasyIoT::getName() {
  return String(deviceId);
}

Config & EasyIoT::config() {
  return *cfg;
}

NTPClient & EasyIoT::ntp() {
  return *ntpClient;
}

String EasyIoT::name() {
  return config().get("device.name", getDeviceId().c_str());
}

/**
  Configure name of device
  */
void EasyIoT::name(const char* name) {
  config().get("device.name", name);
}

void EasyIoT::reset() {
  config().reset();
  each([](Entry* entry){
    entry->reset();
  });
}

/**
  Configure wifi
  */
void EasyIoT::wifi(const char* ssid, const char* password) {
  config().get("wifi.ssid", ssid);
  config().get("wifi.password", password);
}

void EasyIoT::smartthings(String ns, String deviceHandler) {
  smartthingsHandler = new Smartthings(ns, deviceHandler);
}

void EasyIoT::mqtt() {
  mqttHandler = new Mqtt();
}

void EasyIoT::mqtt(const char* host, int port, const char* username, const char* password) {
  config().get("mqtt.host", host);
  config().get("mqtt.port", String(port).c_str());
  config().get("mqtt.username", username);
  config().get("mqtt.password", password);
  mqttHandler = new Mqtt();
}

void EasyIoT::publish(Entry* entry, String message) {
  debug("Publish", entry->getName());
  if(smartthingsHandler) {
    smartthingsHandler->publish();
  }
  if(mqttHandler) {
    mqttHandler->publish(entry, message);
  }
}

/**
  Handle the normal loop
  */
void EasyIoT::loop() {
  connectWiFi();
  if(WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
   
    webPortal.loop();

    if(smartthingsHandler) {
      smartthingsHandler->loop();
    }

    if(mqttHandler) {
      mqttHandler->loop();
    }

    each([](Entry* entry){
      entry->update();
    });
  }
}
