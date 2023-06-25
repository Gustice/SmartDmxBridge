/**
 * @file NexText.h
 *
 * The definition of class NexText. 
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
 
#ifndef __NEXTEXT_H__
#define __NEXTEXT_H__

#include "NexTouch.h"
#include "NexHardware.h"
#include "NexColor.h"
#include "NexAlignment.h"
#include "NexFont.h"

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * NexText component.
 */
class NexText: public NexTouch
{
public: /* methods */
    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, std::string name);
     */
    NexText(uint8_t pid, uint8_t cid, std::string name);
    
    /**
     * Get text attribute of component.
     *
     * @return returned text. 
     */
    std::string getText();
    
    /**
     * Set text attribute of component.
     *
     * @param value - text value. 
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
     * Get picc attribute of component
     *
     * @param number - buffer storing data retur
     * @return the length of the data 
     */
    uint32_t Get_background_crop_picc(uint32_t *number);	

    /**
     * Set picc attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_background_crop_picc(uint32_t number);			
	
    /**
     * Get pic attribute of component
     *
     * @param number - buffer storing data retur
     * @return the length of the data 
     */
    uint32_t Get_background_image_pic(uint32_t *number);	

    /**
     * Set pic attribute of component
     *
     * @param number - To set up the data
     * @return true if success, false for failure
     */
    bool Set_background_image_pic(uint32_t number);	
    
};

/**
 * @}
 */

#endif /* #ifndef __NEXTEXT_H__ */
