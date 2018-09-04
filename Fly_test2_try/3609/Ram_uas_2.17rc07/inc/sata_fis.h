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

#ifndef __SATA_FIS_H__
#define __SATA_FIS_H__

#define FIS_TYPE			0			// FIS Type

//
// Fis 27 - Host to Device 
//
#define FIS_C				1			// update of the Command register
#define FIS_COMMAND			2			// Command register
#define FIS_FEATURE			3			// Features register
#define FIS_LBA_LOW			4			// LBA Low register	// PP_ATA_SECTOR_NUM_REG
#define FIS_LBA_MID			5			// LBA MID register	// PP_ATA_CYL_LOW_REG
#define FIS_LBA_HIGH		6			// LBA HIGH register// PP_ATA_CYL_HIGH_REG
#define FIS_DEVICE			7			// Header register
#define FIS_LBA_LOW_EXP		8			// expanded address field of LBA Low register	// PP_ATA_ExSECTOR_NUM_REG
#define FIS_LBA_MID_EXP		9			// expanded address field of LBA MID register	// PP_ATA_ExCYL_LOW_REG
#define FIS_LBA_HIGH_EXP	10			// expanded address field of LBA HIGH register	// PP_ATA_ExCYL_HIGH_REG
#define FIS_FEATURE_EXP		11			// expanded address field of Features register
#define FIS_SEC_CNT			12			// Sector Count register
#define FIS_SEC_CNT_EXP		13			// expanded address field of Sector Count register
#define FIS_RESERVED_14		14			//
#define FIS_CONTROL			15			// Device Control register
#define FIS_RESERVED_16		16			//
#define FIS_RESERVED_17		17			//
#define FIS_RESERVED_18		18			//
#define FIS_RESERVED_19		19			//
//
// Fis 34 - Host to Device 
//
#define FIS_STATUS			2			// Status register
#define FIS_ERROR			3			// Error register



#endif /* __SATA_H__ */
