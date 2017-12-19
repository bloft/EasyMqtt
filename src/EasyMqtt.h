#ifndef EasyMqtt_h
#define EasyMqtt_h

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MqttEntry.h"
#include "WebPortal.h"

class EasyMqtt : public MqttEntry {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    WebPortal webPortal;

    String deviceId = "deviceId";
    long mqttDelay = 0;

		const char* wifi_ssid = "N/A";
    const char* wifi_password = "N/A";

    const char* mqtt_host = "N/A";
    int         mqtt_port = 1883;
    const char* mqtt_username = "N/A";
    const char* mqtt_password = "N/A";

  protected:
    /**
       Handle connections to mqtt
    */
    void connectWiFi() {
      if(WiFi.status() == WL_DISCONNECTED) {
        debug("Connecting to wifi");
        WiFi.mode(WIFI_STA);

        WiFi.begin(wifi_ssid, wifi_password);

        #ifdef DEBUG
        WiFi.printDiag(Serial);
        #endif

        int timer = 0;
        while ((WiFi.status() == WL_DISCONNECTED) && timer < 60) {
          debug("Wifi Status", String(WiFi.status()));
          delay(500);
          timer++;
        }
        if(timer < 60 && WiFi.status() == WL_CONNECTED) {
          debug("WiFi connected");
          debug("IP address", WiFi.localIP().toString());
        } else {
          debug("WiFi connection timeout - Setup AP");
          WiFi.mode(WIFI_AP);
          WiFi.softAP("EasyMqtt", "123456");
          debug("IP address", WiFi.softAPIP().toString());
        }
        debug("devideId", deviceId);
        webPortal.setup(*this);
      }
    }

    void connectMqtt() {
      if (!mqttClient.connected() && mqttDelay < millis()) {
        debug("Connecting to MQTT");
        mqttClient.setClient(wifiClient);
        mqttClient.setCallback([&](const char* topic, uint8_t* payload, unsigned int length) {
          each([=](MqttEntry* entry){
            entry->callback(topic, payload, length);
          });
        });
		
        mqttClient.setServer(mqtt_host, mqtt_port);
        
        if (mqttClient.connect(deviceId.c_str(), mqtt_username, mqtt_password)) {
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
          mqttDelay = 0;
        } else {
          debug("Connection to MQTT failed, rc", String(mqttClient.state()));

          mqttDelay = millis() + 5000;
        }
      }
    }

  public:
    EasyMqtt() : MqttEntry("easyMqtt", mqttClient) {
        #ifdef DEBUG
	Serial.begin(115200);
        #endif
      deviceId = String(ESP.getChipId());

      setInterval(60);
      setForce(10);

      get("system").setInterval(300);
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
      get("system")["wifi"]["quality"] << []() {
        int rssi = WiFi.RSSI();
        int quality = 0;
        if(rssi <= -100) {
        } else if (rssi >= -50) {
          quality = 100;
        } else {
          quality = 2 * (rssi + 100);
        }
        return String(quality);
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
    }

    void debug(String msg) {
      #ifdef DEBUG
      Serial.println(msg);
      #endif
      if(mqttClient.connected()) {
        get("system/debug").publish(msg);
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
       Configure wifi
       Deprecated
    */
    void wifi(const char* ssid, const char* password) {
			wifi_ssid = ssid;
			wifi_password = password;
    }

    /**
       Configure mqtt
       Deprecated
    */
    void mqtt(const char* host, int port, const char* username, const char* password) {
			mqtt_host = host;
			mqtt_port = port;
			mqtt_username = username;
			mqtt_password = password;
    }

    /**
       Handle the normal loop
    */
    void loop() {
      connectWiFi();
      connectMqtt();
      mqttClient.loop();
      webPortal.loop();
      each([](MqttEntry* entry){
          entry->update();
      });
    }
};

#endif
