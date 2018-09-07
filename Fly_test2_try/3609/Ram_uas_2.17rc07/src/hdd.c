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
 * 3607		2010/12/08	Ted			Fix ATA_PASS_THR handle
 *									SCSI_INQUIRY command support >255 byteCnt
 *
 *****************************************************************************/
#define HDD_C


#include	"general.h"




#ifdef UAS_EN
bit hdd_set_uas_xfer_len()
{
	switch (CmdBlk(2) & 0x03)	// T_LENGTH field
	{

	case 1:						// transfer length is specified in the FEATURES (7:0) field
		byteCnt = ctxt_FIS[FIS_FEATURE];
		break;

	case 2:						// transfer length is specified in the SECTOR_COUNT (7:0) field
		byteCnt = ctxt_FIS[FIS_SEC_CNT];
		break;

	case 0:						//No data is transferred
	case 3:						//transfer length specified in the TPSIU
		// Illegal T_LENGTH field & set xfer length to 512
		byteCnt = 512;
		goto exit;
	}
	// check BYTE_BLOCK field
	if (CmdBlk(2) & 0x04)
	{
		byteCnt = byteCnt << 9;
	}

	if (byteCnt == 0)
	{
		return 1;
	}
exit:
//DBG(("uas %x ", byteCnt));
	*ctxt_XferLen_0 = byteCnt;
	*ctxt_XferLen_1 = byteCnt >> 8;
	*ctxt_XferLen_2 = 0;
	*ctxt_XferLen_3 = 0;

//DBG(("%bx%bx\n", *ctxt_XferLen_1, *ctxt_XferLen_0));

	return 0;

}
#endif

/****************************************\
	hdd_return_descriptor_sense
\****************************************/
#ifdef ATA_PASS_THROUGTH
void hdd_return_descriptor_sense(u8 * buffer)
{
// By Ted: use SATA_FIS instead of CTXT_FIS
	buffer[0] = 0x09;	// ATA return
	buffer[1] = 12;		// ADDITIONAL LENGTH (n-1)
	buffer[2] = 0x00;	//

	buffer[3] = bot_tfr_fis[FIS_ERROR];	// ERROR

	buffer[4] = 0x00;
	buffer[5] = bot_tfr_fis[FIS_SEC_CNT];	// ATA Sector Count

	buffer[6] = 0x00;
    buffer[7] = bot_tfr_fis[FIS_LBA_LOW];	// ATA LBA(7:0))

	buffer[8] = 0x00;
	buffer[9] = bot_tfr_fis[FIS_LBA_MID];	// ATA LBA(15:8))

	buffer[10] = 0x00;
    buffer[11] = bot_tfr_fis[FIS_LBA_HIGH];	// ATA LBA(23:16))

    buffer[12] = bot_tfr_fis[FIS_DEVICE];	// device
    buffer[13] = bot_tfr_fis[FIS_STATUS];	// status

	if (extended_descriptor)
	{
	    buffer[2] = 0x01;	// Extend
		buffer[4] = bot_tfr_fis[FIS_SEC_CNT_EXP];
		buffer[6] = bot_tfr_fis[FIS_LBA_LOW_EXP];	//sata_ch_reg->sata_LbaLH;
		buffer[8] = bot_tfr_fis[FIS_LBA_MID_EXP];	//sata_ch_reg->sata_LbaMH;
		buffer[10] = bot_tfr_fis[FIS_LBA_HIGH_EXP];	//sata_ch_reg->sata_LbaHH;
	}
}
#endif


/****************************************\
	hdd_ata_err_2_sense_data
\****************************************/
void hdd_ata_err_2_sense_data()
{
	//u8	*pSenseData;

	SCTXT_Status = CTXT_STATUS_ERROR;	
#ifdef UAS_EN
	SCTXT_LunStatus	= LUN_STATUS_CHKCOND;

	if (!bot_mode)
	{
		ctxt_CDB[16] = 18;
		ctxt_CDB[17] = SCSI_SENSE_NO_SENSE;
		ctxt_CDB[18] = 0;
		ctxt_CDB[19] = 0;
		pU8 = &(ctxt_CDB[17]);
	}
	else
#endif
	{
		bot_sense_data[0] = 18;			// Sense_Length
		bot_sense_data[1] = SCSI_SENSE_NO_SENSE;
		bot_sense_data[2] = 0;
		bot_sense_data[3] = 0;
		pU8 = &(bot_sense_data[1]);
	}
	{

		val = ctxt_FIS[FIS_ERROR];
		if (val & ATA_ERR_ICRC)  //interface CRC error has occurred
		{
			//DBG(("S CRC %bx\n", ctxt_CDB[0]));
#ifdef EQ_SETTING_TEST
			if(sobj_crc_count < 0xFF)
				sobj_crc_count++;
#endif
			pU8[0] = SCSI_SENSE_ABORTED_COMMAND;
			pU8[1] = 0x47;							//
			pU8[2] = 0x03;							// INFORMATION UNIT iuCRC ERROR DETECTED
			return;
		}
		else if (val & ATA_ERR_UNC)  //data is uncorrectable
		{
			pU8[0] = SCSI_SENSE_MEDIUM_ERROR;		// sense key: MEDIUM Error
			pU8[1] = 0x11;							// ASC: UNRECOVERED READ ERROR
//          pU8[2] = 0x00;							//
			return;
		}
		else if (val & ATA_ERR_IDNF)  //address outside of the range of user-accessible addresses is requested
		{
			pU8[0] = SCSI_SENSE_MEDIUM_ERROR;		// sense key:MEDIUM Error
			pU8[1] = 0x14;							//
			pU8[2] = 0x01;							// RECORD NOT FOUND
			return;
		}
		else // if (val & ATA_ERR_ABRT) // Aborted Command: NO ADDITIONAL SENSE INFORMATION
		{
			pU8[0] =  SCSI_SENSE_ABORTED_COMMAND;	// sense key: 
		}
		return;
	}
}

/****************************************\
	hdd_ata_return_tfr
\****************************************/
#ifdef ATA_PASS_THROUGTH
void hdd_ata_return_tfr()
{
	SCTXT_Status = CTXT_STATUS_ERROR;	
	SCTXT_LunStatus	= LUN_STATUS_CHKCOND;

	val = 0;
	if (CmdBlk(0) == SCSI_ATA_PASS_THR16)
	{
		val = CmdBlk(1) & 0x01;		// extend bit
	}

	if (!bot_mode)
	{	// UAS mode
		ctxt_CDB[16] = 22;
		ctxt_CDB[17] = val;
		ctxt_CDB[18] = 0;
		ctxt_CDB[19] = 0;
	}
	else
	{
		bot_sense_data[0] = 22;			// Sense_Length
		bot_sense_data[1] = val;			// 
		bot_sense_data[2] = 0;			// 
		bot_sense_data[3] = 0;			// 

		//bot_tfr_fis[FIS_STATUS]		= ctxt_FIS[FIS_STATUS] ;
		//bot_tfr_fis[FIS_ERROR]		= ctxt_FIS[FIS_ERROR] ;
		//bot_tfr_fis[FIS_LBA_LOW]		= ctxt_FIS[FIS_LBA_LOW] ;
		//bot_tfr_fis[FIS_LBA_LOW_EXP]= ctxt_FIS[FIS_LBA_LOW_EXP] ;
		//bot_tfr_fis[FIS_SEC_CNT]	= ctxt_FIS[FIS_SEC_CNT];
		//bot_tfr_fis[FIS_SEC_CNT_EXP]	= ctxt_FIS[FIS_SEC_CNT_EXP];
	}
}
#endif

/****************************************\
	hdd_format_unit_cmd()

	Input Global Parameter:
		ctxt_site: site # of CDB Context
\****************************************/
void hdd_format_unit_cmd()
{
	//DBG(("FMT %BX\n", CmdBlk(1)));
	if (CmdBlk(1))
	{	//we don't accept a parameter list.
		hdd_err_2_sense_data(ERROR_ILL_CDB);
	}
	usb_device_no_data();
}
/****************************************\
	hdd_err_2_sense_data
\****************************************/
void hdd_err_2_sense_data(u8 error)
{
	//*ctxt_Status = CTXT_STATUS_ERROR;	
	SCTXT_Status = CTXT_STATUS_ERROR;	

#ifdef UAS_EN
	//*ctxt_LunStatus = LUN_STATUS_CHKCOND;
	SCTXT_LunStatus = LUN_STATUS_CHKCOND;

	if (!bot_mode)
	{
		//DBG0(("A: %BX, OP:%BX\n", *host_ctxtmem_ptr, CmdBlk(0)));
		ctxt_CDB[16] = 18;			// Sense_Length
		ctxt_CDB[17] = 0;			// 0
		ctxt_CDB[18] = 0;			// 
		ctxt_CDB[19] = 0;			//
		pU8 = &(ctxt_CDB[17]);
	}
	else
#endif
	{
		bot_sense_data[0] = 18;		// 
		bot_sense_data[1] = 0;			// 
		bot_sense_data[2] = 0;			// 
		bot_sense_data[3] = 0;			//
		pU8 = &(bot_sense_data[1]);
	}
	
	switch (error)
	{
	case ERROR_ILL_OP:
	    pU8[0] = SCSI_SENSE_ILLEGAL_REQUEST;	// sense key: ILLEGAL REQUEST
	    pU8[1] = 0x20;							// ASC: INVALID COMMAND OPERATION CODE
		return;

	case ERROR_ILL_CDB:
	    pU8[0] = SCSI_SENSE_ILLEGAL_REQUEST;	// sense key: ILLEGAL REQUEST
	    pU8[1] = 0x24;							// ASC: INVALID Field in CDB
		return;

	case ERROR_ILL_PARAM:
		pU8[0] = SCSI_SENSE_ILLEGAL_REQUEST;	// sense key: ILLEGAL REQUEST
		pU8[1] = 0x26;							// ASC
		pU8[2] = 0x01;							// ASCQ: INVALID FIELD IN PARAMETER LIST
		return;

	case ERROR_UA_BECOMING_READY:
	    pU8[0] = SCSI_SENSE_NOT_READY;  // sense key: NOT READY
	    pU8[1] = 0x04;					// ASC: INVALID FIELD IN CDB
	    pU8[2] = 0x01;					// ASCQ
		return;

	case ERROR_UA_NOT_READY_INIT:
	    pU8[0] = SCSI_SENSE_NOT_READY;  // sense key: NOT READY
	    pU8[1] = 0x04;					// LUN NOT READY, INITIALIZING COMMAND REQUIRED
	    pU8[2] = 0x02;					//
		return;

	case ERROR_UA_NOT_READY:
	    pU8[0] = SCSI_SENSE_NOT_READY; // sense key: NOT READY
	    pU8[1] = 0x04;					// LUN NOT READY, CAUSE NOT REPORTABLE
		//pU8[2] = 0x00;				//
		return;

	case ERROR_UA_NO_MEDIA:
	    pU8[0] =  SCSI_SENSE_NOT_READY;  // sense key: NOT READY
	    pU8[1] = 0x3A;					// ASC: MEDIUM NOT PRESENT
		return;

	case ERROR_SELFTEST_FAIL:
	    pU8[0] = SCSI_SENSE_HARDWARE_ERROR;	// sense key: HARDWARE_ERROR,
	    pU8[1] = 0x3E;							// ASC: LOGICAL UNIT FAILED SELF-TEST
	    pU8[2] = 0x03;							//
		return;

	case ERROR_SAV_NOT_SUPP:
	    pU8[0] = SCSI_SENSE_ILLEGAL_REQUEST;	// sense key: ILLEGAL REQUEST
	    pU8[1] = 0x39;							// ASC: SAVING PARAMETERS NOT SUPPORTED
	    //pU8[2] = 0x00;						//
		return;

#if 1
	case ERROR_LBA_OUT_RANGE:
	    pU8[0] = SCSI_SENSE_ILLEGAL_REQUEST;	// sense key: ILLEGAL REQUEST,
	    pU8[1] = 0x21;							// ASC: LOGICAL BLOCK ADDRESS OUT OF RANGE
		return;
#endif

	case ERROR_NO_ACCESS_RIGHT:
	    pU8[0] = SCSI_SENSE_DATA_PROTECT;		// sense key:
	    pU8[1] = 0x20;							// ASC: ACCESS DENIED - NO ACCESS RIGHTS
	    pU8[2] = 0x02;							// 
		return;

	case ERROR_CRC_ERROR:
		pU8[0] = SCSI_SENSE_ABORTED_COMMAND;	// sense key: ABORTED
		pU8[1] = 0x47;							// ASC:
		pU8[2] = 0x03;							// INFORMATION UNIT iuCRC ERROR DETECTED
		return;

#ifdef UAS_EN
	case ERROR_IU_TOO_SHORT:
	    pU8[0] = SCSI_SENSE_ABORTED_COMMAND;	// sense key: ABORTED
	    pU8[1] = 0x0E;							// 
		pU8[2] = 0x01;							// INFORMATION UNIT iuCRC ERROR DETECTED
		return;

	case ERROR_DATA_PHASE:
	    pU8[0] = SCSI_SENSE_ABORTED_COMMAND;	// sense key: ABORTED
	    pU8[1] = 0x4B;							// ASC: DATA PHASE ERROR
		return;
#endif

#ifdef UAS_OVERLAPPED_TAG
	case ERROR_OVERLAPPED_CMD:
	    pU8[0] = SCSI_SENSE_ABORTED_COMMAND;	//
	    pU8[1] = 0x4E;							// ASC: OVERLAPPED COMMANDS ATTEMPTED
	    //pU8[2] = 0x00;						// 
		return;
#endif

#ifdef W_PROTECT
	case ERROR_WRITE_PROTECT:
		pU8[0] = SCSI_SENSE_DATA_PROTECT;		// sense key:
	    pU8[1] = 0x27;							// ASC: ACCESS DENIED - NO ACCESS RIGHTS
	    pU8[2] = 0x03;							// 
		return;
#endif
	}
}

