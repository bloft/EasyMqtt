# EasyMqtt
Easy handling of Mqtt on esp8266

This library is currentlly under heavy development.

## Examble usage EasyMqtt
```C++
#include <EasyMqtt.h>

EasyMqtt mqtt;

void setup() {
	mqtt["foo"] << [](){ return String("bar"); };
}

void loop() {
	mqtt.loop();
}

```
