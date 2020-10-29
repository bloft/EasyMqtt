#include "Entry.h"
#include <FS.h>

#define DEBUG

std::function<void(Entry*, String)> Entry::getPublishFunction() {
  if(publishFunction == NULL && parent) {
    return parent->getPublishFunction();
  }
  return publishFunction;
}

Entry::Entry(const char* name) {
  Entry::name = (char*)malloc(strlen(name)+1);
  strcpy(Entry::name, name);
}

bool Entry::contains(const char* name) {
  Entry *child = children;
  while (child != NULL) {
    if (strcmp(child->name, name) == 0) {
      return true;
    }
    child = child->next;
  }
  return false;
}

Entry *Entry::getOrCreate(const char* name) {
  Entry *child = children;
  while (child != NULL) {
    if (strcmp(child->name, name) == 0) {
      return child;
    }
    child = child->next;
  }
  return addChild(new Entry(name));
}

Entry *Entry::getRoot() {
  if(parent) {
    return parent->getRoot();
  } else {
    return this;
  }
}

Entry *Entry::getParent() {
  return parent;
}

Entry *Entry::setParent(Entry& parent) {
  Entry::parent = &parent;
  return this;
}

Entry* Entry::addChild(Entry* child) {
  child->parent = this;
  child->next = children;
  children = child;
  return child;
}

void Entry::setPublishFunction(std::function<void(Entry*, String)> function) {
  publishFunction = function;
}

void Entry::debug(String key, bool value) {
#ifdef DEBUG
  debug(key + " = " + (value ? "true" : "false"));
#endif
}

void Entry::debug(String key, int value) {
#ifdef DEBUG
  debug(key + " = " + value);
#endif
}

void Entry::debug(String key, String value) {
#ifdef DEBUG
  debug(key + " = " + value);
#endif
}

void Entry::debug(String msg) {
#ifdef DEBUG
  Serial.println(msg);
#endif
}

void Entry::callback(const char* topic, uint8_t* payload, unsigned int length) {
  if (strcmp(getTopic().c_str(), topic) == 0) {
    String _payload = "";
    for (unsigned int i = 0; i < length; i++) {
      _payload += (char)payload[i];
    }
    if(!isIn() || (millis() - lastPublish) > 1000 || strcmp(lastValue, _payload.c_str()) != 0) {
      setValue(_payload, true);
    }
  }
}

void Entry::update() {
  if (isIn()) {
    unsigned long time = millis();
    if (time >= (lastUpdate + (getInterval() * 1000)) || lastUpdate == 0) {
      force++;
      lastUpdate = time;
      String value = inFunction();
      if (value != "") {
          if(updateValue(value.c_str(), force > getForce(), false)) {
            force = 0;
          }
      }
    }
  }
}

void Entry::update(String payload) {
  if (isOut()) {
    outFunction(payload);
  }
}

bool Entry::isSetter() {
  return isOut() && strcmp(name, "set") == 0;
}

bool Entry::isIn() {
  return inFunction != NULL;
}

bool Entry::isOut() {
  return outFunction != NULL;
}

bool Entry::isRoot() {
  return parent == NULL;
}

bool Entry::isInternal() {
  bool internal = name[0] == '$';
  if (parent) {
    return internal || parent->isInternal();
  } else {
    return internal;
  }
}

String Entry::getTopic() {
  if (parent) {
    return parent->getTopic() + "/" + name;
  } else {
    return String(name);
  }
}

int Entry::getInterval() {
  if (interval < 0) {
    return parent->getInterval();
  }
  return interval;
}

void Entry::setInterval(int interval, int force) {
  this->interval = interval;
  forceUpdate = force;
}

void Entry::setPersist(bool persist) {
  this->persist = persist;
  if(persist) {
    File f = SPIFFS.open(getTopic(), "r");
    if (f) {
      setValue(f.readStringUntil('\n').c_str(), true);
      f.close();

    }
  }
}

void Entry::reset() {
  SPIFFS.remove(getTopic());
}

EntryType Entry::getType() {
  return type;
}

void Entry::setType(EntryType type) {
  this->type = type;
}

int Entry::getForce() {
  if(forceUpdate < 0) {
    return parent->getForce();
  }
  return forceUpdate;
}

char *Entry::getValue() {
  return lastValue;
}