/****************************************\
	hdd_inquiry_cmd()

	Input Global Parameter:
		ctxt_site: site # of CDB Context
\****************************************/
void hdd_inquiry_cmd()
{
#if 0
	//if (!bot_mode)
	{
		uas_ci_paused = 1;
		uas_ci_paused_ctxt = ctxt_site;
	}
#endif

//	CmdBlk(1) = ctxt_CDB[1];
//	CmdBlk(2) = ctxt_CDB[2];
//	CmdBlk(3) = ctxt_CDB[3];
//	CmdBlk(4) = ctxt_CDB[4];
//	CmdBlk(5) = ctxt_CDB[5];
	//DBG(("Inq\n"));
		xmemset(mc_buffer, 0x00, 76);

		// check EVPD bit ? 
		if ((CmdBlk(1) & 0x01) == 0)
		{
			// check PAGE CODE
			if (CmdBlk(2) == 0)
			{
				// Standard Inquiry Data
				byteCnt = 76;
				//standard inquiry
				mc_buffer[0] = 0 ;  					// HDD device
				mc_buffer[1] = 0 ;

				//standard inquiry // for CFA mode
				if (sobj_mediaType == MEDIATYPE_REMOVABLE)
				{
					mc_buffer[1] = 0x80;
				}
				//else
				//	*(mc_buffer + 1) = 0 ;
				mc_buffer[2] = INQUIRY_VERSION_SPC4;	//2:Version :SPC-4
				mc_buffer[3] = 0x10 + 2;				//HISUP + 3:Response Data Format

				mc_buffer[4] = 76-5 ;					//4:additional length
				mc_buffer[5] = 0x00 ;
				mc_buffer[6] = 0x20 ;					//vendor specific
				mc_buffer[7] = INQUIRY_CMD_QUE ;		//7:CMD QUEUE

				xmemcpy(module_vendor_text, mc_buffer+8, 8);
				xmemcpy(module_product_text, mc_buffer+16, 16);

				mc_buffer[32] = (NV_VERSION(0) >> 4) + '0';
				mc_buffer[33] = (NV_VERSION(0) & 0x0F) + '0';
				mc_buffer[34] = (NV_VERSION(1) >> 4) + '0';
				mc_buffer[35] = (NV_VERSION(1) & 0x0F) + '0';

				xmemcpy(globalNvram.USB_PID, mc_buffer+36, 2);
				//VID
				xmemcpy(globalNvram.USB_VID, mc_buffer+38, 2);

				xmemcpy(module_product_text, mc_buffer+40, 16);
				mc_buffer[56] = 0;
				mc_buffer[57] = 0;
				mc_buffer[58] = (u8)(VERSION_SPC4>>8);
				mc_buffer[59] = (u8)(VERSION_SPC4);
				mc_buffer[60] = (u8)(VERSION_SBC3>>8);
				mc_buffer[61] = (u8)(VERSION_SBC3);
				if (!bot_mode)
				{
					mc_buffer[62] = (u8)(VERSION_UAS_R04>> 8);
					mc_buffer[63] = (u8)(VERSION_UAS_R04);			
				}
			#ifdef BOT_TOUT
				else
				{
					*dbuf_MuxSel = 0;
					//DBG(("\tInq %BX\n", *dbuf_MuxInOut));
					timerCnt = 100;			// 10 sec
				}
			#endif

			}
			else
			{
				goto illegal_cdb;
			}
		}
		else
		{	// return the vital product data specified by the PAGE CODE field

			mc_buffer[0] = 0 ;			// device type
			mc_buffer[1] = CmdBlk(2) ;		// page code
			mc_buffer[2] = 0;			// Reserved
			// check PAGE CODE
			switch (CmdBlk(2))
			{
			case INQUIRY_STD_SUPPORT:		// supported vital product pages
				//mc_buffer[0] = 0 ;			// device type
				//mc_buffer[1] = INQUIRY_STD_SUPPORT ;		// page code : 0
				//mc_buffer[2] = 0;			// Reserved
				mc_buffer[3] = 0x03;			// page length
												// supported pages list
				mc_buffer[4] = 0x00 ;		// INQUIRY_STD_SUPPORT
				mc_buffer[5] = 0x80 ;			// INQUIRY_STD_VPD_USN
				mc_buffer[6] = 0x83 ;			// INQUIRY_STD_VPD_DEVID
				mc_buffer[7] = 0 ;			// 
				byteCnt = 0x8;
				break;

			case INQUIRY_STD_VPD_USN:		// unit Serial # page
				//*(mc_buffer) = 0 ; 
				//*(mc_buffer+1) = INQUIRY_STD_VPD_USN;	//page code : 0x80
				//*(mc_buffer+2) = 0;
				*(mc_buffer+3) = 20;
				//page length
				xmemcpy(&serialStr[0], &mc_buffer[4], 20);

				byteCnt = 24;
				break;

			case INQUIRY_STD_VPD_DEVID:	// Device Identification Page
				//*(mc_buffer) = 0 ; 
				//*(mc_buffer+1) = INQUIRY_STD_VPD_DEVID;	//page code : 83
				//*(mc_buffer+2) = 0;
				if (bot_mode)
				{
					mc_buffer[3] = 48;	//page length
											// identification descriptors
					mc_buffer[4] = 0x02 ;	//code set = ASCII
					mc_buffer[5] = 0x01 ;	//identifier type
					mc_buffer[6] = 0 ;
					mc_buffer[7] = 44 ;	//identifier length

					xmemcpy(module_vendor_text, &mc_buffer[8], 8);
					xmemcpy(module_product_text, &mc_buffer[16], 16);
					//32-41 product serial #
					xmemcpy(&serialStr[0], &mc_buffer[32], 20);

					byteCnt = 52;
				}
				else
				{
					mc_buffer[3] = 12;	//page length
					mc_buffer[4] = 0x01 ;	//proto id= 0, code set = binary
					mc_buffer[5] = 0x03 ;	//piv=0, assoc=LUN, type=NAA
					//mc_buffer[6] = 0 ;
					mc_buffer[7] = 8 ;	//identifier length
					xmemcpy(sobj_wnn, &mc_buffer[8], 8);
					byteCnt = 16;
				}

				break;

			case INQUIRY_BLOCK_LIMITS:			// Block Limits page
				mc_buffer[3] = 60;				// Page length

				// Bytes 6..7 is the Optimal Transfer Length Granularity.
				// Set this to the # of logical blocks in a physical block.

				if (sobj_sector_4K)  // logical 4k only
				{
					mc_buffer[7] = 1;

					// Bytes 8..11 is the Maximum Transfer Length for read/write cmds.
					// Set this to the largest xfer length accepted by
					// scsi_xlate_rw16_lba_for_hdd(). This is the same same as:
					//  *(u32*)(mc_buffer+8) = (unsigned)0xffff >> IniData.user_bs_log2m9;
					// Bytes 8 and 9 are already 0.
					mc_buffer[10] = 0x1F;

					// Bytes 12..15 is the Optimal Transfer Length.
					mc_buffer[14] = 0x1F;	// Same as the max xfer len.

					// Bytes 16..19 is the max transfer length for XDREAD, XDWRITE, etc.
					mc_buffer[18] = 0x1F;
				}
				else
				{
					// The current logical block size is same or larger than a
					// physical block, so the optimal xfer length is one logical block.
					// (This is OK as long as all block lengths are powers-of-2.)
					mc_buffer[7] = 1;

					// Bytes 8..11 is the Maximum Transfer Length for read/write cmds.
					// Set this to the largest xfer length accepted by
					// scsi_xlate_rw16_lba_for_hdd(). This is the same same as:
					//  *(u32*)(mc_buffer+8) = (unsigned)0xffff >> IniData.user_bs_log2m9;
					// Bytes 8 and 9 are already 0.
					mc_buffer[10] = 0xFF;

					// Bytes 12..15 is the Optimal Transfer Length.
					mc_buffer[14] = 0xFF;	// Same as the max xfer len.

					// Bytes 16..19 is the max transfer length for XDREAD, XDWRITE, etc.
					mc_buffer[18] = 0xFF;
				}
				mc_buffer[11] = 0xff;
				mc_buffer[15] = 0xff;
				mc_buffer[19] = 0xff;
		
				byteCnt = 64;
				break;

			case INQUIRY_BLOCK_DEVICE_CHARACTERISTICS:
				// This page is defined by SBC-3, so only the Disk LUN supports it.
				mc_buffer[3] = 60;				// Page length
				mc_buffer[5] = 1;
				// the rest value shall be return 0
				byteCnt = 64;
				break;		

			default:
illegal_cdb:
				hdd_err_2_sense_data(ERROR_ILL_CDB);
				usb_device_no_data();
				return;
			}
		}

		//byteCnt = Min(byteCnt, *((u16 *)(&CmdBlk(3))));
		if (byteCnt > *((u16 *)(&CmdBlk(3))))
		{
			byteCnt =  *((u16 *)(&CmdBlk(3)));
		}
		usb_device_data_in();
	
		return;

}

#if 1
void hdd_log_sense_cmd()
{
	byteCnt = *((u16 *)&CmdBlk(7));
	if (byteCnt == 0)
	{
		usb_device_no_data();
		return;
	}
	byteCnt = Min(200, byteCnt);

	xmemset(mc_buffer, 0x00, byteCnt);
	switch (CmdBlk[2] & 0x3F)
	{
		case LOG_SENSE_STD_PAGE:
			//*(mc_buffer) = 0;	    // page code
			//*(mc_buffer+1) = 0;		// reserved
			//*(mc_buffer+2) = 0 ;	// page length
			*(mc_buffer+3) = 0x1 ;	// page length

			//*(mc_buffer+4) = LOG_SENSE_STD_PAGE;
			break;

		default:
			hdd_err_2_sense_data(ERROR_ILL_CDB);
	}
	usb_device_data_in();
}
#endif

void hdd_receive_diag_results_cmd()
{
	if (!(CmdBlk(1) & 0x1))
	{	// get Page code from last Send DIAGNOSTIC Command
		goto invalid_CDB_field;
	}
	byteCnt = *((u16 *)&CmdBlk(3));
	if (byteCnt == 0)
	{
		usb_device_no_data();
		return;
	}
	byteCnt = Min(5, byteCnt);

	xmemset(mc_buffer, 0x00, byteCnt);
	switch (CmdBlk[2])
	{
		case DIAG_SUPPORTED_PAGES:
			//*(mc_buffer) = 0;						// page code
			//*(mc_buffer+1) = 0;					// reserved
			//*(mc_buffer+2) = 0 ;					// page length
			*(mc_buffer+3) = 0x1 ;					// page length

			//*(mc_buffer+4) = DIAG_SUPPORTED_PAGES;// Diagnostic parameters
			usb_device_data_in();
			return;

		default:
invalid_CDB_field:
			hdd_err_2_sense_data(ERROR_ILL_CDB);
			usb_device_no_data();
			return;
	}
}

bit hdd_chk_short_block_descriptor(void)
{
	//DBG(("c s B D %BX\n", pU8[6]));
	// Bytes 5..7 is the Logical Block Length.
	if (pU8[5] || pU8[7])
		return 1;

	tmp8 = pU8[6];
#ifdef SUPPORT_4K_ONLY
	// whethetr 4K
	if (tmp8 != 0x10)
		return 1;
#else
	#ifdef SUPPORT_512_ONLY
		// whethetr 512
		if (tmp8 != 0x2)
			return 1;
	#else
		#ifdef PHYSICAL_MORE_512
		if ((sobj_sector_4K) || (sobj_physical_sector_size == 0x800)) 
		#else
		if (sobj_sector_4K)
		#endif	
		{	// whethetr 4K
			if (tmp8 != 0x10)
				return 1;
		}
		else
		{	// whethetr 512
			if (tmp8 != 0x02)
				return 1;
		}
	#endif
#endif
	//DBG(("G s B De\n"));
	return 0;
}

bit hdd_chk_long_block_descriptor(void)
{
	// Bytes 12..15 is the Logical Block Length.
	if (pU8[12] || pU8[13] || pU8[15])
		return 1;

	tmp8 = pU8[14];
#ifdef SUPPORT_4K_ONLY
	// whethetr 4K
	if (tmp8 != 0x10)
		return 1;
#else
	#ifdef SUPPORT_512_ONLY
		// whethetr 512
		if (tmp8 != 0x02)
			return 1;
	#else
		#ifdef PHYSICAL_MORE_512
		if ((sobj_sector_4K) || (sobj_physical_sector_size == 0x800)) 
		#else
		if (sobj_sector_4K)
		#endif	
		{	// whethetr 4K
			if (tmp8 != 0x10)
				return 1;
		}
		else
		{
			// whethetr 512
			if (tmp8 != 0x02)
				return 1;
		}
	#endif
#endif
	//DBG(("G l B De\n"));
	return 0;
}

