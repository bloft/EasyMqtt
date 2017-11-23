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
    long mqttDelay = 0;

  protected:
    /**
       Handle connections to mqtt
    */
    void connectWiFi() {
      if(WiFi.status() == WL_DISCONNECTED) {
        debug("Connecting to wifi: " + config().getString("wifi.ssid", ""));
        WiFi.mode(WIFI_STA);

	String sSsid = config().getString("wifi.ssid", "");
	char ssid[sSsid.length() + 1];
	strcpy(ssid, sSsid.c_str());
        
        String name = config().getString("device.name", deviceId);
        //WiFi.hostname("WebPool"); // Set the name of the device

	String sPass = config().getString("wifi.password", "");
	char pass[sPass.length() + 1];
	strcpy(pass, sPass.c_str());

        WiFi.begin(ssid, pass);

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
          WiFi.softAP(config().getString("wifi.ap", "EasyMqtt").c_str(), "123456");
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
		
	String sHost = config().getString("mqtt.host", "");
	char host[sHost.length() + 1];
	strcpy(host, sHost.c_str());
	
	int port = config().getInt("mqtt.port", 1883);
	
	String sUser = config().getString("mqtt.username", "");
	char user[sUser.length() + 1];
	strcpy(user, sUser.c_str());
	
	String sPass = config().getString("mqtt.password", "");
	char pass[sPass.length() + 1];
	strcpy(pass, sPass.c_str());
	
        mqttClient.setServer(host, port);
        
        if (mqttClient.connect(deviceId.c_str(), user, pass)) {
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

      configEntry = new ConfigEntry(mqttClient, *this);
      addChild(configEntry);

      get("system").setInterval(30);
      get("system")["deviceId"] << [this]() {
        return deviceId;
      };
      get("system")["name"] << [this]() {
        return config().getString("device.name", deviceId);
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

    ConfigEntry & config() {
      return *configEntry;
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
      config().set("wifi.ssid", ssid);
      config().set("wifi.password", password);
    }

    /**
       Configure mqtt
       Deprecated
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
