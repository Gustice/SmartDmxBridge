#include "Nextion.hpp"

namespace nxt {

void Touch::attachPush(eventCb push, void *ptr) {
    this->_pushCb = push;
    this->_ptrForPushCb = ptr;
}

void Touch::detachPush() {
    this->_pushCb = nullptr;
    this->_ptrForPushCb = nullptr;
}

void Touch::attachPop(eventCb pop, void *ptr) {
    this->_popCb = pop;
    this->_ptrForPopCb = ptr;
}

void Touch::detachPop() {
    this->_popCb = nullptr;
    this->_ptrForPopCb = nullptr;
}

void Touch::push() {
    if (_pushCb) {
        _pushCb(_ptrForPushCb);
    }
}

void Touch::pop() {
    if (_popCb) {
        _popCb(_ptrForPopCb);
    }
}

bool Touch::iterate(const SensingList &elements, uint8_t pId, uint8_t cId, int32_t event) {
    for (auto &&e : elements) {
        if (e->pageRef.getId() == pId && e->getObjCid() == cId) {
            e->getObjectInfo();
            if (static_cast<int>(Event::Push) == event) {
                e->push();

            } else if (static_cast<int>(Event::Pop) == event) {
                e->pop();
            }
            return true;
        }
    }
    return false;
}
} // namespace nxt