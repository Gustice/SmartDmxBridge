#pragma once

#include "../NexHardware.h"
#include "NexIncludes.h"
#include <string>
#include <string_view>

namespace Nxt {

class Value {
  public:
    Value(NexObject &parent, std::string_view key) : _parent(parent), _key(key) {}

  protected:
    NexObject &_parent;
    std::string_view _key;
};

class TextValue : Value {
  public:
    TextValue(NexObject &parent, std::string_view key) : Value(parent, key) {}

    std::string get() {
        sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return recvRetString();
    }
    bool set(std::string value) {
        sendCommand(std::string{_parent.getObjName()} + "." + _key.begin() + "=\"" + value + "\"");
        return recvRetCommandFinished();
    }
};

class IntegerValue : public Value {
  public:
    IntegerValue(NexObject &parent, std::string_view key)
        : Value(parent, key) {}

    uint32_t get() {
        sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return recvRetNumber();
    }
    bool set(uint32_t value) {
        sendCommand(_parent.getObjName() + "." + _key.begin() + "=" + std::to_string(value));
        // std::string{"ref "}
        // cmd += getObjName();
        // sendCommand(cmd);
        // return recvRetCommandFinished();
        return true;
    }
};

class LimitedIntegerValue : public Value {
  public:
    LimitedIntegerValue(NexObject &parent, std::string_view key, uint32_t minVal)
        : Value(parent, key), _min(minVal) {}

    uint32_t get() {
        sendCommand(std::string{"get "} + _parent.getObjName() + "." + _key.begin());
        return recvRetNumber();
    }
    bool set(uint32_t value) {
        if (value < _min) {
            value = _min;
        }

        sendCommand(_parent.getObjName() + "." + _key.begin() + "=" + std::to_string(value));
        // std::string{"ref "}
        // cmd += getObjName();
        // sendCommand(cmd);
        // return recvRetCommandFinished();
        return true;
    }

  private:
    uint32_t _min;
};

} // namespace Nxt