#include <string_view>
#include "Nextion.h"

class StreamDummy : public SerialStream {
    public:
    std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) override {
        return {};
    }
    
    void write(char c) override {

    }
    void write(std::string str) override {
        lastWrite = str;
    }


    std::string lastWrite;
};