void hdd_mode_select_cmd(void)
{
//	if (CmdBlk(1) & MODE_SELECT_SP)
//	{
//		hdd_err_2_sense_data(ERROR_INVALID_FIELD_IN_CDB);
//		return;
//	}
	sz16 = *((u16 *)(&CmdBlk[7]));
	//DBG(("MS %bx %x\n", CmdBlk[0], sz16));
	if (CmdBlk[0] == SCSI_MODE_SELECT6)
	{
		// parameter list should have "Mode Parameter Header"
		if (sz16 < 4)
		{
			goto invalid_param_field;
		}

		// save Block Descriptor Length
		tmp8 = mc_buffer[3];

		// skip "Mode Parameter Header"
		pU8 = &mc_buffer[4];		// skip "Mode Parameter Header"
		sz16 -=  4;

		// Check the Block Descriptor Length; 
		if (tmp8 != 0)
		{
			if (tmp8 == SHORT_BLOCK_DESCRIPTOR_SIZE)
			{
				if (sz16 < (SHORT_BLOCK_DESCRIPTOR_SIZE))
				{
					goto invalid_param_field;
				}
				if (hdd_chk_short_block_descriptor() == 0)
				{		// skip "Mode Parameter Header"
					pU8 += SHORT_BLOCK_DESCRIPTOR_SIZE;
					sz16 -=  (SHORT_BLOCK_DESCRIPTOR_SIZE);
					goto parse_mode_parameter_data;
				}
			}
			goto invalid_param_field;
		}
	}
	else		// MODE SELECT (10)
	{
		// parameter list should have "Mode Parameter Header"
		if (sz16 < 8)
		{
			goto invalid_param_field;
		}

		// save LONGLBA
		val = mc_buffer[4];
		// save Block Descriptor Length LSB
		tmp8 = mc_buffer[7];


		// Check the "Block Descriptor Length MSB" in "Mode Parameter Header"
		if (mc_buffer[6])
		{
			goto invalid_param_field;
		}

		// skip "Mode Parameter Header"
		pU8 = &mc_buffer[8];
		sz16 -=  8;


		if ((val & BLOCK_DESCRIPTOR_LONGLBA) && (tmp8 == LONG_BLOCK_DESCRIPTOR_SIZE))
		{
			if (sz16 < (LONG_BLOCK_DESCRIPTOR_SIZE))
			{
				goto invalid_param_field;
			}
			if (hdd_chk_long_block_descriptor() == 0)
			{	// skip "Block Descriptor"
				pU8 += LONG_BLOCK_DESCRIPTOR_SIZE;
				sz16 -=  (LONG_BLOCK_DESCRIPTOR_SIZE);
				goto parse_mode_parameter_data;
			}
		}
		else if (((val & BLOCK_DESCRIPTOR_LONGLBA) == 0) && (tmp8 == SHORT_BLOCK_DESCRIPTOR_SIZE))
		{
			if (sz16 < (SHORT_BLOCK_DESCRIPTOR_SIZE))
			{
				goto invalid_param_field;
			}
			if (hdd_chk_short_block_descriptor() == 0)
			{	// skip "Block Descriptor"
				pU8 = SHORT_BLOCK_DESCRIPTOR_SIZE;
				sz16 -=  (SHORT_BLOCK_DESCRIPTOR_SIZE);
				goto parse_mode_parameter_data;
			}
		}

		goto invalid_param_field;

	}
	// Parse "Mode Parameter Page"
	//
parse_mode_parameter_data:
//DBG(("Page %bx\n", *pU8));
	if (sz16  > 0)
	{
		switch (*pU8 & MODE_PAGE_CODE_MASK)
		{
	 	case MODE_CACHING_MODE_PAGE:
			if (CmdBlk[1] & 0x01)		// save page
			{	// there is no saving for Caching Mode Page
				hdd_err_2_sense_data(ERROR_ILL_CDB);
				return;
			}

			// at least 3 bytes of Caching Mode Page.
			if (sz16 < 03)
			{
				//DBG(("Size %x\n", sz16));
				goto invalid_param_field;
			}

			// Check the Page Length field.
			if (pU8[1] < 1)
			{
				//DBG(("Page Length %bx\n", pU8[1]));
				goto invalid_param_field;
			}

			// Check "Write Cache Enable" bit
			if (pU8[2] & 0x04)
			{
				//globalNvram.WrCacheDisabled = 0;
				sobj_WrCache_enabled = 1;
			}
			else
			{
				//globalNvram.WrCacheDisabled = 1;
				sobj_WrCache_enabled = 0;
			}

			return;

		default:
			// Invalid or unsupported Page Code.
invalid_param_field:
			hdd_err_2_sense_data(ERROR_ILL_PARAM);
			return;
		}
	}
}

u8 hdd_block_descriptor()
{
	if (sobj_sector_4K)  // logical 4k only
	{
		utmp32.udata.u_32 = sobj_sectorLBA_l >> 3;
		utmp32.udata.u_32 |= (sobj_sectorLBA_h & 0x000000007) << 29;
		utmp32.udata.u_32 -= 2;

		tmp8 = 0x10;			//4k
	}
	else
	{
		utmp32.udata.u_32 = sobj_sectorLBA_l - 2;

		tmp8 = 0x02;			//512
	}

	if (CmdBlk(1) & MODE_SENSE_LLBAA)
	{
		// pU8[8-11] - Logical Blocks Address(LBA)[0-7]
		//*((u32 xdata *)(pU8+0)) = 0;	// 0-3
#ifndef SUPPORT_3T_4K	
		if (sobj_sectorLBA_h)	
		{
			*((u32 xdata *)(pU8+0)) = sobj_sectorLBA_h;
		}
#endif	
		*((u32 xdata *)(pU8+4)) = utmp32.udata.u_32;	// 4-7

		// pU8[8-11] - Reserved
		//*((u32 xdata *)(pU8+8)) = 0;	// 8-11

		//pU8[12-15] Logical Block Length
		//pU8[12] = 00;     //12 byte zero
		//pU8[13] = 00;     //13 byte zero
		pU8[14] = tmp8;		//14
		//pU8[15] = 0x00;   //15

		tmp8 = 16;		// block_descriptor length
	}
	else
	{
		// pU8[0-3] - Logical Blocks Address(LBA)[0-3]
#ifndef SUPPORT_3T_4K	
		if (sobj_sectorLBA_h)	
		{
			*((u32 xdata *)pU8) = 0xFFFFFFFF;
		}
		else
#endif	
		*((u32 xdata *)pU8) = utmp32.udata.u_32;

		//pU8[4-7] Logical Block Length
		//pU8[4] = 00;		//4byte zero
		//pU8[5] = 00;		//5 byte zero
		pU8[6] = tmp8;
		//pU8[7] = 0x00;  

		tmp8 =  8;		// block_descriptor length
	}
	pU8 += tmp8;

	return tmp8;

}



/************************************************************************\
 hdd_caching_page() - caching mode page.
 */
bit hdd_caching_page(void)
{

	*pU8 = MODE_CACHING_MODE_PAGE;			// PS= 0 +Page code field
	pU8++;

	*pU8 = 18;								//page length
	pU8++;

	val = CmdBlk[2] & 0xc0;					// Page control
	if (val == 0)					// current values
	{
		if (sobj_WrCache_enabled)
		{
			*pU8 = 0x10 | 0x04;	// "Discontinuity" bit & Write_CACHE enable bit
		}
		else
		{
			*pU8 = 0x10;		// "Discontinuity" bit
		}

		pU8++;

		*pU8 = 0xFF; // MSB, disable pre-fetch transfer length
		pU8++;
		*pU8 = 0xFF; // LSB
		pU8++;
		*pU8 = 0xFF; // MSB, Maximum pre-fetch
		pU8++;
		*pU8 = 0xFF; // LSB
		pU8++;
		*pU8 = 0xFF; //MSB, Maximum pre-fetch ceiling
		pU8++;
		*pU8 = 0xFF; // LSB
		pU8++;
		*pU8 = 0xFF; // number of cache segments
		pU8 += 11;
		return 0;
	}
	else if (val == 0x40)			//changable values
	{
		*pU8 = 0x04;// Write CACHE Enable bit only
		pU8 += 18;
		return 0;
	}
	else if (val == 0x80)			//default values : WCE Enable
	{
		*pU8 = 0x10 | 0x04;				// Write CACHE Enable bit only
		pU8++;
		*pU8 = 0xFF; // MSB, disable pre-fetch transfer length
		pU8++;
		*pU8 = 0xFF; // LSB
		pU8++;
		*pU8 = 0xFF; // MSB, Maximum pre-fetch
		pU8++;
		*pU8 = 0xFF; // LSB
		pU8++;
		*pU8 = 0xFF; //MSB, Maximum pre-fetch ceiling
		pU8++;
		*pU8 = 0xFF; // LSB
		pU8++;
		*pU8 = 0xFF; // number of cache segments
		pU8 += 11;

		return 0;
	}
	else  if (val == 0xC0)		// save value
	{	// none
		return 1;
	}

}

void hdd_mode_sense_cmd()
{
	xmemset(mc_buffer, 0, 255);

	// xlate MODE SENSE(6) into MODE SENSE(10).
	if (CmdBlk[0] == SCSI_MODE_SENSE6)
	{
		mc_buffer[2] = 0x10;			//deviceSpecific/DPOFUA;

		pU8 = &mc_buffer[4];	// start of Block Descriptor or Mode Pages. 
	}
	else
	{	// SCSI_MODE_SENSE (10)
		mc_buffer[3] = 0x10;			//deviceSpecific/DPOFUA;

		pU8 = &mc_buffer[8];	// start of Block Descriptor or Mode Pages. 
	}

	if ((CmdBlk[1] & MODE_SENSE_DBD) == 0 )
	{
		tmp8 = hdd_block_descriptor();	
		//if (tmp8 != 0)
		{	// Set Block Descriptor Length
			if (CmdBlk[0] == SCSI_MODE_SENSE6)
			{
				mc_buffer[3] = tmp8;
			}
			else
			{
				if (CmdBlk[1] & MODE_SENSE_LLBAA)
				{
					mc_buffer[4] |= BLOCK_DESCRIPTOR_LONGLBA;
				}

				mc_buffer[7] = tmp8;
			}
		}
	}


#if 1
		{
			tmp8 = CmdBlk(2) & 0x3f;	   // page code number
			if ((tmp8 == MODE_ALL_PAGES) || (tmp8 == MODE_CACHING_MODE_PAGE))
			{
				if (hdd_caching_page())
				{	// SAVING PARAMETERS NOT SUPPORTED
					hdd_err_2_sense_data(ERROR_SAV_NOT_SUPP);
					usb_device_no_data();

					return;
				}

			}
		}
#endif

#if 0
		tmp8 = CmdBlk(2) & 0x3f;	   // page code number
		if ((tmp8 == MODE_ALL_PAGES) || (tmp8 == MODE_OPERATING_PAGE))
		{							// Operating Parameter mode Page
			hdd_op_mode_page();
		}
#endif

	byteCnt = (u16) (pU8 - &mc_buffer[0]);

	// Set Mode Data Length
	if (CmdBlk[0] == SCSI_MODE_SENSE6)
	{
		mc_buffer[0] = byteCnt - 1;
		#ifdef W_PROTECT
			if (writeprotect == 1)
			{
				mc_buffer[2] = 0x80;
			}
			else
				mc_buffer[2] = 0x00;
		#endif
	}
	else
	{
		mc_buffer[1] = byteCnt - 2;
		#ifdef W_PROTECT
			if (writeprotect == 1)
			{
				mc_buffer[3] = 0x80;
			}
			else
				mc_buffer[3] = 0x00;
		#endif
	}

	//byteCnt = Min(byteCnt, *((u16 *)(&CmdBlk(7))));
	byteCnt = Min(255, *((u16 *)(&CmdBlk(7))));

	usb_device_data_in();

}



