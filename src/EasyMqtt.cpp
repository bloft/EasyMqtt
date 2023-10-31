#include "EasyMqtt.h"

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <PubSubClient.h>
#include <SPIFFS.h>

/**
  * Handle connections to mqtt
  */
void EasyMqtt::connectWiFi() {
  if(WiFi.getMode() != WIFI_STA && wifiDelay < millis()) {
    disconnectWiFi();
    WiFi.mode(WIFI_STA);
  }
  if(WiFi.getMode() == WIFI_STA) {
    if(WiFi.status() != WL_CONNECTED) {
      if(wifiDelay == 0) {
        debug("Connecting to wifi");
        WiFi.setAutoReconnect(true);
        WiFi.begin(config().get("wifi.ssid", ""), config().get("wifi.password", ""));
        wifiDelay = millis() + 10000; // 10 sec
      } else if(wifiDelay < millis()) {
        debug("WiFi connection timeout - Setup AP");
        WiFi.mode(WIFI_AP);
        WiFi.softAP("EasyMqtt", "12345678");
        debug("IP address", WiFi.softAPIP().toString());
        wifiDelay = millis() + 300000; // 5 min
      }
    } else if (wifiDelay != 0) {
      wifiDelay = 0;
      debug("WiFi connected");
      debug("IP address", WiFi.localIP().toString());
      debug("devideId", deviceId);
      webPortal.setup(this, deviceList, &config(), &ntp());
    }
  }
}

void EasyMqtt::disconnectWiFi() {
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

void EasyMqtt::connectMqtt() {
  if (!mqttClient.connected() && mqttDelay < millis()) {
    debug("Connecting to MQTT");
    mqttClient.setClient(wifiClient);
    mqttClient.setCallback([&](const char* topic, uint8_t* payload, unsigned int length) {
      if(strncmp(topic, getTopic().c_str(), strlen(getTopic().c_str())) != 0) {
        deviceList->callback(topic, payload, length);
      }
      each([=](Entry* entry){
        entry->callback(topic, payload, length);
      });
    });

    mqttClient.setServer(config().get("mqtt.host", ""), config().getInt("mqtt.port", 1883));

    if (mqttClient.connect(deviceId.c_str(), config().get("mqtt.username", ""), config().get("mqtt.password", ""), get("$system")["online"].getTopic().c_str(), 1, 1, "OFF")) {
      debug("Connected to MQTT");

      setPublishFunction([&](Entry* entry, String message){
        if(mqttClient.connected()) {

          const char *msg = message.c_str();
          mqttClient.beginPublish(entry->getTopic().c_str(), strlen(msg), true);
          for (size_t i = 0; i < strlen(msg); i++){
            mqttClient.write(msg[i]);
          }
          mqttClient.endPublish();

          //mqttClient.publish(entry->getTopic().c_str(), message.c_str(), true);
        }
      });

      debug("Topic", getTopic());

      each([&](Entry* entry){
        if (entry->isOut()) {
          mqttClient.subscribe(entry->getTopic().c_str());
        }
      });

      // Device list
      deviceList->subscribe(&mqttClient);
      mqttDelay = 0;
    } else {
      debug("Connection to MQTT failed, rc", mqttClient.state());
      mqttDelay = millis() + 5000;
    }
  }
}

EasyMqtt::EasyMqtt() : Entry("easyMqtt") {
}

void EasyMqtt::init() {
  // Add config entry
  cfg = new Config();
  cfg->load();

  deviceList = new Device();

  // Add time(NTP)
  ntpClient = new NTPClient();

  
  #if defined(ESP8266)
    deviceId = config().get("device.id", String(ESP.getChipId()).c_str());
  #elif defined(ESP32)
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    deviceId = config().get("device.name", String(chipId).c_str());
  #endif

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

  /*

  get("$system")["fs"]["usedBytes"] << []() {
    return SPIFFS.usedBytes();
  };

  get("$system")["fs"]["totalBytes"] << []() {
    return SPIFFS.totalBytes();
  };

  get("$system")["fs"]["usage"] << []() {
    return (SPIFFS.usedBytes() / SPIFFS.totalBytes()) * 100;
  };

      */

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




#if defined(ESP8266)
  get("$system")["reset"]["reason"] << [this]() {
    return ESP.getResetReason();
  };
#endif

}

String EasyMqtt::getDeviceId() {
  return deviceId;
}

String EasyMqtt::getTopic() {
  return String("easyMqtt/") + deviceId;
}

Config & EasyMqtt::config() {
  return *cfg;
}

NTPClient & EasyMqtt::ntp() {
  return *ntpClient;
}

String EasyMqtt::name() {
  return config().get("device.name", getDeviceId().c_str());
}

/**
  Configure name of device
  */
void EasyMqtt::name(const char* name) {
  config().get("device.name", name);
}

void EasyMqtt::reset() {
  config().reset();
  each([](Entry* entry){
    entry->reset();
  });
}

/**
  Configure wifi
  */
void EasyMqtt::wifi(const char* ssid, const char* password) {
  config().set("wifi.ssid", ssid);
  config().set("wifi.password", password);
}

/**
  Configure mqtt
  */
void EasyMqtt::mqtt(const char* host, int port, const char* username, const char* password) {
  config().set("mqtt.host", host);
  config().set("mqtt.port", String(port).c_str());
  config().set("mqtt.username", username);
  config().set("mqtt.password", password);
}

void EasyMqtt::setDeviceName(const char* name) {
  config().set("device.name", name);
}

/**
  Handle the normal loop
  */
void EasyMqtt::loop() {
  connectWiFi();
  if(WiFi.status() == WL_CONNECTED) {
    connectMqtt();
    if(mqttClient.connected()) {
      mqttClient.loop();
    }
    webPortal.loop();
    each([](Entry* entry){
      entry->update();
    });
  }
}
