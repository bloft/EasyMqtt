#include "ConfigEntry.h"

String ConfigEntry::getKey(Entry* entry) {
  String key = entry->getTopic();
  key.replace(getTopic() + "/", "");
  return key;
}

ConfigEntry::ConfigEntry() : Entry("$config") {
  SPIFFS.begin();

  // ToDo: add out function
  //get(key) >> [this](String value) {
  //  setValue(value);
  //}
}

void ConfigEntry::load() {
  debug("Load config");
  File f = SPIFFS.open("/config.cfg", "r");
  if (f) {
    while(f.available()) {
      String line = f.readStringUntil('\n');
      int pos = line.indexOf("=");
      String key = line.substring(0, pos);
      String value = line.substring(pos+1, line.length()-1);
      setString(key.c_str(), value.c_str());
      debug(key, value);
    }
    f.close();
  } else {
    debug("Failed to load config");
  }
}

void ConfigEntry::save() {
  debug("Save config");
  File f = SPIFFS.open("/config.cfg", "w");
  if (f) {
    each([&](Entry* entry) {
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

void ConfigEntry::reset() {
  SPIFFS.remove("/config.cfg");
  // ToDo: Remove all child entries (children = NULL;)
}

long ConfigEntry::getLong(const char *key, long defaultValue) {
  char cstr[16];
  get(key).getCValue(cstr);
  if(strlen(cstr) == 0) {
    get(key).setValue(String(defaultValue));
    return defaultValue;
  }
  return atol(cstr);
}

int ConfigEntry::getInt(const char *key, int defaultValue) {
  char cstr[16];
  get(key).getCValue(cstr);
  if(strlen(cstr) == 0) {
    get(key).setValue(String(defaultValue));
    return defaultValue;
  }
  return atoi(cstr);
}

double ConfigEntry::getDouble(const char *key, double defaultValue) {
  char cstr[16];
  get(key).getCValue(cstr);
  if(strlen(cstr) == 0) {
    get(key).setValue(String(defaultValue));
    return defaultValue;
  }
  return atof(cstr);
}
void ConfigEntry::getCString(const char *key, const char *defaultValue, char *destination) {
  get(key).getCValue(destination);
  if(strlen(destination) == 0) {
    get(key).setValue(defaultValue);
    strcpy(destination, defaultValue);
  }
}

String ConfigEntry::getString(const char *key, const char *defaultValue) {
  String value = get(key).getValue();
  if(value == "") {
    get(key).setValue(defaultValue);
    value = defaultValue;
  }
  return value;
}

void ConfigEntry::setString(const char *key, const char *value) {
  get(key).setValue(value);
}
