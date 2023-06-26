#include "../NexHardware.h"
#include "NexIncludes.h"
#include <cstdint>
#include <string>
#include <string_view>

#pragma once

class NexAlignment {
  public:
    NexAlignment(NexObject &parent) : _parent(parent) {}

    /**
     * Get xcen attribute of component
     *
     * @return returns value
     */
    uint32_t getPlaceXcen() {
        sendCommand(std::string{"get "} + _parent.getObjName() + ".xcen");
        return recvRetNumber();
    }

    /**
     * Set xcen attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setPlaceXcen(uint32_t number) {
        sendCommand(_parent.getObjName() + ".xcen=" + std::to_string(number));
        // cmd = "";
        // cmd += "ref ";
        // cmd += _parent.getObjName();
        // sendCommand(cmd);
        // return recvRetCommandFinished();
    }

    /**
     * Get ycen attribute of component
     *
     * @return returns value
     */
    uint32_t getPlaceYcen() {
        sendCommand(std::string{"get "} + _parent.getObjName() + ".ycen");
        return recvRetNumber();
    }

    /**
     * Set ycen attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
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

