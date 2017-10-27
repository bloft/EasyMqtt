#ifndef MqttEntry_h
#define MqttEntry_h

#include <functional>
#include <Arduino.h>
#include <PubSubClient.h>

class MqttEntry {
  private:
    std::function<void(String)> outFunction = NULL;
    std::function<String()> inFunction = NULL;
    std::function<void(MqttEntry*, String)> publishFunction = NULL;

    char* name = "N/A";
    int force = 0;
    int interval = -1;
    int forceUpdate = -1;
    bool internal = false;
    unsigned long lastUpdate = 0;
    String lastValue = "";

    PubSubClient* client = NULL;

    MqttEntry* parent = NULL;
    MqttEntry* next = NULL;
    MqttEntry* children = NULL;

    void setName(const char* name) {
      MqttEntry::name = (char*)malloc(strlen(name)+1);
      strncpy(MqttEntry::name, name, strlen(name)+1);
    }
    
    std::function<void(MqttEntry*, String)> getPublishFunction() {
      if(publishFunction == NULL && parent) {
        return parent->getPublishFunction();
      }
      return publishFunction;
    }

  protected:
    MqttEntry(const char* name, PubSubClient& mqttClient, MqttEntry& parent, bool internal) {
      setParent(parent);
      client = &mqttClient;
      setName(name);
      MqttEntry::internal = internal;
    }

    MqttEntry(const char* name, PubSubClient& mqttClient, MqttEntry& parent) {
      setParent(parent);
      client = &mqttClient;
      setName(name);
    }

    MqttEntry(const char* name, PubSubClient& mqttClient) {
      interval = 5;
      forceUpdate = 10;
      client = &mqttClient;
      setName(name);
    }

    void setParent(MqttEntry& parent) {
      MqttEntry::parent = &parent;
    }

    MqttEntry* addChild(MqttEntry* child) {
      child->setParent(*this);
      MqttEntry * oldChild = children;
      children = child;
      children->next = oldChild;
      return child;
    }

    void setPublishFunction(std::function<void(MqttEntry*, String)> function) {
      publishFunction = function;
    }
    
  public:
    void callback(const char* topic, uint8_t* payload, unsigned int length) {
      if (strcmp(getTopic().c_str(), topic) == 0) {
        String _payload = "";
        for (int i = 0; i < length; i++) {
          _payload += (char)payload[i];
        }
        if(!isIn() || _payload != lastValue) {
          update(_payload);
        }
      }
    }
    
    /**
     * Request a updated value if needed
     */
    void update() {
      if (isIn()) {
        unsigned long time = millis();
        if (time >= (lastUpdate + (getInterval() * 1000)) || lastUpdate == 0) {
          force++;
          lastUpdate = time;
          String value = inFunction();
          if (value != "") {
            if (value != lastValue || force > getForce()) {
              setValue(value);
              force = 0;
            }
          }
        }
      }
    }

    void update(String payload) {
        if (isOut()) {
          outFunction(payload);
        }
    }

    bool isIn() {
      return inFunction != NULL;
    }

    bool isOut() {
      return outFunction != NULL;
    }

    bool isRoot() {
      return parent == NULL;
    }

    bool isInternal() {
      if (parent) {
        return parent->isInternal() || internal;
      } else {
        return internal;
      }
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

    int getForce() {
      if(forceUpdate < 0) {
        return parent->getForce();
      }
      return forceUpdate;
    }

    void setForce(int force) {
      forceUpdate = force;
    }

    /**
     * Get last value
     */
    String getValue() {
      return lastValue;
    }

    void setValue(String value) {
      lastUpdate = millis();
      lastValue = value;
      publish(value);
    }

    /**
     * 
     */
    long getLastUpdate() {
      return lastUpdate;
    }

    /**
     * Publish value to mqtt
     */
    void publish(String message) {
      auto function = getPublishFunction();
      if(function) {
        getPublishFunction()(this,message);
      }
    }
    
    /**
     * Iterate over each child, including sub children
     */
    void each(std::function<void(MqttEntry*)> f) {
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
    MqttEntry & get(String name) {
      int pos = name.indexOf('/');
      if(pos < 0) {
        return operator[](name.c_str());
      } else {
        return get(name.substring(0, pos)).get(name.substring(pos+1));
      }
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
      return *addChild(new MqttEntry(name, *client, *this));
    }

    /**
     *  Read data from function and send it to mqtt
     */
    void operator<<(std::function<String()> inFunction) {
      MqttEntry::inFunction = inFunction;
    }

    /**
     *  Handle data comming from mqtt
     */
    void operator>>(std::function<void(String payload)> outFunction) {
      MqttEntry::outFunction = outFunction;
    }
};

#endif
