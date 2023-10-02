#pragma once

#include "Object.hpp"
#include "bases/Port.hpp"

namespace nxt {

/**
 * @brief Font property of Control
 * 
 */
class Font {
  public:
    /// @brief Constructor
    /// @param parent Reference to parent Control
    Font(Object &parent)
        : _parent(parent) {}

    /// @brief Getter for FontId
    /// @return FontId
    uint32_t getFont() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + ".font");
        return _parent._port.recvRetNumber();
    }

    /// @brief Setter for FontId
    /// @param fontId
    /// @return
    bool setFont(uint32_t fontId) {
        _parent._port.sendCommand(_parent.getObjName() + ".font=" + std::to_string(fontId));
        return _parent._port.recvRetCommandFinished();
    }

  private:
    Object &_parent;
};

} // namespace nxt