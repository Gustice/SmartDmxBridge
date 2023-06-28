/**
 * @file NexScrolltext.h
 *
 * The definition of class NexScrolltext. 
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
 
#ifndef __NEXSCROLLTEXT_H__
#define __NEXSCROLLTEXT_H__

#include "bases/NexTouch.h"
#include "NexHardware.h"
#include "bases/NexColor.h"
#include "bases/NexAlignment.h"
#include "bases/NexFont.h"
#include "bases/NexBackgroundImage.h"

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * NexText component.
 */
class NexScrolltext: public NexTouch
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, const char *name);
     */
    NexScrolltext(uint8_t pid, uint8_t cid, const char *name);
    
    /**
     * Get text attribute of component.
     *
     * @param buffer - buffer storing text returned. 
     * @param len - length of buffer. 
     * @return The real length of text returned. 
     */
    std::string getText();
    
    /**
     * Set text attribute of component.
     *
     * @param buffer - text buffer terminated with '\0'. 
     * @return true if success, false for failure. 
     */
    bool setText(std::string value);    
	
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
     * Background image
     */
    NexBackgroundImage backgroundImage{*this};

    /**
     * Get dir attribute of component
     *
     * @param number - buffer storing data retur
     * @return the length of the data 
     */
    uint32_t Get_scroll_dir();		
	
    /**
     * Set dir attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_scroll_dir(uint32_t number);	

    /**
     * Get dis attribute of component
     *
     * @param number - buffer storing data retur
     * @return the length of the data 
     */
    uint32_t Get_scroll_distance();	

    /**
     * Set dis attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_scroll_distance(uint32_t number);	

    /**
     * Get tim attribute of component
     *
     * @param number - buffer storing data retur
     * @return the length of the data 
     */
    uint32_t Get_cycle_tim();

    /**
     * Set tim attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_cycle_tim(uint32_t number);			
	
    bool enable(void);
    bool disable(void);
};

/**
 * @}
 */

#endif /* #ifndef __NEXSCROLLTEXT_H__ */