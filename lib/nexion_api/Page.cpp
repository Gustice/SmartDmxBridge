#include "Page.hpp"
#include "NexHardware.h"

namespace Nxt
{
    
bool Page::show(void) {
    NxtIo::sendCommand(std::string("page ") + getObjName());
    bool isOk = NxtIo::recvRetCommandFinished();
    if (isOk) {
        _currentActive = this;
    }
    return isOk;
}

Page * Page::_currentActive;
std::map<uint8_t, Page*> Page::_pages;


} // namespace Nxt