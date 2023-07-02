#include "../NexHardware.h"
#include "NexIncludes.h"
#include <cstdint>
#include <string>
#include <string_view>

#pragma once

namespace Nxt {

class BackgroundImage {
  public:
    BackgroundImage(NexObject &parent, std::string_view key = "") : _parent(parent), _key(key) {}

    /**
     * Get pic attribute of component
     *
     * @return return value
     */
    uint32_t getImagePic() {
        NxtIo::sendCommand(std::string{"get "} + _parent.getObjName() + ".pic" + _key.begin());
        return NxtIo::recvRetNumber();
    }

    /**
     * Set pic attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setImagePic(uint32_t number) {
        NxtIo::sendCommand(_parent.getObjName() + ".pic" + _key.begin() + "=" + std::to_string(number));
        return NxtIo::recvRetCommandFinished();
    }

    /**
     * Get picc attribute of component
     *
     * @return return value
     */
    uint32_t getBackgroundCropPicc() {
        NxtIo::sendCommand(std::string{"get "} + _parent.getObjName() + ".picc" + _key.begin());
        return NxtIo::recvRetNumber();
    }

    /**
     * Set picc attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setBackgroundCropPicc(uint32_t number) {
        NxtIo::sendCommand(_parent.getObjName() + ".picc" + _key.begin() + "=" + std::to_string(number));
        return NxtIo::recvRetCommandFinished();
    }

  private:
    NexObject &_parent;
    std::string_view _key;
};

} // namespace Nxt