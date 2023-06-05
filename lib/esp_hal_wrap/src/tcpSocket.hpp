#pragma once

#include "streams.hpp"

class TcpSocket : public CharStream, public VolatileStream {
  public:
    struct Config {
        int keepAlive;
        int keepIdle;
        int keepInterval;
        int keepCount;
    };

    TcpSocket(Config &config, int listener);
    ~TcpSocket();

    std::string read() override;
    void write(char c) override;
    void write(std::string str) override;

  private:
    int _port;
    int _socket = 0;
    uint8_t _data[128];
    char _addr_str[128];
};
