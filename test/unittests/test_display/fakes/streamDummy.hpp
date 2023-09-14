#include "Nextion.h"
#include <string_view>

class StreamDummy : public SerialStream {
  public:
    std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) override {
        return nextRead;
    }

    void write(char c) override {}
    void write(std::string str) override {
        lastWrite = str;
    }

    std::string lastWrite;
    std::vector<uint8_t> nextRead{};
};