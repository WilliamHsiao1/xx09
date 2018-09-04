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
 * 3607		2010/11/20	Odin		AES function
 * 3609		2010/12/21	Ted			Remove AES function
 *****************************************************************************/

#define	ATA_C

#include	"general.h"
//#include "dispatch.h"




/****************************************\
	AtaSetDMAXferMode
\****************************************/
#if 0
u8 ata_SetDMAXferMode(u8 mode)
{

	DBG(("ata_SetDMAXferMode %bx\n", mode));
	
	*sata_CCMSITEINDEX = 0;		// 
	scm_site = 0;
	SCM_INDEX = 0;

	SCM_prot = PROT_ND;
	SCM_CdbIndex = CTXT_NULL;
//	SCM_site = 0;
	SCM_Status = CCM_STATUS_PEND;
	SCM_SegIndex = DBUF_SEG_NULL;

	*sata_Ccm_prot = CCM_V|PROT_ND;
	*sata_Ccm_cmdinten_0 = D2HFISIEN;
	*sata_Ccm_cmdinten_1 = 0;

	*sata_Ccm_xfercnt_0 = 0;
	*sata_Ccm_xfercnt_1 = 0;
	*sata_Ccm_xfercnt_2 = 0;
	*sata_Ccm_xfercnt_3 = 0;

	sata_Ccm_FIS[FIS_TYPE] = HOST2DEVICE_FIS;
	sata_Ccm_FIS[FIS_C] = 0x80;
	sata_Ccm_FIS[FIS_COMMAND] = ATA6_SET_FEATURES;
	sata_Ccm_FIS[FIS_FEATURE] = 0x03;

	sata_Ccm_FIS[FIS_LBA_LOW] = 0;
	sata_Ccm_FIS[FIS_LBA_MID] = 0;
	sata_Ccm_FIS[FIS_LBA_HIGH] = 0;
	sata_Ccm_FIS[FIS_DEVICE] = MASTER;

	sata_Ccm_FIS[FIS_LBA_LOW_EXP] = 0;
	sata_Ccm_FIS[FIS_LBA_MID_EXP] = 0;
	sata_Ccm_FIS[FIS_LBA_HIGH_EXP] = 0;
	sata_Ccm_FIS[FIS_FEATURE_EXP] = 0;

	sata_Ccm_FIS[FIS_SEC_CNT] = mode;
	sata_Ccm_FIS[FIS_SEC_CNT_EXP] = 0;
	sata_Ccm_FIS[FIS_RESERVED_14] = 0;
	sata_Ccm_FIS[FIS_CONTROL] = 0;

	return sata_exec_internal_ccm();
}
#endif

/****************************************\
	AtaCheckPowerMode
\****************************************/
#if 0
u8 ata_CheckPowerMode()
{
	//CTXT_FLAG_RET_TFR;

	*sata_CCMSITEINDEX = 0;		//
	scm_site = 0;
	SCM_INDEX = 0;

	SCM_prot = PROT_ND;

	SCM_CdbIndex = CTXT_NULL;
//	SCM_site = 0;
	SCM_Status = CCM_STATUS_PEND;
	SCM_SegIndex = DBUF_SEG_NULL;

	*sata_Ccm_prot = CCM_V|PROT_ND;
	*sata_Ccm_cmdinten_0 = D2HFISIEN;
	*sata_Ccm_cmdinten_1 = 0;

	*sata_Ccm_xfercnt_0 = 0;
	*sata_Ccm_xfercnt_1 = 0;
	*sata_Ccm_xfercnt_2 = 0;
	*sata_Ccm_xfercnt_3 = 0;


	sata_Ccm_FIS[FIS_TYPE] = HOST2DEVICE_FIS;
	sata_Ccm_FIS[FIS_C] = 0x80;
	sata_Ccm_FIS[FIS_COMMAND] = ATA6_CHECK_POWER_MODE;
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

	sata_exec_internal_ccm();
	DBG(("PowerMode:%BX, 00:StandBy, 80:Idle, FF:Active/Idle\n",sata_Ccm_FIS[FIS_SEC_CNT]));
	return sata_Ccm_FIS[FIS_SEC_CNT];
}
#endif

