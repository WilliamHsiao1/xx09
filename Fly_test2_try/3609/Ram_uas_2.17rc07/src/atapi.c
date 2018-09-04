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
 * atapi.c		2010/04/09			Initial version
 * 				2010/11/20	Odin	AES function
 *
 *****************************************************************************/

#define	ATAPI_C

#include "general.h"

#ifdef ATAPI

/****************************************\
	atapi_ExecUSBNoDataCmd
\****************************************/
#if 0
u8 atapi_ExecUSBNoDataCmd()
{
	DBG(("atapi_ExecUSBNoDataCmd:\n"));

	SCTXT_Flag |= SCTXT_FLAG_U2B|SCTXT_FLAG_B2S;
	
	//*ctxt_SataProto = PROT_PACKET << 4;
	*ctxt_SataProto = PROT_PACKET_ND << 4;

	SCM_CdbIndex = CTXT_NULL;
	SCM_Status = CCM_STATUS_PEND;
	
	//ATAPI no data, AUTO-FIS must D2HFISIEN in sata_SiteCmdIntEn
	*sata_SiteCmdIntEn_0 = D2HFISIEN;

#if 0
	*((u32 *)&(pCtxt->CTXT_FIS[FIS_TYPE])) = (PACKET << 16)|(0x80 << 8)|(HOST2DEVICE_FIS) ;
	*((u32 *)&(pCtxt->CTXT_FIS[FIS_LBA_LOW])) = (MASTER << 24);
	*((u32 *)&(pCtxt->CTXT_FIS[FIS_LBA_LOW_EXP])) = 0;
	*((u32 *)&(pCtxt->CTXT_FIS[FIS_SEC_CNT])) = 0;
#endif

	return sata_exec_ctxt();


}
#endif

/****************************************\
	atapi_exec_ctxt
\****************************************/
#if 0
void atapi_exec_ctxt(u8 prot)
{

	pCCM->ccm_prot = FLAGS_V|prot;
	pCCM->ccm_cmdinten	= 0;
	pCCM->ccm_xfercnt = pCtxt->CTXT_XFRLENGTH;


	*sata_EXQHINP = 0; 
}

#endif


/***************************************************************************\
	atapi_ReadID()

	Reading 512 bytes of information in response to IDENTIFY_PACKET_DEVICE
\*****************************************************************************/
u8 atapi_ReadID()  // could have argument - ATA/ATAPI , which ID command to use     // to do 2 times and compare
{
	//u16 i, read_port;
	//u8 *pU8;
	
	//DBG(("atapi Read ID\n"));
	//DBG0(("atapi Read ID\n"));

	*sata_CCMSITEINDEX = 0;		// 
	scm_site = 0;
	SCM_INDEX = 0;

	// CPU R <-- SATA 0
	SCM_prot = PROT_PIOIN;
	SCM_CdbIndex = CTXT_NULL;
	SCM_Status = CCM_STATUS_PEND;	
	SCM_SegIndex = DBUF_SEG_S2C;
	SCM_SegINOUT  = (TX_DBUF_CPU_R_PORT << 4) |  TX_DBUF_SATA0_W_PORT;			// CPU R <-- SATA 0

	*sata_Ccm_prot = CCM_V|PROT_PIOIN;
	*sata_Ccm_cmdinten_0 = 0;
	*sata_Ccm_cmdinten_1 = 0;	
	
	// 512 bytes;
	*sata_Ccm_xfercnt_0 = 0;
	*sata_Ccm_xfercnt_1 = 0x02;
	*sata_Ccm_xfercnt_2 = 0;
	*sata_Ccm_xfercnt_3 = 0;

	sata_Ccm_FIS[FIS_TYPE] = HOST2DEVICE_FIS;
	sata_Ccm_FIS[FIS_C] = 0x80;
	sata_Ccm_FIS[FIS_COMMAND] = ATA6_IDENTIFY_PACKET_DEVICE;
	sata_Ccm_FIS[FIS_FEATURE] = 0;

	sata_Ccm_FIS[FIS_LBA_LOW] = 0;
	sata_Ccm_FIS[FIS_LBA_MID] = 0;
	sata_Ccm_FIS[FIS_LBA_HIGH] = 0;
	sata_Ccm_FIS[FIS_DEVICE] = MASTER;

	sata_Ccm_FIS[FIS_LBA_LOW_EXP] = 0;
	sata_Ccm_FIS[FIS_LBA_MID_EXP] = 0;
	sata_Ccm_FIS[FIS_LBA_HIGH_EXP] = 0;
	sata_Ccm_FIS[FIS_FEATURE_EXP] = 0;

	sata_Ccm_FIS[FIS_SEC_CNT] = 0;
	sata_Ccm_FIS[FIS_SEC_CNT_EXP] = 0;
	sata_Ccm_FIS[FIS_RESERVED_14] = 0;
	sata_Ccm_FIS[FIS_CONTROL] = 0;

	if ((val = sata_exec_internal_ccm()) == CCM_STATUS_GOOD)
	{
		// copy 512 bytes data from TX DBuf Seg 4 to mc_buffer
		pU8		= (u8 *)mc_buffer;
		while (1)
		{
			sz16	= (dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_0) + (dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_1 << 8);
			if (sz16)
				break;
			Delay(1);	
		}
 		for (; sz16 != 0; sz16--)
			*pU8++ = *dbuf_DataPort;
	}
	
//	pSob->sobj_State = SATA_READY;
	
#ifndef SATA_AUTO_FIFO_RST
	*sata_BlkCtrl_1 = RXSYNCFIFORST; //reset sata RX  fifo
#endif

	*dbuf_MuxSel = DBUF_SEG_S2C;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxCtrl;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;

	return val;
}


/****************************************\
	atapi_init
\****************************************/
bit atapi_init()
{
	//DBG(("Atapi init\n"));

	//FIXME: SATA FIS INIT
	*sata_PacketLba_1 = 0xFF; 
	*sata_PacketLba_2 = 0xFF;
	*sata_PacketFeature = FEATURES_DMA;	

	*usb_Msc0Lun_SAT_0 = SATA_PACKET_CMD;
	*usb_Msc0Lun_SAT_1 |= SAT_DISABLE;

	//obtain 256 words of device information from IDENTIFY_DEVICE command
	if (atapi_ReadID() == CCM_STATUS_ERROR)
		return 1;
	
	//AtaInitDmaMode();

	//ata_get_serial_num();


	return 0;
}

#ifdef USB_FAST_ENUM

bit atapi_fast_enum_init()
{
	if (atapi_init() == 0)
	{
		sobj_init = 1;

		sobj_State = SATA_READY;
		//*chip_status |= HDD_VALID;		// Valid HDD
		return 0;
	}
	else
	{
		//DBG0(("SATA_NO_DEV\n"));
		sobj_State = SATA_NO_DEV;
		usb_active = 0;
		return 1;
	}
}
#endif
#endif
