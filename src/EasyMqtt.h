#ifndef EasyMqtt_h
#define EasyMqtt_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MqttEntry.h"
#include "WebPortal.h"
#include "html.h"

class EasyMqtt : public MqttEntry {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    WebPortal webPortal;

    String deviceId = "deviceId";
    const char* mqtt_username = "N/A";
    const char* mqtt_password = "N/A";

  protected:
    /**
       Handle connections to mqtt
    */
    void mqttReconnect() {
      while (!mqttClient.connected()) {
        if (mqttClient.connect(deviceId.c_str(), mqtt_username, mqtt_password)) {
          debug("Connected to MQTT");
          each([](MqttEntry* entry){
            entry->subscribe();
          });
        } else {
          #ifdef DEBUG
          Serial.print("failed, rc=");
          Serial.print(mqttClient.state());
          Serial.println(" try again in 5 seconds");
          #endif
          delay(5000);
        }
      }
    }

  public:
    EasyMqtt() : MqttEntry("easyMqtt", mqttClient) {
      deviceId = String(ESP.getChipId());

      get("system").setInterval(30);
      get("system")["mem"]["heap"] << []() {
        return String(ESP.getFreeHeap());
      };
      get("system")["uptime"] << []() {
        return String(millis() / 1000);
      };
      get("system")["reset"] >> [](String value) {
        ESP.reset();
      };
    }

    void debug(String msg) {
      #ifdef DEBUG
      Serial.println(msg);
      #endif
      get("system")["debug"].publish(msg);
    }

    void debug(String key, String value) {
      debug(key + " = " + value);
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
        #ifdef DEBUG
        Serial.print(".");
        #endif
        // ToDo: handle timeout, and create AP
      }
      #ifdef DEBUG
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());

      Serial.print("Chip ID : ");
      Serial.println(ESP.getChipId());
      #endif

      webPortal.setup(*this);

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
      
      #ifdef DEBUG
      Serial.print("Topic: ");
      Serial.println(getTopic());
      #endif
    }

    /**
       Handle the normal loop
    */
    void loop() {
      mqttReconnect();
      mqttClient.loop();
      webPortal.loop();
      each([](MqttEntry* entry){
          entry->loop();
      });
    }
};

#endif
