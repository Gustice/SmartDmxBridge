#pragma once

#include "NexHardware.h"
#include "bases/Color.hpp"
#include "bases/NexTouch.h"

namespace Nxt {

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * Rtc component.
 */

class Rtc
{
    public:

    bool write_rtc_time(char *time);
    
    /**
     * write rtc times
     *
     * @param time_type - To type in time   (example:write_rtc_time("year",2016))
     * @param number - the time value
     * @return true if success, false for failure
     */
    
    bool write_rtc_time(char *time_type,uint32_t number);
    
    /**
     * write rtc times
     *
     * @param time - Time to write to the array
     * @return true if success, false for failure
     */
    
    bool write_rtc_time(uint32_t time[6]);
    
    
    /**
     * read rtc time
     *
     * @param time - Access data array
     * @param len - len of array
     * @return true if success, false for failure
     */
    
    bool read_rtc_time(char *time,uint32_t len);
    
    /**
     * read rtc times
     *
     * @param time_type - To type in time   
     * @param number - the time value
     * @return true if success, false for failure
     */
    
    uint32_t read_rtc_time(std::string type);
    
    /**
     * read rtc time
     *
     * @param time - Access data array
     * @param len - len of array
     * @return true if success, false for failure
     */
    
    bool read_rtc_time(uint32_t *time,uint32_t len);
    
};

/**
 * @}
 */

}
