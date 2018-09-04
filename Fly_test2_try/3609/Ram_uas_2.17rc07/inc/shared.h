/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2009-2013, All Rights Reserved
 *
 *   This file contains confidential and propietary information
 *   which is the property of Initio Corporation.
 *
 *						Header of All BOT Project
 *
 *****************************************************************************
 * 
 * 3610		2010/04/09	Odin		Initial version
 * 3610		2010/04/27	Odin		USB2.0 BOT Debugging
 *
 *****************************************************************************/

#ifndef SHARED_H
#define SHARED_H

#undef  Ex
#ifdef SHARED_C
	#define Ex
#else
	#define Ex extern
#endif

#if SCSI_HID
//Ex u8 hidGpio;
Ex u8 softGpioData;
Ex u8 scsiGpioData;
Ex u8	scsi_hid;
#define HID_GPIO_READ()	 ((~(*gpio_DI_0) & (GPIO1_DATA)))
#endif

#ifdef W_PROTECT
Ex u8 writeprotect;
#define IS_WRITE_PROTECT() ((*gpio_DI_0)&(GPIO4_DATA))
#endif

//Ex u8 USB23_DETECT_flag;
//Ex u8 sata2_retry_count;

//Ex u8 CmdBlk[16];
Ex u8 data i8;
Ex u8 data val;

//Ex u8 data site;
//#ifdef DISPATCH
//Ex u32 cmdStartFlag;
//Ex u32 timer;

//Ex u32 b2hCmdStartFlag;

#ifdef EQ_SETTING_TEST

Ex u8	xdata     usb_20_bus_rst;
Ex u8	xdata     usb_30_bus_rst;

Ex u8	data sobj_crc_count;

#endif

//Ex u8	romcode_check_stat;
Ex u8 data segIndex;

//Ex u32 ppRCounter;
//Ex u32 ppWCounter;
//#endif

////////////////////////////////////
//Ex struct DEV_INFO deviceInfo;
//Ex struct X_DEV_INFO xDeviceInfo;
Ex u8 xdata vendorText[8];

Ex bit data	cfa_active;
#ifdef POW_MGMT
Ex bit data			PwrMgmt;
Ex bit data 		DrvPwrMgmt;
#endif
#ifdef ASIC_CONF
	Ex bit data 		cpu_ref_clk;
#endif
Ex u16 data	byteCnt;
//Ex u32	nextDirection;
Ex u8	data	tmp8;
Ex u16	data	tmp16;
//Ex u32	data	tmp32;
Ex U32_DATA	data utmp32;
Ex u8	data	i8;
Ex u16	data	sz16;
//Ex u8	xdata * data p8;
Ex u8	xdata * data    pU8;

#ifdef DBG_ODD_U2
Ex bit data dbg_flag;
#endif


#ifdef ROM
	//Ex u16 data spiVMA;
	//Ex u16 data spiLMA;
	//Ex u16 data spiCnt;
#endif

Ex u8 data CmdBlk[16];
#define CmdBlk(_x_)	(CmdBlk[_x_])

Ex u8 	xdata serialStr[20];
//Ex U8	standby_flag;

Ex u32	data	cbwbyteCnt;


Ex u32	resetCnter;
//Ex u8	ata_Rst_Flg;
//Ex u8	short_packet;
//Ex  u8     ppFlag;


Ex bit data revision_a61;

#ifdef A71_PATCH
	Ex bit data revision_a71;
	Ex bit data revision_a91;
#endif


Ex bit data bot_cbw_active;
Ex bit data extended_descriptor;

//Ex u8 LogiLBA[4];


//Ex u8 Init_hardware_flag;

Ex bit data fast_enum_usb_flag;

Ex bit	data	hdd_serial;
//Ex bit data	valid_hdd;

Ex bit data valid_nvram_from_hdd;
Ex bit data load_nvram_from_HDD_flag;

Ex bit data valid_vital_from_sflash;

Ex u8	xdata usb_AdapterID;
Ex u8	xdata usb_PortID0;
Ex u8	xdata usb_PortID1;
Ex u8	xdata usb_PortID2;

#define MC_BUFFER_SIZE				512
extern u8   xdata mc_buffer[MC_BUFFER_SIZE];


#define DIRECTION_NONE				0
#define DIRECTION_SEND_DATA			1
#define DIRECTION_SEND_STATUS		2
#define DIRECTION_SEND_STATUS_NOW	3
#define DIRECTION_BUS_RESET			4
#define DIRECTION_RECEIVED_DATA		5
#define DIRECTION_RD_INQ			6
#define DIRECTION_RD_NV				7

#define DIRECTION_SEND_STATUS_WRITE_PROTECT		10


extern void xmemcpy(u8 xdata *src, u8 xdata *dest, u16 size);
extern bit xmemcmp(u8 xdata *src, u8 xdata *dest, u8 size);
extern void Delay10us(u8 time);
extern void Delay(u16 time);
extern void memcpySwap16(u8 *src, u8 *dest, u8 n16);
extern void xmemset(u8 xdata *ptr, u8 chr, u8 count);
extern void calc_Idle_timer(u8 time);
extern void spi_phy_wr(u8 phy_type, u8 phy_reg, u8 write_value);
//extern void spi_phy_wr_retry(u8 phy_type, u8 phy_reg, u8 write_value);
#define spi_phy_wr_retry spi_phy_wr
extern u8 spi_phy_rd(u8 phy_type, u8 phy_reg);




