#ifndef EasyMqtt_h
#define EasyMqtt_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MqttEntry.h"
#include "WebPortal.h"
#include "ConfigEntry.h"

class EasyMqtt : public MqttEntry {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    WebPortal webPortal;

    ConfigEntry* config;

    String deviceId = "deviceId";
    const char* mqtt_username = "N/A";
    const char* mqtt_password = "N/A";

  protected:
    /**
       Handle connections to mqtt
    */
    void mqttReconnect() {
     // if(WiFi.status() != WL_CONNECTED) {
      while (!mqttClient.connected()) {
        if (mqttClient.connect(deviceId.c_str(), mqtt_username, mqtt_password)) {
          debug("Connected to MQTT");
          each([&](MqttEntry* entry){
            if (entry->isOut()) {
              mqttClient.subscribe(entry->getTopic().c_str());
            }
          });
        } else {
          debug("failed, rc", String(mqttClient.state()));
          delay(5000);
        }
      }
    }

  public:
    EasyMqtt() : MqttEntry("easyMqtt", mqttClient) {
      Serial.begin(115200);
      deviceId = String(ESP.getChipId());

      debug("test");
      config = new ConfigEntry(mqttClient, *this);
      addChild(config);

      get("system").setInterval(30);
      get("system")["deviceId"] << [this]() {
        return deviceId;
      };
      get("system")["mem"]["heap"] << []() {
        return String(ESP.getFreeHeap());
      };
      get("system")["uptime"] << []() {
        return String(millis() / 1000);
      };
      get("system")["restart"] >> [this](String value) {
        if(value == "restart") {
          debug("Restart");
          ESP.restart();
        }
      };
      get("system")["config"]["reset"] >> [this](String value) {
        if(value == "reset") {
          getConfig()->reset();
        }
      };
    }

    ConfigEntry* getConfig() {
      return config;
    }

    void debug(String msg) {
      #ifdef DEBUG
      Serial.println(msg);
      #endif
      if(mqttClient.connected()) {
        get("system")["debug"].publish(msg);
      }
    }

    void debug(String key, String value) {
      debug(key + " = " + value);
    }

    String getDeviceId() {
      return deviceId;
    }
    
    virtual String getTopic() {
      return String("easyMqtt/") + deviceId;
    }

    /**
       Setup connection to wifi
    */
    void wifi(const char* ssid, const char* password) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        // ToDo: handle timeout, and create AP
      }
      debug("WiFi connected");
      debug("IP address", WiFi.localIP().toString());
      debug("devideId", deviceId);

      // Setup wifi diag
      get("system")["wifi"]["rssi"] << []() {
        return String(WiFi.RSSI());
      };
      get("system")["wifi"]["ssid"] << []() {
        return WiFi.SSID();
      };
      get("system")["wifi"]["ip"] << []() {
        return WiFi.localIP().toString();
      };

      webPortal.setup(*this);
    }

    /**
       Setup connection to mqtt
    */
    void mqtt(const char* host, int port, const char* username, const char* password) {
      mqttClient.setClient(wifiClient);
      mqttClient.setCallback([&](const char* topic, uint8_t* payload, unsigned int length) {
        each([=](MqttEntry* entry){
          entry->callback(topic, payload, length);
        });
      });
      mqttClient.setServer(host, port);
      mqtt_username = username;
      mqtt_password = password;
     
      debug("Topic", getTopic()); 
    }

    /**
       Handle the normal loop
    */
    void loop() {
      mqttReconnect();
      mqttClient.loop();
      webPortal.loop();
      each([](MqttEntry* entry){
          entry->update();
      });
    }
};

#endif
