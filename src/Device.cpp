#include "Device.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Device::Device() {
}

void Device::callback(const char* topic, uint8_t* payload, unsigned int length) {
  if(strncmp("easyMqtt/", topic, 9) == 0) {
    const char * tmp = strchr(topic, '/');
    ++tmp;
    const char * subTopic = strchr(tmp, '/');

    char deviceId[10];
    strncpy(deviceId, tmp, subTopic-tmp);
    
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
      device->next = deviceList;
      deviceList = device;
    }
    
    device->online = true;

    if(strcmp(subTopic, "/$system/wifi/ip") == 0) {
      free(device->ip);
      device->ip = (char*)malloc(length);
      strncpy(device->ip, (const char*)payload, length);
    } else if(strcmp(subTopic, "/$system/online") == 0) {
      if(strncmp((const char*)payload, "OFF", length) == 0) {
        device->online = false;
      }
    }
  }
}

void Device::each(std::function<void(char*, bool, char*)> f) {
  deviceElem * device = deviceList;
  while(device) {
    f(device->deviceId, device->online, device->ip);
    device = device->next;
  }
}
