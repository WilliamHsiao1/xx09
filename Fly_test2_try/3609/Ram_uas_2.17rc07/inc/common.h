/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2009-2013, All Rights Reserved
 *
 *   This file contains confidential and propietary information
 *   which is the property of Initio Corporation.
 *
 *						Header of All BOT Project
 *
 *****************************************************************************/

#ifndef COMMON_H
#define COMMON_H

// ++++++++++++++++++++++++++++++++++++++++++++++ //
// +++++++++++++++ READ/WRITE Macro +++++++++++++ //
// ++++++++++++++++++++++++++++++++++++++++++++++ //



#define	BIT_0	(0x00000001)
#define	BIT_1	(0x00000002)
#define	BIT_2	(0x00000004)
#define	BIT_3	(0x00000008)
#define	BIT_4	(0x00000010)
#define	BIT_5	(0x00000020)
#define	BIT_6	(0x00000040)
#define	BIT_7	(0x00000080)
//#define	BIT_8	(0x00000100)
//#define	BIT_9	(0x00000200)
//#define	BIT_10	(0x00000400)
//#define	BIT_11	(0x00000800)
//#define	BIT_12	(0x00001000)
//#define	BIT_13	(0x00002000)
//#define	BIT_14	(0x00004000)
//#define	BIT_15	(0x00008000)
//#define	BIT_16	(0x00010000)
//#define	BIT_17	(0x00020000)
//#define	BIT_18	(0x00040000)
//#define	BIT_19	(0x00080000)
//#define	BIT_20	(0x00100000)
//#define	BIT_21	(0x00200000)
//#define	BIT_22	(0x00400000)
//#define	BIT_23	(0x00800000)
//#define	BIT_24	(0x01000000)
//#define	BIT_25	(0x02000000)
//#define	BIT_26	(0x04000000)
//#define	BIT_27	(0x08000000)
//#define	BIT_28	(0x10000000)
//#define	BIT_29	(0x20000000)
//#define	BIT_30	(0x40000000)
//#define	BIT_31	(0x80000000)

#define B_BIT_0		BIT_0
#define B_BIT_1		BIT_1
#define B_BIT_2		BIT_2
#define B_BIT_3		BIT_3
#define B_BIT_4		BIT_4
#define B_BIT_5		BIT_5
#define B_BIT_6		BIT_6
#define B_BIT_7		BIT_7
#define B_BIT_8		BIT_0
#define B_BIT_9		BIT_1
#define B_BIT_10	BIT_2
#define B_BIT_11	BIT_3
#define B_BIT_12	BIT_4
#define B_BIT_13	BIT_5
#define B_BIT_14	BIT_6
#define B_BIT_15	BIT_7
#define B_BIT_16	BIT_0
#define B_BIT_17	BIT_1
#define B_BIT_18	BIT_2
#define B_BIT_19	BIT_3
#define B_BIT_20	BIT_4
#define B_BIT_21	BIT_5
#define B_BIT_22	BIT_6
#define B_BIT_23	BIT_7
#define B_BIT_24	BIT_0
#define B_BIT_25	BIT_1
#define B_BIT_26	BIT_2
#define B_BIT_27	BIT_3
#define B_BIT_28	BIT_4
#define B_BIT_29	BIT_5
#define B_BIT_30	BIT_6
#define B_BIT_31	BIT_7


#define	u8	unsigned char
#define	u16	unsigned short
#define	u32	unsigned long

#define	U8	unsigned char
#define	U16	unsigned short
#define	U32	unsigned long

//FIXME
//#define	u64	unsigned long long
//#define	u64	unsigned long

typedef struct _U16_DATA {
	union {
		u16	u_16;
		u8	u_8[2];
    } u16data;
} U16_DATA;

typedef struct _U32_DATA {
	union {
		u32	u_32;
		u16	u_16[2];
		u8	u_8[4];
    } udata;
} U32_DATA;

#define	reg_w8(addr, value)		*((u8 volatile xdata *)(addr)) = value
#define	reg_w16(addr, value)	*((u16 volatile xdata *)(addr)) = value	
#define	reg_w32(addr, value)	*((u32 volatile xdata *)(addr)) = value
#define	reg_r8(addr)			*((u8 volatile xdata *)(addr))
#define	reg_r16(addr)			*((u16 volatile xdata *)(addr))
#define	reg_r32(addr)			*((u32 volatile xdata *)(addr))

#define	SUCCESS		0
#define	FAILURE		1

#define INITIO_TEXT_24 "(C) 2002-13 Initio Corp."
#define INITIO_bin_liao "Hello bin_liao!"
#define INITIO_Hello   "Hello Initio!"

#ifdef ROM
	//-------------------------------------------------------
	// LED
	//-------------------------------------------------------
	// BOOT
	#define LED_DARK			0
	#define LED_BOOT_START		1
	#define LED_CPU_SWAP		2
	#define LED_SPI_COPY		3
	#define LED_CPU_RESET		4
	#define LED_BOOT_FAIL		5
	// ROM
	#define LED_ROM_START		6
	#define LED_PRINT_MSG		7
	#define LED_ROM_END			8

	#define LED(_x_)	{ *gpio_CtrlDO_0 = (*gpio_CtrlDO_0 & 0xC3) | (((~_x_) << 2) & 0x3C);	}
#endif

#endif //COMMON_H