# ToDo:

## Improvements

Change MqttMap to include reference to mqttclient
 * Change loop, publish, ... to not need mqttclient

Add easyMqtt/$id/system/connected + will to indicate if device is online

## Future

Add system info
 * easyMqtt/$id/system/uptime
 * easyMqtt/$id/system/mem
 * easyMqtt/$id/system/reset
 * easyMqtt/$id/system/debug
 * easyMqtt/$id/system/config

Add better debugging
 * Add new EasyDebug class, extends Print (Print.h), and prints to easyMqtt/$id/system/debug if enabled
 * Add getDebugger on EasyMqtt that returns the EasyDebug instance

Add support for filters
 * Don't send if value is the same as last.
 * Don't send if value is Nan

Add support for float type (extend MqttMap)

Add publish configured endpoints, to support openhab2 auto configure
something like
easyMqtt/$id/system/config
{
  "id"="1dfasfa",
  "ip"="192.168.1.79",
  "endpoints"=["/temparatur", "/humidity"]
}

   Read / Write / String / Number / ...
