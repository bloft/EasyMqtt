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

    ConfigEntry* configEntry;

    String deviceId = "deviceId";

  protected:
    /**
       Handle connections to mqtt
    */
    void connect() {
      if(WiFi.status() != WL_CONNECTED) {
        const char* ssid = config().getCString("wifi.ssid", "");
        const char* password = config().getCString("wifi.password", "");
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);

        int timer = 0;
        while (WiFi.status() != WL_CONNECTED && timer < 10) {
          delay(500);
          timer++;
          // ToDo: handle timeout, and create AP
        }
        if(timer < 10) {
          debug("WiFi connected");
        } else {
          debug("WiFi connection timeout");
          // ToDo: create AP
        }
        debug("IP address", WiFi.localIP().toString());
        debug("devideId", deviceId);
        webPortal.setup(*this);
      }
      if(mqttClient.state() == MQTT_DISCONNECTED) {
        // Setup MQTT
        const char* host = config().getCString("mqtt.host", "");
        int port = config().getInt("mqtt.port", 1883);
        mqttClient.setClient(wifiClient);
        mqttClient.setCallback([&](const char* topic, uint8_t* payload, unsigned int length) {
          each([=](MqttEntry* entry){
            entry->callback(topic, payload, length);
          });
        });
        mqttClient.setServer(host, port);
      }
      if (!mqttClient.connected()) {
        const char* username = config().getCString("mqtt.username", "");
        const char* password = config().getCString("mqtt.password", "");
        if (mqttClient.connect(deviceId.c_str(), username, password)) {
          debug("Connected to MQTT");
          debug("Topic", getTopic()); 
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
      deviceId = String(ESP.getChipId());

      debug("test");
      configEntry = new ConfigEntry(mqttClient, *this);
      addChild(configEntry);

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

      get("system")["restart"] >> [this](String value) {
        if(value == "restart") {
          debug("Restart");
          ESP.restart();
        }
      };
      get("system")["config"]["reset"] >> [this](String value) {
        if(value == "reset") {
          config().reset();
        }
      };
    }

    ConfigEntry & config() {
      return *configEntry;
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
      config().set("wifi.ssid", ssid);
      config().set("wifi.password", password);
    }

    /**
       Setup connection to mqtt
    */
    void mqtt(const char* host, int port, const char* username, const char* password) {
      config().set("mqtt.host", host);
      config().set("mqtt.port", String(port));
      config().set("mqtt.username", username);
      config().set("mqtt.password", password);
    }

    /**
       Handle the normal loop
    */
    void loop() {
      connect();
      mqttClient.loop();
      webPortal.loop();
      each([](MqttEntry* entry){
          entry->update();
      });
    }
};

#endif
