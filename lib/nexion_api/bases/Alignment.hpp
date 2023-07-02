#pragma once

#include "Values.hpp"
#include "Object.hpp"

namespace Nxt {

class Alignment {
  private:
    Object &_parent; // this member must be initialized first

  public:
    Alignment(Object &parent) : _parent(parent) {}

    /**
     * Xcen attribute of component.
     */
    IntegerValue placeXcen{_parent, "xcen"};

    /**
     * Ycen attribute of component.
     */
    IntegerValue placeYcen{_parent, "ycen"};
};

}