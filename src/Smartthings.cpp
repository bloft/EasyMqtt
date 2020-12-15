#include "Smartthings.h"

#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

Smartthings:: Smartthings(String ns, String deviceHandler) {
    /***** Setup SSDP */
	SSDP.setSchemaURL("description.xml");
	SSDP.setHTTPPort(8080);
	SSDP.setURL("state.json");
    SSDP.setModelName(deviceHandler);
    SSDP.setManufacturer(ns);
    SSDP.setDeviceType("urn:schemas-upnp-org:device:st-wifi-dev");
}

void Smartthings::setup(Entry *entries, Config *config, String name) {
    Serial.printf("Starting Smartthings...\n");

    Smartthings::entries = entries;
    Smartthings::config = config;

    webServer.reset(new ESP8266WebServer(8080));

    webServer->on("/state.json", HTTP_GET, std::bind(&Smartthings::handleState, this));
    webServer->on("/description.xml", HTTP_GET, std::bind(&Smartthings::handleDescription, this));
    webServer->on("/update", HTTP_PUT, std::bind(&Smartthings::handleUpdate, this));
    webServer->on("/subscribe", HTTP_POST, std::bind(&Smartthings::handleSubscribe, this));
    webServer->begin();

    SSDP.setName("Test");
	SSDP.begin();
}

String Smartthings::getName(Entry* root, Entry* entry) {
  String name = entry->getName();
  name.replace(root->getName(), "");
  return name.substring(1);
}

String Smartthings::getPath(Entry* entry) {
  return "" + entry->getTopic();
}

String Smartthings::getAsJson() {
    DynamicJsonBuffer jsonBuffer(200);

    JsonObject& root = jsonBuffer.createObject();

    // Construct json
    entries->each([&](Entry* entry) {
        if(entry->isIn()) {
            root[getName(entries, entry)] = entry->getValue();
        }
    });

    String output;
    root.prettyPrintTo(output);
    return output;
}

void Smartthings::handleState() {
    Serial.println("Smartthings -> State");
    webServer->send(200, "application/json", getAsJson());
}

bool Smartthings::sendState() {
    String callback = config->get("st.callback", "");
    if (callback.length() > 0) {
        Serial.println("Smartthings -> publish");
        Serial.print("Callback: ");
		Serial.println(callback);

        WiFiClient client = webServer->client();
        HTTPClient http;
	    http.begin(client, callback);
	    http.addHeader("Content-Type", "application/json");
	    int code = http.POST(getAsJson()); 
	    Serial.printf("POST Returned %d\n", code);
	    http.end();
        return code >= 200 && code < 400;
    }
    return false;
}

void Smartthings::handleDescription() {
    Serial.println("Smartthings -> Description");
    SSDP.schema(webServer->client());
}

void Smartthings::handleSubscribe() {
    Serial.println("Smartthings -> Subscribe");

	DynamicJsonBuffer jsonBuffer(200);
	JsonObject& json = jsonBuffer.parseObject(webServer->arg("plain"));
	if (!json.success()) {
		webServer->send(500, "application/json", "{}");
	} else {
        Serial.print("Updated with new callback: ");
        Serial.println(json["callback"].asString());
        config->set("st.callback", json["callback"].asString());
		webServer->send(200, "application/json", "{}");
	}
}

void Smartthings::handleUpdate() {
    Serial.println("Smartthings -> Update");
    DynamicJsonBuffer jsonBuffer(200);
	JsonObject& json = jsonBuffer.parseObject(webServer->arg("plain"));
    if (!json.success()) {
		webServer->send(500, "application/json", "{}");
	} else {
        // Handle update
        entries->each([&](Entry* entry) {
            String name = getName(entries, entry);
            if(entry->isOut() && json.containsKey(name)) {
                Serial.print("Smartthings -> Update -> ");
                Serial.print(name);
                Serial.print(" = ");
                Serial.println(json[name].asString());
                entry->setValue(json[name].asString(), true);
            }
        });
		webServer->send(200, "application/json", "{}");
	}
}

void Smartthings::publish() {
    publishNow = true;
}

void Smartthings::loop() {
    webServer->handleClient();
    if(publishNow) {
        if(sendState()) {
            publishNow = false;
        }
    }
}