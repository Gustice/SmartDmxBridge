#pragma once

#include "../bases/Object.hpp"
#include <vector>

namespace nxt {

class Page;

/**
 * @brief Base of the components with touch events
 */
class Touch : public Object {
  public:
    /// @brief Type of touch event
    enum class Event {
        /// Pop touch event on release panel.
        Pop = 0,
        /// Push touch event on push
        Push = 1,
    };

    /// @brief Type of callback function when an touch event occurs.
    using eventCb = void (*)(void *ptr);

    /// @brief List of elements that should be checkt for action
    using SensingList = std::vector<Touch *>;

    /// @brief Iterate events
    /// @param elements elements to be checked
    /// @param pId propagated pageId
    /// @param cId propagated componentId
    /// @param event propagated event
    /// @return true if successful, false if not found
    static bool iterate(const SensingList &elements, uint8_t pId, uint8_t cId,
                        int32_t event);

    /// @copydoc Object::Object(Page &page, Port &port, uint8_t cId, std::string_view name);
    Touch(Page &page, Port &port, uint8_t cid, std::string_view name)
        : Object(page, port, cid, name) {
    }

    /// @brief Constructor for pages
    /// @param name component name
    /// @param port reference to port
    Touch(std::string_view name, Port &port)
        : Object(name, port) {
    }

    /// @brief Attach one callback function of push touch event
    /// @param push callback called with ptr when a push touch event occurs
    /// @param ptr parameter passed into push
    void attachPush(eventCb push, void *ptr = nullptr);

    /// @brief Detach an callback function for push-event
    void detachPush();

    /// @brief Attach one callback function of pop touch event
    /// @param pop callback called with ptr when a pop touch event occurs
    /// @param ptr parameter passed into pop
    void attachPop(eventCb pop, void *ptr = nullptr);

    /// @brief Detach an callback function for pop-event
    void detachPop();

  private:
    void push();
    void pop();

  private:
    eventCb _pushCb = nullptr;
    void *_ptrForPushCb = nullptr;
    eventCb _popCb = nullptr;
    void *_ptrForPopCb = nullptr;
};

} // namespace nxt