#ifndef ConfigEntry_h
#define ConfigEntry_h

#include "FS.h" // for SPIFFS
#include "Entry.h"

class ConfigEntry : public Entry {
  private:

  protected:
    void load() {
      debug("Load config");
      File f = SPIFFS.open("/config.cfg", "r");
      if (f) {
        while(f.available()) {
          String line = f.readStringUntil('\n');
          int pos = line.indexOf("=");
          String key = line.substring(0, pos);
          String value = line.substring(pos+1, line.length()-1);
          set(key, value);
          debug(key, value);
        }
        f.close();
      } else {
        debug("Failed to load config");
      }
    }

    void save() {
      debug("Save config");
      File f = SPIFFS.open("/config.cfg", "w");
      if (f) {
        each([&](MqttEntry* entry) {
          if(entry->getTopic() != getTopic()) {
            f.print(getKey(entry));
            f.print("=");
            f.println(entry->getValue());
            debug(getKey(entry), entry->getValue());
          }
        });
        f.close();
      } else {
        debug("Failed to save config");
      }
    }

		void reset() {
      SPIFFS.remove("/config.cfg");
      // ToDo: Remove all child entries (children = NULL;)
    }

    String getKey(MqttEntry* entry) {
      String key = entry->getTopic();
      key.replace(getTopic() + "/", "");
      return key;
    }

  public:
    ConfigEntry() : Entry("$config", mqttClient) {
      SPIFFS.begin();
      load();

      setPublishFunction([&](Entry* entry, String message){
        save();
      });
    }

    char *getString(const char *key, const char *defaultValue) {
      // ToDo: if no value. setString(key, defaultValue)
      return get(key).getValue();
    }

    void setString(const char *key, const char *value) {
      get(key).setValue(value);
    }
};

#endif
