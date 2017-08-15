#ifndef MqttMap_h
#define MqttMap_h

#include <Arduino.h>
#include <PubSubClient.h>

class MqttMap {
  private:
    void (*outFunction)(String payload) = NULL;
    String (*inFunction)() = NULL;

    const char* name = "N/A";
    int interval = -1;
    unsigned long lastUpdate = 0; // Last read of data

    PubSubClient* client = NULL;

    MqttMap* parent = NULL;
    MqttMap* next = NULL;
    MqttMap* children = NULL;

  protected:
    MqttMap(const char* _name, PubSubClient& _mqttClient, MqttMap& _parent) {
      name = _name;
      parent = &_parent;
      client = &_mqttClient;
      interval = -1;
    }

    MqttMap(const char* _name, PubSubClient& _mqttClient) {
      name = _name;
      client = &_mqttClient;
      interval = 5;
    }

    virtual String getTopic() {
      if (parent) {
        return parent->getTopic() + "/" + name;
      } else {
        return String(name);
      }
    }

    void callback(const char* _topic, uint8_t* payload, unsigned int length) {
      if (strcmp(getTopic().c_str(), _topic) == 0) {
        if (outFunction != NULL) {
          String _payload = "";
          for (int i = 0; i < length; i++) {
            _payload += (char)payload[i];
          }
          outFunction(_payload);
        }
      } else {
        MqttMap* child = children;
        while (child != NULL) {
          child->callback(_topic, payload, length);
          child = child->next;
        }
      }
    }
    
    /**

    */
    void subscribe() {
      if (outFunction != NULL) {
        client->subscribe(getTopic().c_str());
      }
      MqttMap* child = children;
      while (child != NULL) {
        child->subscribe();
        child = child->next;
      }
    }

    /**

    */
    void loop() {
      if (inFunction != NULL) {
        unsigned long time = millis();
        if (time >= (lastUpdate + (getInterval() * 1000))) {
          lastUpdate = time;
          String value = inFunction();
          if (value != "") {
            publish(value);
          }
        }
      }
      MqttMap* child = children;
      while (child != NULL) {
        child->loop();
        child = child->next;
      }
    }

  public:
    int getInterval() {
      if (interval < 0) {
        return parent->getInterval();
      }
      return interval;
    }

    void setInterval(int _interval) {
      interval = _interval;
    }

    void publish(String message) {
      client->publish(getTopic().c_str(), message.c_str());
    }

    /**
     * Create or get the sub topic with the name {name}
     */
    MqttMap & get(const char* name) {
      return operator[](name);
    }

    /**
     * Create or get the sub topic with the name {name}
     */
    MqttMap & operator[](const char* name) {
      MqttMap * child = children;
      while (child != NULL) {
        if (strcmp(child->name, name) == 0) {
          return *child;
        }
        child = child->next;
      }
      MqttMap * oldChild = children;
      children = new MqttMap(name, *client, *this);
      children->next = oldChild;
      return *children;
    }

    /**
     *  Read data from function and send it to mqtt
     */
    void operator<<(String (*inFunction)()) {
      MqttMap::inFunction = inFunction;
    }

    /**
     *  Handle data comming from mqtt
     */
    void operator>>(void (*outFunction)(String payload)) {
      MqttMap::outFunction = outFunction;
    }
};

#endif
