#include "Mqtt.h"

#include <PubSubClient.h>

Mqtt::Mqtt() {
    deviceList = new Device();
}

void Mqtt::connect() {
    if (wifiClient && !mqttClient.connected() && mqttDelay < millis()) {
        Serial.println("Connecting to MQTT");
        mqttClient.setClient(*wifiClient);
        mqttClient.setCallback([&](const char* topic, uint8_t* payload, unsigned int length) {
            if(strncmp(topic, entries->getTopic().c_str(), strlen(entries->getTopic().c_str())) != 0) {
                deviceList->callback(topic, payload, length);
            }
            entries->each([=](Entry* entry){
                entry->callback(topic, payload, length);
            });
        });

        mqttClient.setServer(config->get("mqtt.host", ""), config->getInt("mqtt.port", 1883));

        if (mqttClient.connect(entries->getName().c_str(), config->get("mqtt.username", ""), config->get("mqtt.password", ""), entries->get("$system")["online"].getTopic().c_str(), 1, 1, "OFF")) {
            Serial.println("Connected to MQTT");


            entries->each([&](Entry* entry){
                if (entry->isOut()) {
                    mqttClient.subscribe(entry->getTopic().c_str());
                }
            });

            // Device list
            deviceList->subscribe(&mqttClient);

            mqttDelay = 0;
        } else {
            Serial.print("Connection to MQTT failed, rc");
            Serial.println(mqttClient.state());
            mqttDelay = millis() + 5000;
        }
    }
}

void Mqtt::setup(Entry *entries, Config *config, WiFiClient *wifiClient) {
    Mqtt::entries = entries;
    Mqtt::config = config;
    Mqtt::wifiClient = wifiClient;
}

Device *Mqtt::getDeviceList() {
    return deviceList;
}

void Mqtt::publish(Entry *entry, String message) {
    if(mqttClient.connected()) {
        const char *msg = message.c_str();
        mqttClient.beginPublish(entry->getTopic().c_str(), strlen(msg), true);
        for (size_t i = 0; i < strlen(msg); i++){
            mqttClient.write(msg[i]);
        }
        mqttClient.endPublish();
    }
}

void Mqtt::loop() {
    connect();
    if(mqttClient.connected()) {
        mqttClient.loop();
    }
}