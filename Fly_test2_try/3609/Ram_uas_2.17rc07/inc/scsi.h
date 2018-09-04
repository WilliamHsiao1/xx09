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

#ifndef SCSI_H
#define SCSI_H


typedef   /* SCSI commands */
enum {
	SCSI_ATA_PASS_THR			= 0xA1,
	SCSI_ATA_PASS_THR16			= 0x85,
	SCSI_FORMAT_UNIT			= 0x04,
	SCSI_INQUIRY				= 0x12,
	SCSI_LOG_SELECT				= 0x4C,
	SCSI_LOG_SENSE				= 0x4D,
	SCSI_MODE_SELECT6			= 0x15,
	SCSI_MODE_SENSE6			= 0x1A,
	SCSI_MODE_SELECT			= 0x55,
	SCSI_MODE_SENSE				= 0x5A,
	SCSI_PERSIST_RESERVE_IN		= 0x5E,
	SCSI_RCV_DIAG_RESULTS		= 0x1C,
	SCSI_REPORT_LUNS			= 0xA0,
	SCSI_REQUEST_SENSE			= 0x03,
	SCSI_REZERO					= 0x01,
	SCSI_PREVENT_MEDIUM_REMOVAL = 0x1E,
	SCSI_ALLOW_MEDIUM_REMOVAL   = 0x1E,
	SCSI_READ6					= 0x08,
	SCSI_READ10					= 0x28,
	SCSI_READ12					= 0xA8,
	SCSI_READ16					= 0x88,
	SCSI_READ_BUFFER10			= 0x3C,
	SCSI_READ_CAPACITY			= 0x25,
	SCSI_READ_FORMAT_CAPACITY	= 0x23,
	SCSI_SEEK10					= 0x2b,
	SCSI_SEND_DIAGNOSTIC		= 0x1D,
	SCSI_SERVICE_ACTION_IN16	= 0x9E,
	SCSI_START_STOP_UNIT		= 0x1B,
	SCSI_SYNCHRONIZE_CACHE		= 0x35,
	SCSI_SYNCHRONIZE_CACHE16	= 0x91,
	SCSI_TEST_UNIT_READY		= 0x00,
	SCSI_VERIFY10				= 0x2F,
	SCSI_VERIFY16				= 0x8F,
	SCSI_WRITE6					= 0x0A,
	SCSI_WRITE10				= 0x2A,
	SCSI_WRITE12				= 0xAA,
	SCSI_WRITE16				= 0x8A,
	SCSI_WRITE_BUFFER10			= 0x3B
	}	SCSIOpCode ;

// Service Action codes for the SERVICE ACTION IN and OUT commands.
//
enum ServiceAction {
	SA_READ_CAPACITY16			= 0x10,

	SA_MASK						= 0x1F
};


// Service Action codes for SCSI_PERSIST_RESERVE_IN command
enum ServiceAct2 {
	SA_READ_KEY			= 0x00
};

// Standard Vital Product Data(VPD)
#define INQUIRY_STD_SUPPORT						0x00			// Supported VPD Pages
#define INQUIRY_STD_VPD_USN						0x80			// Unit Serial Number
#define INQUIRY_STD_VPD_DEVID					0x83
#define	INQUIRY_BLOCK_LIMITS					0xB0		// Block Limits page for disk LUN
#define	INQUIRY_BLOCK_DEVICE_CHARACTERISTICS	0xB1		// Block Device Characteristics for disk LUN

#define INQUIRY_CMD_QUE			BIT_1			// Supported SCSI Command Queue

// Inquiry Versions, i.e., SPC compliance level.
//
enum InquiryVersion {
	INQUIRY_VERSION_NONE		= 0,
	INQUIRY_VERSION_SCSI2		= 2,
	INQUIRY_VERSION_SPC			= 3,
	INQUIRY_VERSION_SPC2		= 4,
	INQUIRY_VERSION_SPC3		= 5,
	INQUIRY_VERSION_SPC4		= 6
};

// SCSI standards Version Descriptors.
//
enum VersionDescriptor {
	VERSION_NONE				= 0,

	VERSION_SAM					= 0x0020,
	VERSION_SAM_R18				= 0x003B,
	VERSION_SAM_ANSI_1996		= 0x003C,

	VERSION_USB_1_1				= 0x1728,
	VERSION_USB_2_0				= 0x1729,

	VERSION_BOT_1_0				= 0x1730,		// USB Bulk-Only Transport 1.0

	VERSION_UAS					= 0x1740,
	VERSION_UAS_R02				= 0x1743,
	VERSION_UAS_R04				= 0x1747,

