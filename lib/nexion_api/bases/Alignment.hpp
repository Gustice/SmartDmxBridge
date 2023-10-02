#pragma once

#include "Object.hpp"
#include "Values.hpp"

namespace nxt {

/**
 * @brief Alignment property of Control
 */
class Alignment {
  private:
    Object &_parent; // this member must be initialized first

  public:
    /// @brief Constructor
    /// @param parent Reference to Control
    Alignment(Object &parent)
        : _parent(parent) {}

    /// @brief Horizontal alignment: 0-Left;1-Center;2-Right
    IntegerValue placeXcen{_parent, "xcen"};

    /// @brief Vertical alignment: 0-Up;1-Center;2-Down
    IntegerValue placeYcen{_parent, "ycen"};
};

} // namespace nxt