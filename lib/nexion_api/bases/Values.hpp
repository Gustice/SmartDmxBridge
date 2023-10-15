#pragma once

#include "Nextion.hpp"
#include "Object.hpp"

namespace nxt {

/**
 * @brief Generic Base for Component properties
 */
class Value {
  public:
    /// @brief Constructor
    /// @param parent Reference to parent Control
    /// @param key Property name
    Value(Object &parent, std::string_view key)
        : _parent(parent), _key(key) {}

  protected:
    Object &_parent;
    std::string_view _key;
};

/**
 * @brief Generic Text property
 */
class TextValue : Value {
  public:
    /// @brief Constructor
    /// @param parent Reference to parent Control
    /// @param key Property name
    TextValue(Object &parent, std::string_view key)
        : Value(parent, key) {}

    /// @brief Getter for string-property
    /// @return Property value as string
    std::string get() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return _parent._port.recvRetString();
    }

    /// @brief Setter for string-property
    /// @param value value
    /// @return success flag
    bool set(std::string value) {
        _parent._port.sendCommand(std::string{_parent.getObjName()} + "." + _key.begin() + "=\"" + value +
                                  "\"");
        return _parent._port.recvRetCommandFinished();
    }
};

/**
 * @brief Generic Integer property
 */
class IntegerValue : public Value {
  public:
    /// @brief Constructor
    /// @param parent Reference to parent Control
    /// @param key Property name
    IntegerValue(Object &parent, std::string_view key)
        : Value(parent, key) {}

    /// @brief Getter for integer-property
    /// @return Property value as integer
    uint32_t get() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return _parent._port.recvRetNumber();
    }

    /// @brief Setter for integer-property
    /// @param value value
    /// @return success flag
    bool set(uint32_t value) {
        _parent._port.sendCommand(_parent.getObjName() + "." + _key.begin() + "=" + std::to_string(value));
        return _parent._port.recvRetCommandFinished();
        return true;
    }
};

/**
 * @brief Generic Integer property with constraints
 */
class LimitedIntegerValue : public Value {
  public:
    /// @brief Constructor
    /// @param parent Reference to parent Control
    /// @param key Property name
    /// @param minVal minimal allowed value
    LimitedIntegerValue(Object &parent, std::string_view key, uint32_t minVal)
        : Value(parent, key), _min(minVal) {}

    /// @brief Getter for integer-property
    /// @return Property value as integer
    uint32_t get() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return _parent._port.recvRetNumber();
    }

    /// @brief Setter for integer-property
    /// @param value value
    /// @return success flag
    bool set(uint32_t value) {
        if (value < _min) {
            value = _min;
        }

        _parent._port.sendCommand(_parent.getObjName() + "." + _key.begin() + "=" + std::to_string(value));
        return _parent._port.recvRetCommandFinished();
        return true;
    }

  private:
    uint32_t _min;
};

} // namespace nxt