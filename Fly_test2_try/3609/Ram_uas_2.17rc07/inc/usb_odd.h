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

#ifndef USB_ODD_H
#define USB_ODD_H

#undef  Ex
#ifdef USB_ODD_C
	#define Ex
#else
	#define Ex extern
#endif

void bot_odd_exec_ctxt();
void initio_RdInqCmd();
void initio_RdNvramCmd();



extern void usb_odd();

#endif