#pragma once

#include "NexHardware.h"

/**
 * @addtogroup Component
 * @{
 */

/**
 * A special component , which can contain other components such as NexButton,
 * NexText and NexWaveform, etc.
 */
class NexPage : public Nxt::Touch {
  public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string_view name);
     */
    NexPage(uint8_t pid, uint8_t cid, std::string_view name) : Nxt::Touch(pid, cid, name) {}

    /**
     * Show itself.
     *
     * @return true if success, false for faileure.
     */
    bool show(void) {
        NxtIo::sendCommand(std::string("page ") + getObjName());
        return NxtIo::recvRetCommandFinished();
    }
};
/**
 * @}
 */
