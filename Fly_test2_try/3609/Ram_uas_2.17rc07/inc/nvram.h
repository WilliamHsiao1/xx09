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
 * 3607		2010/07/23	Odin		RomEnum version 1.0.6
 *
 *****************************************************************************/

#ifndef NVRAM_H
#define NVRAM_H

//------------------------
#undef  Ex
#ifdef NVRAM_C
	#define Ex
#else
	#define Ex extern
#endif

#define	NVR_DATASIZE		(sizeof(NVRAM))	// used in nvram

#define	NVR_PWR_MGMT		0x02
#define	NVR_DRV_PWR_MGMT	0x01

#define NVRAM_SCSI_HID		0x20

typedef struct _NVRAM {
	u8	signature[2];				//0-1
	u8  ChipID[2];					//2-3
	u8	ModelId[2];					//4-5
	U8  GUIDHi[3];					//6-8		Node vendor id (byte 0) 
	U8  ChipIDHi;					//9		Chip id hi 
	U8  ChipIDlo[4];				//Ah-Dh 
	u8	fwCtrl;						//Eh
	u8	WrCacheDisabled;			//Fh
	u8	revision;					//10h
	U8	rock_signature[4];			//11h-14h
	u8	reserved15[2];				//15h-16h
	u8	vendorStrLength;			//17h
	u8	vendorText[16];				//18h-27h
	u8	reserved2;					//28h
	u8	feVersionNvramOnly[2];		//29-2A
	u8	reserved3[2];				//2B-2C
	u8	sataHddIdleTime;			//2D
	u8	powerManagement;			//2E
	u8	modelStrLength;				//2F
	u8	modelText[32];				//30-4F
	//u8	reserved4E[2];				//4E-4F
	u8	ataPioDmaMode[2];			//50-51
	u8	reserved52[3];				//52-54
	u8	USB3_PWR;					//55
	u8	USB_ATTR;					//56
	u8	USB2_PWR;					//57
	u8	USB_VID[2];					//58-59
	u8	USB_PID[2];					//5A-5B
	u8	iSerial[20];				//5C-6F
	u8 	reserved70[0x7F - 0x70 + 1]; // 70-7F
	u8  test_version;   //write_buffer_test
	u8  test_version2;   //write_buffer_test

}NVRAM, *PNVRAM;

extern xdata NVRAM globalNvram;
extern code NVRAM localNvram;

#define NVRAM_SIG_0		0x25
#define NVRAM_SIG_1		0xC9

//fwCtrl
#define VENDOR_STR_FROM_NVRAM		0x01
#define PRODUCT_STR_FROM_NVRAM		0x02

Ex bit data vendor;
	#define	INITIO	0


#ifdef USB_FAST_ENUM

typedef struct _VITAL_DATA
{
	u8	signature[2];				//00h-01h
	//u8  ChipID[2];				//02-03
	u8  sobj_class;					//02
	u8  IsODD;					//03
	u8	vendor;						//04h
	u8	reserved25;				//05h
	u32	sectorLBA_l;				//06h-09h
	u32	sectorLBA_h;				//0Ah-0Ch
	u8	qdepth;						//0E
	u8	modelStrLength;				//0F
	u8	modelText[16];				//10-1F
	u8	reserved40[16];				//20-2F
	u8	ataPioDmaMode[2];			//30-31
#ifdef PHYSICAL_MORE_512
	u16 physical_sector_size;				
	u8	reserved52[1];
#else
	u8	reserved52[3];				//32-34
#endif	
	u8	USB3_PWR;					//35
	u8	USB_ATTR;					//36
	u8	USB2_PWR;					//37
	u8	USB_VID[2];					//38-39
	u8	USB_PID[2];					//3A-3B
	u8	iSerial[20];				//3C-4F
	u8	reserved70[0x10];			// 50h-5Fh
	u8 	standbyTimer8;				// 60h    
	u8 	standbyTimer0;				// 61h   
	u8 	standbyTimer1;				// 62h   
	u8 	standbyTimer2;				// 63h   
	u8 	standbyTimer3;				// 64h   
	u8  reserved85[2];				// 65h
	u8 	op_param_mode;				// 67h   
	u8 	iStr[20];	            	// 68-7Bh
	u8 	led0Status;             	// 7Ch
	u8	led1Status;					// 7Dh
	u8	Rsv9E;						// 7Eh
	u8	check_sum;					// 7Fh
} VITAL_DATA;

Ex xdata VITAL_DATA sflash_vital_data;

#define VITAL_SIG_0		0x25
#define VITAL_SIG_1		0xCB

#endif

Ex bit data valid_sflash_vital_data;

	Ex u8 xdata sflash_vid;
	Ex u8 xdata sflash_pid0;
	Ex u8 xdata sflash_pid1;
	Ex u8 xdata sflash_type;
		#define MXIC			0x00
		#define PMC				0x01
		#define WINBOND			0x02
		#define SST				0x03
		#define GIGADATA		0x04
		#define ESMT			0x05
		#define UNKNOWN_FLASH 	0xFF



Ex u8 xdata fw_version[2];

#define NV_VERSION(i) (fw_version[i])
//#define NV_VERSION(i) (localNvram.feVersionNvramOnly[i]) 

#define NV_DMA(i)		(SWAP16((u16)(1 << ((i) + 5))))
#define NV_UDMA(i)		(SWAP16((u16)(1 << ((i) + 8))))

#if (defined USB_FAST_ENUM || defined SCSI_DOWNLOAD_FW)
	extern void sflash_rd_jedec_id();
	extern void sflash_rd_id();
	extern void sflash_get_type();
	extern void sflash_Write_enable();
	extern void sflash_Write_disable();
	extern void sflash_erase_sector(u8 sector);
#endif
#ifdef	USB_FAST_ENUM
	extern void sflash_wr_vital_data();
	extern void sflash_init_vital_data();
	extern void sflash_rd_vital_data();
#endif
extern void NvReadDefaultlNvram(void);
extern void NvReadGlobalNvram(void);

#ifdef SCSI_DOWNLOAD_FW
extern void sflash_write_data(u8 xdata *ptr,u16 flash_addr,u16 downlozd_size);
extern void sflash_read_data(u16 flash_addr,u16 read_size);
extern void sflash_erase_chip();
#endif

#define ESMT_VID		0x8C
	#define F25L05PA			0x05		//512K


#define PMC_VID		0x7F					//JEDEC 0x9D
	#define	PM25LV512			0x7B		//512K
	#define	PM25LV010			0x7C		//1M
	#define	PM25LV020			0x7D		//2M
	#define	PM25LV040			0x7D		//4M
	#define	PM25LD010			0x10		//PM25LD010C
	#define	PM25LD020			0x11		//PM25LD020C

#define MXIC_VID		0xC2				//JEDEC 
	#define	MX25L512			0x10		//512K
	#define	MX25L1005			0x11		//1M/MX25L1006
	#define	MX25L2005			0x12		//2M/MX25L2006

#define SST_VID		0xBF
	#define	SST25VF512			0x48		//512K
	#define	SST25VF010A			0x49		//1M
	#define	SST25VF020			0x43		//2M

#define WINBOND_VID		0xEF				// JEDEC 
	#define	W25X10B				0x11		//1M
	#define	W25X20B				0x12		//2M

#define GIGADATA_VID	0xC8			// JEDEC 
	#define	GD25Q512			0x10		//512K
	#define	GD25Q10				0x11		//1M
	#define	GD25Q80B			0x14    	//8M

#define ESMT_VID        0x8C                // JEDEC 
	#define	F25L08PA            0x30        //8M
	
#endif
