# EasyMqtt
Easy handling of Mqtt on esp8266

Including a web interface to se current values, and avalible topic, and a rest api

## Examble usage EasyMqtt
```C++
#include <EasyMqtt.h>

EasyMqtt mqtt;

void setup() {
  // Setup wifi
  mqtt.wifi("ssid", "password");
  mqtt.mqtt("host", 1883, "user", "pass");

	mqtt["foo"] << [](){ return String("bar"); };
}

void loop() {
	mqtt.loop();
}

```
