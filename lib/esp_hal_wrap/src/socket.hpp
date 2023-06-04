#pragma once

#include "charStream.hpp"

class Socket : public CharStream {
  public:
    struct Config {
        int keepAlive;
        int keepIdle;
        int keepInterval;
        int keepCount;
    };

    Socket(Config &config, int listener);
    ~Socket();

    std::string read() override;
    void write(char c) override;

  private:
    int _port;
    int _socket = 0;
    uint8_t _data[128];
    char _addr_str[128];
};
