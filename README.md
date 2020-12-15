# EasyIoT
Easy handling of mqtt or Smartthings on esp8266

* Easy wifi configuration using web interface
* Easy mqtt configuration using web interface
* Easy configuration of iot endpoints
* Web based UI to see current values
* Smartthings discovery

## Examble usage EasyIoT
```C++
#include <EasyIoT.h>

EasyIoT iot;

void setup() {
  iot.config().set("foo", "My Foo");

  iot["foo"] << [&](){
    return String(iot.config().get("foo", "default"));
  };
}

void loop() {
  iot.loop();
}

```
