#include <EasyMqtt.h>

EasyMqtt mqtt;

void setup() {
  mqtt.wifi("ssid", "pass");
  mqtt.mqtt("server", 1883, "user", "password");

  mqtt["foo"] << []() { return "bar"; };
}

void loop() {
  mqtt.loop();
}
