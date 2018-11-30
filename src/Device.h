#pragma once

#include <functional>
struct deviceElem {
  char * deviceId;
  char * name;
  bool online;
  char * ip;
  unsigned long lastUpdate = 0;
  struct deviceElem * next;
};

class Device {
  private:
    struct deviceElem * deviceList = nullptr;

  public:
    Device();

    void callback(const char* topic, uint8_t* payload, unsigned int length);

    void each(std::function<void(char*, bool, char*)> f);
};
