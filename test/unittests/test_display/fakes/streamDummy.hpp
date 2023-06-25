#include "Streams.hpp"
#include <string_view>

class StreamDummy : public CharStream {
    public:
    std::string read() override {

    }
    void write(char c) override {

    }
    void write(std::string str) override {
        lastWrite = str;
    }


    std::string lastWrite;
};