void hdd_rd_buffer_cmd()
{
	if ((CmdBlk(1) == 0x01) &&				// MODE == Vendor specific
		(CmdBlk(2) == 0x00)	&&				// BUFFER ID == 0
		(CmdBlk(6) == 0x00))				// ALLOCATION LENGTH[0] == 0
	{
			// read model & Firmware ver
			if ((CmdBlk(3) == 0x00) &&		// BUFFER OFFSET[0] == 0x00
				(CmdBlk(5) == 0x00))		// BUFFER OFFSET[2] == 0x00
			{
				if (CmdBlk(4)== 0x00)		// BUFFER OFFSET[1] == 0x00
				{
					//InitioReadInquiry - start		BUFFER OFFSET == 0x000000
					*(mc_buffer + 0) = globalNvram.ModelId[0];
					*(mc_buffer + 1) = globalNvram.ModelId[1];
                    //DBG0(("~globalNvram.ModelId[0] = %x\n",globalNvram.ModelId[0]));
					*(mc_buffer + 2) = NV_VERSION(0);
					*(mc_buffer + 3) = NV_VERSION(1);

					xmemcpy(module_vendor_text, &mc_buffer[4], 8);		//4-11
					xmemcpy(module_product_text, &mc_buffer[12], 16);		//12-27
	
					*(mc_buffer + 28) = (NV_VERSION(0) & 0x0F) + '0';
					*(mc_buffer + 29) = '.';
					*(mc_buffer + 30) = (NV_VERSION(1) >> 4) + '0';
					*(mc_buffer + 31) = (NV_VERSION(1) & 0x0F) + '0';
	
					//INITIO_TEXT_24 = "(C) 2002-04 Initio Corp."
					WRITE_BLOCK(INITIO_TEXT_24, ((u8 *)(mc_buffer + 32)), 24);
	
					xmemset(mc_buffer + 56, 0x00, 8);
	
					// boot status
					*(mc_buffer + 57) = (*fw_status & 0x03);
					


					// USB Speed
					*(mc_buffer + 58) = (usbMode << 1);

	//				*(mc_buffer + 58) |= 0x04;
	//				if (valid_hdd)
	//				{
	//					*(mc_buffer + 58) |= 0x40;
	//				}
					*(mc_buffer + 59) = 0x80 | usb_AdapterID;
					*(mc_buffer + 60) = usb_PortID0;
					*(mc_buffer + 61) = usb_PortID1;
					*(mc_buffer + 62) = usb_PortID2;
					*(mc_buffer + 63) = *chip_Revision;
					byteCnt = 64;
			
					usb_device_data_in();
					return;
			
				}
				else if (CmdBlk(4) == 0xFF)		// BUFFER OFFSET[1] == 0xFF
				{
					//
					// INITIO Read NVRAM		BUFFER OFFSET == 0x00FF00
					//
					xmemset(mc_buffer, 0x00, 0x200);
					xmemcpy(((u8 xdata *)&globalNvram), mc_buffer, NVR_DATASIZE);
					byteCnt = NVR_DATASIZE;
                    DBG0(("byteCnt = %x \n,byteCnt"));
					byteCnt = Min(byteCnt, *((u16 *)(&CmdBlk[7])));
                    DBG0(("byteCnt_Min = %x \n,byteCnt"));
					usb_device_data_in();
					return;
				}
                else if (CmdBlk(4) == 0x98)		// BUFFER OFFSET[1] == 0x98
				{
				    DBG(("~Return 1024 Byte of data! case(7):Hi < Di\n"));
                    byteCnt = 600;
                    usb_device_data_in();                    
					return;
				}
                else if (CmdBlk(4) == 0x99)		// BUFFER OFFSET[1] == 0x99
				{
				    DBG(("~Return No Data! case(4):Hi > Dn\n"));
                    byteCnt = 12;
                    usb_device_data_in();                    
					return;
				}
#ifdef EQ_SETTING_TEST
				else if ((CmdBlk(4) == 0x04) && (CmdBlk(8) == 0x40))
				{
					mc_buffer[0] = 0x36; 	//Chip ID 0
					mc_buffer[1] = 0x09;	 //Chip ID 1
					mc_buffer[2] = 0x4C;	 //SIGNATURE 
					mc_buffer[3] = 0x3C;	 //Length 
					mc_buffer[4] = *chip_test_result; 	//Chip test result
					mc_buffer[5] = (usbMode << 1);; 	//USB connection Speed
					mc_buffer[6] = usb_20_bus_rst; 	//USB2.0/1.1 Bus Reset Count
					mc_buffer[7] = usb_30_bus_rst; 	//USB3.0 Warm/Hot Reset Count
					mc_buffer[8] = recovery_count; 	//USB3.0 Recovery Count
					xmemset(mc_buffer + 9, 0x00, 3); // 9.10.11 reserverd
					
					//12-15	USB3.0 Link Error Count
					mc_buffer[12] = *usb_LinkErrorCount_3; 	
					mc_buffer[13] = *usb_LinkErrorCount_2; 	
					mc_buffer[14] = *usb_LinkErrorCount_1; 	
					mc_buffer[15] = *usb_LinkErrorCount_0; 	

					//16-19	USB3.0 Decode Error Count
					mc_buffer[16] = *usb_DecodeErrorCount_3; 	
					mc_buffer[17] = *usb_DecodeErrorCount_2; 	
					mc_buffer[18] = *usb_DecodeErrorCount_1; 	
					mc_buffer[19] = *usb_DecodeErrorCount_0; 	

					//20-23	USB3.0 Disparity Error Count
					mc_buffer[20] = *usb_DisparityErrorCount_3; 	
					mc_buffer[21] = *usb_DisparityErrorCount_2; 	
					mc_buffer[22] = *usb_DisparityErrorCount_1; 	
					mc_buffer[23] = *usb_DisparityErrorCount_0; 	

					mc_buffer[24] = *usb_RecoveryError; 	//USB3.0 Recovery Error Status
					xmemset(mc_buffer + 25, 0x00, 15); //25-39	Reserved
					
					mc_buffer[40] = sobj_crc_count; //SATA CRC Error Count
					xmemset(mc_buffer + 41, 0x00, 24); //41-63	Reserved
					
					byteCnt = 0x40;
					usb_device_data_in();
					return;
				}
#endif
			}
			else if ((CmdBlk[3] == 0x02) &&
					 (CmdBlk[4] == 0x00) &&
					 (CmdBlk[5] == 0x5C) &&
					 (CmdBlk[8] == 0x10)) 				// ALLOCATION LENGTH[2] == 0x10
			{	
				//get iStr		BUFFER OFFSET == 0x02005C	
				//
				xmemcpy(globalNvram.iSerial,mc_buffer, 16);
				byteCnt = 16;
				usb_device_data_in();
				return;
			}

#if SCSI_HID
			else if (scsi_hid)
			{
			//	MSG("R_HID CmdBlk(5): (%BX);scsiGpioData:%x\n", CmdBlk(5),scsiGpioData);
	            DBG0(("SCSI_HID = %d \n",scsi_hid));
				//	read model & Firmware ver
				if ((CmdBlk[3] == 0x02) && (CmdBlk[4] == 0x00) && (CmdBlk[5] == 0x00) && (CmdBlk[8] == 0x04)) 
				{	// HID_SIG
					mc_buffer[0] = 0x25;
					mc_buffer[1] = 0xC9;
					mc_buffer[2] = 0x64;	
					mc_buffer[3] = 0x01;
					byteCnt = 4;
					usb_device_data_in();
					return;
				}
				if ((CmdBlk[3] == 0x02) && (CmdBlk[4] == 0x00) && (CmdBlk[5] == 0x0C) && (CmdBlk[8] == 0x04)) 
				{	//SCSI_HID_GPIO 		
					MSG(("gpio0 %bx\n", (*gpio_DD_0 & GPIO1_DATA)));
					MSG(("gpio0 %bx\n", (*gpio_DI_0 & GPIO1_DATA)));
					mc_buffer[0] = 0;//0
					mc_buffer[1] = 0;//0		
					mc_buffer[2] = 0;//0
					mc_buffer[3] = scsiGpioData;//
					byteCnt = 4;
					usb_device_data_in();
					return;
				}
			}
#endif
#ifdef  WRITE_BUFFER_TEST //write command 3B 01 00 22 33 44 00 00 01 00
        if(             (CmdBlk(1) == 0x01)&& //MODE = Vendor Specific
                        (CmdBlk(2) == 0x00)&& //BUFFER ID= 0
                        (CmdBlk(3) == 0x22)&& //BUFFER OFFSET[2] = 0
                        (CmdBlk(4) == 0x33)&& //BUFFER OFFSET[1] = 0
                        (CmdBlk(5) == 0x44) //BUFFER OFFSET[0] = 1
          )
         {
            DBG(("~11 22 33 44READ WRITE_BUFFER_TEST\n"));
            DBG(("read CmdBlk(8) = %x CmdBlk(7) = %x CmdBlk(6) = %x\n",*((u16*)(&CmdBlk[8])),CmdBlk(7),CmdBlk(6)));
            //byteCnt = *((u8 *)(&CmdBlk[8]));//+*((u16 *)(&CmdBlk[7]));
            byteCnt = Min(1024, *((u16 *)(&CmdBlk[8]))+*((u16 *)(&CmdBlk[7])));
            DBG((" read byteCnt = %x CmdBlk[8] = %x (CmdBlk[7]<<2) = %x\n",byteCnt,CmdBlk[8],(CmdBlk[7]<<2) ));
         }
         else if(       (CmdBlk(1) == 0x01)&&   //MODE = WRITE DATA
                        (CmdBlk(2) == 0x00)&& //BUFFER ID= 0
                        (CmdBlk(3) == 0x00)&& //BUFFER OFFSET[2] = 0
                        (CmdBlk(4) == 0x01)&& //BUFFER OFFSET[1] = 0
                        (CmdBlk(5) == 0x00) //BUFFER OFFSET[0] = 1
                 )
         {  //Hello William
            //xmemset(mc_buffer, globalNvram.test_version2, 15);

            WRITE_BLOCK(INITIO_bin_liao, ((u8 *)(mc_buffer + 16)), 15);
            byteCnt = 30;
            
         }
         usb_device_data_in();
         return;         
#endif

		}

		hdd_err_2_sense_data(ERROR_ILL_CDB);
		usb_device_no_data();
}



/****************************************\
	hdd_request_sense_cmd()

	Input Global Parameter:
		ctxt_site: site # of CDB Context
\****************************************/
void hdd_request_sense_cmd()
{

#if 0
	//if (!bot_mode)
	{
		uas_ci_paused = 1;
		uas_ci_paused_ctxt = ctxt_site;
	}
#endif

	if (CmdBlk(1) & DESC)
	{
	#ifdef ATA_PASS_THROUGTH
		if (bot_mode)
		{
			if (bot_sense_data[0] == 22)
			{
				goto bot_desc;
			}
		}
	#endif
		hdd_err_2_sense_data(ERROR_ILL_CDB);
		usb_device_no_data();
		return;

	}
		byteCnt = 18;
		xmemset(mc_buffer, 0, 18);
		mc_buffer[0] = 0x70;
		mc_buffer[7] = 10;		// additional Sense Length

		if (bot_mode)
		{
		#ifdef ATA_PASS_THROUGTH
			if (bot_sense_data[0] == 22)
			{	// extended Descriptor
bot_desc:
				mc_buffer[0] = 0x72;		// response code
				mc_buffer[7] = 14;		// additional Sense Length

				//sata_return_tfr();

				hdd_return_descriptor_sense(&mc_buffer[8]);
				byteCnt = 22;
			}
			else
		#endif
			if (bot_sense_data[0] == 18)
			{
				mc_buffer[2] =  bot_sense_data[1];	// sense code
				mc_buffer[12] = bot_sense_data[2];	// ASC
				mc_buffer[13] = bot_sense_data[3];	// ASCQ
			}
			bot_sense_data[0] = 0;
		}

		byteCnt = Min(byteCnt, CmdBlk(4));
		usb_device_data_in();
		return;
}

/****************************************\
	hdd_reserve_in_cmd()

	Input Global Parameter:
		ctxt_site: site # of CDB Context
\****************************************/
#ifdef UAS_EN
void hdd_reserve_in_cmd()
{
	CmdBlk(1) &= SA_MASK; 
	if (CmdBlk(1) == SA_READ_KEY)
	{	// no 
		xmemset(mc_buffer, 0, 8);
		byteCnt = 8;

		byteCnt = Min(byteCnt, *((u32 *)(&CmdBlk[6])));
		usb_device_data_in();
		return;
	}
	hdd_err_2_sense_data(ERROR_ILL_CDB);
	usb_device_no_data();
	return;
}
#endif

/****************************************\
	hdd_report_lun_cmd()

	Input Global Parameter:
		ctxt_site: site # of CDB Context
\****************************************/
#ifdef UAS_EN
void hdd_report_lun_cmd()
{
#if 0
	//if (!bot_mode)
	{
		uas_ci_paused = 1;
		uas_ci_paused_ctxt = ctxt_site;
	}
#endif
//	CmdBlk(1) = ctxt_CDB[1];
//	CmdBlk(2) = ctxt_CDB[2];
//	CmdBlk(6) = ctxt_CDB[6];
//	CmdBlk(7) = ctxt_CDB[7];
//	CmdBlk(8) = ctxt_CDB[8];
//	CmdBlk(9) = ctxt_CDB[9];

	//DBG(("Report LUN %lx\n", *((u32 *)(&CmdBlk[6]))));
	{

		// REPORT LUNS DATA Header
		//mc_buffer[0] = 0;						// LUN LIST LENGTH
		//mc_buffer[1] = 0;
		mc_buffer[2] = 0;
		//mc_buffer[3] = 0;
		//mc_buffer[4] = 0;						// Reserved
		//mc_buffer[5] = 0;
		//mc_buffer[6] = 0;
		//mc_buffer[7] = 0;
		xmemset(mc_buffer, 0, 32);


		// check SELECT REPORT
		if ((CmdBlk(2) == 0) ||				// Select Report 0
			(CmdBlk(2) == 2) )				// All Logical Units
		{
			mc_buffer[3] = 8;				// // LUN LIST LENGTH for 1 Lun

			// 1st Lun
			//mc_buffer[8] = 0;				// (ADDRESS METHOD = 0) & (BUS IDENTIFIER = 0)
			//mc_buffer[9] = 0;				// Lun 0
			//mc_buffer[10] = 0;			// Null second level LUN = 0
			//mc_buffer[11] = 0;
			//mc_buffer[12] = 0;			// Null third level LUN = 0 & Null forth level LUN = 0
			//mc_buffer[13] = 0;
			//mc_buffer[14] = 0;
			//mc_buffer[15] = 0;

			byteCnt = 16;
		}
		else if (CmdBlk(2) == 1)
		{	// do not support any "well known logical unit"
			byteCnt = 8;
		}
		else
		{	// invalid "SELECT REPORT"
			hdd_err_2_sense_data(ERROR_ILL_CDB);
			usb_device_no_data();
			return;
		}

		byteCnt = Min(byteCnt, *((u32 *)(&CmdBlk[6])));
		usb_device_data_in();
		return;
	}
}
#endif
	
void hdd_StartAtaNoMediaCmd()
{

#ifdef UAS_EN
	uas_ci_paused = 1;
	uas_ci_paused_ctxt = ctxt_site;
#endif

	CmdBlk(1) = ctxt_CDB[1];
	CmdBlk(2) = ctxt_CDB[2];
	CmdBlk(3) = ctxt_CDB[3];
	CmdBlk(4) = ctxt_CDB[4];
	CmdBlk(5) = ctxt_CDB[5];
	CmdBlk(6) = ctxt_CDB[6];
	CmdBlk(7) = ctxt_CDB[7];
	CmdBlk(8) = ctxt_CDB[8];
	CmdBlk(9) = ctxt_CDB[9];
	//CmdBlk(10) = ctxt_CDB[10];
	//CmdBlk(11) = ctxt_CDB[11];
#if 0
	DBG(("CDB "));
	for(i8=0; i8<10; i8++)
	{
		printf("%bx ", CmdBlk(i));
	}
	DBG(("\n"));
#endif


	switch(CmdBlk(0))
	{
	case SCSI_INQUIRY:
		hdd_inquiry_cmd();
		break;

	case SCSI_REQUEST_SENSE : //
		hdd_request_sense_cmd();
		break;

#ifdef UAS_EN
	case SCSI_REPORT_LUNS:
		hdd_report_lun_cmd();
		break;
#endif

	case SCSI_READ_BUFFER10 : 		        // read buffer
		hdd_rd_buffer_cmd();
		break;

	case SCSI_WRITE_BUFFER10:
		hdd_err_2_sense_data(ERROR_ILL_CDB);
		usb_device_no_data();
		return;




	case SCSI_TEST_UNIT_READY:
	case SCSI_READ6:
	case SCSI_WRITE6:
	case SCSI_MODE_SELECT6:
	case SCSI_MODE_SENSE6:
	case SCSI_START_STOP_UNIT:
	case SCSI_READ_CAPACITY:
	case SCSI_READ10:
	case SCSI_WRITE10:
	case SCSI_VERIFY10 :
	case SCSI_MODE_SELECT:
	case SCSI_MODE_SENSE:
	case SCSI_READ16:
	case SCSI_WRITE16:
	//case SCSI_READ12:
	//case SCSI_WRITE12:
		hdd_err_2_sense_data(ERROR_UA_NO_MEDIA);
		usb_device_no_data();
		return;

	default:
		hdd_err_2_sense_data(ERROR_ILL_CDB);
		usb_device_no_data();
			
		return;
	}
}

