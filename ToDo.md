# ToDo:

## Improvements

Change MqttMap to include reference to mqttclient
 * Change loop, publish, ... to not need mqttclient

Add connected + will to indicate if device is online

Add better debugging (remote ?)

## Future

Add support for filters
 * Don't send if value is the same as last.
 * Don't send if value is Nan

Add support for float type (extend MqttMap)

Add publish configured endpoints, to support openhab2 auto configure

Add web interface
