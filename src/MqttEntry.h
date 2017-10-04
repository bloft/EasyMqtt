#ifndef MqttEntry_h
#define MqttEntry_h

#include <Arduino.h>
#include <PubSubClient.h>

class MqttEntry {
  private:
    void (*outFunction)(String payload) = NULL;
    String (*inFunction)() = NULL;

    const char* name = "N/A";
    int interval = -1;
    unsigned long lastUpdate = 0; // Last read of data
    String lastValue = "";

    PubSubClient* client = NULL;

    MqttEntry* parent = NULL;
    MqttEntry* next = NULL;
    MqttEntry* children = NULL;

  protected:
    MqttEntry(const char* name, PubSubClient& mqttClient, MqttEntry& parent) {
      MqttEntry::name = name;
      MqttEntry::parent = &parent;
      client = &mqttClient;
      interval = -1;
    }

    MqttEntry(const char* name, PubSubClient& mqttClient) {
      MqttEntry::name = name;
      client = &mqttClient;
      interval = 5;
    }
  
  public:
    void callback(const char* topic, uint8_t* payload, unsigned int length) {
      if (strcmp(getTopic().c_str(), topic) == 0) {
        if (isOut()) {
          String _payload = "";
          for (int i = 0; i < length; i++) {
            _payload += (char)payload[i];
          }
          outFunction(_payload);
        }
      }
    }
    
    /**
      
    */
    void subscribe() {
      if (isOut()) {
        client->subscribe(getTopic().c_str());
      }
    }

    /**
     * Request a updated value if needed
     */
    void loop() {
      if (isIn()) {
        unsigned long time = millis();
        if (time >= (lastUpdate + (getInterval() * 1000))) {
          lastUpdate = time;
          String value = inFunction();
          if (value != "") {
            publish(value);
            lastValue = value;
          }
        }
      }
    }

    bool isIn() {
      return inFunction != NULL;
    }

    bool isOut() {
      return outFunction != NULL;
    }

    virtual String getTopic() {
      if (parent) {
        return parent->getTopic() + "/" + name;
      } else {
        return String(name);
      }
    }

    int getInterval() {
      if (interval < 0) {
        return parent->getInterval();
      }
      return interval;
    }

    void setInterval(int interval) {
      MqttEntry::interval = interval;
    }

    /**
     * Get last value
     */
    String getValue() {
      return lastValue;
    }

    /**
     * Publish value to mqtt
     */
    void publish(String message) {
      client->publish(getTopic().c_str(), message.c_str());
    }
    
    /**
     * Iterate over each child, including sub children
     */
    void each(void (*f)(MqttEntry*)) {
      f(this);
      MqttEntry* child = children;
      while (child != NULL) {
        child->each(f);
        child = child->next;
      }
    }

    /**
     * Create or get the sub topic with the name {name}
     */
    MqttEntry & get(const char* name) {
      return operator[](name);
    }

    /**
     * Create or get the sub topic with the name {name}
     */
    MqttEntry & operator[](const char* name) {
      MqttEntry * child = children;
      while (child != NULL) {
        if (strcmp(child->name, name) == 0) {
          return *child;
        }
        child = child->next;
      }
      MqttEntry * oldChild = children;
      children = new MqttEntry(name, *client, *this);
      children->next = oldChild;
      return *children;
    }

    /**
     *  Read data from function and send it to mqtt
     */
    void operator<<(String (*inFunction)()) {
      MqttEntry::inFunction = inFunction;
    }

    /**
     *  Handle data comming from mqtt
     */
    void operator>>(void (*outFunction)(String payload)) {
      MqttEntry::outFunction = outFunction;
    }
};

#endif