	VERSION_SPC2				= 0x0260,
	VERSION_SPC2_R20			= 0x0276,
	VERSION_SPC2_ANSI_2001		= 0x0277,

	VERSION_SPC3				= 0x0300,
	VERSION_SPC3_R23			= 0x0312,
	VERSION_SPC3_ANSI_2005		= 0x0314,

	VERSION_SPC4				= 0x0460,
	VERSION_SPC4_R18			= 0x0462,

	VERSION_MMC					= 0x0140,
	VERSION_MMC_R10A			= 0x015B,
	VERSION_MMC_ANSI_1997		= 0x015C,

	VERSION_SBC2				= 0x0320,
	VERSION_SBC2_R16			= 0x033B,
	VERSION_SBC2_ANSI_2005		= 0x033D,

	VERSION_SBC3				= 0x04C0,

	VERSION_SES					= 0x01C0,
	VERSION_SES_R8B				= 0x01DB,
	VERSION_SES_R8B_AM1			= 0x01DD,		// SES rev 8b with Amendment 1-2000
	VERSION_SES_ANSI_1998		= 0x01DC,
	VERSION_SES_ANSI_1998_AM1	= 0x01DE,		// SES standard with Amendment 1-2000

	VERSION_SAT					= 0x1EA0,
	VERSION_SAT_R9				= 0x1EAB,
	VERSION_SAT_ANSI_2007		= 0x1EAD,

	VERSION_SAT2				= 0x1EC0,
	VERSION_SAT2_R9				= 0x1EC8
};

// Mode parameter block descriptor sizes.
//
enum {
	SHORT_BLOCK_DESCRIPTOR_SIZE	=  8,
	LONG_BLOCK_DESCRIPTOR_SIZE	= 16,

	// Flags in the MODE SENSE(10) mode parameter header, byte 4.
	BLOCK_DESCRIPTOR_LONGLBA	= 0x01
};


// MODE SENSE and MODE SELECT command flags.
//
enum {
	// MODE SENSE CDB byte 1
	MODE_SENSE_DBD				= 0x08,		// Disable Block Descriptors
	MODE_SENSE_LLBAA			= 0x10,		// Long LBA Accepted; MODE SENSE(10) only

	// MODE SELECT byte 1
	MODE_SELECT_SP				= 0x01,		// Save Pages
	MODE_SELECT_PF				= 0x10		// Page Format
};

// Mode Parameter Page Codes for  MODE SENSE and MODE SELECT commands.
//
enum ModePageCode {
	MODE_FLEXIBLE_DISK_PAGE		= 0x05,		// Flexible disk page (SBC)
	MODE_CACHING_MODE_PAGE 		= 0x08,		// Caching mode page
	MODE_CONTROL_PAGE 			= 0x0A,		// Control page 
	MODE_POWER_COND_PAGE		= 0x1A,		// Power Condition page
	MODE_OPERATING_PAGE			= 0x22,		// Operating Parameter mode Page
	MODE_ALL_PAGES				= 0x3f,		// All pages

	MODE_PAGE_CODE_MASK			= 0x3f		// Bitmask of the Page Code field.
};


#define	LOG_SENSE_STD_PAGE			0x00		// Supported Log Pages

// Diagnostic page codes.
//
enum DiagnosticPageCode {
	DIAG_SUPPORTED_PAGES		= 0x00,		// Supported Diagnostics Pages
	DIAG_SHORT_STATUS			= 0x08		// Short Enclosure Status page
};



// for Request Sense Command
#define DESC			0x01	//supports descriptor format sense data

// Sense codes
#define SCSI_SENSE_NO_SENSE         	0x00
#define SCSI_SENSE_RECOVERED_ERROR  	0x01
#define SCSI_SENSE_NOT_READY        	0x02
#define SCSI_SENSE_MEDIUM_ERROR     	0x03
#define SCSI_SENSE_HARDWARE_ERROR   	0x04
#define SCSI_SENSE_ILLEGAL_REQUEST  	0x05
#define SCSI_SENSE_UNIT_ATTENTION   	0x06
#define SCSI_SENSE_DATA_PROTECT     	0x07
#define SCSI_SENSE_BLANK_CHECK      	0x08
#define SCSI_SENSE_UNIQUE           	0x09
#define SCSI_SENSE_COPY_ABORTED     	0x0A
#define SCSI_SENSE_ABORTED_COMMAND  	0x0B
#define SCSI_SENSE_EQUAL            	0x0C
#define SCSI_SENSE_VOL_OVERFLOW     	0x0D
#define SCSI_SENSE_MISCOMPARE       	0x0E
#define SCSI_SENSE_RESERVED         	0x0F


#endif     // SCSI_H
