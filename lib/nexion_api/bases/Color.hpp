#include "../NexHardware.h"
#include "NexIncludes.h"
#include "Values.hpp"
#include <cstdint>
#include <string>
#include <string_view>

#pragma once

namespace Nxt {

class Color : public IntegerValue {
  public:
    Color(NexObject &parent, std::string_view key) : IntegerValue(parent, key) {}

    /**
     * Color-getter
     */
    uint32_t getColor() {
        sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return recvRetNumber();
    }

    /**
     * Color-setter
     * @return true if success, false for failure
     */
    bool setColor(uint32_t number) {
        sendCommand(_parent.getObjName() + "." + _key.begin() + "=" + std::to_string(number));
        // cmd = "";
        // cmd += "ref ";
        // cmd += getObjName();
        // sendCommand(cmd);
        // return recvRetCommandFinished();
        return true;
    }

    static uint32_t calcNextionColor(uint8_t r, uint8_t g, uint8_t b) {
        // red = 63488 / F800
        // green = 2016 /  07E0
        // blue = 31 / 1F
        return scaleColorDown(b, 0x1F) | (scaleColorDown(g, 0x3F) << 5) |
               (scaleColorDown(r, 0x1F) << (5 + 6));
    }

  private:
    static uint32_t scaleColorDown(uint8_t val, uint8_t max) {
        return (uint8_t)((uint32_t)val * max / 0xFF);
    }
};

}