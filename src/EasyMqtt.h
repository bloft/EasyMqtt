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
        debug("Connecting to wifi: " + config().getString("wifi.ssid", ""));
        WiFi.mode(WIFI_STA);
        WiFi.begin(config().getCString("wifi.ssid", ""), config().getCString("wifi.password", ""));

        int timer = 0;
        while ((WiFi.status() != WL_CONNECTED) && timer < 10) {
          delay(500);
          timer++;
          // ToDo: handle timeout, and create AP
        }
        if(timer < 10) {
          debug("WiFi connected");
        } else {
          debug("WiFi connection timeout - Setup AP");
          // ToDo: create AP
          WiFi.mode(WIFI_AP);
          WiFi.softAP(config().getCString("wifi.ap", "EasyMqtt"), "123456");
        }
        debug("IP address", WiFi.localIP().toString());
        debug("devideId", deviceId);
        webPortal.setup(*this);
      }
      if(mqttClient.state() == MQTT_DISCONNECTED) {
        // Setup MQTT
        mqttClient.setClient(wifiClient);
        mqttClient.setCallback([&](const char* topic, uint8_t* payload, unsigned int length) {
          each([=](MqttEntry* entry){
            entry->callback(topic, payload, length);
          });
        });
        mqttClient.setServer(config().getCString("mqtt.host", ""), config().getInt("mqtt.port", 1883));
      }
      if (!mqttClient.connected()) {
        debug("Connecting to MQTT");
        if (mqttClient.connect(deviceId.c_str(), config().getCString("mqtt.username", ""), config().getCString("mqtt.password", ""))) {
          debug("Connected to MQTT");
    
          setPublishFunction([&](MqttEntry* entry, String message){
            mqttClient.publish(entry->getTopic().c_str(), message.c_str());
          });

          debug("Topic", getTopic()); 
          each([&](MqttEntry* entry){
            if (entry->isOut()) {
              mqttClient.subscribe(entry->getTopic().c_str());
            }
          });
        } else {
          debug("Connection to MQTT failed, rc", String(mqttClient.state()));
          delay(500);
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
