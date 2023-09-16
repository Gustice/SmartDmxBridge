#pragma once

#include "bases/BaseIncludes.hpp"
#include "Touch.hpp"

extern const std::string EndSequence;

class SerialStream {
  public:
    virtual std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) = 0;
    virtual void write(char c) = 0;
    virtual void write(std::string str) = 0;
};

enum class NxtLogSeverity {
    Debug,
    Warning,
    Error
};

/**
 * @addtogroup CoreAPI 
 * @{ 
 */


class NxtIo {
    public:
    using LogCallback = void (*)(NxtLogSeverity, std::string);
    using SensingList = Nxt::Touch::SensingList;

    /**
     * Init Nextion.  
     * 
     * @return true if success, false for failure. 
     */
    static bool nexInit(SerialStream &port, LogCallback logCb);

    /**
     * Listen touch event and calling callbacks attached before.
     * 
     * Supports push and pop at present. 
     *
     * @param nex_listen_list - index to Nextion Components list. 
     * @return none. 
     *
     * @warning This function must be called repeatedly to response touch events
     *  from Nextion touch panel. Actually, you should place it in your loop function. 
     */
    static void nexLoop(const SensingList & nex_listen_list);

    static uint32_t recvRetNumber(uint32_t timeout = 100);
    static std::string recvRetString(uint32_t timeout = 100);
    static void sendCommand(std::string cmd);
    static bool recvRetCommandFinished(uint32_t timeout = 100);

    private:
    static SerialStream * serialPort;
    static LogCallback logCallback;
    static void sendLog(NxtLogSeverity level, std::string log);
};

/**
 * @}
 */
