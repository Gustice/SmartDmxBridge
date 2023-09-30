#pragma once

#include "../Nextion.hpp"
#include "Object.hpp"

namespace nxt {

class Value {
  public:
    Value(Object &parent, std::string_view key) : _parent(parent), _key(key) {}

  protected:
    Object &_parent;
    std::string_view _key;
};

class TextValue : Value {
  public:
    TextValue(Object &parent, std::string_view key) : Value(parent, key) {}

    std::string get() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return _parent._port.recvRetString();
    }
    bool set(std::string value) {
        _parent._port.sendCommand(std::string{_parent.getObjName()} + "." + _key.begin() + "=\"" + value +
                           "\"");
        return _parent._port.recvRetCommandFinished();
    }
};

class IntegerValue : public Value {
  public:
    IntegerValue(Object &parent, std::string_view key) : Value(parent, key) {}

    uint32_t get() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return _parent._port.recvRetNumber();
    }
    bool set(uint32_t value) {
        _parent._port.sendCommand(_parent.getObjName() + "." + _key.begin() + "=" + std::to_string(value));
        return _parent._port.recvRetCommandFinished();
        return true;
    }
};

class LimitedIntegerValue : public Value {
  public:
    LimitedIntegerValue(Object &parent, std::string_view key, uint32_t minVal)
        : Value(parent, key), _min(minVal) {}

    uint32_t get() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return _parent._port.recvRetNumber();
    }
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

} // namespace Nxt