/****************************************\
	AtaExecNoDataCmd
\****************************************/
u8 ata_ExecNoDataCmd(u8 command, u8 feature)
{
	*sata_CCMSITEINDEX = 0;		// 
	scm_site = 0;
	SCM_INDEX = 0;

	SCM_prot = PROT_ND;
	SCM_CdbIndex = CTXT_NULL;
//	SCM_site = 0;
	SCM_Status = CCM_STATUS_PEND;
	SCM_SegIndex = DBUF_SEG_NULL;

	*sata_Ccm_prot = CCM_V|PROT_ND;
	
	//FIXME: AUTO_FIS will reset it!
	*sata_Ccm_cmdinten_0 = D2HFISIEN;
	*sata_Ccm_cmdinten_1 = 0;

	*sata_Ccm_xfercnt_0 = 0;
	*sata_Ccm_xfercnt_1 = 0;
	*sata_Ccm_xfercnt_2 = 0;
	*sata_Ccm_xfercnt_3 = 0;

	sata_Ccm_FIS[FIS_TYPE] = HOST2DEVICE_FIS;
	sata_Ccm_FIS[FIS_C] = 0x80;
	sata_Ccm_FIS[FIS_COMMAND] = command;
	sata_Ccm_FIS[FIS_FEATURE] = feature;

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

	return sata_exec_internal_ccm();
}

/****************************************\
	ata_ExecUSBNoDataCmd

	Input Global Parameter:
		ctxt_site: site # of CDB Context
		SCTXT_INDEX: site # of CDB Context
		*host_ctxtmem_pt: site # of CDB Context
\****************************************/
u8 ata_ExecUSBNoDataCmd()
{
	//DBG(("ata_ExecUSBNoDataCmd\n"));


	*ctxt_SataProto = PROT_ND << 4;

	*ctxt_CCMcmdinten = D2HFISI;
	if (bot_mode)
	{
		*ctxt_PhaseCase_0 = 0x02;
		*ctxt_PhaseCase_1 = 0x00;
	}
	else
	{	// UAS Mode
		*ctxt_XferLen_0 = 0;
		*ctxt_XferLen_1 = 0;
		*ctxt_XferLen_2 = 0;
		*ctxt_XferLen_3 = 0;
	}

	SCTXT_Flag |= SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B|SCTXT_FLAG_B2S;
	SCTXT_DbufIndex = DBUF_SEG_NULL;

	ctxt_FIS[FIS_TYPE] = HOST2DEVICE_FIS;		//
	ctxt_FIS[FIS_C] = 0x80;
	//ctxt_FIS[FIS_COMMAND] = command;			//COMMAND
	//ctxt_FIS[FIS_FEATURE] = 0;				//FEATURES (7:0)

	ctxt_FIS[FIS_LBA_LOW] = 0;
	ctxt_FIS[FIS_LBA_MID] = 0;
	ctxt_FIS[FIS_LBA_HIGH] = 0;
	ctxt_FIS[FIS_DEVICE] = 0;			//MASTER;				//Device

	ctxt_FIS[FIS_LBA_LOW_EXP] = 0;
	ctxt_FIS[FIS_LBA_MID_EXP] = 0;
	ctxt_FIS[FIS_LBA_HIGH_EXP] = 0;
	ctxt_FIS[FIS_FEATURE_EXP] = 0;

	ctxt_FIS[FIS_SEC_CNT] = 0;
	ctxt_FIS[FIS_SEC_CNT_EXP] = 0;
//	ctxt_FIS[FIS_RESERVED_14] = 0;
//	ctxt_FIS[FIS_CONTROL] = 0;


	return sata_exec_ctxt();


}

