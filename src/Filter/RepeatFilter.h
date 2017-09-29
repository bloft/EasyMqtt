#ifndef Filter_h
#define Filter_h

class RepeatFilter : public Filter {
  private:
    String lastValue = NULL;
    unsigned long lastUpdate = 0;

  public:
    RepeatFilter() {
    }

    virtual String execute(String value) {
      if(value != lastValue) {
        lastValue = value;
        return value;
      }
      return "";
    }
}

#endif
