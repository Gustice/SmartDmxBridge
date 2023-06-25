#include "NexHardware.h"
#include "NexIncludes.h"
#include <cstdint>
#include <string>
#include <string_view>

#pragma once

class NexAlignment {
  public:
    NexAlignment(NexObject &parent) : _parent(parent) {}

    uint32_t getPlaceXcen(uint32_t *number) {
        sendCommand(std::string{"get "} + _parent.getObjName() + ".xcen");
        return recvRetNumber();
    }

    bool setPlaceXcen(uint32_t number) {
        sendCommand(_parent.getObjName() + ".xcen=" + std::to_string(number));
        // cmd = "";
        // cmd += "ref ";
        // cmd += _parent.getObjName();
        // sendCommand(cmd);
        // return recvRetCommandFinished();
    }

    uint32_t getPlaceYcen(uint32_t *number) {
        sendCommand(std::string{"get "} + _parent.getObjName() + ".ycen");
        return recvRetNumber();
    }

    bool setPlaceYcen(uint32_t number) {
        sendCommand(_parent.getObjName() + ".ycen=" + std::to_string(number));
        // cmd = "";
        // cmd += "ref ";
        // cmd += _parent.getObjName();
        // sendCommand(cmd);
        // return recvRetCommandFinished();
    }

  private:
    NexObject &_parent;
};