#if 1
bit hdd_chk_drv_init()
{
	if (sobj_init == 0)
	{
		hdd_que_ctxt_site = ctxt_site;
		hdd_que_ctxt_tout = HDD_QUE_SHORT_TOUT;					// 

		return 1;
	}
	return 0;
}
#endif

#ifdef PWR_SAVING
bit hdd_chk_drv_state()
{
	//if (sobj_State < SATA_STANDBY)
	if (sobj_State == SATA_PWR_DWN)
	{
		//if (sobj_State == SATA_PWR_DWN)
		{
			sata_pwr_up();
			// turn-on LED
			//hdd_on_led();
		}
		goto que_ctxt;

	}
	else if (sobj_State >= SATA_NCQ_FLUSH)
	{
		sobj_State = SATA_NCQ_FLUSH;
que_ctxt:
		hdd_que_ctxt_site = ctxt_site;
		hdd_que_ctxt_tout = HDD_QUE_TOUT;					// 
	#ifdef UAS_EN
		if (bot_mode == 0)
			hdd_que_ctxt_tout = HDD_UAS_QUE_TOUT;					// 
	#endif

		return 1;
	}

	return 0;
}
#endif


#ifdef UAS_EN
bit hdd_chk_drv_busy()
{

	if (sobj_State == SATA_ACTIVE)
	{
		goto que_busy;
	}
	else if (sobj_State >= SATA_NCQ_FLUSH)
	{
		sobj_State = SATA_NCQ_FLUSH;
que_busy:
		hdd_que_ctxt_site = ctxt_site;
		hdd_que_ctxt_tout = HDD_UAS_QUE_TOUT;					// 

		return 1;
	}

	return 0;
}
#endif

#ifdef SCSI_DOWNLOAD_FW
bit hdd_odd_scsi_downlod_fw(void)
{
	if ((CmdBlk[1] == 0x5) && 
		(CmdBlk[2] == 0x00) && 
		(CmdBlk[3] == 0x03) && 
		(CmdBlk[5] == 0x00) &&
		(CmdBlk[7] == 0x02) &&
		(CmdBlk[10] == 0x25) &&
		(CmdBlk[11] == 0xc9))
	{
		//sectorBytesLeft = 512;
		if (sflash_type == UNKNOWN_FLASH)
		{
			sflash_get_type();
			if (sflash_type == UNKNOWN_FLASH)
			{
				//DBG(("unknow flash\n"));
				hdd_err_2_sense_data(ERROR_ILL_PARAM);
				usb_device_no_data();
				return 1;
			}
		}

		if (FW_Download != FW_DOWNLOAD_PROECESSING)
		{
			//DBG(("start download\n"));
			FW_Download = FW_DOWNLOAD_PROECESSING;

			sflash_erase_chip();
		}

		tmp8 = CmdBlk[4];
		flash_addr_low = (u16)(tmp8);

		sflash_write_data((u8  xdata *)&mc_buffer[0],flash_addr_low,0x100);
		flash_addr_low += 0x01;
		sflash_write_data((u8  xdata *)&mc_buffer[0x100],flash_addr_low,0x100);

		if (flash_addr_low == 0xFF) // downlaod 0xFFFFh bta file
		{
			//DBG(("download end\n"));
			FW_Download = FW_DOWNLOAD_SUCCESS;				
		}
		return 1;
	}
	hdd_err_2_sense_data(ERROR_ILL_PARAM);
	usb_device_no_data();
	return 1;
}
#endif

