#include "../NexHardware.h"
#include "NexIncludes.h"
#include "Values.hpp"

#pragma once

namespace Nxt {

class Alignment {
  private:
    NexObject &_parent; // this member must be initialized first

  public:
    Alignment(NexObject &parent) : _parent(parent) {}

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