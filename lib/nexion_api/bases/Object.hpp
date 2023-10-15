#pragma once

#include "bases/Port.hpp"

namespace nxt {

class Page;

/**
 * @brief Root class of all Nextion components.
 * @details Provides essential attributes
 */
class Object {
  public:
    /// @brief Constructor typical components
    /// @param page Parent-page-reference
    /// @param port Port-reference
    /// @param cId ComponentId
    /// @param name Component name
    Object(Page &page, Port &port, uint8_t cId, std::string_view name)
        : pageRef(page), _port(port), _componentId(cId), _name(name) {}

    /// @brief Constructor for pages
    /// @param name Component name
    /// @param port Port-reference
    Object(std::string_view name, Port &port)
        : pageRef(*(reinterpret_cast<Page *>(this))), _port(port), _componentId(0), _name(name) {}

    /// @brief Getter for ObjectInfo
    /// @return Infos as string "[this:PageId,ComponentId,Name]"
    std::string getObjectInfo();

    /// @brief Getter for ObjectName
    /// @return Name of object
    std::string getObjName();

    /// @brief Reference to parent page
    Page &pageRef;

    /// @brief Reference to associated port
    Port &_port;

  protected:
    /// @brief Getter for ObjectId
    /// @return ObjectId
    uint8_t getObjCid() {
        return _componentId;
    }

  private:
    uint8_t _componentId;
    std::string _name;
};
} // namespace nxt