/****************************************\
	scsi_StartAtaCmd()

	Input Global Parameter:
		ctxt_site: site # of CDB Context
		SCTXT_INDEX: site # of CDB Context
		*host_ctxtmem_pt: site # of CDB Context
\****************************************/
void hdd_StartAtaCmd()
{

//	AES_REG volatile* pAesReg;
//	pAesReg = (PAES_REG)(AES_BASE_ADDR);
//	DBG(("%BX\n", CmdBlk(0)));

#ifdef UAS_EN
	uas_ci_paused = 1;
	uas_ci_paused_ctxt = ctxt_site;
#endif


	switch ( CmdBlk(0) )
	{
	/****************************************\
		SCSI_ATA_PASS_THR(12)
	\****************************************/
#ifdef ATA_PASS_THROUGTH
	case SCSI_ATA_PASS_THR:

		extended_descriptor = 0;

		ctxt_FIS[FIS_TYPE] = HOST2DEVICE_FIS;		//
		ctxt_FIS[FIS_C] = 0x80;
		ctxt_FIS[FIS_COMMAND] = CmdBlk(9);			//COMMAND
		ctxt_FIS[FIS_FEATURE] = CmdBlk(3);			//FEATURES (7:0)

		ctxt_FIS[FIS_LBA_LOW] = CmdBlk(5);			//LBA (7:0)
		ctxt_FIS[FIS_LBA_MID] = CmdBlk(6);			//LBA (15:8)
		ctxt_FIS[FIS_LBA_HIGH] = CmdBlk(7);			//LBA (23:16)
		ctxt_FIS[FIS_DEVICE] = CmdBlk(8);			//Device

//		ctxt_FIS[FIS_LBA_LOW_EXP]= 0;
//		ctxt_FIS[FIS_LBA_MID_EXP]= 0;
//		ctxt_FIS[FIS_LBA_HIGH_EXP]= 0;
//		ctxt_FIS[FIS_FEATURE_EXP]= 0;

		ctxt_FIS[FIS_SEC_CNT] = CmdBlk(4);			// SECTOR_CNT(7:0)
//		ctxt_FIS[FIS_SEC_CNT_EXP] = 0;		
//		ctxt_FIS[FIS_RESERVED_14] = 0;
//		ctxt_FIS[FIS_CONTROL] = 0;

		goto ata_pass_thru;
#endif

	/****************************************\
		SCSI_ATA_PASS_THR(16)
	\****************************************/
#ifdef ATA_PASS_THROUGTH
	case SCSI_ATA_PASS_THR16:
	{
		extended_descriptor = CmdBlk(1) & 1;
		ctxt_FIS[FIS_TYPE] = HOST2DEVICE_FIS;		//
		ctxt_FIS[FIS_C] = 0x80L;
		ctxt_FIS[FIS_COMMAND] = CmdBlk(14);			//COMMAND
		ctxt_FIS[FIS_FEATURE] = CmdBlk(4);			//FEATURES (7:0)

		ctxt_FIS[FIS_LBA_LOW] = CmdBlk(8);			//LBA (7:0)
		ctxt_FIS[FIS_LBA_MID] = CmdBlk(10);			//LBA (15:8)
		ctxt_FIS[FIS_LBA_HIGH] = CmdBlk(12);		//LBA (23:16)
		ctxt_FIS[FIS_DEVICE] = CmdBlk(13);			//Device

		ctxt_FIS[FIS_LBA_LOW_EXP]= CmdBlk(7);		// LBA (31:24)
		ctxt_FIS[FIS_LBA_MID_EXP]= CmdBlk(9);		// LBA (39:32)
		ctxt_FIS[FIS_LBA_HIGH_EXP]= CmdBlk(11);		// LBA (47:40)
		ctxt_FIS[FIS_FEATURE_EXP]= CmdBlk(3);		////FEATURES (15:8)

		ctxt_FIS[FIS_SEC_CNT] = CmdBlk(6);			// SECTOR_CNT(7:0)
		ctxt_FIS[FIS_SEC_CNT_EXP] = CmdBlk(5);		// SECTOR_CNT(15:8)
//		ctxt_FIS[FIS_RESERVED_14] = 0;			// 
//		ctxt_FIS[FIS_CONTROL] = 0;			// 

ata_pass_thru:

	#ifdef PWR_SAVING
		if (hdd_chk_drv_state())
		{
			return;
		}
	#endif

#define   protocol	tmp8
		protocol = CmdBlk(1) & 0x1E;
		//DBG(("protocol %bx\n", protocol));
		if ((protocol) == (0 << 1))			// ATA Hard Reset
		{
			//sata_Reset(SATA_HARD_RST);
			//delay_sec(off_line());
			//SCTXT_Status = CTXT_STATUS_GOOD;
			usb_device_no_data();
			return;
		}
		else if ((protocol) == (1 << 1))		// ATA Soft Reset
		{
			//sata_Reset(SATA_SOFT_RST);
			//delay_sec(off_line(pCtxt));
			//SCTXT_Status = CTXT_STATUS_GOOD;
			usb_device_no_data();
			return;
		}
		else if ((protocol) == (3 << 1))		//non-data
		{
			SCTXT_Flag |= SCTXT_FLAG_U2B|SCTXT_FLAG_B2S|SCTXT_ATA_PASS;

			if (CmdBlk(2) & 0x20)
			{	// check CK_COND
				SCTXT_Flag |= SCTXT_FLAG_RET_TFR;
			}


			*ctxt_SataProto = PROT_ND << 4;
	
		#ifdef UAS_EN
			if (bot_mode)
		#endif
			{
				*ctxt_PhaseCase_0 = 0x02;
				//*ctxt_PhaseCase_1 = 0x00;
			}
		#ifdef UAS_EN
			else
			{	//UAS Mode
				SCTXT_Flag |= SCTXT_FLAG_NO_DATA;
				*ctxt_XferLen_0 = 0;
				*ctxt_XferLen_1 = 0;
				*ctxt_XferLen_2 = 0;
				*ctxt_XferLen_3 = 0;
			}
		#endif

			//*ctxt_DBufIndex = DBUF_SEG_NULL;
			SCTXT_DbufIndex = DBUF_SEG_NULL;

			*ctxt_CCMcmdinten = D2HFISI;
			if (sata_exec_ctxt() == CTXT_STATUS_ERROR)
			{	// sense Sense IU if error return 
				usb_device_no_data();
				return;
			}										

			if (ctxt_FIS[FIS_COMMAND] == ATA6_STANDBY_IMMEDIATE)
			{
				hdd_led_going_off();
				//hdd_post_write = 0;

			}
			else
				hdd_start_act_led();
			return;
		}
		else if (((protocol) == (4 << 1)) && ((CmdBlk(2) & 0x08)))	//PIO data-in
		{
			SCTXT_Flag |=SCTXT_ATA_PASS;
			if (CmdBlk(2) & 0x20)
			{	// check CK_COND
				SCTXT_Flag |= SCTXT_FLAG_RET_TFR;
			}


			*ctxt_SataProto = PROT_PIOIN << 4;
		
		#ifdef UAS_EN
			if (bot_mode)
		#endif
			{
				// set case 6		
				*ctxt_PhaseCase_0 = 0x40;
				//*ctxt_PhaseCase_1 = 0x00;
				//DBG(("PIO-IN %bx%bx\n", *ctxt_PhaseCase_1, *ctxt_PhaseCase_0));

			}
		#ifdef UAS_EN
			else
			{	//UAS Mode
				if (hdd_set_uas_xfer_len())
				{
					goto invalid_cdb;
				}
				SCTXT_Flag |= SCTXT_FLAG_DIN;
			}
		#endif

			// input port of TX buffer Seg 0  is SATA 0 Read
			// Output port of TX buffer Seg 0  is USB Write
			//*ctxt_DBufIndex = DBUF_SEG_S2U;
			//*ctxt_DBuf_IN_OUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_SATA0_W_PORT; 
			SCTXT_DbufIndex = DBUF_SEG_S2U;
			SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_SATA0_W_PORT; 


			*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 

			if (sata_exec_ctxt() == CTXT_STATUS_ERROR)
			{
				//usb_append_ctxt_que();
				usb_device_no_data();
				return;
			}


			hdd_start_act_led();
			return;

		}
		else if (((protocol) == (5 << 1)) && ((CmdBlk(2) & 0x08)) == 0)	//PIO data-out
		{
			SCTXT_Flag |=SCTXT_ATA_PASS;
			if (CmdBlk(2) & 0x20)
			{	// check CK_COND
				SCTXT_Flag |= SCTXT_FLAG_RET_TFR;
			}

			*ctxt_SataProto = PROT_PIOOUT << 4;
		
		#ifdef UAS_EN
			if (bot_mode)
		#endif
			{
				// set case 12		
				//*ctxt_PhaseCase_0 = 0x00;
				*ctxt_PhaseCase_1 = 0x10;
			}
		#ifdef UAS_EN
			else
			{
				if (hdd_set_uas_xfer_len())
				{
					goto invalid_cdb;
				}
			}
		#endif

			//*ctxt_DBufIndex = DBUF_SEG_U2S;
			//*ctxt_DBuf_IN_OUT = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_USB_W_PORT; 
			SCTXT_DbufIndex = DBUF_SEG_U2S;
			SCTXT_DbufINOUT = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_USB_W_PORT; 

			if (sata_exec_ctxt() == CTXT_STATUS_ERROR)
			{	// UAS only
				//usb_append_ctxt_que();
				usb_device_no_data();
				return;
			}										


			hdd_post_write = 1;
			hdd_start_act_led();
			return;

		}
		else if ((protocol == (0xA << 1)) && ((CmdBlk(2) & 0x08)))	//Udma data-in
		{
			SCTXT_Flag |=SCTXT_ATA_PASS;
			if (CmdBlk(2) & 0x20)
			{	// check CK_COND
				SCTXT_Flag |= SCTXT_FLAG_RET_TFR;
			}


			*ctxt_SataProto = PROT_DMAIN << 4;
		
		#ifdef UAS_EN
			if (bot_mode)
		#endif
			{
				// set case 6		
				*ctxt_PhaseCase_0 = 0x40;
				//*ctxt_PhaseCase_1 = 0x00;
				//DBG(("PIO-IN %bx%bx\n", *ctxt_PhaseCase_1, *ctxt_PhaseCase_0));

			}
		#ifdef UAS_EN
			else
			{	//UAS Mode
				if (hdd_set_uas_xfer_len())
				{
					goto invalid_cdb;
				}
				SCTXT_Flag |= SCTXT_FLAG_DIN;
			}
		#endif

			// input port of TX buffer Seg 0  is SATA 0 Read
			// Output port of TX buffer Seg 0  is USB Write
			//*ctxt_DBufIndex = DBUF_SEG_S2U;
			//*ctxt_DBuf_IN_OUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_SATA0_W_PORT; 
			SCTXT_DbufIndex = DBUF_SEG_S2U;
			SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_SATA0_W_PORT; 


			*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 

			if (sata_exec_ctxt() == CTXT_STATUS_ERROR)
			{
				//usb_append_ctxt_que();
				usb_device_no_data();
				return;
			}


			hdd_start_act_led();
			return;

		}
		else if ((protocol == (0xB << 1)) && ((CmdBlk(2) & 0x08)) == 0)	//UDMA data-out
		{
			SCTXT_Flag |=SCTXT_ATA_PASS;
			if (CmdBlk(2) & 0x20)
			{	// check CK_COND
				SCTXT_Flag |= SCTXT_FLAG_RET_TFR;
			}

			*ctxt_SataProto = PROT_DMAOUT << 4;
		
		#ifdef UAS_EN
			if (bot_mode)
		#endif
			{
				// set case 12		
				//*ctxt_PhaseCase_0 = 0x00;
				*ctxt_PhaseCase_1 = 0x10;
			}
		#ifdef UAS_EN
			else
			{
				if (hdd_set_uas_xfer_len())
				{
					goto invalid_cdb;
				}
			}
		#endif

			//*ctxt_DBufIndex = DBUF_SEG_U2S;
			//*ctxt_DBuf_IN_OUT = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_USB_W_PORT; 
			SCTXT_DbufIndex = DBUF_SEG_U2S;
			SCTXT_DbufINOUT = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_USB_W_PORT; 

			if (sata_exec_ctxt() == CTXT_STATUS_ERROR)
			{	// UAS only
				//usb_append_ctxt_que();
				usb_device_no_data();
				return;
			}										


			hdd_post_write = 1;
			hdd_start_act_led();
			return;

		}
		else if ((protocol) == (15 << 1)) // Return response Information
		{
			// get TFR
			sata_return_tfr();

			hdd_return_descriptor_sense(mc_buffer);
			byteCnt = 14;
			usb_device_data_in();
			return;
		}
		else
		{

			//DBG(("Bad protocol\n"));
#ifdef UAS_EN
invalid_cdb:
#endif
			hdd_err_2_sense_data(ERROR_ILL_CDB);
			usb_device_no_data();

		}

		return;
	}
#endif
	/****************************************\
		SCSI_FORMAT_UNIT
	\****************************************/
	case SCSI_FORMAT_UNIT:
		hdd_format_unit_cmd();
		return;

	/****************************************\
		SCSI_INQUIRY
	\****************************************/
	case SCSI_INQUIRY:
		hdd_inquiry_cmd();
		return;

	/****************************************\
		SCSI_LOG_SENSE
	\****************************************/
	case SCSI_LOG_SENSE:
	#if 0
		if (hdd_chk_drv_state())
		{
			return;
		}
	#endif
		hdd_log_sense_cmd();
		return;

	/****************************************\
		SCSI_RCV_DIAG_RESULTS
	\****************************************/
#if 1
	case SCSI_RCV_DIAG_RESULTS:
		hdd_receive_diag_results_cmd();
		break;
#endif

	/****************************************\
		SCSI MODE SENSE (6)
	\****************************************/
	case SCSI_MODE_SENSE6:
#if 1
		if (hdd_chk_drv_init())
		{
			return;
		}
#endif
		// MODE SENSE (6) command supports Short Block Descriptor only.
		CmdBlk[1] &= ~MODE_SENSE_LLBAA;

		// new "Allocation Length" field.
		CmdBlk(7) = 0;
		CmdBlk(8) = CmdBlk(4);

		// fall through to "SCSI_MODE_SENSE"

	/****************************************\
		SCSI MODE SENSE (10)
	\****************************************/
	case SCSI_MODE_SENSE:
#if 1
		if (hdd_chk_drv_init())
		{
			return;
		}
#endif
		hdd_mode_sense_cmd();
		return;

	/****************************************\
		SCSI_MODE_SELECT (6)
	\****************************************/
	case SCSI_MODE_SELECT6:  // 
		CmdBlk(7) = 0;
		CmdBlk(8) = CmdBlk(4);

		// fall through to "SCSI_MODE_SELECT"

	/****************************************\
		SCSI_MODE_SELECT(10)
	\****************************************/
	case SCSI_MODE_SELECT:   //

		byteCnt = *((u16 *)(&CmdBlk[7]));
		if (byteCnt == 0)
		{
			//*ctxt_Status = CTXT_STATUS_GOOD;
			usb_device_no_data();
			return;
		}

	#ifdef PWR_SAVING
		if (hdd_chk_drv_state())
		{
			return;
		}
	#endif
		usb_device_data_out();
		return;



	/****************************************\
		SCSI_READ_CAPACITY
	\****************************************/
	case SCSI_READ_CAPACITY:

		//DBG(("Rd Cap %bx\n", sobj_State));
	#if 1
		if (hdd_chk_drv_init())
		{
			return;
		}
	#endif
		
		if (sobj_sector_4K)  // logical 4k only
		{
			utmp32.udata.u_32 = sobj_sectorLBA_l >> 3;
			utmp32.udata.u_32 |= (sobj_sectorLBA_h & 0x000000007) << 29;
			*((u32 xdata *)mc_buffer) = utmp32.udata.u_32 - 2;

			mc_buffer[4] = 00;     //4/5 byte zero
			mc_buffer[5] = 00;     //4/5 byte zero
			mc_buffer[6] = 0x10;
			mc_buffer[7] = 0x00;     
		}
		else
		{
#ifndef SUPPORT_3T_4K	
			if (sobj_sectorLBA_h)	
			{
				if(rCapLimit)
				{
					*((U32 xdata *)mc_buffer) = 0xFFFFFFFF;
					rCapLimit--;
				}
				else
					*((U32 xdata *)mc_buffer) = 0xFFFFFFFE;
			}
			else
#endif				
			{
				*((u32 xdata *)mc_buffer) = sobj_sectorLBA_l - 2;
			}


			mc_buffer[4] = 00;     //4/5 byte zero
			mc_buffer[5] = 00;     //4/5 byte zero
#ifdef PHYSICAL_MORE_512
			mc_buffer[6] = (u8)((sobj_physical_sector_size << 1) >> 8) ;
#else
			mc_buffer[6] = 0x02;
#endif
			mc_buffer[7] = 0x00; 
		}
		MSG(("rc_ss %x", sobj_physical_sector_size));
		MSG(("rc_ss %x", mc_buffer[6]));

		byteCnt = 8;
		usb_device_data_in();
		return;


	/****************************************\
		SCSI_SERVICE_ACTION_IN16
	\****************************************/
	case SCSI_SERVICE_ACTION_IN16:
		if ((CmdBlk(1) & SA_MASK) != SA_READ_CAPACITY16)
		{
			hdd_err_2_sense_data(ERROR_ILL_CDB);
			usb_device_no_data();
			return;
		}
		// check allocation length
		if ((CmdBlk[10]|CmdBlk(11)|CmdBlk[12]|CmdBlk[13]) == 0)
		{	// allocation length is zero
			usb_device_no_data();
			return;
		}
		// fall thru "scsi_read_disk_capacity_cmd()"
#if 1
		if (hdd_chk_drv_init())
		{
			return;
		}
#endif
		xmemset(mc_buffer, 0, 32);
		
		//DBG(("Read Capacity 16\n"));
		
		if (sobj_sector_4K)  // logical 4k only
		{
			//*((u8 *)(mc_buffer)) 	= 0x00;
			//*((u8 *)(mc_buffer+1)) 	= 0x00;
			//*((u8 *)(mc_buffer+2)) 	= 0x00;
			//*((u8 *)(mc_buffer+3)) 	= 0x00;

			utmp32.udata.u_32 = sobj_sectorLBA_l >> 3;
			utmp32.udata.u_32 |= (sobj_sectorLBA_h & 0x000000007) << 29;
			*((u32 xdata *)(mc_buffer+4)) = utmp32.udata.u_32 - 2;
			
			*((u8 *)(mc_buffer+8)) = 0x00;
			*((u8 *)(mc_buffer+9)) = 0x00;
			*((u8 *)(mc_buffer+10)) = 0x10;		//4k block
			*((u8 *)(mc_buffer+11)) = 0x00;
		}
		else
		{
#ifndef 	SUPPORT_3T_4K	
			rCapLimit = 3;
#endif
		
			//*((u8 *)(mc_buffer)) 	= 0x00;
			//*((u8 *)(mc_buffer+1)) 	= 0x00;
			//*((u8 *)(mc_buffer+2)) 	= 0x00;
			//*((u8 *)(mc_buffer+3)) 	= 0x00;
			*((u32 xdata *)(mc_buffer)) = sobj_sectorLBA_h;			
			*((u32 xdata *)(mc_buffer+4)) = sobj_sectorLBA_l - 2;
			
			//*((u8 *)(mc_buffer+8)) = 0x00;
			//*((u8 *)(mc_buffer+9)) = 0x00;
#ifdef PHYSICAL_MORE_512
			mc_buffer[10] = (u8)((sobj_physical_sector_size << 1) >> 8) ;
#else			
			*((u8 *)(mc_buffer+10)) = 0x02;		//512 byte
#endif			
			//*((u8 *)(mc_buffer+11)) = 0x00;
		}
		byteCnt = Min(32, *((u32 *)(&CmdBlk(10))) );
		usb_device_data_in();
		return;

#ifdef UAS_EN
	case SCSI_PERSIST_RESERVE_IN:
		hdd_reserve_in_cmd();
		return;
#endif

	/****************************************\
		SCSI_REPORT_LUNS
	\****************************************/
#ifdef UAS_EN
	case SCSI_REPORT_LUNS:
		hdd_report_lun_cmd();
		return;
#endif

	/****************************************\
		SCSI_REQUEST_SENSE
	\****************************************/
	case SCSI_REQUEST_SENSE : //
		hdd_request_sense_cmd();
		return;

	/****************************************\
		SCSI_VERIFY16
	\****************************************/
	case SCSI_VERIFY16:

	#ifdef PWR_SAVING
		if (hdd_chk_drv_state())
		{
			return;
		}
	#endif
		if (CmdBlk(2) || CmdBlk(3) ||	//LBA > 48-bits 
			CmdBlk(10) || CmdBlk(11))	//transfer length > 0xffff
		{
			hdd_err_2_sense_data(ERROR_ILL_CDB);
			usb_device_no_data();
			return;
		}
		
		if (sobj_sector_4K) // logical 4k only
		{
			if (CmdBlk[12] & 0xE0)
			{	// Block size is too large
				hdd_err_2_sense_data(ERROR_ILL_CDB);
				usb_device_no_data();
				return;
			}
			// xlat to SATA block size
			*((u16 *)(&CmdBlk[12])) = (*(u16 *)(&CmdBlk[12])) << 3;


			if (CmdBlk[4] & 0xE0)
			{	// LBA is too large
				hdd_err_2_sense_data(ERROR_ILL_CDB);
				usb_device_no_data();
				return;
			}

			// xlat to SATA LBA
			val = CmdBlk[6] >> 5;
			(*(u32*)&CmdBlk[6]) = (*(u32*)&CmdBlk[6]) << 3;
			(*(u16*)&CmdBlk[4]) = ((*(u16*)&CmdBlk[4]) << 3) | val;
		}

		{
//			ctxt_FIS[FIS_TYPE] = HOST2DEVICE_FIS;		//
//			ctxt_FIS[FIS_C] = 0x80;
//			ctxt_FIS[FIS_COMMAND] = ATA6_READ_VERIFY_SECTORS_EXT;			//COMMAND
//			ctxt_FIS[FIS_FEATURE] = 0;										//FEATURES (7:0)

			ctxt_FIS[FIS_LBA_LOW] = CmdBlk(9);			//LBA (7:0)
			ctxt_FIS[FIS_LBA_MID] = CmdBlk(8);			//LBA (15:8)
			ctxt_FIS[FIS_LBA_HIGH] = CmdBlk(7);			//LBA (23:16)
			ctxt_FIS[FIS_DEVICE] = 0x40;				//Device

			ctxt_FIS[FIS_LBA_LOW_EXP]= CmdBlk(6);
			ctxt_FIS[FIS_LBA_MID_EXP]= CmdBlk(5);
			ctxt_FIS[FIS_LBA_HIGH_EXP]= CmdBlk(4);
			ctxt_FIS[FIS_FEATURE_EXP]= 0;

			ctxt_FIS[FIS_SEC_CNT] = CmdBlk(13);			// SECTOR_CNT(7:0)
			ctxt_FIS[FIS_SEC_CNT_EXP] = CmdBlk(12);		// SECTOR_CNT(15:8)
//			ctxt_FIS[FIS_RESERVED_14] = 0;
//			ctxt_FIS[FIS_CONTROL] = 0;
		}
		goto _verify10;
	
	/****************************************\
		SCSI_VERIFY10
	\****************************************/
	case SCSI_VERIFY10 :
	#ifdef PWR_SAVING
		if (hdd_chk_drv_state())
		{
			return;
		}
	#endif
		CmdBlk(1) = 0;
		if (sobj_sector_4K)  // logical 4k only
		{
			if (CmdBlk[7] & 0xE0)
			{	// Block size is too large
				hdd_err_2_sense_data(ERROR_ILL_CDB);
				usb_device_no_data();
				return;
			}
			*(u16 *)(&CmdBlk[7]) = (*(u16 *)(&CmdBlk[7])) << 3;

			// xlat to SATA LBA
			CmdBlk(1) = CmdBlk[2] >> 5;
			(*(u32*)&CmdBlk[2]) = (*(u32*)&CmdBlk[2]) << 3;
		}


			ctxt_FIS[FIS_LBA_LOW] = CmdBlk(5);			//LBA (7:0)
			ctxt_FIS[FIS_LBA_MID] = CmdBlk(4);			//LBA (15:8)
			ctxt_FIS[FIS_LBA_HIGH] = CmdBlk(3);			//LBA (23:16)
			ctxt_FIS[FIS_DEVICE] = 0x40;				//Device

			ctxt_FIS[FIS_LBA_LOW_EXP]= CmdBlk(2);
			ctxt_FIS[FIS_LBA_MID_EXP]= CmdBlk(1);
			ctxt_FIS[FIS_LBA_HIGH_EXP]= 0;
			ctxt_FIS[FIS_FEATURE_EXP]= 0;

			ctxt_FIS[FIS_SEC_CNT] = CmdBlk(8);			// SECTOR_CNT(7:0)
			ctxt_FIS[FIS_SEC_CNT_EXP] = CmdBlk(7);		// SECTOR_CNT(15:8)
_verify10:
			ctxt_FIS[FIS_RESERVED_14] = 0;
			ctxt_FIS[FIS_CONTROL] = 0;

		*ctxt_SataProto = PROT_ND << 4;
		SCTXT_Flag |= SCTXT_FLAG_U2B|SCTXT_FLAG_B2S;

		if (bot_mode)
		{
			*ctxt_PhaseCase_0 = 0x02;
			*ctxt_PhaseCase_1 = 0x00;
		}
		else
		{	//UAS Mode
			*ctxt_XferLen_0 = 0;
			*ctxt_XferLen_1 = 0;
			*ctxt_XferLen_2 = 0;
			*ctxt_XferLen_3 = 0;
		}


		*ctxt_CCMcmdinten = D2HFISI;
		//*ctxt_DBufIndex = DBUF_SEG_NULL;
		SCTXT_DbufIndex = DBUF_SEG_NULL;

		ctxt_FIS[FIS_TYPE] = HOST2DEVICE_FIS;		//
		ctxt_FIS[FIS_C] = 0x80;
		ctxt_FIS[FIS_COMMAND] = ATA6_READ_VERIFY_SECTORS_EXT;			//COMMAND
		ctxt_FIS[FIS_FEATURE] = 0;										//FEATURES (7:0)

		if (sata_exec_ctxt() == CTXT_STATUS_ERROR)
		{	// sense Sense IU if error return 
			usb_device_no_data();
			return;
		}
		hdd_start_act_led();

		//start_act_led();
		return;

	/****************************************\
		SCSI_WRITE_BUFFER10
	\****************************************/
	case SCSI_WRITE_BUFFER10: 				// write buffer
        DBG(("~SCSI_WRITE_BUFFER10\n"));

#ifdef SCSI_DOWNLOAD_FW
			byteCnt = *((u16 *)(&CmdBlk(7)));

			if (byteCnt == 0)
			{	// PARAMETER LIST LENGTH is zero
				usb_device_no_data();
				return;
			}
	
			if ((CmdBlk(1) == 0x5) && 
				(CmdBlk(2) == 0x00) && 
				(CmdBlk(3) == 0x03) && 
				//(CmdBlk(4) == 0x00) && 
				(CmdBlk(5) == 0x00) &&
				(CmdBlk(7) == 0x02) &&
				(CmdBlk(10) == 0x25) &&
				(CmdBlk(11) == 0xc9))
			{
				//DBG(("TUR %BX\n", sobj_State));
				usb_device_data_out();
				return;
	
			}


#endif

#if SCSI_HID
		// get PARAMETER LIST LENGTH
		else if (scsi_hid)
		{
		    DBG(("~SCSI_HID WRITE BUFFER\n"));
			if (((CmdBlk[1] == 0x1) || (CmdBlk[1] == 0x5)) && 
				(CmdBlk[2] == 0x00) &&
				(CmdBlk[3] == 0x02) &&
				(CmdBlk[4] == 0x00) &&
				(CmdBlk[5] == 0x24) &&
				(byteCnt == 0x04)
				)
			{	// SCSI_HID_GPIO_IN_ACK
				usb_device_data_out();
				return;
			}
		}					
#endif

#ifdef  WRITE_BUFFER_TEST

            byteCnt = Min(1024, *((u16 *)(&CmdBlk[8]))+*((u16 *)(&CmdBlk[7])));
            if (byteCnt == 0)
            {   // PARAMETER LIST LENGTH is zero
                //globalNvram.test_version = 0x00;
               // globalNvram.test_version2 = 0x00;
                usb_device_no_data();
                return;
            }
            if(         (CmdBlk(1) == 0x01)&& //MODE = Vendor specific
                        (CmdBlk(2) == 0x11)&& //BUFFER ID= 0
                        (CmdBlk(3) == 0x22)&& //BUFFER OFFSET[2] = 0
                        (CmdBlk(4) == 0x33)&& //BUFFER OFFSET[1] = 0
                        (CmdBlk(5) == 0x44) //BUFFER OFFSET[0] = 1
                        &&(CmdBlk(6) == 0x00)   //PARAMETER LIST LENGTH[2] = 0
                        )
            {
                DBG(("~Write data\n"));
                //globalNvram.test_version = 0x01;
                //WRITE_BLOCK(INITIO_Hello, ((u8 *)(mc_buffer)), 13);
                //WRITE_BLOCK(((u16 *)(mc_buffer)), ((u16 *)(mc_buffer)), byteCnt);
                usb_device_data_out();
                return;
            }
            else if(    (CmdBlk(1) == 0x01)&& //MODE = Vendor specific
                        (CmdBlk(2) == 0x00)&& //BUFFER ID= 0
                        (CmdBlk(3) == 0x00)&& //BUFFER OFFSET[2] = 0
                        (CmdBlk(4) == 0x01)&& //BUFFER OFFSET[1] = 0
                        (CmdBlk(5) == 0x00) //BUFFER OFFSET[0] = 1
                        &&(CmdBlk(6) == 0x00)   //PARAMETER LIST LENGTH[2] = 0

                    )
                {
                    //globalNvram.test_version2 = 0x02;
                    usb_device_data_out();
                    return;
                }
#endif
		
		goto invalid_op;

	/****************************************\
		SCSI_READ_BUFFER10
	\****************************************/
	case SCSI_READ_BUFFER10:      // read buffer
		hdd_rd_buffer_cmd();
		break;

	/****************************************\
		SCSI_TEST_UNIT_READY
	\****************************************/
	case SCSI_TEST_UNIT_READY:
		//DBG(("TUR %BX\n", sobj_State));
	//#ifdef USB_FAST_ENUM
	#ifdef UAS_EN
		if (!bot_mode)
		{
			//if ((SCTXT_Tag == 0x01) && (usb_curCtxt == CTXT_NULL))
			if (SCTXT_Tag == 0x01)
			{
				if (uas_chk_TSFull())
				{
					usb_device_no_data();
					uas_tur_tsf_cnt = 31;
					return;
				}
				uas_tur_tsf_cnt = 0;
			}
			else
			{
				if (uas_tur_tsf_cnt)
				{
					uas_tur_tsf_cnt--;
					usb_device_no_data();
					return;
				}
			}
		}
	#endif

	#if 1
		if (sobj_init == 0)
		{
			hdd_err_2_sense_data(ERROR_UA_BECOMING_READY);
			usb_device_no_data();
			return;
		}
	#endif
#if 1
		if (sobj_State == SATA_PWR_DWN)
		{
			//if (hdd_tst_unit_cnt < 10)
			{
				//hdd_tst_unit_cnt++;
				hdd_err_2_sense_data(ERROR_UA_NOT_READY_INIT);
			}
			//else
			//{
			//	sata_pwr_up();
			//	hdd_tst_unit_cnt = 0;
			//	hdd_err_2_sense_data(ERROR_UA_BECOMING_READY);
			//	if (hdd_chk_drv_state())
			//	{
			//		return;
			//	}
			//}
		}
		//else if (sobj_State <= SATA_STANDBY)
		else if (sobj_State <= SATA_PHY_RDY)
		{
//become_ready:
			hdd_err_2_sense_data(ERROR_UA_BECOMING_READY);
		}

		usb_device_no_data();
#else

	#ifdef PWR_SAVING
		if (hdd_chk_drv_state())
		{
			return;
		}
	#endif
		//SCTXT_Status = CTXT_STATUS_GOOD;
		usb_device_no_data();
#endif
		return;

	/****************************************\
		SCSI_SYNCHRONIZE_CACHE
	\****************************************/
	case SCSI_SYNCHRONIZE_CACHE:	
	case SCSI_SYNCHRONIZE_CACHE16:	
		//DBG(("SCSI_SYNCHRONIZE_CACHE\n"));

		//if ((sobj_State == SATA_PWR_DWN))
		//if ((!sobj_WrCache_enabled) || (sobj_State < SATA_READY) || (hdd_post_write == 0))
		//if ((sobj_State != SATA_READY) || (hdd_post_write == 0))
		if (sobj_State < SATA_READY)
		{
			//DBG(("No WrCache\n"));
			usb_device_no_data();
			return;
		}

#ifdef PWR_SAVING
		if (hdd_chk_drv_state())
		{
			return;
		}
#endif

		if (hdd_post_write)
		{
			ctxt_FIS[FIS_COMMAND] = ATA6_FLUSH_CACHE_EXT;			//COMMAND
			if (ata_ExecUSBNoDataCmd() == CTXT_STATUS_PENDING)
			{
				//start_act_led();
				hdd_post_write = 0;
				hdd_start_act_led();
				return;
			}
		}
			usb_device_no_data();


		return;

	/****************************************\
		SCSI_START_STOP_UNIT
	\****************************************/
	case SCSI_START_STOP_UNIT:   // it seems that nothing may be done
#if 0
		//CTXT_Status = CTXT_STATUS_GOOD;
		usb_device_no_data();
		return;
		
#else
		switch (CmdBlk(4) & 0xF0)
		{
		case 0x0:		// Start_Valid
			switch(CmdBlk(4) & 0x03)
			{
			case 0: //stop
				//DBG0(("STP HD\n"));

spin_down:
	#ifdef UAS_EN
				if (sobj_State > SATA_READY)
				{
				#if 1
					//DBG(("HDD Bsy %BX\n", sobj_State)); 
					hdd_chk_drv_busy();
				#else
					SCTXT_Status = CTXT_STATUS_ERROR;
					SCTXT_LunStatus = LUN_STATUS_TASK_SET_FULL;

					uas_device_no_data();
				#endif
					return;
				}
	#endif
				if (sobj_State == SATA_READY)
				{
					{
						ata_ExecNoDataCmd(ATA6_FLUSH_CACHE_EXT, 0);
						hdd_post_write = 0;
					}

					//DBG(("Spin Dn\n"));
	#if 1
					ata_ExecNoDataCmd(ATA6_STANDBY_IMMEDIATE, 0);
					//sobj_State = SATA_STANDBY;
		#ifdef PWR_SAVING
					sata_pwr_down();
		#endif
	#else
					ctxt_FIS[FIS_COMMAND] = ATA6_STANDBY_IMMEDIATE;			//COMMAND
					if (ata_ExecUSBNoDataCmd() == CTXT_STATUS_ERROR)
					{
						usb_device_no_data();
						return;
					}
					hdd_post_write = 0;
	#endif
				}
	#if 1
				{

		
		#ifdef PWR_SAVING
					//if (sobj_State < SATA_READY)
					{
						sata_pwr_down();
					}
		#endif
				}
	#endif
				//SCTXT_Status = CTXT_STATUS_GOOD;
				usb_device_no_data();

				return;

			case 1: //start
				//DBG(("STA HD\n"));
//dev_Active:
				//powerStatus = PWR_STATUS_ACTIVE;

spin_up:
	#ifdef PWR_SAVING
				if (hdd_chk_drv_init())
				{
					return;
				}
	#endif


				//DBG(("Spin Up\n"));
				if (sobj_State >= SATA_READY)
				{
					//DBG(("(spin Ignore)\n"));
					//SCTXT_Status = CTXT_STATUS_GOOD;
					usb_device_no_data();
					return;
				}

				if ((sobj_State == SATA_PWR_DWN))
				{
					sata_pwr_up();
					//hdd_on_led();
					if (CmdBlk(1) & 0x1)
					{
						usb_device_no_data();
						return;
					}
				}

				if ((sobj_State == SATA_STANDBY))
				{
					ctxt_FIS[FIS_COMMAND] = ATA6_IDLE_IMMEDIATE;			//COMMAND
					ctxt_FIS[FIS_FEATURE] = 0;								//FEATURES (7:0)
					if (ata_ExecUSBNoDataCmd() == CTXT_STATUS_ERROR)
					{
						usb_device_no_data();
					}
					hdd_start_act_led();
					return;
				}
				else
				{
					SCTXT_Status = CTXT_STATUS_GOOD;
					usb_device_no_data();
					return;
				}

			default:
				//goto ill_cdb;
//				DBG(("SCSI_START_STOP_UNIT:????\n"));
//				hdd_err_2_sense_data(ERROR_ILL_CDB);
				usb_device_no_data();
				return;
			}
#if 1
		case 0x10:		// Active
		case 0x20:		// idle
			hdd_lun_ctrl = 0;
			//powerStatus = PWR_STATUS_ACTIVE;			

			// Disable Standby Condition Timer
			hdd_standby_enabled  = 0;

			goto spin_up;
		case 0x30:		// standby
			//hdd_lun_ctrl = 0;
			//powerStatus = PWR_STATUS_STANDBY;

			// Disable Standby Condition Timer
			hdd_standby_enabled  = 0;
			if (sobj_State == SATA_READY)
			{
				goto spin_down;
			}
			usb_device_no_data();
			return;

		case 0x70:		// LU Control
			hdd_lun_ctrl = 1;
			//check active
			//if (powerStatus == PWR_STATUS_ACTIVE)	
			//if (sobj_State >= SATA_READY)
			{	// in active Power condition Page

				if (hdd_standby_enabled == 0)
				{	
					if (hdd_Standby_Timer)
					{
						hdd_standby_tick = 0;
						//hdd_Standby_Timer = standbyTimer32;
						hdd_standby_enabled = 1;				// 
					}
				}
			}
			usb_device_no_data();
			return;

		case 0xB0:		// Force_standby_0
			if (sobj_State == SATA_READY)
			{
				if (hdd_standby_enabled)
				{
					goto spin_down;
				}
			}

			goto ill_cdb;

	#endif

		}
ill_cdb:

		hdd_err_2_sense_data(ERROR_ILL_CDB);
		usb_device_no_data();
		return;
#endif





	/****************************************\
		default
	\****************************************/
	default:
invalid_op:
#if 1
		if (hdd_chk_drv_init())
		{
			return;
		}
#endif
		ERR(("Iv C:%BX\n", CmdBlk(0)));
		hdd_err_2_sense_data(ERROR_ILL_OP);
		usb_device_no_data();
		return;
	}

}


