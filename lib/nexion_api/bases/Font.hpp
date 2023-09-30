#pragma once

#include "bases/Port.hpp"
#include "Object.hpp"

namespace nxt {

class Font {
  public:
    Font(Object &parent) : _parent(parent) {}

    /**
     * Get font attribute of component
     *
     * @return returns value
     */
    uint32_t getFont() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + ".font");
        return _parent._port.recvRetNumber();
    }

    /**
     * Set font attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setFont(uint32_t number) {
        _parent._port.sendCommand(_parent.getObjName() + ".font=" + std::to_string(number));
        return _parent._port.recvRetCommandFinished();
    }

  private:
    Object &_parent;
};

}