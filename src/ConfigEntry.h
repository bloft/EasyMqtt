#ifndef ConfigEntry_h
#define ConfigEntry_h

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
    String getString(const char *key, const char *defaultValue);
    void setString(const char *key, const char *value);
};

#endif