void hdd_post_data_out()
{
	//u32	byteCnt;
	//DBG(("post data out\n"));


#if 1
	dbuf_get_data(DBUF_SEG_U2C);	
	usb_rx_fifo_rst();
#else
	*dbuf_MuxSel = DBUF_SEG_U2C;
	// wait for Data Out Done
//	if ((*dbuf_IntStatus & DBUF_RDRDY))
	{
		sz16 = (dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_0) + (dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_1 << 8);
//		if (sz16 >= byteCnt)
		{	// 	done of drain stream
			*dbuf_MuxCtrl = SEG_DONE;
		}
		pU8		= (u8 *)mc_buffer;
 		for (; sz16 != 0; sz16--)
			*pU8++ = *dbuf_DataPort;
	}

	usb_rx_fifo_rst();
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxCtrl;
	*dbuf_MuxCtrl = SEG_RESET;
#endif

	if (!bot_mode)
	{	// for UAS Mode ony
		*host_ctxtmem_ptr = ctxt_site;
		SCTXT_INDEX = ctxt_site;

		CmdBlk(0) = ctxt_CDB[0];
		CmdBlk(1) = ctxt_CDB[1];
		CmdBlk(2) = ctxt_CDB[2];
		CmdBlk(3) = ctxt_CDB[3];
		CmdBlk(4) = ctxt_CDB[4];
		CmdBlk(5) = ctxt_CDB[5];
		CmdBlk(6) = ctxt_CDB[6];
		CmdBlk(7) = ctxt_CDB[7];
		CmdBlk(8) = ctxt_CDB[8];
		CmdBlk(9) = ctxt_CDB[9];
	}
	SCTXT_Status = CTXT_STATUS_GOOD;
	
	switch (CmdBlk(0))
	{
	/****************************************\
		SCSI_MODE_SELECT(6)
	\****************************************/
	case SCSI_MODE_SELECT6:  // 
		// process Mode Parameter
		//if (!bot_mode)
		{
			CmdBlk(7) = 0;
			CmdBlk(8) = CmdBlk(4);
		}


	/****************************************\
		SCSI_MODE_SELECT(10)
	\****************************************/
	case SCSI_MODE_SELECT:   //
		hdd_mode_select_cmd();
		break;

	/****************************************\
		SCSI_WRITE_BUFFER10
	\****************************************/
	case SCSI_WRITE_BUFFER10:   //

#ifdef SCSI_DOWNLOAD_FW	
		if ((CmdBlk(1) == 0x5) && 
			(CmdBlk(2) == 0x00) && 
			(CmdBlk(3) == 0x03) && 
			(CmdBlk(5) == 0x00) &&
			(CmdBlk(7) == 0x02) &&
			(CmdBlk(10) == 0x25) &&
			(CmdBlk(11) == 0xc9))
		{
			//DBG(("w r download 1\n"));
			hdd_odd_scsi_downlod_fw();
			break;
		}
#endif

#if SCSI_HID
		else if (scsi_hid)
		{
			if ((CmdBlk(5) == 0x24) && (CmdBlk(8) == 0x04))
			{	// SCSI_HID_GPIO_IN_ACK
#if 1
				DBG(("HID_InA: %BX\n)", mc_buffer[3]));
#endif
	
				{	
					scsiGpioData &=  (~mc_buffer[3]);
				}
				break;
			}
		}					
#endif		
		break;

	}	
	

#ifdef UAS_EN
	if (bot_mode)
#endif
	{
		*usb_Msc0Residue_0 = 0;			// for BOT only
		*usb_Msc0Residue_1 = 0;			// for BOT only
		*usb_Msc0Residue_2 = 0;			// for BOT only
		*usb_Msc0Residue_3 = 0;			// for BOT only
	}
	usb_rx_ctxt_send_status();

}


