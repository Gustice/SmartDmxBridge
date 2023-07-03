#include "bases/Object.hpp"
#include "Page.hpp"

namespace Nxt {

std::string Object::printObjInfo(void) {
    std::stringstream output;
    output << "[" << this << ":" << pageRef.getId() << "," << _componentId << "," << _name << "]";
    return output.str();
}

std::string Object::getObjName(void) {
    if (pageRef.isActive()) {
        return _name;
    }
    if (&pageRef == this) {
        return _name;
    }

    return (pageRef._name + "." + _name);
}

} // namespace Nxt