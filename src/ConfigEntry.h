#ifndef ConfigEntry_h
#define ConfigEntry_h

#include "FS.h" // for SPIFFS
#include "MqttEntry.h"

class ConfigEntry : public MqttEntry {
  public:
    ConfigEntry(PubSubClient& mqttClient, MqttEntry& parent) : MqttEntry("config", mqttClient, parent) {
      SPIFFS.begin();
      load();
      setPublishFunction([this](MqttEntry* entry, String message){
        store();
      });
    }

    String getString(String key, String defaultValue) {
      key.replace(".", "/");
      String value = get(key).getValue();
      if(value == "") {
        return defaultValue;
      } else {
        return value;
      }
    }
    
    const char* getCString(String key, String defaultValue) {
      return getString(key, defaultValue).c_str();
    }

    int getInt(String key, int defaultValue) {
      return getString(key, String(defaultValue)).toInt();
    }

    void set(String key, String value) {
      key.replace(".", "/");
      get(key).setValue(value);
    }

    void reset() {
      SPIFFS.remove("/config.cfg");
    }

  protected:
    String getKey(MqttEntry* entry) {
      String key = entry->getTopic();
      key.replace(getTopic() + "/", "");
      key.replace("/", ".");
      return key;
    }

  private:
    void load() {
      File f = SPIFFS.open("/config.cfg", "r");
      if (f) {
        while(f.available()) {
          String line = f.readStringUntil('\n');
          int pos = line.indexOf("=");
          String key = line.substring(0, pos);
          String value = line.substring(pos+1, line.length()-1);
          set(key, value);
        }
        f.close();
      }
    }

    void store() {
      File f = SPIFFS.open("/config.cfg", "w");
      if (f) {
        each([&](MqttEntry* entry) {
          if(entry->getTopic() != getTopic()) {
            f.print(getKey(entry));
            f.print("=");
            f.println(entry->getValue());
          }
        });
        f.close();
      } else {
        Serial.println("Failed to store config");
      }
    }
};

#endif
