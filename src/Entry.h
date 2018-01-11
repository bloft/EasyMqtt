#ifndef Entry_h
#define Entry_h

#include <functional>
#include <Arduino.h>
#include <PubSubClient.h>

class Entry {
  private:
    std::function<void(String)> outFunction = NULL;
    std::function<String()> inFunction = NULL;
    std::function<void(Entry*, String)> publishFunction = NULL;

    char* name = "N/A";
    int force = 0;
    int interval = -1;
    int forceUpdate = -1;
    unsigned long lastUpdate = 0;
    String lastValue = "";

    PubSubClient* client = NULL;

    Entry* parent = NULL;
    Entry* next = NULL;
    Entry* children = NULL;

    void setName(const char* name) {
      Entry::name = (char*)malloc(strlen(name)+1);
      strncpy(Entry::name, name, strlen(name)+1);
    }
    
    std::function<void(Entry*, String)> getPublishFunction() {
      if(publishFunction == NULL && parent) {
        return parent->getPublishFunction();
      }
      return publishFunction;
    }

  protected:
    Entry(const char* name, PubSubClient& mqttClient) {
      client = &mqttClient;
      setName(name);
    }

    Entry *getOrCreate(const char* name) {
      Entry *child = children;
      while (child != NULL) {
        if (strcmp(child->name, name) == 0) {
          return child;
        }
        child = child->next;
      }
      return addChild(new Entry(name, *client));
    }

    Entry *getRoot() {
      if(parent) {
        return parent->getRoot();
      } else {
        return this;
      }
    }

    Entry *getParent() {
      return parent;
    }

    Entry *setParent(Entry& parent) {
      Entry::parent = &parent;
      return this;
    }

    Entry* addChild(Entry* child) {
      child->parent = this;
      child->next = children;
      children = child;
      return child;
    }

    void setPublishFunction(std::function<void(Entry*, String)> function) {
      publishFunction = function;
    }
    
  public:
    void debug(String key, String value) {
      debug(key + " = " + value);
    }

    void debug(String msg) {
      #ifdef DEBUG
      Serial.println(msg);
      #endif
      if(client->connected()) {
        getRoot()->get("$system/debug").publish(msg);
      }
    }

    void callback(const char* topic, uint8_t* payload, unsigned int length) {
      if (strcmp(getTopic().c_str(), topic) == 0) {
        String _payload = "";
        for (int i = 0; i < length; i++) {
          _payload += (char)payload[i];
        }
        if(!isIn() || _payload != getValue()) {
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
            if (value != getValue() || force > getForce()) {
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
      bool internal = name[0] == '$';
      if (parent) {
        return internal || parent->isInternal();
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
      this->interval = interval;
    }

    void setInterval(int interval, int force) {
      this->interval = interval;
      forceUpdate = force;
    }

    int getForce() {
      if(forceUpdate < 0) {
        return parent->getForce();
      }
      return forceUpdate;
    }

    /**
     * Get last value
     */
    String getValue() {
      return lastValue;
    }

    const char* getCValue() {
      return lastValue.c_str();
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
        function(this,message);
      }
    }
    
    /**
     * Iterate over each child, including sub children
     */
    void each(std::function<void(Entry*)> f) {
      f(this);
      Entry* child = children;
      while (child != NULL) {
        child->each(f);
        child = child->next;
      }
    }

    /**
     * Create or get the sub topic with the name {name}
     */
    Entry & get(const char* name) {
      char *subName = strtok((char *)name, "/");
      Entry *entry = this;
      while(subName != NULL) {
        entry = entry->getOrCreate(subName);
        subName = strtok(NULL, "/");
      }
      return *entry;
    }

    /**
     * Create or get the sub topic with the name {name}
     */
    Entry & operator[](const char* name) {
      return *getOrCreate(name);
    }

    /**
     *  Read data from function and send it to mqtt
     */
    void operator<<(std::function<String()> inFunction) {
      Entry::inFunction = inFunction;
    }

    /**
     *  Handle data comming from mqtt
     */
    void operator>>(std::function<void(String payload)> outFunction) {
      Entry::outFunction = outFunction;
    }
};

#endif