/****************************************\
//	ata_ReadID
//	Reading 512 bytes of information
//	in response to IDENTIFY_DEVICE
//
// Return:
//	0: OK;
//	1: failed.
/****************************************/
u8 ata_ReadID()  // could have argument - ATA/ATAPI , which ID command to use     // to do 2 times and compare
{
	//u16 i;
	//u8 *pU8;

	
	// SATA0 --> CPU
	//DBG(("RdId\n"));
	
	*sata_CCMSITEINDEX = 0;		// 
	scm_site = 0;
	SCM_INDEX = 0;
	

	SCM_prot = PROT_PIOIN;
	SCM_CdbIndex = CTXT_NULL;
//	SCM_site = 0;
	SCM_Status = CCM_STATUS_PEND;
	SCM_SegIndex = DBUF_SEG_S2C;
	SCM_SegINOUT  = (TX_DBUF_CPU_R_PORT << 4) |  TX_DBUF_SATA0_W_PORT;			// CPU R <-- SATA W

	//FIXME: I do not know whether CCM_V be clear
	//*sata_Ccm_prot_3 = CCM_V;

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
	sata_Ccm_FIS[FIS_COMMAND] = ATA6_IDENTIFY_DEVICE;
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

	//sata_Ccm_FIS[FIS_RESERVED_16] = 0;
	//sata_Ccm_FIS[FIS_RESERVED_17] = 0;
	//sata_Ccm_FIS[FIS_RESERVED_18] = 0;
	//sata_Ccm_FIS[FIS_RESERVED_19] = 0;
	
	if ((val = sata_exec_internal_ccm()) == CCM_STATUS_GOOD)
	{
		// copy 512 bytes data from DBuf Seg 1 to mc_buffer
		sz16	= (dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_0) + (dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_1 << 8);

		if (sz16 > MC_BUFFER_SIZE)
			sz16 = MC_BUFFER_SIZE;

		if (sz16)
		{
			pU8		= (u8 *)mc_buffer;
 			for (; sz16 != 0; sz16--)
				*pU8++ = *dbuf_DataPort;
		}
		else
		{
			val = CCM_STATUS_ERROR;
		}
	}
	
//	pSob->sobj_State = SATA_READY;
	
#ifndef SATA_AUTO_FIFO_RST
//	*sata_BlkCtrl_1 = RXSYNCFIFORST; //reset sata RX  fifo
#endif

	*dbuf_MuxSel = DBUF_SEG_S2C;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxCtrl;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;

	return val;


}

/****************************************\
	ata_get_vendor_product
\****************************************/
static void ata_get_vendor_product()
{
	sz16 = 0;
	i8 = 0;
	for (tmp16 = 0; tmp16 < 20; tmp16++)
	{
		//first byte
		tmp8 = mc_buffer[(27 + tmp16) * 2 + 1];
		if(tmp8 != ' ')
		{
			if(i8 == 0)
			{
				module_vendor_text[sz16] = tmp8;
				sz16++;
			}
			else
			{
				module_product_text[sz16] = tmp8;
				sz16++;
			}
		}
		else
		{
			if(i8 == 1)
			{
				while(sz16 < 32)
				{
					module_product_text[sz16] = ' ';
					sz16++;
				}
			}
			else
			{
				while(sz16 < 16)
				{
					module_vendor_text[sz16] = ' ';
					sz16++;
				}
				i8 = 1;
				sz16 = 0;
			}
		}
		//second byte
		tmp8 = mc_buffer[(27 + tmp16) * 2];
		if(tmp8 != ' ')
		{
			if(i8 == 0)
			{
				module_vendor_text[sz16] = tmp8;
				sz16++;
			}
			else
			{
				module_product_text[sz16] = tmp8;
				sz16++;
			}
		}
		else
		{
			if(i8 == 1)
			{
				while(sz16 < 32)
				{
					module_product_text[sz16] = ' ';
					sz16++;
				}
			}
			else
			{
				while(sz16 < 16)
				{
					module_vendor_text[sz16] = ' ';
					sz16++;
				}
				i8 = 1;
				sz16 = 0;
			}
		}
	}
}


