#ifndef EasyMqtt_h
#define EasyMqtt_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MqttMap.h"

class EasyMqtt : public MqttMap {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;

    String deviceId = "EasyMqtt";
    const char* mqtt_username = "N/A";
    const char* mqtt_password = "N/A";

  protected:
    virtual String getTopic() {
      return String("easyMqtt/") + deviceId;
    }

    MqttMap & getDiag() {
      return operator[]("diag");
    }

    /**
       Handle connections to mqtt
    */
    void mqttReconnect() {
      while (!mqttClient.connected()) {
        if (mqttClient.connect(deviceId.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("MQTT connected");
          subscribe(mqttClient);
          getDiag()["log"].publish(mqttClient, "connected to MQTT");
        } else {
          Serial.print("failed, rc=");
          Serial.print(mqttClient.state());
          Serial.println(" try again in 5 seconds");
          delay(5000);
        }
      }
    }

  public:
    EasyMqtt() : MqttMap("easyMqtt") {
      deviceId = String(ESP.getChipId());

      getDiag().setInterval(30);
      getDiag()["mem"]["heap"] << []() {
        return String(ESP.getFreeHeap());
      };
      getDiag()["uptime"] << []() {
        return String(millis());
      };
    }

    /**
       Setup connection to wifi
    */
    void wifi(const char* ssid, const char* password) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());

      Serial.print("Chip ID : ");
      Serial.println(ESP.getChipId());

      // Setup wifi diag
      getDiag()["wifi"]["rssi"] << []() {
        return String(WiFi.RSSI());
      };
      getDiag()["wifi"]["ssid"] << []() {
        return WiFi.SSID();
      };
    }

    /**
       Setup connection to mqtt
    */
    void mqtt(const char* host, int port, const char* username, const char* password) {
      mqttClient.setClient(wifiClient);
      mqttClient.setCallback([&](const char* topic, uint8_t* payload, unsigned int length) {
        callback(topic, payload, length);
      });
      mqttClient.setServer(host, port);
      mqtt_username = username;
      mqtt_password = password;
    }

    /**
       Handle the normal loop
    */
    void loop() {
      mqttReconnect();
      mqttClient.loop();
      MqttMap::loop(mqttClient);
    }
};

#endif
