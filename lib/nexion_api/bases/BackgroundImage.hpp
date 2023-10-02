#pragma once

#include "bases/Port.hpp"
#include "Object.hpp"

namespace nxt {

/**
 * @brief Background image property of Control
 */
class BackgroundImage {
  public:
    /// @brief Constructor
    /// @param parent Reference to parent Control
    /// @param key Property name
    BackgroundImage(Object &parent, std::string_view key = "")
        : _parent(parent), _key(key) {}

    /// @brief Getter for ImageId
    /// @return ImageId
    uint32_t getImagePic() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + ".pic" + _key.begin());
        return _parent._port.recvRetNumber();
    }

    /// @brief Setter for ImageId
    /// @param imageId ImageId
    /// @return success flag
    bool setImagePic(uint32_t imageId) {
        _parent._port.sendCommand(_parent.getObjName() + ".pic" + _key.begin() + "=" + std::to_string(imageId));
        return _parent._port.recvRetCommandFinished();
    }

    /// @brief Getter for CropImageId
    /// @return ImageId
    uint32_t getBackgroundCropPicc() {
        _parent._port.sendCommand(std::string{"get "} + _parent.getObjName() + ".picc" + _key.begin());
        return _parent._port.recvRetNumber();
    }

    /// @brief Setter for CropImageId
    /// @param imageId ImageId
    /// @return success flag
    bool setBackgroundCropPicc(uint32_t imageId) {
        _parent._port.sendCommand(_parent.getObjName() + ".picc" + _key.begin() + "=" + std::to_string(imageId));
        return _parent._port.recvRetCommandFinished();
    }

  private:
    Object &_parent;
    std::string_view _key;
};

} // namespace nxt