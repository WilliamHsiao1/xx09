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
 * usb_hdd		2010/09/01	winston		USB HDD object
 *
 *****************************************************************************/
#ifndef USB_HDD_H
#define USB_HDD_H

extern void switch_regs_setting(u8 reg_mode);
	#define NORMAL_MODE   			0x00
	#define COMPLIANCE_TEST_MODE	0x01
	#define LOOPBACK_TEST_MODE		0x02

void usb_start();

#endif
