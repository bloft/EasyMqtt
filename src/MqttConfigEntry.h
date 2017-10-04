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
      return getTopic();
    }

    String getValue(String defaultValue) {
      String value = getValue();
      if(value == NULL) {
        value = defaultValue;
      }
      return value;
    }

  private:
    String toString() {
      return String(getName()) + "=" + getValue();
    }

    static EasyConfig* load() {
      SPIFFS.begin();
      File f = SPIFFS.open("/config.cfg", "r");
      if (f) {
        while(f.available()) {
          String line = f.readStringUntil('\n');
          int pos = line.indexOf("=");
          String key = line.substring(0, pos);
          String value = line.substring(pos+1);
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
        each([&](MqttEntry* entry) {
          f.println(entry.toString().c_str());
        });
        f.close();
      }
    }
}

#endif
