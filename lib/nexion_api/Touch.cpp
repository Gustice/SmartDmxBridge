#include "Touch.hpp"
#include <vector>
#include "Page.hpp"

namespace Nxt {

void Touch::attachPush(eventCb push, void *ptr) {
    this->_pushCb = push;
    this->_ptrForPushCb = ptr;
}

void Touch::detachPush(void) {
    this->_pushCb = nullptr;
    this->_ptrForPushCb = nullptr;
}

void Touch::attachPop(eventCb pop, void *ptr) {
    this->_popCb = pop;
    this->_ptrForPopCb = ptr;
}

void Touch::detachPop(void) {
    this->_popCb = nullptr;
    this->_ptrForPopCb = nullptr;
}

void Touch::push(void) {
    if (_pushCb) {
        _pushCb(_ptrForPushCb);
    }
}

void Touch::pop(void) {
    if (_popCb) {
        _popCb(_ptrForPopCb);
    }
}

bool Touch::iterate(const SensingList &elements, uint8_t pid, uint8_t cid, int32_t event) {
    for (auto &&e : elements) {
        if (e->pageRef.getId() == pid && e->getObjCid() == cid) {
            e->printObjInfo();
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
} // namespace Nxt