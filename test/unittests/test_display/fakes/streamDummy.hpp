#include "Nextion.hpp"
#include <algorithm>
#include <queue>
#include <string_view>

class StreamDummy : public nxt::Stream {
  public:
    std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) override {
        if (nextReadQueue.size() == 0) {
            return {};
        }

        auto ret = nextReadQueue.front();
        nextReadQueue.pop();
        return ret;
    }

    void write(char c) override {}
    void write(std::string str) override {
        lastWrite = str;
    }

    void appendNextRead(nxt::Return type, std::vector<uint8_t> payload) {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(type));
        buffer.insert(buffer.end(), payload.begin(), payload.end());
        appendTerminator(buffer);
        nextReadQueue.push(buffer);
    }

    void appendNextRead(std::vector<uint8_t> buffer){
        nextReadQueue.push(buffer);
    }

    std::string lastWrite;

  private:
    std::queue<std::vector<uint8_t>> nextReadQueue;

    void appendTerminator(std::vector<uint8_t> &buffer) {
        buffer.push_back(0xFFu);
        buffer.push_back(0xFFu);
        buffer.push_back(0xFFu);
    }
};