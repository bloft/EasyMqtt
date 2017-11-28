#ifndef EasyMqtt_h
#define EasyMqtt_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MqttMap.h"

class EasyMqtt : public MqttMap {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;

    String deviceId = "deviceId";

    const char* wifi_ssid = "N/A";
    const char* wifi_password = "N/A";

    const char* mqtt_host = "N/A";
    int         mqtt_port = 1883;
    const char* mqtt_username = "N/A";
    const char* mqtt_password = "N/A";

  protected:
    virtual String getTopic() {
      return String("easyMqtt/") + deviceId;
    }

    /**
       Handle connections to mqtt
    */
    void reconnect() {
      if(WiFi.status() == WL_DISCONNECTED) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(wifi_ssid, wifi_password);

        int timer = 0;
        while ((WiFi.status() == WL_DISCONNECTED) && timer < 60) {
          delay(500);
          #ifdef DEBUG
          Serial.print(".");
          #endif
          timer++;
        }
        if(WiFi.status() == WL_CONNECTED) {
          #ifdef DEBUG
          Serial.println("WiFi connected");
          Serial.print(" * IP address: ");
          Serial.println(WiFi.localIP());
          #endif
        } else {
          #ifdef DEBUG
          Serial.println("WiFi Unable to connect");
          #endif
          delay(5000); 
        }

      }
      if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
        mqttClient.setServer(host, port);
        if (mqttClient.connect(deviceId.c_str(), mqtt_username, mqtt_password)) {
          debug("Connected to MQTT");
          subscribe();
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
    EasyMqtt() : MqttMap("easyMqtt", mqttClient) {
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

    /**
       Setup connection to wifi
    */
    void wifi(const char* ssid, const char* password) {
      wifi_ssid = ssid;
      wifi_password = password;

      // Setup wifi diag
      get("system")["wifi"]["quality"] << []() {
        if(WiFi.RSSI() <= -100) {
          return String(0);
        } else if(WiFi.RSSI() >= -50) {{
          return String(100);
        } else {
          return String(2 * (WiFi.RSSI() + 100));
        }
      };
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
        callback(topic, payload, length);
      });
      
      mqtt_host = host;
      mqtt_port = port;
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
      reconnect();
      mqttClient.loop();
      MqttMap::loop();
    }
};

#endif
