# EasyMqtt
Easy handling of Mqtt on esp8266

This library is currentlly under heavy development.

## Examble usage EasyMqtt
```C++
#include <EasyMqtt.h>

EasyMqtt mqtt;

void setup() {
	Serial.begin(115200);
	Serial.println();

	// Setup wifi
	mqtt.wifi("ssid", "password");
	mqtt.mqtt("host", 1883, "user", "pass");


	mqtt["foo"] << [](){ return "bar"; };
}

void loop() {
	mqtt.loop();
}

```
