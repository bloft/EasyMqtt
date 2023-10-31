#pragma once

#include <functional>
#include <Arduino.h>

enum EntryType { text, number, onOff, openClose, dimmer, rollershutter, colorRGB };

inline const char* toString(EntryType v) {
  switch(v) {
    case text: return "string";
    case number: return "number";
    case onOff: return "switch";
    case openClose: return "contact";
    case dimmer: return "dimmer";
    case rollershutter: return "rollershutter";
    case colorRGB: return "colorRGB";
  }
  return nullptr;
}

class Entry {
  private:
    std::function<void(String)> outFunction = NULL;
    std::function<String()> inFunction = NULL;
    std::function<void(Entry*, String)> publishFunction = NULL;

    char* name;
    int force = 0;
    int interval = -1;
    int forceUpdate = -1;
    unsigned long lastUpdate = 0;
    unsigned long lastPublish = 0;
    char* lastValue = NULL;
    bool persist = false;

    EntryType type = text;

    Entry* parent = NULL;
    Entry* next = NULL;
    Entry* children = NULL;

    std::function<void(Entry*, String)> getPublishFunction();

  protected:
    Entry(const char* name);
    Entry *getOrCreate(const char* name);
    Entry *getRoot();
    Entry *getParent();
    Entry *setParent(Entry& parent);
    Entry* addChild(Entry* child);
    void setPublishFunction(std::function<void(Entry*, String)> function);

    String eachToString(std::function<String(Entry*)> f);

  public:
    void debug(String key, bool value);
    void debug(String key, int value);
    void debug(String key, String value);
    void debug(String msg);

    void callback(const char* topic, uint8_t* payload, unsigned int length);

    /**
     * Request a updated value if needed
     */
    void update();
    void update(String payload);

    bool isIn();
    bool isOut();
    bool isSetter();
    bool isRoot();
    bool isInternal();

    virtual String getTopic();

    int getInterval();
    void setInterval(int interval, int force = 1);

    void setPersist(bool persist);

    /**
     * Reset a persisted value
     */
    void reset();

    EntryType getType();
    void setType(EntryType type);

    int getForce();

    /**
     * Get last value
     */
    char *getValue();

    /**
     * Set the current value
     */
    void setValue(const char *value, bool callUpdate = true);
    void setValue(String value, bool callUpdate = true);

    /**
     * Update the value if change to force is true
     * This will also publis the value if it hash been changed
     */
    bool updateValue(const char *value, bool force = true, bool callUpdate = true);

    /**
     *
     */
    long getLastUpdate();

    /**
     * Publish value to mqtt
     */
    void publish(const char *message);
    void publish(String message);

    /**
     * Iterate over each child, including sub children
     */
    void each(std::function<void(Entry*)> f);

    /**
     * Check if a child with name exists
     */
    bool contains(const char* name);

    /**
     * Create or get the sub topic with the name {name}
     */
    Entry & get(const char* name);

    /**
     * Create or get the sub topic with the name {name}
     */
    Entry & operator[](int index);

    /**
     * Create or get the sub topic with the name {name}
     */
    Entry & operator[](const char* name);

    /**
     * Report value at internal, and show value in UI
     */
    void reportValue();

    /**
     *  Read data from function and send it to mqtt
     */
    void operator<<(std::function<String()> inFunction);
    void operator<<(std::function<char *()> inFunction);
    void operator<<(std::function<double()> inFunction);

    void onOff(std::function<char *()> inFunction);
    void onOff(std::function<void(String payload)> outFunction);

    void openClose(std::function<char *()> inFunction);
    void openClose(std::function<void(String payload)> outFunction);

    void color(std::function<void(uint8_t red, uint8_t green, uint8_t blue)> outFunction);

    /**
     *  Handle data comming from mqtt
     */
    void operator>>(std::function<void(String payload)> outFunction);
    void operator>>(std::function<void(long payload)> outFunction);
};
