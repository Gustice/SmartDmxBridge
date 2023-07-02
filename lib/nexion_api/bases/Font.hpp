#include "NexHardware.h"
#include "NexIncludes.h"
#include <cstdint>
#include <string>
#include <string_view>

#pragma once

namespace Nxt {

class Font {
  public:
    Font(NexObject &parent) : _parent(parent) {}

    /**
     * Get font attribute of component
     *
     * @return returns value
     */
    uint32_t getFont() {
        NxtIo::sendCommand(std::string{"get "} + _parent.getObjName() + ".font");
        return NxtIo::recvRetNumber();
    }

    /**
     * Set font attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setFont(uint32_t number) {
        NxtIo::sendCommand(_parent.getObjName() + ".font=" + std::to_string(number));
        return NxtIo::recvRetCommandFinished();
    }

  private:
    NexObject &_parent;
};

}