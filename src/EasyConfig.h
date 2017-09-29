#ifndef EasyConfig_h
#define EasyConfig_h

#include "MqttMap.h"

class EasyConfig : public MqttMap {
  private:
    char* value = NULL;


  public:
    EasyConfig(char* key, char* value) {
      >> [](String value){
        // ToDo: Handle update of value based on mqtt
        // ToDo: Call store();
      };
    }

    char* getValue(char* defaultValue) {
      return defaultValue;
    }

    String toString() {
      return String(getName()) + "=" + getValue(NULL);
    }



  private:
    static EasyConfig* load() {
      // ToDo: read lines from file, parse line, create EasyConfig object
      return NULL;
    }

    void store() {
      // f.open
      each([&](MqttMap* child) {
        // f.println(child.toString().c_str());
        Serial.println(child.toString());
      });
      // f.close
    }
}

#endif