/****************************************\
	ata_get_serial_num
\****************************************/
void ata_get_serial_num()
{
	u8	i, indx;

	// left-justfied Serial #
	for (i = 0; i < 20; i++)
	{
		if (serialStr[i] != ' ')
		{
			// found 1st non-space character
			if (i == 0)
			{
				// Left-justfied already
				// replace 0 with Space charcter.
				for (; i < 20; i++)
				{
					if (serialStr[i] == 0)
						serialStr[i] = ' ';
				}
				break;
			}

			// not Left-justfied
			indx = 0;
			for (; i < 20; i++)
			{
				u8 val;

				// quit as soon as we hit a 0.  WD pads the serial number with 0's (not
				// to be confused with an ascii '0')
				val = serialStr[i];
				if (val == 0)
					val = ' ';
				serialStr[indx] = val;
				indx++;
			}
			for (; indx < 20; indx++)
				serialStr[indx] = ' ';
			break;
		}
	}
}

void ata_set_usb_msc()
{
	//if (sobj_sectorLBA_h)
	if (sobj_sector_4K)
	{
		//sobj_sector_4K = 1;
		tmp8 = HOST_BLK_4K | SATA_BLK_512 | SATA_DMAE_CMD;
		utmp32.udata.u_32 = (sobj_sectorLBA_l & 0xFFFFFFF8) - 0x09;
	}
#ifdef PHYSICAL_MORE_512
	else if (sobj_physical_sector_size == 0x800)
	{
		tmp8 = HOST_BLK_4K | SATA_BLK_4K | SATA_DMAE_CMD;

		utmp32.udata.u_32 = sobj_sectorLBA_l - 2;
	}
#endif
	else
	{
		tmp8 = HOST_BLK_512 | SATA_BLK_512 | SATA_DMAE_CMD;

		utmp32.udata.u_32 = sobj_sectorLBA_l - 2;
	}
	COPYU32_REV_ENDIAN_D2X(&utmp32, usb_Msc0LunExtentLo_0);
	COPYU32_REV_ENDIAN_D2X(&sobj_sectorLBA_h, usb_Msc0LunExtentHi_0);

#ifdef UAS_EN
	if (!bot_mode)
	{
		if (sobj_qdepth >= MIN_SATA_Q_DEPTH)
		{
			//DBG(("sobj_qdepth %BX\n", sobj_qdepth));
			tmp8 = (tmp8 & ~SAT_CMD)  | SATA_NCQ_CMD;
		}
	}
#endif
	*usb_Msc0Lun_SAT_0 = tmp8;
	// Does not support FUA bit in SCSI "Write" command
	*usb_Msc0LunSATX = FUA_FPDMA_DISABLE|FUA_EXT_DISABLE;


	//MSG(("\nTotal LBA: %lx %lx\n", (u32)(sobj_sectorLBA_h), (u32)sobj_sectorLBA_l));
}

/****************************************\
	ata_init
\****************************************/


