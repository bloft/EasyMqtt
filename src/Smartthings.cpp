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
    #ifdef DEBUG
        Serial.printf("Starting Smartthings...\n");
    #endif

    Smartthings::entries = entries;
    Smartthings::config = config;

    webServer.reset(new ESP8266WebServer(8080));

    webServer->on("/state.json", HTTP_GET, std::bind(&Smartthings::handleState, this));
    webServer->on("/description.xml", HTTP_GET, std::bind(&Smartthings::handleDescription, this));
    webServer->on("/update", HTTP_PUT, std::bind(&Smartthings::handleUpdate, this));
    webServer->begin();

    SSDP.setName(name);
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
        String value = entry->getValue();
        if(value != NULL) {
            root[getName(entries, entry)] = value;
        }
    });

    String output;
    root.prettyPrintTo(output);
    return output;
}

void Smartthings::handleState() {
    #ifdef DEBUG
        Serial.println("Smartthings -> State");
    #endif
    webServer->send(200, "application/json", getAsJson());
}

bool Smartthings::sendState() {
    String callback = config->get("st.callback", "");
    if (callback.length() > 0) {
        #ifdef DEBUG
            Serial.println("Smartthings -> publish");
            Serial.print("Callback: ");
            Serial.println(callback);
        #endif

        WiFiClient client = webServer->client();
        HTTPClient http;
	    http.begin(client, callback);
	    http.addHeader("Content-Type", "application/json");
	    int code = http.POST(getAsJson()); 
	    http.end();
        return code >= 200 && code < 400;
    }
    return false;
}

void Smartthings::handleDescription() {
    #ifdef DEBUG
        Serial.println("Smartthings -> Description");
    #endif
    SSDP.schema(webServer->client());
}

void Smartthings::handleUpdate() {
    #ifdef DEBUG
        Serial.println("Smartthings -> Update");
    #endif
    DynamicJsonBuffer jsonBuffer(200);
	JsonObject& json = jsonBuffer.parseObject(webServer->arg("plain"));
    if (!json.success()) {
		webServer->send(500, "application/json", "{}");
	} else {
        if(json.containsKey("smartthings.callback")) {
            config->set("st.callback", json["smartthings.callback"].asString());
        }
        // Handle update
        entries->each([&](Entry* entry) {
            String name = getName(entries, entry);
            if(entry->isOut() && json.containsKey(name)) {
                #ifdef DEBUG
                    Serial.print("Smartthings -> Update -> ");
                    Serial.print(name);
                    Serial.print(" = ");
                    Serial.println(json[name].asString());
                #endif
                entry->setValue(json[name].asString(), true);
            }
        });
		webServer->send(200, "application/json", getAsJson());
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
