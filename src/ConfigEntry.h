#pragma once

#include "FS.h" // for SPIFFS
#include "Entry.h"

class ConfigEntry : public Entry {
  private:

  protected:
    String getKey(Entry* entry);

  public:
    ConfigEntry();
    void load();
    void save();
    void reset();
    
    void getCString(const char *key, const char *defaultValue, char *destination);
    String getString(const char *key, const char *defaultValue);
    
    void setString(const char *key, const char *value);
    
    int getInt(const char *key, int defaultValue);
    long getLong(const char *key, long defaultValue);
    double getDouble(const char *key, double defaultValue);
};
