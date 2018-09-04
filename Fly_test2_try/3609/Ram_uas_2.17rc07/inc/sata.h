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
 * 3607		2010/11/18	Odin		Support AES (_EXT_CCM) 
 *
 *****************************************************************************/

#ifndef SATA_H
#define SATA_H

//----------------------
#undef  Ex
#ifdef SATA_C
	#define Ex
#else
	#define Ex extern
#endif
//----------------------

#define HOST2DEVICE_FIS		0x27
#define DEVICE2HOST_FIS		0x34
#define SET_DEVICE_BITS_FIS	0xA1


//----------------------
//Ex u8	indx;
Ex	bit	data	DownLoad_Flash;
Ex	bit	data	DevicesFound;
Ex	bit	data	cfa_flag;
Ex	bit	data	sata_init;

Ex	u8	data sata_frame_int_0;
Ex	u8	data sata_int_status_0;


#define CCM_NULL         0xFF


#if 0
typedef  struct _SATA_OBJ
 {
	u8				sobj_State;
	u8				sobj_que;
	u8				sobj_ncq_que;
	u8				sobj_curScm;


	u8				sobj_resved;
	u8				sobj_init;
	u16				sobj_init_wait;

	u8				sobj_WrCache;
	u8				sobj_class;
	u8				sobj_mediaType;
	u8				sobj_deviceType;
	u32				sectorLBA_l;
	u32				sectorLBA_h;

	u8				modelText[40];
//	u32(*AllocateCCM)(PCDB_CTXT,u32,u32);
//	u32(*DetachCCM)(u32);
 }SATA_OBJ,*PSATA_OBJ;
#endif

Ex u8 code  i2bit[];
Ex u8 code  i2mbit[];


Ex bit	data	sobj_init;
Ex u8	xdata	sobj_init_tout;
Ex bit	data	sobj_sector_4K;
Ex bit	data	sobj_WrCache_supported;
Ex bit	data	sobj_WrCache_enabled;
Ex bit	data	sobj_smartSupport;

Ex bit	data	sobj_mediaType;
Ex u8	data	sobj_State;
Ex u8	idata	sobj_State_tout;
Ex u8	data	sobj_que;
Ex u8	data	sobj_ncq_que;
Ex u8	data	sobj_curScm;
	#define SCM_NULL	0xFF
Ex u8	data	sobj_class;
Ex u8	xdata	org_sobj_class;

//Ex u8	xdata	sobj_deviceType;
//Ex u32	data	sobj_cFree;
Ex U32_DATA	data	sobj_cFree1;
//Ex u32	idata	sobj_default_cFree;
//Ex U32_DATA	idata	sobj_default_cFree1;
Ex bit	data	sobj_ncq_mode;
Ex u8	data	sobj_qdepth;
#ifdef UAS_EN
	#define MIN_SATA_Q_DEPTH	7
#endif
//Ex u32	data	sobj_sBusy;
Ex U32_DATA	data	sobj_sBusy1;

Ex u32	data	sobj_sectorLBA_l;
Ex u32	data	sobj_sectorLBA_h;

#ifdef PHYSICAL_MORE_512
Ex u16	xdata	sobj_physical_sector_size;  // context is WORD TYPE  & just support 4k now
#endif

Ex u8 	xdata	sobj_serialStr[20];
Ex u8 	xdata	sobj_wnn[8];

#ifdef SUPPORT_WWN
	Ex u8 	xdata	sobj_wwn[8];
#endif
//Ex bit data autoFis;

// states for sobj_State
//
enum {
	SATA_NO_DEV			= 0x00 ,
	SATA_PWR_DWN,
	SATA_RESETING,
	SATA_PHY_RDY,
	SATA_DRV_RDY,
	SATA_STANDBY,
	SATA_READY,
	SATA_ACTIVE,
	SATA_NCQ_FLUSH,
	SATA_NCQ_ACTIVE
};

// definition for sobj_class
#define DEVICECLASS_ATA			0
#define DEVICECLASS_ATAPI		1
#define DEVICECLASS_CFA			2
#define DEVICECLASS_NONE		3
#define DEVICECLASS_UNKNOWN		4

// definition for sobj_mediaType
#define MEDIATYPE_FIXED			0x00
#define MEDIATYPE_REMOVABLE		0x01
//#define	MEDIATYPE_NON_INIT		0x02

// definition for sobj_deviceType
#define DEVICETYPE_HD_ATA5		(0)
#define DEVICETYPE_HD_ATA6		(1)



#define SATA_RESET_TOUT	25		//2.5sec
#define SATA_PRDY_TOUT	25		//2.5sec	120		//12sec
#define SATA_INIT_TOUT  255     //25sec

#ifdef UAS_EN
// Local Functions
u8 sata_AllocateSCM();
void sata_DetachSCM();
#endif
void sata_HardReset();


// External Functions
void sata_return_tfr();

void sata_isr();

u8 sata_exec_internal_ccm();

// Execute a SATA non-NCQ command to seecified SATA OBJ
u8 sata_exec_dmae_RW_ctxt();

u8 sata_exec_ctxt();

// Execute a SATA NCQ command to SATA OBJ
u8 sata_exec_ncq_ctxt();



// 
// type of Reset Command:
enum {
	USB_SATA_RST			= 0x00 ,		// SATA Hard Reset by USB Reset
	SATA_HARD_RST,							// SATA Hard Reset	
	SATA_SOFT_RST							// SATA Software Reset
};


#ifdef UAS_EN
	bit sata_abort_task(u8 tid);

	void sata_abort_all();
#endif

// Execute a SATA Bus Reset to seecified SATA OBJ
// Input
//	Reset Command:
//		USB_SATA_RST 
//		SATA_HARD_RST
//		SATA_SOFT_RST
void sata_Reset(u8 rst_cmd);

//void sata_srst();

void sata_get_class();

void scan_sata_device();

void sata_RegInit();

void sata_set_cfree();

void sata_InitSataObj();

#ifdef PWR_SAVING
void sata_pll_pwr_up();
void sata_pwr_up();
void sata_pwr_down();
#endif

#endif
