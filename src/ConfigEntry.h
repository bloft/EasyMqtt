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
    
    char* getCString(String key, String defaultValue) {
      String value = getString(key, defaultValue);
      char cValue[value.length() + 1];
      memset(cValue, 0, value.length() + 1);
      for(int i=0; i < value.length(); i++) {
        cValue[i] = value.charAt(i);
      }
      return cValue;
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

    String toString(MqttEntry* entry) {
      return getKey(entry) + "=" + entry->getValue();
    }
    
  private:
    void load() {
      File f = SPIFFS.open("/config.cfg", "r");
      if (f) {
        while(f.available()) {
          String line = f.readStringUntil('\n');
          int pos = line.indexOf("=");
          String key = line.substring(0, pos);
          String value = line.substring(pos+1);
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
            f.println(toString(entry).c_str());
          }
        });
        f.close();
      } else {
        Serial.println("Failed to store config");
      }
    }
};

#endif