/*******************************************\
	Util
	ChgEndian_16(0x1234) return 0x3412
	ChgEndian_32(0x12345678) return 0x78563412
\*******************************************/
#define Min(a,b)        (((a) < (b)) ? (a) : (b))
#define Max(a,b)        (((a) > (b)) ? (a) : (b))

//----------------
#define	ChgEndian_16(hex) \
		( ((u16)(hex)>>8)+((u16)(hex)<<8) )

#define	ChgEndian_32(hex) \
		( ((u32)(hex)>>24)+((u32)((hex)&0xFF0000)>>8)+((u32)((hex)&0xFF00)<<8)+((u32)(hex)<<24) )

//----------------------
#define SWAP16(UB16) (((UB16) << 8) | ((UB16) >> 8))
#define SWAP32(UB32) ((((U32)(UB32) & 0xFF000000) >> 24) | (((U32)(UB32) & 0x00FF0000) >> 8) | (((U32)(UB32) & 0x0000FF00) << 8) | (((U32)(UB32) & 0x000000FF) << 24))

#define COPYU16(src, dest) \
	{\
		*(((u8 *)(dest)) + 0) = *(((u8 *)(src)) + 0); \
		*(((u8 *)(dest)) + 1) = *(((u8 *)(src)) + 1); \
	}


#define COPYU32(src, dest) \
	{\
		*(((u8 *)(dest)) + 0) = *(((u8 *)(src)) + 0); \
		*(((u8 *)(dest)) + 1) = *(((u8 *)(src)) + 1); \
		*(((u8 *)(dest)) + 2) = *(((u8 *)(src)) + 2); \
		*(((u8 *)(dest)) + 3) = *(((u8 *)(src)) + 3); \
	}

#define COPYU16_REV_ENDIAN(src, dest) \
	{\
		*(((u8 *)(dest)) + 0) = *(((u8 *)(src)) + 1); \
		*(((u8 *)(dest)) + 1) = *(((u8 *)(src)) + 0); \
	}


#define COPYU32_REV_ENDIAN(src, dest) \
	{\
		*(((u8 volatile xdata *)(dest)) + 0) = *(((u8 volatile xdata *)(src)) + 3); \
		*(((u8 volatile xdata *)(dest)) + 1) = *(((u8 volatile xdata *)(src)) + 2); \
		*(((u8 volatile xdata *)(dest)) + 2) = *(((u8 volatile xdata *)(src)) + 1); \
		*(((u8 volatile xdata *)(dest)) + 3) = *(((u8 volatile xdata *)(src)) + 0); \
	}

#define COPYU16_REV_ENDIAN_X2D(src, dest) \
	{\
		*(((u8 *)(dest)) + 1) = *(((u8 volatile xdata *)(src)) + 0); \
		*(((u8 *)(dest)) + 0) = *(((u8 volatile xdata *)(src)) + 1); \
	}

#define COPYU32_REV_ENDIAN_X2D(src, dest) \
	{\
		*(((u8 *)(dest)) + 3) = *(((u8 volatile xdata *)(src)) + 0); \
		*(((u8 *)(dest)) + 2) = *(((u8 volatile xdata *)(src)) + 1); \
		*(((u8 *)(dest)) + 1) = *(((u8 volatile xdata *)(src)) + 2); \
		*(((u8 *)(dest)) + 0) = *(((u8 volatile xdata *)(src)) + 3); \
	}

#define COPYU32_REV_ENDIAN_D2X(src, dest) \
	{\
		*(((u8 volatile xdata *)(dest)) + 0) = *(((u8 *)(src)) + 3); \
		*(((u8 volatile xdata *)(dest)) + 1) = *(((u8 *)(src)) + 2); \
		*(((u8 volatile xdata *)(dest)) + 2) = *(((u8 *)(src)) + 1); \
		*(((u8 volatile xdata *)(dest)) + 3) = *(((u8 *)(src)) + 0); \
	}

#define WRITE_BLOCK(src, dest, size)  \
	{ int i;  \
		for(i = 0; i < (size); i++) \
			(dest)[i] = (src)[i];     \
	}
	
	
//-----------------------
// Temp Variables
//-----------------------	
//Ex u8 SATA_FIS[20];
//Ex u8 RESP_FIS[20];

//Ex u8 data CTXT_FLAG;
//Ex u32 CTXT_XFRLENGTH;
//Ex u8 CTXT_CONTROL;
//Ex u16 CTXT_PHASE_CASE;
//Ex u8 data CTXT_No_Data;
//Ex u8 data CTXT_DbufIndex;
//Ex u8 data CTXT_Status;
//Ex u16 CTXT_SAT_FAIL_REASON;
//Ex u8 data	CTXT_ccm_cmdinten;
//Ex u8 CTXT_Index;

//Ex u8 CTXT_ccmIndex;
//Ex u8 CTXT_Prot;

//Ex u8 data ccm_prot;

//Ex u8 modelText[40];

Ex u8 data ctxt_site;
Ex u8 xdata save_ctxt_site;
Ex u8 xdata save_tag;

Ex u8 data scm_site;

//Ex volatile TI_PASS;

u8 check_fake_suspend();

void CrSetModelText(u8 *buf);
void CrSetVendorText(u8 *buf);

//void ljmp_bootstrap();

//void crc16();

void dbuf_bot_reset();
void dbuf_init();

void led_init();
#endif
