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

#ifndef ATA_H
#define ATA_H

#undef  Ex
#ifdef ATA_C
	#define Ex
#else
	#define Ex extern
#endif

//Ex u8 ATA_WrCache;

#ifdef STANDBY_TIMER
	extern u8 ata_get_temp();
	extern u8 ata_read_smart_data();
#endif

extern u8 ata_ReadID();

extern u8 ata_SetDMAXferMode(u8 mode);
extern void AtaInitDmaMode();

extern u8 ata_ExecUSBNoDataCmd();
extern u8 ata_ExecUSBSmartDataCmd(u8 subcommand);

extern u8 ata_SmartWriteCmd(u8 subcommand, u8 page);

extern u8 ata_ExecNoDataCmd(u8 command, u8 feature);
extern u8 ata_SmartNoDataCmd(u8 subcommand);

//extern void Ata_SetupErrorBlock(u8 error);
extern u8 ata_CheckPowerMode();

#ifdef HD_BOOT
	u8 ata_ReadGlobalNvram();
#endif

extern void ata_get_serial_num();

extern void ata_set_usb_msc();

extern bit ata_init();

#ifdef USB_FAST_ENUM
	extern bit ata_fast_enum_init();
#endif

///////////////////////////////////////////////////////////////
// ATA Shadow Registers
///////////////////////////////////////////////////////////////
// Bit definition of ATA STATUS Register  
#define   ATA_STATUS_BSY  0x80
#define   ATA_STATUS_DRDY 0x40
#define   ATA_STATUS_DSC  0x10
#define   ATA_STATUS_DRQ  0x08
#define   ATA_STATUS_CORR 0x04
#define   ATA_STATUS_IDX  0x02
#define   ATA_STATUS_CHECK  0x01

// Bit definition of ATA ERROR Register
#define   ATA_ERR_ICRC    BIT_7
#define   ATA_ERR_UNC     BIT_6
#define   ATA_ERR_MC      BIT_5
#define   ATA_ERR_IDNF    BIT_4
#define   ATA_ERR_MCR     BIT_3
#define   ATA_ERR_ABRT    BIT_2
//#define   ATA_ERR_NM      (1 << 1)
//#define   ATA_ERR_MED     (1 << 0)

//Bit definition of ATA FEATURES Register
#define   FEATURES_DMA        0x01

//definition of ATA Device Register
#define MASTER              0xA0
//	#define   DRV_HEAD_DFT        0xA0


// Bit definition of ATA DEVICE Control Register
#define   DEVICE_CRTL_DFT     0x08
#define   DEVICE_CRTL_SRST    0x04
#define   DEVICE_CRTL_IENT    0x02



//#define ERROR_NONE			0
//#define ERROR_ATA				1
#define ERROR_ILL_OP			0
#define ERROR_ILL_CDB			1
#define ERROR_ILL_PARAM			2
#define ERROR_UA_NOT_READY		3
#define ERROR_UA_BECOMING_READY 4
#define ERROR_UA_NO_MEDIA		5
#define ERROR_UA_NOT_READY_INIT	6
#define ERROR_SELFTEST_FAIL		7
#define ERROR_SAV_NOT_SUPP		8
#define ERROR_LBA_OUT_RANGE		9
#define ERROR_NO_ACCESS_RIGHT	10
#define ERROR_CRC_ERROR			11
#define ERROR_IU_TOO_SHORT		12
#define ERROR_DATA_PHASE		13
#define ERROR_OVERLAPPED_CMD	14
//#define ERROR_MEDIA_CHANGED	8
//#define ERROR_WRITE_PROTECT	9
//#define ERROR_COMMAND_ABORTED	0x0A
#define ERROR_WRITE_PROTECT		15








// lock or unlock state
//Ex u8 mediaStatus;
//	#define MEDIASTATUS_MEDIA				0x00
//	#define MEDIASTATUS_IS_LOCKED			0x01




