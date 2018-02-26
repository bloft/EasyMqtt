# EasyMqtt
Easy handling of Mqtt on esp8266

* Easy wifi configuration
* Easy mqtt configuration
* Easy configuration of mqtt endpoints
* Web based UI to see current values

## Examble usage EasyMqtt
```C++
#include <EasyMqtt.h>

EasyMqtt mqtt;

void setup() {
  // Setup wifi
  mqtt.wifi("ssid", "password");
  mqtt.mqtt("host", 1883, "user", "pass");
  
  mqtt.config().setString("foo", "My Foo");

  mqtt["foo"] << [](){
    return mqtt.config().getString("foo");
  };
}

void loop() {
  mqtt.loop();
}

```