bit ata_init()
{
	sobj_sector_4K = 0;

	//obtain 256 words of device information from IDENTIFY_DEVICE command
	if (ata_ReadID() == CCM_STATUS_ERROR)
		return 1;

	sobj_mediaType  = MEDIATYPE_FIXED;

#ifdef UAS_EN
	if (mc_buffer[76 * 2 + 1] & 0x01)
	{	// support NCQ
		sobj_ncq_mode = 1;
		sobj_qdepth = (mc_buffer[75 * 2] & 0x1F);
		//DBG(("QDEP %BX\n",sobj_qdepth));
	}
	else
    {
		sobj_ncq_mode = 0;
		sobj_qdepth = 1;
		//DBG(("no NCQ\n"));
    }
#else
	sobj_qdepth = (mc_buffer[75 * 2] & 0x1F);
	if ((mc_buffer[76 * 2 + 1] & 0x01) && (sobj_qdepth > 1))
	{	// support NCQ
		//DBG0(("QDEP %BX\n",sobj_qdepth));
	}
	else
    {
		sobj_qdepth = 1;
		sobj_ncq_mode = 0;
		//DBG(("no NCQ\n"));
    }
#endif
	sata_set_cfree();

	//ata 6 && 48 bit addr supported
	//word 80 & 83 from device information
	if (mc_buffer[(80 * 2)] & 0x40 && mc_buffer[(83 * 2) + 1] & 0x04)
	{
		//sobj_deviceType= DEVICETYPE_HD_ATA6;
		COPYU32_REV_ENDIAN_X2D(mc_buffer + 100 * 2, &sobj_sectorLBA_l);
		COPYU32_REV_ENDIAN_X2D(mc_buffer + 102 * 2, &sobj_sectorLBA_h);
//DBG(("Cap %LX %LX\n", sobj_sectorLBA_h, sobj_sectorLBA_l));
	}
	else
	{
		//sobj_deviceType = DEVICETYPE_HD_ATA5;
		//COPYU32_REV_ENDIAN_X2D(mc_buffer + 60 * 2, &sobj_sectorLBA_l);
		//sobj_sectorLBA_h = 0;
		//*usb_Msc0Lun_SAT_0 = HOST_BLK_512 | SATA_BLK_512 | SATA_DMAE_CMD;
		ERR(("\nATA5\n"));
		return 1;
	}
#ifdef PHYSICAL_MORE_512
	// word 106, bit14 shall 1, bit 15 shall 0, then word 106 is valid, else invlaid
	// bit 13 is set to 1, then more logical sector per physical, and bits(3:0) are vaild  (hdd's logical is F/W's physical)
	// bit 12 is set to 1, then word117...118 are valid, and logical sector than 256 words.
	if ((mc_buffer[(106*2)+1] & 0xC0) == 0x40)  // word 106 valid
	{
		// if (mc_buffer[(106*2)+1] & 0x20) // bit 13  // looks like we do NOT need care it
		if (mc_buffer[(106*2)+1] & 0x10) // bit 12
		{
			COPYU16_REV_ENDIAN_X2D(mc_buffer + 117*2, &sobj_physical_sector_size);
		}
		else
		{
			sobj_physical_sector_size = 0x100;  // 512 byte / sector
		}
	}
	else
	{
		sobj_physical_sector_size = 0x100;  // 512 byte / sector
	}
	MSG(("ai_ss %x", sobj_physical_sector_size));
#endif
#ifdef SUPPORT_3T_4K
	if (sobj_sectorLBA_h)
	{
//DBG(("> 2T\n"));
	
		sobj_sector_4K = 1;
	}
#else	
	rCapLimit = 3;
#endif	
	ata_set_usb_msc();

	ata_get_vendor_product();

	memcpySwap16(mc_buffer + 10 * 2, sobj_serialStr, 10);

	if (hdd_serial)
		xmemcpy(sobj_serialStr, serialStr, 20);
	else
		xmemcpy(globalNvram.iSerial, serialStr, 20);

	ata_get_serial_num();
	memcpySwap16(mc_buffer + 108 * 2, sobj_wnn,  4);	

	sobj_WrCache_supported = 0;
	sobj_WrCache_enabled = 0;
#if 0
	// Is Write Cache supported ?
	if (mc_buffer[(82 * 2)] & 0x20)
	{
		sobj_WrCache_supported = 1;
		// Is Write Cache Enabled ?
		if ((mc_buffer[(85 * 2)] & 0x20) == 0)
		{
			//pCCM->ccm_fis[FIS_FEATURE] = 0x02;	//set cache mode
			ata_ExecNoDataCmd(ATA6_SET_FEATURES, 0x02);
		}
		sobj_WrCache_enabled = 1;		// Write Cache enabled
	}
#endif
	ata_get_serial_num();

	//compare the return data from device (identify dev cmd) with fixed data
	//from global NVRAM to determine operation mode for SET_FEATURES command

	//memcpySwap16((mc_buffer + 27 * 2), modelText, 20);	//

#ifdef SUPPORT_WWN
	memcpySwap16((mc_buffer + 108 * 2), sobj_wwn, 4);	//
	#ifdef DBG_FUNCTION
		mem_dump((u8  xdata *)sobj_wwn, 8);
	#endif
#endif

//	AtaInitDmaMode();
	return 0;
}


