#pragma once
#include "NexIncludes.h"
#include <sstream>

/**
 * @addtogroup CoreAPI 
 * @{ 
 */

/**
 * Root class of all Nextion components. 
 *
 * Provides the essential attributes of a Nextion component and the methods accessing
 * them. At least, Page ID(pid), Component ID(pid) and an unique name are needed for
 * creating a component in Nexiton library. 
 */
class NexObject 
{
public: /* methods */

    /**
     * Constructor. 
     *
     * @param pid - page id. 
     * @param cid - component id.    
     * @param name - pointer to an unique name in range of all components. 
     */
    NexObject(uint8_t pid, uint8_t cid, std::string_view name) 
    : _pageId(pid), _componentId(cid), __name(name) {}

    /**
     * Print current object'address, page id, component id and name. 
     *
     * @warning this method does nothing, unless debug message enabled. 
     */
    void printObjInfo(void) {
        std::stringstream output;
        output << "[" << this << ":" << _pageId << "," << _componentId << "," << __name << "]";
    }

    /**
     * Get component name.
     *
     * @return the name of component. 
     */
    std::string getObjName(void) {
        return __name;
    }

protected: /* methods */

    /*
     * Get page id.
     *
     * @return the id of page.  
     */
    uint8_t getObjPid(void) {
        return _pageId;
    }

    /*
     * Get component id.
     *
     * @return the id of component.  
     */
    uint8_t getObjCid(void) {
        return _componentId;
    }
    
private: /* data */ 
    uint8_t _pageId;
    uint8_t _componentId;
    std::string __name;
};
/**
 * @}
 */

