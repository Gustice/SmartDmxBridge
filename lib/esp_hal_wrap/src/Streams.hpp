#pragma once

#include <string>
#include <vector>

class VolatileStream {
  public: 
    virtual bool isActive() {
        return active;
    }

  protected:
    bool active;
};

class CharStream {
  public:
    virtual std::string read() = 0;
    virtual void write(char c) = 0;
    virtual void write(std::string str) = 0;
};

class ByteStream {
    virtual std::vector<uint8_t> read() = 0;
    virtual void write(uint8_t b) = 0;
    virtual void write(std::vector<uint8_t> data) = 0;
};