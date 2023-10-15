#include "Nextion.hpp"

namespace nxt {

bool Page::isActive() {
    return (&_parent.getActivePage() == this);
}

bool Page::show(void) {
    return _parent.showPage(*this);
}

} // namespace nxt