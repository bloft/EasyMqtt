#include "Device.h"
#include <Arduino.h>

Device::Device() {
}

void Device::callback(const char* topic, uint8_t* payload, unsigned int length) {
  if(strncmp("easyMqtt/", topic, 9) == 0) {
    const char * tmp = strchr(topic, '/');
    ++tmp;
    const char * subTopic = strchr(tmp, '/');

    int deviceLength = subTopic-tmp;
    char deviceId[deviceLength+1];
    strncpy(deviceId, tmp, deviceLength);
    deviceId[deviceLength] = 0;
    
    deviceElem * device = deviceList;
    while(device) {
      if(strcmp(deviceId, device->deviceId) == 0) {
        break;
      }
      device = device->next;
    }
    if(!device) {
      device = new deviceElem();
      device->deviceId = (char*)malloc(strlen(deviceId)+1);
      strcpy(device->deviceId, deviceId);
      device->ip = (char*)malloc(4);
      strcpy(device->ip, "N/A");
      device->name = (char*)malloc(4);
      strcpy(device->name, "N/A");
      device->next = deviceList;
      deviceList = device;
    }
    
    device->online = true;
    device->lastUpdate = millis();

    if(strcmp(subTopic, "/$system/wifi/ip") == 0) {
      free(device->ip);
      device->ip = (char*)malloc(length+1);
      strncpy(device->ip, (const char*)payload, length);
      device->ip[length] = 0;
    } else if(strcmp(subTopic, "/$system/name") == 0) {
      free(device->name);
      device->name = (char*)malloc(length+1);
      strncpy(device->name, (const char*)payload, length);
      device->name[length] = 0;
    } else if(strcmp(subTopic, "/$system/online") == 0) {
      if(strncmp((const char*)payload, "OFF", length) == 0) {
        device->online = false;
      }
    }
  }

  // Iterate all elements to mark old devices offline
  each([&](deviceElem* elem) {
    if(elem->lastUpdate < (millis() - 300000)) {
      elem->online = false;
    }
  });
}

void Device::subscribe(PubSubClient* mqttClient) {
  mqttClient->subscribe("easyMqtt/+/+/online");
  mqttClient->subscribe("easyMqtt/+/+/name");
  mqttClient->subscribe("easyMqtt/+/+/wifi/ip");
}

void Device::each(std::function<void(deviceElem *)> f) {
  deviceElem * device = deviceList;
  while(device) {
    f(device);
    device = device->next;
  }
}
