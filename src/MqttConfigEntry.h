#ifndef EasyConfig_h
#define EasyConfig_h

#include "FS.h" // for SPIFFS
#include "MqttEntry.h"

class MqttConfigEntry : public MqttEntry {
  public:
    EasyConfig(char* key, char* value) {
      >> [](String value){
        setValue(value);
        // ToDo: Call store();
      };
    }

    String getKey() {
      String prefix = getRootEntry()->get("config")->getTopic();
      return getTopic().replace(prefix, "").replace("/", ".");
    }

    String getValue(String defaultValue) {
      String value = getValue();
      if(value == NULL) {
        value = defaultValue;
      }
      return value;
    }

  protected:
    MqttEntry* getRootEntry() {
      MqttEntry* root = NULL;
      each([&](MqttEntry* entry) {
        if(entry->isRoot()) {
          root = entry;
        }
      });
      return root;
    }

    virtual String toString() {
      return String(getName()) + "=" + getValue();
    }

  private:
    static EasyConfig* load() {
      SPIFFS.begin();
      File f = SPIFFS.open("/config.cfg", "r");
      if (f) {
        while(f.available()) {
          String line = f.readStringUntil('\n');
          int pos = line.indexOf("=");
          String key = line.substring(0, pos);
          String value = line.substring(pos+1);
          MqttEntry* entry = getRootEntry().get(key.replace(".", "/"));
          Serial.println(line);
        }
        f.close();
      }
      // ToDo: read lines from file, parse line, create EasyConfig object
      return NULL;
    }

    void store() {
      SPIFFS.begin();
      File f = SPIFFS.open("/config.cfg", "w");
      if (f) {
        getRootEntry()->each([&](MqttEntry* entry) {
          f.println(entry.toString().c_str());
        });
        f.close();
      }
    }
}

#endif
