#include "NexHardware.h"
#include "NexIncludes.h"
#include <cstdint>
#include <string>
#include <string_view>

#pragma once

class NexFont {
  public:
    NexFont(NexObject &parent) : _parent(parent) {}

    uint32_t getPlaceXcen() {
        sendCommand(std::string{"get "} + _parent.getObjName() + ".xcen");
        return recvRetNumber();
    }

    uint32_t getFont()
    {
        sendCommand(std::string{"get "} + _parent.getObjName() + ".font");
        return recvRetNumber();
    }

    bool setFont(uint32_t number)
    {
        sendCommand(_parent.getObjName() + ".font=" + std::to_string(number));

        // cmd = "";
        // cmd += "ref ";
        // cmd += getObjName();
        // sendCommand(cmd);
        // return recvRetCommandFinished();
    }

  private:
    NexObject &_parent;
};