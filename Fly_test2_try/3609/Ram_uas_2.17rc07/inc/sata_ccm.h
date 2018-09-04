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

#ifndef __SATA_CCM_H__
#define __SATA_CCM_H__

//
// SATA Command Context Memory
//
#if 0
typedef struct _SATA_CCM {
	u8	ccm_prot;				// 00h
	u8	ccm_next;				// 01h
	u8	ccm_site;				// 02h
	u8	ccm_valid;				// 03h

	u16	ccm_cmdinten;			// 04h
	u8	ccm_SegIndex;			// 06C
	u8	ccm_Seg_INOUT;			// 07h
	u32	ccm_xfercnt;			// 08h
	u8	ccm_CdbIndex;			// 0ch
	u8	ccm_reserved13[3];		// 0Dh
	u8	ccm_fis[16];			// 10h
} SATA_CCM, *PSATA_CCM;
#endif


//
// CCM Prot
//
#define PROT_ND			0			// Non-Data protocol
#define PROT_PIOIN		1			// PIO-In protocol
#define PROT_PIOOUT		2			// PIO-Out protocol
#define PROT_UNKN		3			// Protocol is to be determined by the attached device
#define PROT_DMAIN		5			// DMA-In protocol
#define PROT_DMAOUT		6			// DMA-Out protocol
#define PROT_FPDMAIN	9			// First-Party DMA protocol, READ
#define PROT_FPDMAOUT	0xA			// First-Party DMA protocol, WRITE

#define PROT_PACKET_ND	0x0C
#define PROT_PACKET_DI	0x0D
#define PROT_PACKET_DO	0x0E
//#define PROT_PACKET	0xF			// PACKET protocol

//
// CCM Flag Bits
//
#define FLAGS_X		0x10			// indicates that the total transfer count is unknown
#define FLAGS_V		0x80			// indicates that the contents of this command site is valid.



#endif /* __SATA_CCM_H__ */
