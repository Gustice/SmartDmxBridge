#pragma once

#include "bases/Port.hpp"

namespace nxt {

class BackgroundImage {
  public:
    BackgroundImage(Object &parent, std::string_view key = "") : _parent(parent), _key(key) {}

    /**
     * Get pic attribute of component
     *
     * @return return value
     */
    uint32_t getImagePic() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + ".pic" + _key.begin());
        return _parent._port.recvRetNumber();
    }

    /**
     * Set pic attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setImagePic(uint32_t number) {
        _parent._port.sendCommand(_parent.getObjName() + ".pic" + _key.begin() + "=" + std::to_string(number));
        return _parent._port.recvRetCommandFinished();
    }

    /**
     * Get picc attribute of component
     *
     * @return return value
     */
    uint32_t getBackgroundCropPicc() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + ".picc" + _key.begin());
        return _parent._port.recvRetNumber();
    }

    /**
     * Set picc attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setBackgroundCropPicc(uint32_t number) {
        _parent._port.sendCommand(_parent.getObjName() + ".picc" + _key.begin() + "=" + std::to_string(number));
        return _parent._port.recvRetCommandFinished();
    }

  private:
    Object &_parent;
    std::string_view _key;
};

} // namespace Nxt