/****************************************\
   hdd_start_act_led
\****************************************/
void hdd_on_led()
{
	{	// Initio Mode
		ACT_LED_ON();
		led_state = LED_ON;
	}

}


/****************************************\
   hdd_start_act_led
\****************************************/
void hdd_start_act_led()
{
	{	// Initio Mode
		if ((led_state != LED_ACTIVITY))
		{
			ACT_LED_ON();
			led_on = 1;
			led_state = LED_ACTIVITY;
			if (usbMode == CONNECT_USB3)
			{	// blink fast 3.3Hz 
				led_interval = ACTIVITY_ON_TIME_USB3;
				led_activity_repeat = ACTIVITY_REPEAT_USB3;
			}
			else
			{	// blink fast 1Hz
				led_interval = ACTIVITY_ON_TIME;
				led_activity_repeat = ACTIVITY_REPEAT;
			}
		}
		else
		{
			EAL = 0;
			if (usbMode == CONNECT_USB3)
			{
				led_activity_repeat = ACTIVITY_REPEAT_USB3;
			}
			else
			{
				led_activity_repeat = ACTIVITY_REPEAT;
			}
			EAL = 1;
		}
	}

#ifdef STANDBY_TIMER
	hdd_standby_tick = 0;
#endif

}

/****************************************\
   hdd_standby_led()
\****************************************/
void hdd_standby_led()
{
	{	// Initio Mode
		led_on = 0;

		ACT_LED_OFF();
	}
}

/****************************************\
   hdd_led_going_off()
\****************************************/
void hdd_led_going_off()
{
	{	// Initio Mode

		ACT_LED_OFF();
		led_on = 0;
		led_state = LED_OFF;

	}
}


#ifdef STANDBY_TIMER
void hdd_calc_Idle_timer(U8 time)
{
	if (time == 0)
	{
		hdd_standby_enabled = 0;
		//hddStandbyTimer32 = 0;// no power management
	}
	else
	{
		hdd_standby_enabled = 1;
		if (time == 36)// 3min
		{
			hdd_Standby_Timer = 1800;
		}
		else if (time == 60)// 5min
		{
			hdd_Standby_Timer = 3000;
		}
		else if (time == 120)// 10min
		{
			hdd_Standby_Timer = 6000;
		}
		else if (time == 180)// 15min
		{
			hdd_Standby_Timer = 9000;
		}
		else if (time == 240)// 20min
		{
			hdd_Standby_Timer = 12000;
		}
		else if (time == 241)// 30min
		{
			hdd_Standby_Timer = 18000;
		}
		else if (time == 242)// 1hours
		{
			hdd_Standby_Timer = 36000;
		}
		else if (time == 244)// 2hours
		{
			hdd_Standby_Timer = 72000;
		}
		else if (time == 246)// 3hours
		{
			hdd_Standby_Timer = 108000;
		}
		else if (time == 248)// 4hours
		{
			hdd_Standby_Timer = 144000;
		}
		else if (time == 250)// 5hours
		{
			hdd_Standby_Timer = 180000;
		}
		else
		{
			hdd_standby_enabled = 0;
			//hddStandbyTimer32 = 0;// not support
		}
	}
}
#endif


/***********************************************************\
	hdd_exec_bot_RW_ctxt()
		Execute a BOT R/W command on SATA Obj 

	Input Global Parameter:
		ctxt_site: site # of CDB Context for BOT R/W command 
\***********************************************************/
//#ifdef USB_FAST_ENUM
void hdd_exec_bot_RW_ctxt()
{
#ifdef PWR_SAVING
	if (sobj_State < SATA_STANDBY)
	{
		if (sobj_State == SATA_PWR_DWN)
		{
			sata_pwr_up();
			// turn-on LED
			//hdd_on_led();
		}

		hdd_que_ctxt_site = ctxt_site;
		hdd_que_ctxt_tout = HDD_QUE_TOUT;					// 
	}
	else
#endif
		sata_exec_dmae_RW_ctxt();

}
//#endif

#ifdef STANDBY_TIMER
void hdd_tick_isr()
{
#ifdef REJECT_U1U2
	if (usbMode == CONNECT_USB3)
	{
		if (u1U2_reject_state == U1U2_REJECT)
		{
			if (*usb_Msc0CtxtUsed_0 == 0)
			{	// CTxt is not pending, count down U1U2_ctrl_timer
				if (U1U2_ctrl_timer)
				{
					--U1U2_ctrl_timer;
					if (U1U2_ctrl_timer == 0)
					{	
						//if (usb_ctxt_que == CTXT_NULL)
						{
 							*usb_DevCtrlClr_1 = USB3_U1_REJECT;	
							*usb_DevCtrlClr_2 = USB3_U2_REJECT;
							u1U2_reject_state = U1U2_ACCEPT;
						}
					}
				}
			}
			else
			{	// CTxt is/are pending, restore U1U2_REJECT_TIME to U1U2_ctrl_timer
				U1U2_ctrl_timer = U1U2_REJECT_TIME;
			}
		}
	}
#endif

	if (hdd_standby_enabled)
	{
		if (sobj_State == SATA_READY)
		{
			hdd_standby_tick++;
			if (hdd_standby_tick > hdd_Standby_Timer)
			{
				{
					ata_ExecNoDataCmd(ATA6_FLUSH_CACHE_EXT, 0);
					hdd_post_write = 0;
				}

				if (ata_ExecNoDataCmd(ATA6_STANDBY_IMMEDIATE, 0) != CTXT_STATUS_ERROR)
				{
					DBG(("HDD STB\n"));
					hdd_standby_tick = 0;
					sobj_State = SATA_STANDBY;
					//hdd_standby_counter = 0;
					//hdd_post_write = 0;

				}
				else
				{
					DBG(("HDD STB Fail\n"));
				}

		#ifdef PWR_SAVING
				//if (DrvPwrMgmt)
				{
					//sata_pwr_down();
				}
		#endif

				hdd_standby_led();


			}
		}
		else
		{
			hdd_standby_tick = 0;
		}
	}

	#if 0
	if (hdd_que_ctxt_site != CTXT_NULL)
	{
		if (sobj_State < SATA_STANDBY)
		{
			if (hdd_que_ctxt_site != CTXT_NULL)
			{
				hdd_que_ctxt_tout--;
				if (hdd_que_ctxt_tout == 0)
				{
					ctxt_site = hdd_que_ctxt_site;
					hdd_que_ctxt_site = CTXT_NULL;

					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;

					// Time out
					hdd_err_2_sense_data(ERROR_UA_BECOMING_READY);
					usb_device_no_data();

				}
			}
		}
	}
	#endif

	//if (( SATA_DRV_RDY > sobj_State) && (sobj_State >= SATA_RESETING))
	{
		#if 1
			if (sobj_State == SATA_RESETING)
			{
				sobj_State_tout--;
				if (sobj_State_tout == 0)
				{
					if (sobj_init)
					{
						//DBG(("A S_init RST TO\n"));
						//sobj_init = 0;
						//sobj_init_tout = 255;	// 25.5sec
						usb_active = 0;
						sobj_State = SATA_NO_DEV;
						return;
					}
					//DBG(("S RST TO\n"));
					// SATA Phy Ready Time-Out, Reset SATA Again
					//sata_Reset(SATA_HARD_RST);
					sata_HardReset();
				}
				return;
			}
		#endif

		#if 1
			else if (sobj_State == SATA_PHY_RDY)
			{
				sobj_State_tout--;
				if (sobj_State_tout == 0)
				{
					//DBG(("S PRDY TO\n"));
					// SATA Drive Ready Time-Out
					//sata_Reset(SATA_HARD_RST);
					sata_HardReset();
				}
				return;
			}
		#endif

		#if 0
			if	((sobj_State == SATA_DRV_RDY) && (sobj_State_tout > SATA_DRV_INIT_TOUT))
			{
				// SATA Drive INIT Time-Out
				usb_active = 0;
				return;
			}
		#endif
	}
}
#endif
