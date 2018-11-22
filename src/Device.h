#pragma once

#include <functional>
struct deviceElem {
  char * deviceId;
  bool online;
  char * ip;
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
