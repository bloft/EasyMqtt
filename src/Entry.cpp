#include "Entry.h"

void Entry::setName(const char* name) {
  Entry::name = (char*)malloc(strlen(name)+1);
  strncpy(Entry::name, name, strlen(name)+1);
}

std::function<void(Entry*, String)> Entry::getPublishFunction() {
  if(publishFunction == NULL && parent) {
    return parent->getPublishFunction();
  }
  return publishFunction;
}

Entry::Entry(const char* name) {
  setName(name);
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
  debug(key + " = " + (value ? "true" : "false"));
}

void Entry::debug(String key, int value) {
  debug(key + " = " + value);
}

void Entry::debug(String key, String value) {
  debug(key + " = " + value);
}

void Entry::debug(String msg) {
#ifdef DEBUG
  Serial.println(msg);
#endif
  getRoot()->get("$system")["debug"].publish(msg);
}

void Entry::callback(const char* topic, uint8_t* payload, unsigned int length) {
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

void Entry::update() {
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

void Entry::update(String payload) {
  if (isOut()) {
    outFunction(payload);
  }
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

void Entry::setInterval(int interval) {
  this->interval = interval;
}

void Entry::setInterval(int interval, int force) {
  this->interval = interval;
  forceUpdate = force;
}

int Entry::getForce() {
  if(forceUpdate < 0) {
    return parent->getForce();
  }
  return forceUpdate;
}

String Entry::getValue() {
  return lastValue;
}

void Entry::getCValue(char * destination) {
  strcpy(destination, lastValue.c_str());
}

void Entry::setValue(String value) {
  lastUpdate = millis();
  lastValue = value;
  publish(value);
}

long Entry::getLastUpdate() {
  return lastUpdate;
}

void Entry::publish(String message) {
  auto function = getPublishFunction();
  if(function) {
    function(this, message);
  }
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
  Entry::inFunction = inFunction;
}

void Entry::operator>>(std::function<void(String payload)> outFunction) {
  Entry::outFunction = outFunction;
}
