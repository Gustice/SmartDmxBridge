#pragma once

#include "../bases/Object.hpp"
#include <vector>

namespace nxt {

class Page;

/**
 * @addtogroup TouchEvent
 * @{
 */


/// @brief Base of the components with touch events.
class Touch : public Object {
  public:
    
    /// @brief Type of touch event
    enum class Event {
        Pop = 0,  ///< Pop touch event on release
                  ///< panel.
        Push = 1, ///< Push touch event on push
    };

    /// @brief Type of callback function when an touch event occurs.
    using eventCb = void (*)(void *ptr);


    /// @brief List of elements that should be checkt for action
    using SensingList = std::vector<Touch *>;
    /// @brief Iterate events
    /// @param elements elements to be checked
    /// @param page propagated page id
    /// @param cid propagated component id
    /// @param event propagated event
    /// @return true if successful, false if not found
    static bool iterate(const SensingList &elements, uint8_t page, uint8_t cid,
                        int32_t event);

    /**
     * @copydoc Object::Object(uint8_t page, uint8_t cid, std::string_view name);
     */
    Touch(Page & page, Port & port, uint8_t cid, std::string_view name) 
    : Object(page, port, cid, name) {
      
    }

    Touch(std::string_view name, Port & port) : Object(name, port) {
      
    }

    /**
     * Attach an callback function of push touch event.
     *
     * @param push - callback called with ptr when a push touch event occurs.
     * @param ptr - parameter passed into push[default:NULL].
     * @return none.
     *
     * @note If calling this method multiply, the last call is valid.
     */
    void attachPush(eventCb push, void *ptr = nullptr);

    /**
     * Detach an callback function.
     *
     * @return none.
     */
    void detachPush(void);

    /**
     * Attach an callback function of pop touch event.
     *
     * @param pop - callback called with ptr when a pop touch event occurs.
     * @param ptr - parameter passed into pop[default:NULL].
     * @return none.
     *
     * @note If calling this method multiply, the last call is valid.
     */
    void attachPop(eventCb pop, void *ptr = nullptr);

    /**
     * Detach an callback function.
     *
     * @return none.
     */
    void detachPop(void);

  private: /* methods */
    void push(void);
    void pop(void);

  private: /* data */
    eventCb _pushCb = nullptr;
    void *_ptrForPushCb = nullptr;
    eventCb _popCb = nullptr;
    void *_ptrForPopCb = nullptr;
};

/**
 * @}
 */

} // namespace Nxt