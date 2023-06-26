#include "../NexHardware.h"
#include "NexIncludes.h"
#include <cstdint>
#include <string>
#include <string_view>

#pragma once

class NexColor {
  public:
    NexColor(NexObject &parent, std::string_view key) : _key(key), _parent(parent) {}

    std::string getKey() {
        return std::string(_key);
    }

    /**
     * Get bco attribute of component
     *
     * @return return value 
     */
    uint32_t getColor() {
        sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return recvRetNumber();
    }

    /**
     * Set bco attribute of component
     *
     * @param number - To set up the data
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
    std::string_view _key;
    NexObject &_parent;

    static uint32_t scaleColorDown(uint8_t val, uint8_t max) {
        return (uint8_t)((uint32_t)val * max / 0xFF);
    }
};