//struct DEV_INFO
//{
//	u8 class;
//	u8 mediaType;
//	u8 deviceType;
//	u8 reserved;
//};

//struct X_DEV_INFO
//{
//	u32 sectorLBA_high;
//	u32 sectorLBA;
//	u16 ioMode;
//	u8 modelText[40];
//};

typedef          /* Data type with only possible values - ATA-6 command codes */
enum {
    ATA6_CHECK_POWER_MODE			= 0xE5 ,
	DOOR_LOCK						= 0xDE ,  DOOR_UNLOCK                = 0xDF ,
	DOWNLOAD_MICROCODE				= 0x92 ,
    ATA6_EXECUTE_DEVICE_DIAGNOSTICS	= 0x90 ,
	ATA6_FLUSH_CACHE				= 0xE7 ,
	ATA6_FLUSH_CACHE_EXT			= 0xEA ,
	ATA6_GET_MEDIA_STATUS			= 0xDA ,
	ATA6_IDENTIFY_DEVICE			= 0xEC ,
	ATA6_IDENTIFY_PACKET_DEVICE		= 0xA1 ,
	ATA6_IDLE						= 0xE3,
	ATA6_IDLE_IMMEDIATE             = 0xE1 ,
	ATA6_MEDIA_EJECT				= 0xED ,
	ATA6_NOP						= 0,
	ATA6_PACKET						= 0xA0 ,
	ATA6_READ_BUFFER				= 0xE4 ,
	ATA6_READ_DMA					= 0xC8 ,
	ATA6_READ_DMA_EXT				= 0x25 ,
	READ_MULTIPLE					= 0xC4 ,
	ATA6_READ_SECTORS				= 0x20 ,
	ATA6_READ_VERIFY_SECTORS		= 0x40 ,
	ATA6_READ_VERIFY_SECTORS_EXT	= 0x42 ,
    SECURITY_SET_PASSWORD			= 0xF1,   SECURITY_DISABLE_PASSWORD  = 0xF6 ,
    SECURITY_ERAZE_PREPARE			= 0xF3 ,  SECURITY_ERAZE_UNIT        = 0xF4 ,
    SECURITY_FREEZE_LOCK			= 0xF5 ,  SECURITY_UNLOCK            = 0xF2 ,
 	ATA6_SEEK						= 0x70 ,
	ATA6_SET_MULTIPLE_MODE			= 0xC6 ,
	ATA6_SET_FEATURES				= 0xEF ,
	ATA6_SLEEP						= 0xE6 ,
	ATA6_SMART						= 0xB0 ,
	ATA6_STANDBY					= 0xE2 ,
	ATA6_STANDBY_IMMEDIATE			= 0xE0 ,
	ATA6_WRITE_BUFFER				= 0xE8 ,
	ATA6_WRITE_DMA					= 0xCA ,
	ATA6_WRITE_DMA_EXT				= 0x35 ,
	ATA6_WRITE_MULTIPLE				= 0xC5 ,
	ATA6_WRITE_SECTORS				= 0x30 ,
	ATA6_RD_FPDMA_QUEUED            = 0x60 ,
	ATA6_WR_FPDMA_QUEUED            = 0x61 ,
    ATA6_WRITE_SECTORS_EXT			= 0x34
}  ATA6_OpCode ; /*ATA-6 command codes - end */

// SMART Feature Commands
typedef
enum  {
	SMART_READ_DATA					= 0xD0,
	SMART_READ_ATTR					= 0xD1,
	SMART_SAVE_ATTR_VALUES			= 0xD3,
	SMART_EXEC_OFF_LINE_IM			= 0xD4,
	SMART_READ_LOG					= 0xD5,
	SMART_WRITE_LOG					= 0xD6,
	SMART_ENABLE_OPERATIONS			= 0xD8,
	SMART_DISABLE_OPERATIONS		= 0xD9,
	SMART_RETURN_STATUS				= 0xDA
} SmartFeatureCommands;

#endif