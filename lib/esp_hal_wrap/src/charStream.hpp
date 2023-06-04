#pragma once

#include <string>

class CharStream {
  public:
    virtual std::string read() = 0;
    virtual void write(char c) = 0;
    virtual void write(std::string str) = 0;
    bool isActive() {
        return active;
    }

  protected:
    bool active;
};