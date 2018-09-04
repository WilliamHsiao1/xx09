/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2009-2013, All Rights Reserved
 *
 *   This file contains confidential and propietary information
 *   which is the property of Initio Corporation.
 *
 *
 *
 *****************************************************************************
 * 
 * 3610		2010/04/09	Odin		Initial version
 * 3610		2010/04/27	Odin		USB2.0 BOT Debugging
 *
 *****************************************************************************/

#ifndef BOT_H
#define BOT_H

#undef  Ex
#ifdef BOT_C
	#define Ex
#else
	#define Ex extern
#endif


Ex u8 xdata bot_tfr_fis[16];

void bot_init();

void bot_device_no_data();
void bot_device_data_in();
void bot_device_data_out();

void bot_usb_bus_reset();



extern void usb_bot();

#endif

