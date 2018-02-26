# ToDo:

## Improvements

### Add easyMqtt/$id/system/connected + will to indicate if device is online

### Add config values
Add config values that is persisted to flash memory

http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html

```C++
#include <EasyMqtt.h>

EasyMqtt mqtt;

setup() {
  // init config with name "test" with default value 123
  // The value can be changed using easyMqtt/{id}/$config/test
  // Is loaded from 
  mqtt.config().set("timeout", 5000);  
}

loop() {
  mqtt.loop();
  mqtt.config().getInt("timeout"); // Read back config value
}

```

### OTA support
Add support for OTA using ArduinoOTA

## Future

### Support writing to a topic with a subscriber
Support both << and >> on the same topic with out read out own published values

### Add support for filters
 * Don't send if value is the same as last.
 * Don't send if value is Nan

### Add support for float type (extend Entry)
This will make it possible to generate graphs in UI and ease implementations

### Add publish configured endpoints, to support openhab2 auto configure
something like
easyMqtt/$id/system/config
{
  "id"="1dfasfa",
  "ip"="192.168.1.79",
  "endpoints"=["/temparatur", "/humidity"]
}

   Read / Write / String / Number / ...

