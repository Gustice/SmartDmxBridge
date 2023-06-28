/**
 * @file NexButton.h
 *
 * The definition of class NexButton. 
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

#ifndef __NEXBUTTON_H__
#define __NEXBUTTON_H__

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
 * NexButton component. 
 *
 * Commonly, you want to do something after push and pop it. It is recommanded that only
 * call @ref NexTouch::attachPop to satisfy your purpose. 
 * 
 * @warning Please do not call @ref NexTouch::attachPush on this component, even though you can. 
 */
class NexButton: public NexTouch
{
public: /* methods */

    /**
     * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, const char *name);
     */
    NexButton(uint8_t pid, uint8_t cid, const char *name);

    /**
     * Get text attribute of component.
     *
     * @return The real length of text returned. 
     */
    std::string getText();

    /**
     * Set text attribute of component.
     *
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
     * Background-color (bco) attribute of component in active state 
     */
    NexColor backgroundPressed{*this, "bco2"};

    /**
     * Font-color (pco) attribute of component in active state 
     */
    NexColor fontColorPressed{*this, "pco2"};
		
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
     * Background image
     */
    NexBackgroundImage backgroundImagePressed{*this,"2"};

};
/**
 * @}
 */


#endif /* #ifndef __NEXBUTTON_H__ */