bool Entry::updateValue(const char *value, bool force, bool callUpdate) {
  if(force || !lastValue || strcmp(value, lastValue) != 0) {
    setValue(value, callUpdate);
    publish(value);
    return true;
  }
  return false;
}

void Entry::setValue(const char *value, bool callUpdate) {
  lastUpdate = millis();
  if(lastValue) {
    free(lastValue);
  }
  lastValue = (char*)malloc(strlen(value)+1);
  strcpy(lastValue, value);

  if(callUpdate) {
    update(String(lastValue));
  }

  if(persist) {
    File f = SPIFFS.open(getTopic(), "w");
    f.println(value);
    f.close();
  }
}

void Entry::setValue(String value, bool callUpdate) {
  setValue(value.c_str(), callUpdate);
}

long Entry::getLastUpdate() {
  return lastUpdate;
}

void Entry::publish(const char *message) {
  auto function = getPublishFunction();
  if(function) {
    lastPublish = millis();
    function(this, String(message));
  }
}

void Entry::publish(String message) {
  publish(message.c_str());
}

String Entry::eachToString(std::function<String(Entry*)> f) {
  String res = "";
  res += f(this);
  Entry* child = children;
  while (child != NULL) {
    res += child->eachToString(f);
    child = child->next;
  }
  return res;
}

void Entry::each(std::function<void(Entry*)> f) {
  f(this);
  Entry* child = children;
  while (child != NULL) {
    child->each(f);
    child = child->next;
  }
}

Entry & Entry::get(const char* name) {
  char *subName = strtok((char *)name, "/");
  Entry *entry = this;
  while(subName != NULL) {
    entry = entry->getOrCreate(subName);
    subName = strtok(NULL, "/");
  }
  return *entry;
}

void Entry::reportValue() {
  inFunction = [&]() {
    return String(getValue());
  };
}

Entry & Entry::operator[](int index) {
  int numOfDigits = log10(index) + 1;
  char* arr = (char*)calloc(numOfDigits, sizeof(char));
  itoa(index, arr, 10);
  return *getOrCreate(arr);
}

Entry & Entry::operator[](const char* name) {
  return *getOrCreate(name);
}

void Entry::operator<<(std::function<String()> inFunction) {
  type = EntryType::text;
  Entry::inFunction = inFunction;
}

void Entry::operator<<(std::function<char *()> inFunction) {
  type = EntryType::text;
  Entry::inFunction = [&, inFunction]() {
    return String(inFunction());
  };
}

void Entry::operator<<(std::function<double()> inFunction) {
  type = EntryType::number;
  Entry::inFunction = [&, inFunction]() {
    double value = inFunction();
    if(isnan(value)) {
      return String("");
    } else {
      return String(value);
    }
  };
}

void Entry::onOff(std::function<char *()> inFunction) {
  type = EntryType::onOff;
  Entry::inFunction = [&, inFunction]() {
    char *value = inFunction();
    // ToDo: Validate value
    return value;
  };
}

void Entry::onOff(std::function<void(String payload)> outFunction) {
  type = EntryType::onOff;
  Entry::outFunction = outFunction;
}

void Entry::openClose(std::function<char *()> inFunction) {
  type = EntryType::openClose;
  Entry::inFunction = [&, inFunction]() {
    char *value = inFunction();
    // ToDo: Validate value
    return value;
  };
}

void Entry::openClose(std::function<void(String payload)> outFunction) {
  type = EntryType::openClose;
  Entry::outFunction = outFunction;
}

void Entry::color(std::function<void(uint8_t red, uint8_t green, uint8_t blue)> outFunction) {
  type = EntryType::colorRGB;
  Entry::outFunction = [&, outFunction](String payload) {
    int commaIndex = payload.indexOf(',');
    int secondCommaIndex = payload.indexOf(',', commaIndex + 1);

    String firstValue = payload.substring(0, commaIndex);
    String secondValue = payload.substring(commaIndex + 1, secondCommaIndex);
    String thirdValue = payload.substring(secondCommaIndex + 1);

    outFunction(firstValue.toInt(), secondValue.toInt(), thirdValue.toInt());
  };
}

void Entry::operator>>(std::function<void(String payload)> outFunction) {
  Entry::outFunction = outFunction;
}

void Entry::operator>>(std::function<void(long payload)> outFunction) {
  type = EntryType::number;
  Entry::outFunction = [&, outFunction](String payload) {
    outFunction(payload.toInt());
  };
}
