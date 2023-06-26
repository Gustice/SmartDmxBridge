/**
 * @file NexNumber.h
 *
 * The definition of class NexNumber. 
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
 
#ifndef __NEXNUMBER_H__
#define __NEXNUMBER_H__

#include "NexTouch.h"
#include "NexHardware.h"
#include "bases/NexColor.h"
#include "bases/NexAlignment.h"
#include "NexFont.h"
#include "bases/NexBackgroundImage.h"

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * NexNumber component.
 */
class NexNumber: public NexTouch
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, const char *name);
     */
    NexNumber(uint8_t pid, uint8_t cid, const char *name);
    
    /**
     * Get number attribute of component.
     *
     * @param number - buffer storing text returned. 
     * @return The real length of text returned. 
     */
    bool getValue();
    
    /**
     * Set number attribute of component.
     *
     * @param number - number buffer. 
     * @return true if success, false for failure. 
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
     * Text alignment
     */
    NexAlignment alignment{*this};
	
    /**
     * Text font
     */
    NexFont font{*this};
	
    /**
     * Get lenth attribute of component
     *
     * @param number - buffer storing data return
     * @return the length of the data 
     */
    uint32_t Get_number_lenth();

    /**
     * Set lenth attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_number_lenth(uint32_t number);	

    /**
     * Background image
     */
    NexBackgroundImage backgroundImage{*this};
};

/**
 * @}
 */

#endif /* #ifndef __NEXNUMBER_H__ */
