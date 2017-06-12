# EasyMqtt
Easy handling of Mqtt on esp6288

This library is currentlly under heavy development.
And shoud not be used at the moment.

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


	mqtt["wifi"]["rssi"] << [](){ return String(WiFi.RSSI()); };
}

void loop() {
	mqtt.loop();
}

```
