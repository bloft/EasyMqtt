#ifndef EasyConfig_h
#define EasyConfig_h

#include "MqttEntry.h"

class EasyConfig : public MqttEntry {
  public:
    EasyConfig(char* key, char* value) {
      >> [](String value){
        setValue(value);
        // ToDo: Call store();
      };
    }

    char* getValue(char* defaultValue) {
      String value = getValue();
      if(value == NULL) {
        value = defaultValue;
      }
      return value;
    }

    String toString() {
      return String(getName()) + "=" + getValue();
    }



  private:
    static EasyConfig* load() {
      // ToDo: read lines from file, parse line, create EasyConfig object
      return NULL;
    }

    void store() {
      // f.open
      each([&](MqttEntry* child) {
        // f.println(child.toString().c_str());
        Serial.println(child.toString());
      });
      // f.close
    }
}

#endif
