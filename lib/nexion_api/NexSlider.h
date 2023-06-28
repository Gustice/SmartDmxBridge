/**
 * @file NexSlider.h
 *
 * The definition of class NexSlider. 
 *
 * @author Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date 2015/8/13
 *
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#ifndef __NEXSLIDER_H__
#define __NEXSLIDER_H__

#include "bases/NexTouch.h"
#include "NexHardware.h"
#include "bases/NexColor.h"

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * NexSlider component. 
 */
class NexSlider: public NexTouch
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, const char *name);
     */
    NexSlider(uint8_t pid, uint8_t cid, const char *name);

    /**
     * Get the value of slider. 
     * 
     * @retval returns value
     */
    uint32_t getValue();
    
    /**
     * Set the value of slider.
     *
     * @param number - the value of slider.  
     *
     * @retval true - success. 
     * @retval false - failed. 
     */
    bool setValue(uint32_t number);
	
    /**
     * Background-color (bco) attribute of component
     */
    NexColor background{*this, "bco"};
	
    /**
     * Font-color (pco) attribute of component
     */
    NexColor fontColor{*this, "pco"};
	
    /**
     * Get wid attribute of component
     *
     * @param number - buffer storing data retur
     * @return the length of the data 
     */
    uint32_t Get_pointer_thickness_wid();

    /**
     * Set wid attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_pointer_thickness_wid(uint32_t number);

    /**
     * Get hig attribute of component
     *
     * @param number - buffer storing data retur
     * @return the length of the data 
     */
    uint32_t Get_cursor_height_hig();	

    /**
     * Set hig attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_cursor_height_hig(uint32_t number);
	
    /**
     * Get maxval attribute of component
     *
     * @param number - buffer storing data return
     * @return the length of the data 
     */
    uint32_t getMaxval();	
	
    /**
     * Set maxval attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setMaxval(uint32_t number);		
	
    /**
     * Get minval attribute of component
     *
     * @param number - buffer storing data return
     * @return the length of the data 
     */
    uint32_t getMinval();	
	
    /**
     * Set minval attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool setMinval(uint32_t number);		
};
/**
 * @}
 */


#endif /* #ifndef __NEXSLIDER_H__ */
