#ifndef Filter_h
#define Filter_h

class Filter {
  public:
    Filter() {}

    virtual String execute(String value) {
      return value;
    }
}

#endif
