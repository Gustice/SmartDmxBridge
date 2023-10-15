#pragma once

#include "../Nextion.hpp"
#include "Object.hpp"
#include "Values.hpp"

namespace nxt {

/**
 * @brief Color property and utils
 */
class Color : public IntegerValue {
  public:
    /// @brief Constructor
    /// @param parent Reference to parent Control
    /// @param key Property name
    Color(Object &parent, std::string_view key)
        : IntegerValue(parent, key) {}

    /// @brief Getter for color-code
    /// @return color-code
    uint32_t getColor() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return _parent._port.recvRetNumber();
    }

    /// @brief Setter for color-code
    /// @param number color-code
    /// @return success flag
    bool setColor(uint32_t code) {
        _parent._port.sendCommand(_parent.getObjName() + "." + _key.begin() + "=" + std::to_string(code));
        return _parent._port.recvRetCommandFinished();
    }

    /// @brief Convert-function from RGB to color code
    /// @param r, g, b Red/Green/Blu values 0..UINT8_MAX
    /// @return success flag
    static uint32_t calcColor(uint8_t r, uint8_t g, uint8_t b) {
        constexpr uint8_t R_MAX = 0x1F;
        constexpr uint8_t G_MAX = 0x3F;
        constexpr uint8_t B_MAX = 0x1F;
        // red = 63488 / F800
        // green = 2016 /  07E0
        // blue = 31 / 1F
        return scaleColorDown(b, B_MAX) | (scaleColorDown(g, G_MAX) << 5) |
               (scaleColorDown(r, R_MAX) << (5 + 6));
    }

  private:
    /// @brief Convert-function from RGB to color code
    /// @param r, g, b Red/Green/Blu values
    /// @return success flag
    static uint32_t scaleColorDown(uint8_t val, uint8_t max) {
        return (uint8_t)((uint32_t)val * max / UINT8_MAX);
    }
};

} // namespace nxt