#ifdef USB_FAST_ENUM

bit ata_fast_enum_init()
{
	if (ata_init() == 0)
	{
		sobj_init = 1;

		sobj_State = SATA_READY;
		*chip_status |= HDD_VALID;		// Valid HDD

		if ((sobj_sectorLBA_l != sflash_vital_data.sectorLBA_l) ||
			(sobj_sectorLBA_h != sflash_vital_data.sectorLBA_h) ||
#ifdef PHYSICAL_MORE_512
			(sobj_physical_sector_size != sflash_vital_data.physical_sector_size) ||
#endif
			(sobj_qdepth != sflash_vital_data.qdepth) ||
			(xmemcmp((U8 *)sobj_serialStr,  (u8 xdata *)&(sflash_vital_data.iStr[0]), 20))
			)
		{
#ifdef DBG_FUNCTION0
		mem_dump((U8 *)sobj_serialStr, 20);
		mem_dump((u8 xdata *)&(sflash_vital_data.iStr[0]), 20);

#endif

#ifdef ASIC_CONF
			if (revision_a61)
			{
				spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
			}
#endif

#if 0
			if (sobj_sectorLBA_l != sflash_vital_data.sectorLBA_l)
			{
				DBG0(("bad rLBA_l\n"));
			}
			if (sobj_sectorLBA_h != sflash_vital_data.sectorLBA_h)
			{
				DBG0(("bad rLBA_h\n"));
			}
			if (sobj_qdepth != sflash_vital_data.qdepth) 
			{
				DBG0(("bad qd %bx\n", sobj_qdepth));
			}
			if ((xmemcmp((u8 *)sobj_serialStr,  (u8 xdata *)&(sflash_vital_data.iStr[0]), 20)))
			{
				DBG0(("bad istr\n", sobj_qdepth));
//mem_dump(sobj_serialStr, 20);
//mem_dump(sflash_vital_data.iStr, 20);

			}
#endif

#ifdef UAS_EN
			//if (sobj_qdepth >= MIN_SATA_Q_DEPTH)
			//{	// at least support 8 tags
			//	*usb_Msc0Ctrl_0 = MSC_UAS_ENABLE|MSC_BOT_ENABLE;
			//}
			//else
			//{	// enable BOT only
			//	*usb_Msc0Ctrl_0 = MSC_BOT_ENABLE;
			//}
#endif

			sflash_init_vital_data();

			if (bot_cbw_active)
			{
				usb_reenum_flag = 1;
			}
			else
			{
				//*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
				//*usb_DevCtrlClr_0 = USB_ENUM;

				//InitDesc();

				//*usb_DevCtrl_1 = USB_CORE_RESET;
				//Delay(600);
				//*usb_DevCtrl_0 = USB_ENUM;
				usb_reenum();
				if (bot_mode)
				{
					bot_init();
				}
			#ifdef UAS_EN
				else
				{
					uas_init();
				}
			#endif
			}
		}
		return 0;
	}
	else
	{
		//DBG0(("NO_DEV\n"));
		sobj_State = SATA_NO_DEV;
		usb_active = 0;
		return 1;
	}
}
#endif
