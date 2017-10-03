#ifndef EasyMqtt_h
#define EasyMqtt_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include "MqttMap.h"
#include "html.h"

class EasyMqtt : public MqttMap {
  private:
    WiFiClient wifiClient;
    PubSubClient mqttClient;

    std::unique_ptr<ESP8266WebServer> webServer;

    String deviceId = "deviceId";
    const char* mqtt_username = "N/A";
    const char* mqtt_password = "N/A";

  protected:
    virtual String getTopic() {
      return String("easyMqtt/") + deviceId;
    }

    /**
       Handle connections to mqtt
    */
    void mqttReconnect() {
      while (!mqttClient.connected()) {
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
      // ToDo: Registre config
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

      setupWeb();

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

    void setupWeb() {
      webServer.reset(new ESP8266WebServer(80));
      webServer->on("/", std::bind(&EasyMqtt::handleWebRoot, this));
      webServer->onNotFound(std::bind(&EasyMqtt::handleNotFound, this));
      webServer->begin();
    }

    void handleWebRoot() {
      String page = "";
      page += FPSTR(HTML_HEADER);

	// Sensors
      page += FPSTR(HTML_SENSOR);
      page.replace("{name}", "door");
      page.replace("{value}", "123");
      page.replace("{last_updated}", "123");

      page += FPSTR(HTML_INPUT_PANEL);

	// Inputs
      page += FPSTR(HTML_INPUT);
      page.replace("{name}", "door");

      page += FPSTR(HTML_FOOTER);
      page.replace("{device_id}", deviceId);
      page.replace("{topic}", getTopic());
      webServer->send(200, "text/html", page);
    }

    void handleNotFound() {
      webServer->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
      webServer->sendHeader("Pragma", "no-cache");
      webServer->sendHeader("Expires", "-1");
      webServer->send ( 404, "text/plain", "Not Found" );
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
      
      #ifdef DEBUG
      Serial.print("Topic: ");
      Serial.println(getTopic());
      #endif
    }

    char* config(const char* key, const char* defaultValue) {
      // return get("config")[key]->getValue(defaultValue);
      return (char*)defaultValue;
    }

    int configInt(const char* key, int defaultValue) {
      return defaultValue;
    }

    /**
       Handle the normal loop
    */
    void loop() {
      webServer->handleClient();
      mqttReconnect();
      mqttClient.loop();
      MqttMap::loop();
    }
};

#endif
