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

    MqttMap* parent = NULL;
    MqttMap* next = NULL;
    MqttMap* children = NULL;

  protected:
    MqttMap(const char* _name, MqttMap& _parent) {
      name = _name;
      parent = &_parent;
      interval = -1;
    }

    virtual String getTopic() {
      if (parent) {
        return parent->getTopic() + "/" + name;
      } else {
        return String(name);
      }
    }

  public:
    MqttMap(const char* _name) {
      name = _name;
      interval = 5;
    }

    int getInterval() {
      if (interval < 0) {
        return parent->getInterval();
      }
      return interval;
    }

    void setInterval(int _interval) {
      interval = _interval;
    }

    void publish(PubSubClient& client, String message) {
      String topic = getTopic();
      //      printf("publish[%s]: %s\n", topic.c_str(), message.c_str());
      client.publish(topic.c_str(), message.c_str());
    }

    void callback(const char* _topic, uint8_t* payload, unsigned int length) {
      if (strcmp(getTopic().c_str(), _topic) == 0) {
        if (outFunction != NULL) {
          String _payload = "";
          for (int i = 0; i < length; i++) {
            _payload += (char)payload[i];
          }
          //        printf("Found callback, %s: %s\n", _topic, _payload.c_str());
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
    void subscribe(PubSubClient& client) {
      if (outFunction != NULL) {
        String topic = getTopic();
        printf("Subscribe: %s\n", topic.c_str());
        client.subscribe(topic.c_str());
      }
      MqttMap* child = children;
      while (child != NULL) {
        child->subscribe(client);
        child = child->next;
      }
    }

    /**

    */
    void loop(PubSubClient& client) {
      if (inFunction != NULL) {
        unsigned long time = millis();
        if (time >= (lastUpdate + (getInterval() * 1000))) {
          lastUpdate = time;
          String value = inFunction();
          if (value != "") {
            publish(client, value);
          }
        }
      }
      MqttMap* child = children;
      while (child != NULL) {
        child->loop(client);;
        child = child->next;
      }
    }

    /**

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
      children = new MqttMap(name, *this);
      children->next = oldChild;
      return *children;
    }

    /**
       Read data from function and send it to mqtt
    */
    void operator<<(String (*inFunction)()) {
      MqttMap::inFunction = inFunction;
    }

    /**
       Handle data comming from mqtt
    */
    void operator>>(void (*outFunction)(String payload)) {
      MqttMap::outFunction = outFunction;
    }
};

#endif
