#ifndef ConfigEntry_h
#define ConfigEntry_h

#include "FS.h" // for SPIFFS
#include "MqttEntry.h"

class ConfigEntry : public MqttEntry {
  public:
    ConfigEntry(PubSubClient& mqttClient, MqttEntry& parent) : MqttEntry("config", mqttClient, parent) {
      SPIFFS.begin();
      load();
    }

    String getString(String key, String defaultValue) {
      key.replace(".", "/");
      return get(key).getValue();
    }

  protected:
    String getKey(MqttEntry* entry) {
      String key = entry->getTopic();
      key.replace(getTopic(), "");
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
          key.replace(".", "/");
          get(key).setValue(value);
        }
        f.close();
      }
    }

    void store() {
      File f = SPIFFS.open("/config.cfg", "w");
      if (f) {
        each([&](MqttEntry* entry) {
          f.println(toString(entry).c_str());
        });
        f.close();
      }
    }
};

#endif
