#include <EasyMqtt.h>
#include <DHT.h>

EasyMqtt mqtt;
DHT dht(D7, DHT22);

void setup() {
  dht.begin();
  mqtt.wifi("ssid", "pass");
  mqtt.mqtt("server", 1883, "user", "password");

  mqtt["temperature"] << []() {
    float value = dht.readTemperature();
    if(isnan(value)) {
      return String("");
    } else {
      return String(value);
    }
  };

  mqtt["humidity"] << []() {
    float value = dht.readHumidity();
    if(isnan(value)) {
      return String("");
    } else {
      return String(value);
    }
  };

}

void loop() {
  mqtt.loop();
}
