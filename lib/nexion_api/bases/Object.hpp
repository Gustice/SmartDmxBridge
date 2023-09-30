#pragma once

#include "bases/Port.hpp"

namespace nxt {

/**
 * @addtogroup CoreAPI
 * @{
 */

class Page;

/**
 * Root class of all Nextion components.
 *
 * Provides the essential attributes of a Nextion component and the methods accessing
 * them. At least, Page ID(pid), Component ID(pid) and an unique name are needed for
 * creating a component in Nexiton library.
 */
class Object {
  public: /* methods */
    /**
     * Constructor.
     *
     * @param pid - page id.
     * @param cid - component id.
     * @param name - pointer to an unique name in range of all components.
     */
    Object(Page &pid, Port & port, uint8_t cid, std::string_view name)
        : pageRef(pid), _port(port), _componentId(cid), _name(name) {}

    /**
     * Constructor for pages
     *
     * @param name - pointer to an unique name in range of all components.
     */
    Object(std::string_view name, Port & port)
        : pageRef(*(reinterpret_cast<Page *>(this))), _port(port),  _componentId(0), _name(name) {}

    /**
     * Print current object'address, page id, component id and name.
     *
     * @warning this method does nothing, unless debug message enabled.
     */
    std::string printObjInfo(void);

    /**
     * Get component name.
     *
     * @return the name of component.
     */
    std::string getObjName(void);

    Page &pageRef;
    Port &_port;

  protected: /* methods */
    /*
     * Get component id.
     *
     * @return the id of component.
     */
    uint8_t getObjCid(void) {
        return _componentId;
    }


  private: /* data */
    uint8_t _componentId;
    std::string _name;
};
/**
 * @}
 */

} // namespace nxt