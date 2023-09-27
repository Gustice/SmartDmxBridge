#include "Nextion.h"
#include <string_view>

class StreamDummy : public SerialStream {
  public:
    std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) override {
        auto ret = nextRead;
        nextRead = std::vector<uint8_t>();
        return ret;
    }

    void write(char c) override {}
    void write(std::string str) override {
        lastWrite = str;
    }

    void setNextRead(NxtIo::Return type, std::vector<uint8_t> payload = {}) {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(type));
        for (auto &&e : payload) {
            buffer.push_back(e);
        }
        buffer.push_back(0xFFu);
        buffer.push_back(0xFFu);
        buffer.push_back(0xFFu);
        nextRead = buffer;
    }

    void setNextRead(std::vector<uint8_t> buffer) {
        nextRead = buffer;
    }

    std::string lastWrite;

  private:
    std::vector<uint8_t> nextRead{};
};