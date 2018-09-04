/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2004-2013, All Rights Reserved
 *
 *   This file contains confidential and proprietary information
 *   which is the property of Initio Corporation.
 *
 *
 *****************************************************************************
 * Modification History
 *
 * Mod No	Date		Who				Description
 * 3610		2010/04/09	Odin			initial version
 * 3610		2010/04/27	Odin			USB2.0 BOT Debugging
 *										initial sata follow hardward recommand
 * 3607		2010/11/20	Odin			AES function
 * 3609		2010/12/21	Ted				SATA single-side
 *										Remove AES function
 *
 *******************************************************************************/


#define SATA_C
#include	"general.h"

u8 code  i2bit[] = {
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80
};

u8 code  i2mbit[] = {
	0x01,
	0x03,
	0x07,
	0x0F,
	0x1F,
	0x3F,
	0x7F,
	0xFF
};

#ifdef UAS_EN

u8 sata_AllocateSCM()
{

	//DBG(("alloc cFree: %LX\n", sobj_cFree1.udata.u_32));

	for (tmp8= 0; tmp8 < 4; tmp8++)
	{
		for (i8= 0, val = 1; i8 < 8; i8++, val= val<< 1)
		{
			if((sobj_cFree1.udata.u_8[tmp8] & val))
			{
	//			sobj_cFree &= ~bit32;
				sobj_cFree1.udata.u_8[tmp8] &= ~val;
				return ((tmp8 << 3) + i8);
			}
		}
	}

	//ERR("get ccm fail");
	return CCM_NULL;
}

/****************************************\
   sata_DetachSCM

	Input Global Parameter:
		utmp32: SActive
\****************************************/
void sata_DetachSCM()
{
	//u8			scm_site;

#if 1
	sobj_cFree1.udata.u_8[0] |= utmp32.udata.u_8[0];
	sobj_cFree1.udata.u_8[1] |= utmp32.udata.u_8[1];
	sobj_cFree1.udata.u_8[2] |= utmp32.udata.u_8[2];
	sobj_cFree1.udata.u_8[3] |= utmp32.udata.u_8[3];
#else
	utmp32.udata.u_32 = SActive;
	sobj_cFree |= utmp32.udata.u_32;

	//DBG("free tag sactive: %LX\n", SActive);

	for (scm_site = 0; scm_site < 32; tmp32= tmp32 >> 1, scm_site++)
	{
		if (tmp32 & 1)
		{
			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;

			SCM_Next = SCM_NULL;
			SCM_CdbIndex = CTXT_NULL;
			if (tmp32 == 0)
				return;
		}
	}
#endif
}
#endif
/****************************************\
 sata_abort_task

\****************************************/
//#ifdef UAS_EN
#if 0
bit sata_abort_task(u8 tid)
{
	u8 prevScm;

	if ((scm_site == sobj_curScm)  != SCM_NULL)
	{
		SCM_INDEX = scm_site;
		if ((ctxt_site = SCM_CdbIndex) != CTXT_NULL)
		{
			//*host_ctxtmem_ptr = ctxt_site;
			SCTXT_INDEX = ctxt_site;
			// found match TID
			//if (*ctxt_ITag_0 == tid)
			if (SCTXT_Tag == tid)
			{
				SCM_CdbIndex = CTXT_NULL;
				//sobj_curScm = SCM_NULL;
				uas_abort_ctxt(ctxt_site);
				return 0;
			}
		}
		return 0;
	}
	// search non-NCQ queue
	if ((scm_site = sobj_que) != SCM_NULL)
	{
		while (scm_site != SCM_NULL)
		{
			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;
			if ((ctxt_site = SCM_CdbIndex) != CTXT_NULL)
			{

				//*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;
				// found match TID
				//if (*ctxt_ITag_0 == tid)
				if (SCTXT_Tag == tid)
				{
					// un-queue scm_site
					if ((scm_site == sobj_que))
					{
						sobj_que = SCM_Next;
					}
					else
					{
						tmp8 = SCM_Next;

						//*sata_CCMSITEINDEX = prevScm;	
						SCM_INDEX = prevScm;
						SCM_Next = tmp8;
						//*sata_CCMSITEINDEX = scm_site;
						SCM_INDEX = scm_site;
					}
					SCM_CdbIndex = SCM_NULL;

					//sata_DetachSCM(1 << scm_site); //
					//sobj_cFree |= (1 << scm_site);
					sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

					uas_abort_ctxt(ctxt_site);
					return 0;
				}
			}
			prevScm = scm_site;
			scm_site = SCM_Next;
		}
	}

	// search NCQ queue
	if ((scm_site = sobj_ncq_que) != SCM_NULL)
	{
		while (scm_site != SCM_NULL)
		{
			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;
			if ((ctxt_site = SCM_CdbIndex) != CTXT_NULL)
			{
				//*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;

				// check TID
				//if (*ctxt_ITag_0 == tid)
				if (SCTXT_Tag == tid)
				{	// found match TID
					// un-queue scm
					if ((scm_site == sobj_ncq_que))
					{
						sobj_ncq_que = SCM_Next ;
					}
					else
					{
						tmp8 = SCM_Next;

						//*sata_CCMSITEINDEX = prevScm;		
						SCM_INDEX = prevScm;
						SCM_Next = tmp8;
						//*sata_CCMSITEINDEX = scm_site;		
						SCM_INDEX = scm_site;
					}

					SCM_CdbIndex = CTXT_NULL;
					//sata_DetachSCM(1 << scm_site); //
					//sobj_cFree |= (1 << scm_site);
					sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

					uas_abort_ctxt(ctxt_site);
					return 0;
				}
			}
			prevScm = scm_site;
			scm_site = SCM_Next;
		}
	}
	return 1;
}
#endif	//UAS_EN


/****************************************\
   sata_abort_all
\****************************************/
//#ifdef UAS_EN
#if 0
void sata_abort_all()
{
	//u8	scm_site;

	// abort non-NCQ queue
	if ((scm_site = sobj_que) != SCM_NULL)
	{

		while (scm_site != SCM_NULL)
		{	// release CCM/SCM
			//sata_DetachSCM(pSob, 1 << pScm->scm_ccmIndex); //
			//sobj_cFree |= (1 << scm_site);
			sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;

			if ((ctxt_site = SCM_CdbIndex) != CTXT_NULL)
			{
				SCM_CdbIndex = CTXT_NULL;

				*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;
				uas_abort_ctxt(ctxt_site);
			}
			scm_site =  SCM_Next;
			SCM_Next = SCM_NULL;
		}
		sobj_que = SCM_NULL;
	}

	// abort NCQ queue
	if ((scm_site = sobj_ncq_que) != SCM_NULL)
	{
		while (scm_site != SCM_NULL)
		{	// release CCM/SCM
//			sata_DetachSCM(1 << pScm->scm_ccmIndex); //
			//sobj_cFree |= (1 << scm_site);
			sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;
			if ((ctxt_site = SCM_CdbIndex) != CTXT_NULL)
			{
				SCM_CdbIndex = CTXT_NULL;
				// abort pCtxt 
				//*host_ctxtmem_ptr = ctxt_site;
				//SCTXT_INDEX = ctxt_site;
				uas_abort_ctxt(ctxt_site);
			}
			scm_site =  SCM_Next;
			SCM_Next = SCM_NULL;
		}
	}
	if (sobj_State > SATA_READY)
	{
		for (tmp8= 0; tmp8 < 4; tmp8++)
		{
			for (i8= 0, val = 1; i8 < 8; i8++, val= val<< 1)
			{
				if((sobj_sBusy1.udata.u_8[tmp8] & val))
				{
					scm_site = (tmp8 << 3) + i8 ;					
					SCM_INDEX = scm_site;
					if ((ctxt_site = SCM_CdbIndex) != CTXT_NULL)
					{
						SCM_CdbIndex = CTXT_NULL;
						// abort pCtxt 
						//*host_ctxtmem_ptr = ctxt_site;
						//SCTXT_INDEX = ctxt_site;
						uas_abort_ctxt(ctxt_site);
					}
				}
			}
		}
		sata_Reset(SATA_HARD_RST);
	}
	
}
#endif	//UAS_EN

#ifdef UAS_EN
void sata_abort_all()
{
	if (sobj_State > SATA_READY)
	{
		sata_HardReset();
		sata_InitSataObj();
	}
}
#endif

#ifdef UAS_EN
void sata_abort_all_USBIO()
{
	//u8 scm_site;
	//DBG0(("sata_InitSataObj:%LX\n", sobj_default_cFree1.udata.u_32));

	//FIXME
	for(scm_site=0; scm_site<32; scm_site++)
	{	// select SCM
		SCM_INDEX = scm_site;

		//DBG(("scm_site %BX %BX\n", scm_site, SCM_CdbIndex));
		if (SCM_CdbIndex != CTXT_NULL)
		{
			*host_ctxtmem_ptr = SCM_CdbIndex;
			SCTXT_INDEX = SCM_CdbIndex;
			ctxt_site = SCM_CdbIndex;

			//if (SCTXT_Flag & SCTXT_FLAG_NCQ)
			{
				hdd_err_2_sense_data(ERROR_DATA_PHASE);
				uas_device_no_data();
				SCM_CdbIndex = CTXT_NULL;
			}
		}

		SCM_Next = SCM_NULL;
	}
}
#endif

void sata_abort_bot_io()
{
	if (sobj_curScm != SCM_NULL)
	{
		SCM_INDEX = sobj_curScm;

		sobj_curScm = SCM_NULL;

		if ((SCM_prot == PROT_DMAIN) || (SCM_prot == PROT_PIOIN))
		{
			if (SCM_SegIndex != DBUF_SEG_NULL)
			{
				*dbuf_MuxSel = SCM_SegIndex;		//SCTXT_DbufIndex
				SCM_SegIndex = DBUF_SEG_NULL;		

				// Disconnect input port
				*dbuf_MuxInOut &= 0xf0;
				*dbuf_MuxInOut;
					
			#ifndef SATA_AUTO_FIFO_RST
				*sata_BlkCtrl_1 = RXSYNCFIFORST;	//reset sata RX  fifo	
				//*sata_BlkCtrl_1;
			#endif
			}

			if ( (ctxt_site = SCM_CdbIndex) != CTXT_NULL)
			{
				// break association from ctxt to scm
				SCM_CdbIndex = CTXT_NULL;
				SCTXT_CCMIndex = CCM_NULL;

				*usb_Msc0DICtrl = MSC_DATAIN_RESET;
				if (SCTXT_DbufIndex != DBUF_SEG_NULL)
				{
					DBUF_SegFree(SCTXT_DbufIndex);
					SCTXT_DbufIndex = DBUF_SEG_NULL;
				}
				hdd_err_2_sense_data(ERROR_CRC_ERROR);
				SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
				usb_exec_tx_ctxt();
			}
		}
		else if ((SCM_prot == PROT_DMAOUT) || (SCM_prot == PROT_PIOOUT))
		{
			if ((segIndex = SCM_SegIndex) != DBUF_SEG_NULL)
			{
				*dbuf_MuxSel = SCM_SegIndex;		//SCTXT_DbufIndex;
			
				if (!(SCTXT_Flag & SCTXT_FLAG_B2S))
				{

				#ifndef SATA_AUTO_FIFO_RST
					*sata_BlkCtrl_1 = TXSYNCFIFORST;	//reset sata TX  fifo
					*sata_BlkCtrl_1;
				#endif
					// Reset on USB RX FIFO will propagate to DBUFF & 
					// it is not necssary to reset DBUFF again
					usb_rx_fifo_rst();
				}
				else
				{	// If CPU is source, F/W has to reset DBUFF.
					*dbuf_MuxCtrl = SEG_RESET;	//3609
					*dbuf_MuxCtrl;
				}
				*dbuf_MuxStatus = SEG_IDLE;
				// Disconnect both input & output port
				//DBUF_SegFree(segIndex);
				*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
				*dbuf_MuxInOut;
			}

			if ((ctxt_site = SCM_CdbIndex) == CTXT_NULL)
			{
				SCM_CdbIndex = CTXT_NULL;
				SCTXT_CCMIndex = CCM_NULL;

				*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
				//*usb_Msc0DOutCtrl;

				*usb_Msc0DOutCtrlClr = MSC_DOUT_HALT;
				//*usb_Msc0DOutCtrlClr;

				*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET;
				//*usb_Msc0DOutCtrlClr;

				*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT;
				*usb_Msc0IntStatus_0;

				hdd_err_2_sense_data(ERROR_CRC_ERROR);
				SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
				usb_exec_tx_ctxt();
			}
		}
		else	//if ((SCM_prot == PROT_ND))
		{
			if ((ctxt_site = SCM_CdbIndex) == CTXT_NULL)
			{
				SCM_CdbIndex = CTXT_NULL;
				SCTXT_CCMIndex = CCM_NULL;

				if (SCTXT_Flag & SCTXT_FLAG_U2B)
				{
					//usb_append_ctxt_que();
					usb_device_no_data();
				}
				else if (!(SCTXT_Flag & SCTXT_FLAG_B2S))
				{
//						usb_tx_ctxt_send_status(pCtxt);
					MSG(("D2HFISI with no Data between USB & SATA\n"));
				}
			}
		}
	}
}

/*****************************************************\
   sata_exec_scm : for Non-NCQ SATA command

   Input Global Parameter:
		scm_site: site # of CCM/SCM for SATA OBJ 
		sata_CCMSITEINDEX: site # of CCM for SATA OBJ
		SCM_INDEX: site # of SCM for SATA OBJ

\*****************************************************/
void sata_exec_scm()
{
//DBG0(("s_e_s %BX %bx\n", scm_site, SCM_CdbIndex));
	
	//DBG(("exec_scm: scm_site: %BX, segIndex: %BX, seg_INOUT: %BX\n", 
	//scm_site, *sata_Ccm_SegIndex, *sata_Ccm_SegINOUT));
	
	//DBG(("SI %BX\n\n", *sata_IntStatus_0));
	//*sata_EXQNINP = 0;
	*sata_EXQNINP = scm_site;
	*sata_Status = ATA_STATUS_BSY;
	sobj_curScm = scm_site;

	if ((SCM_CdbIndex) == CTXT_NULL)
	{
		if ((segIndex = SCM_SegIndex) != DBUF_SEG_NULL)
		{
			*dbuf_MuxSel = segIndex;
			*dbuf_MuxInOut = SCM_SegINOUT;
		}
	}
	else
	{	
		ctxt_site = SCM_CdbIndex;
		SCTXT_INDEX = ctxt_site;
		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{	// for Data-in
			//*sata_Status |= ATA_STATUS_BSY;
			//if ((tmp8 = CTXT_DbufIndex) != DBUF_SEG_NULL)
			if ((segIndex = SCM_SegIndex) != DBUF_SEG_NULL)
			{
				*dbuf_MuxSel = segIndex;
				
				if (SCTXT_Flag & SCTXT_FLAG_B2S)
				{
					*dbuf_MuxInOut = SCM_SegINOUT;
					//*dbuf_MuxInOut = SCM_SegINOUT & 0x0F;
				}
				else
				{
					*dbuf_MuxInOut =  (TX_DBUF_NULL << 4) | TX_DBUF_SATA0_W_PORT;
				}
			}
			if (!(SCTXT_Flag & SCTXT_FLAG_B2S))
			{
				//usb_exec_tx_ctxt();
				usb_append_ctxt_que();
			}
		}
		else
		{
			if (!(SCTXT_Flag & SCTXT_FLAG_B2S))
			{
				//usb_exec_rx_ctxt();
				usb_append_ctxt_que();
			}
		}
	}

	sobj_State = SATA_ACTIVE;
}


/******************************************************\
   sata_append_scm()

   Input Global Parameter:
		scm_site: site # of SCM for SATA OBJ 
		sata_CCMSITEINDEX: site # of SCM for SATA OBJ
\*****************************************************/
void sata_append_scm()
{
	u8 qScm;

	if ((sobj_State == SATA_STANDBY) ||
		(sobj_State == SATA_READY) )
	{
		sata_exec_scm();
		return;
	}
#ifdef UAS_EN
	if (sobj_State == SATA_NCQ_ACTIVE)
	{
//DBG0(("S_NCQ_F %bx %bx\n", scm_site, SCM_CdbIndex));

		sobj_State = SATA_NCQ_FLUSH;
	}
#endif

	// append to non-ncq SCM que
	//*SHADOW_CCMSiteIndex = scm_site;

	SCM_Next = SCM_NULL;

	if ((qScm = sobj_que) == SCM_NULL)
	{
		sobj_que = scm_site;
	}
	else
	{
//		*SHADOW_CCMSiteIndex = qScm;
		SCM_INDEX = qScm;
		while ((tmp8 = SCM_Next) != SCM_NULL)
		{
			qScm = tmp8;
			//*SHADOW_CCMSiteIndex = tmp8;
			SCM_INDEX = tmp8;
		}
		SCM_Next = scm_site;
	}
			
}


/****************************************\
   sata_exec_ncq_scm
\****************************************/
#ifdef UAS_EN
void sata_exec_ncq_scm(u8 scm_site)
{

	//DBG(("exec_ncq_scm %bx\n", scm_site));
	*sata_EXQNINP = scm_site;

	//*sata_CCMSITEINDEX = scm_site;
	SCM_INDEX	= scm_site;

	SCM_Next = SCM_NULL;
	
	//sobj_sBusy |= (1 << scm_site);
	sobj_sBusy1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];
	sobj_State = SATA_NCQ_ACTIVE;				
}
#endif	//UAS_EN

/****************************************\
   sata_append_ncq_scm
\****************************************/
#ifdef UAS_EN
void sata_append_ncq_scm(u8 scm_site)
{
u8 data qScm;
//#define qScm	tmp8
	
	//DBG(("append_ncq_scm %bx\n", sobj_State));
	//SCM_Next = SCM_NULL;

	if ((sobj_State == SATA_STANDBY) ||
		(sobj_State == SATA_READY) ||
		(sobj_State == SATA_NCQ_ACTIVE) )
	{
		sata_exec_ncq_scm(scm_site);
		return;
	}
	if ((qScm = sobj_ncq_que) == SCM_NULL)
	{
		sobj_ncq_que = scm_site;
	}
	else
	{
		//*SHADOW_CCMSiteIndex = qScm;
		SCM_INDEX = qScm;
		while ((tmp8 = SCM_Next) != SCM_NULL)
		{
			qScm = tmp8;
			//*SHADOW_CCMSiteIndex = tmp8;
			SCM_INDEX = tmp8;
		}
		SCM_Next = scm_site;
	}
//#undef qScm
			
}
#endif	//UAS_EN


/****************************************\
   sata_return_tfr
\****************************************/
void sata_return_tfr()
{

	{
		// For BOT & UAS only
		bot_tfr_fis[FIS_STATUS] =		*sata_Status;
		bot_tfr_fis[FIS_ERROR] =		*sata_Error;

		bot_tfr_fis[FIS_LBA_LOW] =		*sata_LbaL;
		bot_tfr_fis[FIS_LBA_MID] =		*sata_LbaM;
		bot_tfr_fis[FIS_LBA_HIGH] =		*sata_LbaH;
		bot_tfr_fis[FIS_DEVICE] =		*sata_Device;

		bot_tfr_fis[FIS_LBA_LOW_EXP] =  *sata_LbaLH;
		bot_tfr_fis[FIS_LBA_MID_EXP] =  *sata_LbaMH;
		bot_tfr_fis[FIS_LBA_HIGH_EXP] = *sata_LbaHH;

		bot_tfr_fis[FIS_SEC_CNT] =		*sata_SectCnt;
		bot_tfr_fis[FIS_SEC_CNT_EXP] =	*sata_SectCntH;
	}
}

/****************************************\
   sata_exec_next_scm
\****************************************/
void sata_exec_next_scm()
{
//	u8	scm_site;

	// any pending non-NCQ command ?
	if ((scm_site = sobj_que) != SCM_NULL)
	{
		//*sata_CCMSITEINDEX = scm_site;
		SCM_INDEX = scm_site;

		sobj_que = SCM_Next;
		SCM_Next = SCM_NULL;

		sata_exec_scm();
	}
#ifdef UAS_EN
	else if ((scm_site = sobj_ncq_que) != SCM_NULL)
	{
		do
		{
			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;
			sobj_ncq_que =  SCM_Next;
			SCM_Next = SCM_NULL;

			if (SCM_CdbIndex != CTXT_NULL)
			{
				sata_exec_ncq_scm(scm_site);
				return;
			}
			// SCM is aborted, so released it
			sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

			//DBG0(("\tSCM %BX abted\n", scm_site));
			// Get next SCM in Que
			scm_site = sobj_ncq_que;
	
		} while (scm_site != SCM_NULL);
	}
#endif
}					

#ifdef UAS_EN
void sata_ncq_error_handling()
{
	//DBG0(("NCQ Er\n"));
	if 	(sobj_curScm != CCM_NULL)
	{
		SCM_INDEX = sobj_curScm;
		sobj_curScm = CCM_NULL;
		if(SCM_SegIndex != DBUF_SEG_NULL)
		{
//DBG(("sNCQ E3 %BX\n", SCM_SegIndex));
			DBUF_SegFree(SCM_SegIndex);
			SCM_SegIndex = DBUF_SEG_NULL;
		}

		if (SCM_CdbIndex != CTXT_NULL)
		{
			*host_ctxtmem_ptr = SCM_CdbIndex;
			SCTXT_INDEX = SCM_CdbIndex;
			hdd_err_2_sense_data(ERROR_CRC_ERROR);

			if (usb_curCtxt == SCM_CdbIndex)
			{	// Current USB context
				if (SCTXT_Flag & SCTXT_FLAG_DIN)
				{
					//*usb_Msc0DICtrl = MSC_DIN_DONE;
				}
				else
				{
					if (SCTXT_Status == CTXT_STATUS_XFER_DONE)
					{
						usb_rx_ctxt_send_status();
					}
					else
					{
						*usb_Msc0DOutCtrl = MSC_DOUT_BBKT|MSC_DOUT_DONE;
					}
				}
			}
			else
			{	// not  Current USB context, it should be in USB Pending Que 
				// Set to no Data Transfer
				SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
			}
			SCM_CdbIndex = CTXT_NULL;
		}
	}

	sata_abort_all_USBIO();
	sata_Reset(SATA_HARD_RST);
	*sata_BlkCtrl_1 = (RXSYNCFIFORST | TXSYNCFIFORST);	//reset sata TX  fifo
	*sata_BlkCtrl_1;

				//3609 reset sata_SiteCmdIntEn to 0
	*sata_SiteCmdIntEn_0 = 0;
}
#endif

/****************************************\
	sata_isr
\****************************************/
void sata_isr()
{
	u8 sataIsrCnt = 0;

sata_isr_begin:
	sata_frame_int_0 = *sata_FrameInt_0;
	sata_int_status_0 = *sata_IntStatus_0;

	DBG(("sata_isr: status:%BX frame:%BX, sobj_State:%BX\n", 
		sata_int_status_0, 
		sata_frame_int_0,
		sobj_State
	));

#ifdef UAS_EN
	if (sobj_State >= SATA_NCQ_FLUSH)
	{
		// Sata is in SATA_NCQ_ACTIVE or SATA_NCQ_FLUSH state
		if (sata_frame_int_0 & DMASETUPI)
		{
			
			if (sobj_curScm != SCM_NULL)
			{
				if ( sata_int_status_0 & (RXDATARLSDONE|DATATXDONE) )
					goto sata_isr_ncq_chk_tr_done;
#if 0
				ERR(("DMASETUP but sobj_curScm exist %bx %bx %bx\n", sobj_State, sobj_curScm, *sata_RxTag));
				//dump_reg();
				*usb_DevCtrlClr_0 = USB_ENUM;
				while (1)
				{
				}
#else
				// wait for data transfer done interrupt (RXDATARLSDONE/DATATXDONE) 
				MSG(("DMASET1 %bx\n", sobj_curScm));
				//dump_reg();
				return;
#endif
			}
//			u8	scm_site;
			//DBG(("DMASetup\n", *sata_RxTag));
			//DBG(("TCNT: %BX%BX%BX%BX\n", *sata_TCnt_3, *sata_TCnt_2, *sata_TCnt_1, *sata_TCnt_0));
			scm_site = *sata_RxTag & 0x1F;   	  // we need get from sata protocal:(rigester)
			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;


		#if 0
			//if ((sobj_sBusy & (1 << scm_site)) == 0)
			if ((sobj_sBusy1.udata.u_8[scm_site >> 3] & i2bit[scm_site & 0x07]) == 0)
			{
				//ERR("DMAsetup: tag:%lx sBusy:%lx\n", sobj_sBusy1.udata.u_32);
			}
		#endif

		#if 0
			if ((SCM_prot & 0x0C) != 0x80)
			{
				ERR("DMAsetup: %bx\n", SCM_prot);
				*usb_DevCtrlClr_0 = USB_ENUM;
				while (1)
					;
			}
		#endif

			//DBG("DMA setup\n");
			if((segIndex = dbuf_GetIdleSeg()) == DBUF_SEG_NULL)
			{
				// no Dbuffer 0/1 avaiable & just wait
				return;
			}
			// free 
			//
			*sata_FrameInt_0 = DMASETUPI;
			*sata_CurrTag = scm_site;

			SCM_SegIndex = segIndex;

			// current Active SATA command with CCM & SCM
			sobj_curScm = scm_site;

			//DBG("\n\nDmaSetup:  Seg: %lx  sataTag: %lx  usbTag: %lx", segIndex, cur_tag, pScm->pCdbCtxt->CTXT_ITAG);

			ctxt_site = SCM_CdbIndex;
			if (ctxt_site == CTXT_NULL)
			{	// no associated ctxt site
				// abort Data xfer
				*dbuf_MuxSel = segIndex;
				if ((SCM_prot) == PROT_FPDMAIN)
				{
					*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_SATA0_W_PORT;
				}
				else
				{
					*dbuf_MuxInOut = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_NULL;
				}
				*dbuf_MuxCtrl = SEG_DONE;
				//DBG0(("SCM abted %BX\n", scm_site));
			}
			else
			{	// vaid associated ctxt site
				//*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;
				if ((SCM_prot) == PROT_FPDMAIN)
				{	
					// rout Data From Sata to Dbuffer 

					*dbuf_MuxSel = segIndex;
					*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_SATA0_W_PORT;

				
					SCTXT_DbufIndex = segIndex;
					SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_SATA0_W_PORT;

					usb_append_ctxt_que();
				}
				else
				{	
					SCTXT_DbufIndex = segIndex;
					SCTXT_DbufINOUT = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_USB_W_PORT;

			
					usb_append_ctxt_que();
				}	
			}
		}	// 	if (sata_frame_int_0 & DMASETUPI)

sata_isr_ncq_chk_tr_done:
		if ( sata_int_status_0 & RXDATARLSDONE )
		{	// Data-in Done  for NCQ command
sata_isr_ncq_tr_done:

			//DBG("RXDATAELSDONE: teg: %BX\n", *sata_CurrTag);

			//DBG0(("SRD, tag:%BX\n", *sata_CurrTag));
			//DBG0(("TCNT: %BX%BX%BX%BX", *sata_TCnt_3, *sata_TCnt_2, *sata_TCnt_1, *sata_TCnt_0));
	
			// 1stly read sata_DataRxStat
			EAL = 0;
			tmp8 = *sata_DataRxStat_0;
			val = CACHE_BYTE3 & DRXPROTE;	// check RX "Protocol error" only
			EAL = 1;
			if (tmp8 || val)
			{
					MSG(("\tsRS:%BX%BX\n", val, tmp8));
			}

			if((scm_site = sobj_curScm) != SCM_NULL)
			{

				SCM_INDEX = scm_site;

				sobj_curScm = SCM_NULL;

				// any associated DBUF segment with this SCM
				if((segIndex = SCM_SegIndex) != DBUF_SEG_NULL)
				{	// disconnect input port only of DBUF segment
					SCM_SegIndex = DBUF_SEG_NULL;		

					*dbuf_MuxSel = segIndex;
					// disconnect input port
					*dbuf_MuxInOut &= 0xF0;
					*dbuf_MuxInOut;

				#ifndef SATA_AUTO_FIFO_RST
					*sata_BlkCtrl_1 = RXSYNCFIFORST; //reset sata RX  fifo
//					check_done_bit();
				#endif
				}

				// any associated USB CTXT with this SATA SCM
				if ((ctxt_site = SCM_CdbIndex) != CTXT_NULL)
				{
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;

					// break association between SCTXT & SCM
					SCTXT_CCMIndex = SCM_NULL;
					SCM_CdbIndex = CTXT_NULL;

					// save original SCTXT_Status 
					//i8 = SCTXT_Status;

					// check sata_DataRxStat_1 & sata_DataRxStat_0
					if (tmp8 | val)
					{	//something wrong on sata_DataRxStat register
						if (tmp8 & (DRXSYNCETX|DRXSYNCERX|DRXRERR))
						{	// set CRC error sense ccode into USB CTXT
							hdd_err_2_sense_data(ERROR_CRC_ERROR);
						}
						else
						{
							hdd_err_2_sense_data(ERROR_DATA_PHASE);
						}

						// check whether this USB CTXT is current active USB CTXT
						if (usb_curCtxt != ctxt_site)
						{	// if CTXT is not active. assume CTXT in USB USB CTXT Pending queue
							// change tobe  no data transfer
							SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);

							// any associated DBUF segment with this SCTXT
							if (SCTXT_DbufINOUT != DBUF_SEG_NULL)
							{	// release DBUF segment
								DBUF_SegFree(SCTXT_DbufINOUT);
								SCTXT_DbufINOUT = DBUF_SEG_NULL;
							}
						}
						sata_ncq_error_handling();
						return;
					}
					else
					{
						SCTXT_Status = CTXT_STATUS_GOOD;
					}

					//if (i8 == CTXT_STATUS_XFER_DONE)
					//{
					//	usb_tx_ctxt_send_status();
					//}
				}
			
			#if 0
				if (sobj_sBusy1.udata.u_32 == 0)
				{
							{
								sobj_State = SATA_READY;
								sata_exec_next_scm();
							}
				}
			#endif
			}

			*sata_CurrTag = 0xFF;			//must reset CurrTag
			*sata_IntStatus_0 = 0xFF;
			return;
		}	// if ( sata_int_status_0 & RXDATARLSDONE )

		if (sata_int_status_0 & DATATXDONE )
		{	// Data-out Done for NCQ command

			//DBG0(("STD, tag:%BX\n", *sata_CurrTag));
			if((scm_site = sobj_curScm) != SCM_NULL)
			{
				//*sata_CCMSITEINDEX = scm_site;
				SCM_INDEX = scm_site;

				// Any releated USB CDB
				if ((ctxt_site = SCM_CdbIndex) == CTXT_NULL)
				{
					//DBG(("sata_isr: ctxt NULL in DATATXDONE\n"));
					*sata_CurrTag = 0xFF;			//must reset CurrTag			
					*sata_IntStatus_0 = DATATXDONE;		
					sobj_curScm = SCM_NULL;

					return;
				}

				// Select CDB Context
				*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;

				if (SCTXT_Status != CTXT_STATUS_XFER_DONE)
				{
					DBG(("wait USB rx done\n"));
					return;
				}

			#if 1
				EAL = 0;
				tmp8 = *sata_DataTxStat_0;
				val = CACHE_BYTE1 & DTXPROTE;	// only check "Protocol error"
				EAL = 1;

				if (tmp8 | val)
				{
					//DBG0(("sTS:%BX%BX %BX\n", val, tmp8, scm_site));
					if (tmp8 & (DTXSYNCETX|DTXSYNCERX|DTXRERR))
					{	// Data transmission completed with R_ERRp
						hdd_err_2_sense_data(ERROR_CRC_ERROR);
					}
					else if (tmp8 & DTXUNDRN)
					{
						SCTXT_Status = CTXT_STATUS_PHASE;
					}
					else
					{
						hdd_err_2_sense_data(ERROR_DATA_PHASE);
					}
				#ifdef DEBUG_DP
					//DBG(("NCQ DO FAIL, sata_DataTxStat_0:%BX\n", *sata_DataTxStat_0));
					//dump_reg();
					//*usb_DevCtrlClr_0 = USB_ENUM;
					//while (1)
					//{
					//}
				#endif
				}
				else
				{
					SCTXT_Status = CTXT_STATUS_GOOD;
				}
			#endif
				*sata_CurrTag = 0xFF;			//must reset CurrTag			
				*sata_IntStatus_0 = DATATXDONE;		

				// break association from SCM to CTXT
				SCM_CdbIndex = CTXT_NULL;

				//no Current ACTIVE SCM
				sobj_curScm = SCM_NULL;

				if((segIndex = SCM_SegIndex) != DBUF_SEG_NULL)
				{
					usb_rx_fifo_rst();

					*dbuf_MuxSel = segIndex;
					SCM_SegIndex = DBUF_SEG_NULL;
					*dbuf_MuxStatus = SEG_IDLE;
					*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
					*dbuf_MuxInOut;
				}

				// break association from Ctxt to Scm
				SCTXT_CCMIndex = CCM_NULL;
				if (SCTXT_Status == CTXT_STATUS_PHASE)
				{	// 4.26
					//DBG(("Do un\n"));
					hdd_err_2_sense_data(ERROR_IU_TOO_SHORT);
					usb_rx_ctxt_send_status();

					sata_ncq_error_handling();
					return;
				}
				else
				{
					{	// I/O between USB & SATA only for NCQ command
//						if (SCTXT_Status == CTXT_STATUS_XFER_DONE)
						{
							//SCTXT_Status = CTXT_STATUS_GOOD;
							usb_rx_ctxt_send_status();
						}
					}
				}

				return;
			}

			*sata_CurrTag = 0xFF;			//must reset CurrTag			
			*sata_IntStatus_0 = DATATXDONE;
			return;
		}
			
		if (sata_frame_int_0 & SETDEVBITSI)
		{
			//FW can not get the original SActive from HD, 
			//so fetch the SETDEVBITS FIS from Receive FIFO
		#if 1
			if(sobj_curScm != SCM_NULL)
			{
				if ((sata_int_status_0 = *sata_IntStatus_0) & (DATATXDONE|RXDATARLSDONE) )
				{
					goto sata_isr_ncq_chk_tr_done;
				}
				else
				{
					MSG(("SETDEV1 %bx\n", sata_int_status_0));
					//dump_reg();
					return;
				}
			}
		#endif

			//*sata_CCMSITEINDEX = scm_site;
			SCM_INDEX = scm_site;
			// check 
			if (sata_FISRCV0[0] == SET_DEVICE_BITS_FIS )
			{	// Set Device Bits - Device to Host

				//DBG0(("SETDEVBITS\n"));
				{	// Error bit is not set

					//tmp32 = *((u32 *)&sata_FISRCV0[4]);
					//COPYU32_REV_ENDIAN_X2D(sata_FISRCV0, &tmp32);
					utmp32.udata.u_8[0] = sata_FISRCV0[4];
					utmp32.udata.u_8[1] = sata_FISRCV0[5];
					utmp32.udata.u_8[2] = sata_FISRCV0[6];
					utmp32.udata.u_8[3] = sata_FISRCV0[7];

					//if (tmp32)
					if (utmp32.udata.u_32)
					{
						//sata_DetachSCM(tmp32); //
						sata_DetachSCM();

						//DBG("\nSet Dev Bit=%lx  cFree:%lx\n", tmp32, sobj_cFree1.udata.u_32);
						sobj_sBusy1.udata.u_8[0] &= ~utmp32.udata.u_8[0];
						sobj_sBusy1.udata.u_8[1] &= ~utmp32.udata.u_8[1];
						sobj_sBusy1.udata.u_8[2] &= ~utmp32.udata.u_8[2];
						sobj_sBusy1.udata.u_8[3] &= ~utmp32.udata.u_8[3];
//						if ((sobj_sBusy &= (~tmp32)) == 0)
						if (sobj_sBusy1.udata.u_32 == 0)
						{
							if (sobj_curScm == SCM_NULL)
							{
								sobj_State = SATA_READY;
								//sata_exec_next_scm();
							}
						}
					}
				}

				if ((sata_FISRCV0[2] & B_BIT_16))
				{	// NCQ commands Fail
					sata_ncq_error_handling();
					MSG(("SETDEV2 F\n"));
					return;
				}	
				else
				{
					*sata_FrameInt_0 = SETDEVBITSI;
					if (sobj_State == SATA_READY)
					{
						sata_exec_next_scm();
					}
					return;
				}
			}	// 	if (sata_FISRCV0[0] == SET_DEVICE_BITS_FIS )
			else
			{
				//DBG0(("SDev %BX\n", *sata_Status));
				if ((*sata_Status & ATA_STATUS_CHECK))
				{
					sata_ncq_error_handling();
					return;
				}

			#ifdef DEBUG_DP
				dump_reg();
				*usb_DevCtrlClr_0 = USB_ENUM;
	
				while (1)
				{
				}
			#endif
			}
			*sata_FrameInt_0 = SETDEVBITSI;
			return;
		}

		if (sata_frame_int_0 & D2HFISI)
		{
			if ((*sata_Status & ATA_STATUS_CHECK) == 0)
			{
				*sata_FrameInt_0 = D2HFISI;
				return;
			}
			sata_ncq_error_handling();
			//*sata_FrameInt_0 = D2HFISI;
			return;
		}

	}	//  if (sobj_State >= SATA_NCQ_FLUSH)
	else
#endif		// UAS_EN
	if (sobj_State == SATA_ACTIVE)
	{

		if (sata_frame_int_0 & (SETDEVBITSI|DMASETUPI))
		{
			*sata_FrameInt_0 = (SETDEVBITSI|DMASETUPI);
			// clear SATA Frame interrupt
			//*sata_IntStatus_0 = FRAMEINTPEND;
		}

		// SATA non-NCQ command (Data-in) 
		if ( sata_int_status_0 & RXDATARLSDONE )
		{
			//DBG(("sata_isr: SATA_ACTIVE, RXDATARLSDONE\n"));
#if 0			
			if (*sata_DataRxStat_0)
			{
				DBG(("<R%X>", *sata_DataRxStat_0));
			}
#endif	
#ifdef DBG_ODD_U2
			if (dbg_flag)
			{
				//DBG0(("vSrxd\n"));
			}
#endif	


			if ((scm_site =sobj_curScm) != SCM_NULL)
			{
				//*sata_CCMSITEINDEX = scm_site;
				SCM_INDEX = scm_site;

				if (SCM_SegIndex != DBUF_SEG_NULL)
				{
					*dbuf_MuxSel = SCM_SegIndex;		//SCTXT_DbufIndex

					SCM_SegIndex = DBUF_SEG_NULL;		
				
					//DBG(("s_isr: SATA_ACTIVE, RXDATARLSDONE %BX\n", segIndex));
					//DBG(("\t: SATA_ISR Port: %LX -> %LX, %LX -> %LX\n", 
					//	dbuf_Port[5].dbuf_Port_Count, 
					//	dbuf_Port[9].dbuf_Port_Count,
					//	dbuf_Port[10].dbuf_Port_Count,
					//	dbuf_Port[4].dbuf_Port_Count
					//));

					// Disconnect input port
					*dbuf_MuxInOut &= 0xf0;
					*dbuf_MuxInOut;
					
#ifndef SATA_AUTO_FIFO_RST
					*sata_BlkCtrl_1 = RXSYNCFIFORST;	//reset sata RX  fifo	
					//*sata_BlkCtrl_1;
#endif
				}

				tmp8 =  *sata_Status;
				
#if 0//def DBG_ODD_U2
				if (dbg_flag)
				{
					DBG0(("vSs %bx cs %bx cf %bx\n", tmp8, SCM_CdbIndex, SCTXT_Flag));
				}
#endif	
				// wait for device is ready
				if (tmp8 & ATA_STATUS_BSY)
				{
					//DBG(("a Bsy"));
					if (sata_FISRCV0[0] != DEVICE2HOST_FIS)
					{	// D2HFIS is not received in FIS FIFO
						//DBG(("\n"));
						return;
					}

					// D2HFIS is received & not in ATA shadow registers
					// This means SATA state machine is stuck in error state

					// clr "RXDATARLSDONE|RXRLSDONE|DATARXDONE" interrupt bits,
					// so SATA state machine can move D2HFIS from FIS FIFO 
					*sata_IntStatus_0 = RXDATARLSDONE|RXRLSDONE|DATARXDONE;
					*sata_IntStatus_0;
					tmp8 =  *sata_Status;
				#if 0
					if (tmp8 & ATA_STATUS_BSY)
					{
						tmp8 =  *sata_Status;
						if (tmp8 & ATA_STATUS_BSY)
						{
						#ifdef BOT_TOUT
							EAL = 0;
							dump_reg();
							*usb_DevCtrlClr_0 = USB_ENUM;
							ET0 = 0;
							EAL = 1;

							while (1)
							{
							}
						#endif
						}
					}
				#endif
					//DBG(("\taStat %BX\n", tmp8));
				}

				//DBG(("sisr: SATA_ACTIVE, RXDATARLSDONE 1\n"));

				if ( (ctxt_site = SCM_CdbIndex) == CTXT_NULL)
				{
					// Local SATA command only
					if (tmp8 & ATA_STATUS_CHECK)
					{
						SCM_Fis_Error = *sata_Error;
						SCM_Fis_Staus = tmp8;
						SCM_Status = CCM_STATUS_ERROR;
					}
					else
					{
						SCM_Status = CCM_STATUS_GOOD;
					}
				}
				else
				{	// SATA Command has associated USB Command
					//DBG(("sata_isr: SATA_ACTIVE, RXDATARLSDONE %BX\n", tmp8));
#ifdef UAS_EN // for UAS+BOT only
					// break association from SCM to Ctxt
					SCM_CdbIndex = CTXT_NULL;
					// select CTXT
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;
#endif
					// break association from ctxt to scm
					SCTXT_CCMIndex = CCM_NULL;

					// keep orignial ctxt status
					i8 = SCTXT_Status;
					if (tmp8 & ATA_STATUS_CHECK)
					{
					
#ifdef DBG_ODD_U2
						if (dbg_flag)
						{
							//DBG0(("ckCond\n"));
						}
#endif	
						ctxt_FIS[FIS_STATUS] = tmp8;

		
						//*ctxt_Status = CTXT_STATUS_ERROR;
						SCTXT_Status = CTXT_STATUS_ERROR;

						if (sobj_class == DEVICECLASS_ATA)
						{
						#ifdef ATA_PASS_THROUGTH
							if (SCTXT_Flag & SCTXT_ATA_PASS)
							{
								sata_return_tfr();
								hdd_ata_return_tfr();					
							}
							else
						#endif
							{
								ctxt_FIS[FIS_ERROR] = *sata_Error;
								// generate SCSI Sense code
								hdd_ata_err_2_sense_data();
							}
						}
					}
					else
					{
					#ifdef ATA_PASS_THROUGTH
						if (SCTXT_Flag & SCTXT_FLAG_RET_TFR)
						{
							//ctxt_FIS[FIS_ERROR] = *sata_Error;
							sata_return_tfr();
							hdd_ata_return_tfr();					
						}					
						else
					#endif
						{
							//*ctxt_Status = CTXT_STATUS_GOOD;
							SCTXT_Status = CTXT_STATUS_GOOD;
						}
					}
					
					if ((SCTXT_Flag & SCTXT_FLAG_B2S) == 0)
					{
					
#ifdef DBG_ODD_U2
						if (dbg_flag)
						{
							//DBG0(("vNotB2s %bx\n", tmp8));
							
#if 1
								//DBG0(("uI:%BX%BX\n",  *usb_IntStatus_shadow_1, *usb_IntStatus_shadow_0));	
							
								//DBG0(("mI:%BX%BX\n", *usb_Msc0IntStatus_1, *usb_Msc0IntStatus_0));
								//DBG0(("mTCt:%BX%BX\n", *usb_Msc0TxCtxt_1, *usb_Msc0TxCtxt_0));
								//DBG0(("mTXC:%BX%BX%BX\n", *usb_Msc0TxXferCount_2, *usb_Msc0TxXferCount_1, *usb_Msc0TxXferCount_0));
								//DBG0(("mTXBCNT:%BX%BX\n", *usb_Msc0_TXBCNT_1, *usb_Msc0_TXBCNT_0));
								//MSG(("mDICtr:%BX\n", *usb_Msc0DICtrlClr));
								//MSG(("mDIXSt:%BX\n", *usb_Msc0DIXferStatus));
								//MSG(("mDISt:%BX\n", *usb_Msc0DIStatus));

								*dbuf_MuxSel = SCTXT_DbufIndex;
										DBG0(("dUR %BX %BX%BX ",
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_3,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_1,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_0
										));
					
										DBG0(("%BX%BX\n",
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_1,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_0
										));
										DBG0(("dSW %BX %BX%BX ",
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_3,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_1,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_0
										));

										DBG0(("%BX%BX\n",
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_1,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_0
										));
#endif
						}
#endif	
						//if (tmp8 == CTXT_STATUS_XFER_DONE)
						if (SCTXT_Status == CTXT_STATUS_XFER_DONE)
						{
							usb_tx_ctxt_send_status();
						}
					}
#ifdef USB2_DI_PATCH	// USB2.0 patch
					else	//if ((SCTXT_Flag & SCTXT_FLAG_B2S))
					{
						if ((SCTXT_Flag & SCTXT_FLAG_U2B))
						{
							if (usb_curCtxt != CTXT_NULL)
							{
								ERR(("sI: Rls U2B Bsy %BX\n", usb_curCtxt)); 
								return;
							}
							else
							{
								if (SCTXT_Status == CTXT_STATUS_ERROR)
								{
									if (SCTXT_DbufIndex != DBUF_SEG_NULL )
									{
#ifdef DBG_ODD_U2
										DBG0(("B2S_error!!!!!"));				
										*dbuf_MuxSel = SCTXT_DbufIndex;
										DBG0(("dUR %BX %BX%BX ",
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_3,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_1,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_0
										));
					
										DBG0(("%BX%BX\n",
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_1,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_0
										));
										DBG0(("dSW %BX %BX%BX ",
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_3,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_1,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_0
										));

										DBG0(("%BX%BX\n",
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_1,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_0
										));
#endif
									
										//sss DBUF_SegFree(SCTXT_DbufIndex);
										*dbuf_MuxSel = SCTXT_DbufIndex;
										*dbuf_MuxCtrl = SEG_RESET;
										*dbuf_MuxCtrl;

										*usb_Msc0DICtrlClr	= MSC_TXFIFO_RST;
										*usb_Msc0DICtrlClr;

										*dbuf_MuxCtrl = SEG_RESET;
										*dbuf_MuxCtrl;
										
										*dbuf_MuxStatus = SEG_IDLE;
										*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
										*dbuf_MuxInOut;
#ifdef DBG_ODD_U2
										DBG0(("B2S_error!!!!!"));				
										*dbuf_MuxSel = SCTXT_DbufIndex;
										DBG0(("dUR %BX %BX%BX ",
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_3,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_1,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_0
										));
					
										DBG0(("%BX%BX\n",
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_1,
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_0
										));

										DBG0(("dSW %BX %BX%BX ",
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_3,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_1,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_0
										));

										DBG0(("%BX%BX\n",
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_1,
										dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_0
										));
										
#endif

										SCTXT_DbufIndex = DBUF_SEG_NULL;
									}

									usb_device_no_data();
								}
								else	//if (SCTXT_Status == CTXT_STATUS_GOOD)
								{
#ifdef DBG_ODD_U2
									//DBG0(("B2S GOOD\n"));				
#endif
								
									if (SCTXT_DbufIndex == DBUF_SEG_NULL)
									{
										usb_device_no_data();
									}
									else	//if (SCTXT_DbufIndex != DBUF_SEG_NULL )
									{
										*dbuf_MuxSel = SCTXT_DbufIndex;
										*dbuf_MuxInOut = (TX_DBUF_USB_R_PORT << 4);
										val = dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_0;
										if ((16 > val) && (val > 8))
										{
											dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_0 = 16;
										}
									}
									//SCTXT_Status = CTXT_STATUS_PENDING;
									usb_exec_tx_ctxt();
								}
							}
						}	//if ((SCTXT_Flag & SCTXT_FLAG_U2B))
					}	//if ((SCTXT_Flag & SCTXT_FLAG_B2S))
#endif

				}
				sobj_curScm = SCM_NULL;

				//sata_DetachSCM(1 << scm_site); //
				sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

				sobj_State = SATA_READY;
				sata_exec_next_scm();
			}	//	if ((scm_site =sobj_curScm) != SCM_NULL)


			*sata_FrameInt_0 = D2HFISI;
			*sata_IntStatus_0 = 0xff;
			return;
		}

		// SATA non-NCQ command (Data-out) 
		if ( sata_int_status_0 & DATATXDONE )
		{
			//DBG(("<W %X>", *sata_DataTxStat_0));

			if ((scm_site = sobj_curScm) != SCM_NULL)
			{
				//*sata_CCMSITEINDEX = scm_site;
				SCM_INDEX = scm_site;
			#if 1
				EAL = 0;
				tmp8 = *sata_DataTxStat_0;
				val = CACHE_BYTE1 & DTXPROTE;	// only check "Protocol error"
				EAL = 1;
			#endif
				ctxt_site = SCM_CdbIndex;
				if (ctxt_site != CTXT_NULL)
				{	// has associated USB command
					SCTXT_INDEX = ctxt_site;
					// wait for USB xfer done
					if (SCTXT_Status != CTXT_STATUS_XFER_DONE)
					{
					//#ifdef DEBUG_DP
					#if 0
						if (tmp8 | val)
						{
							DBG0(("\tsTS:%BX%BX\n", val, tmp8));
							if ((*usb_Msc0IntStatus_0 & MSC_RX_DONE_INT) == 0)
							{
								dump_reg();
								*usb_DevCtrlClr_0 = USB_ENUM;
								while (1)
									;
							}
						}
					#endif
						return;
					}
				}
				// check connection of DBUF
				if ((segIndex = SCM_SegIndex) != DBUF_SEG_NULL)
				{
					*dbuf_MuxSel = segIndex;		//SCTXT_DbufIndex;
				
					if (!(SCTXT_Flag & SCTXT_FLAG_B2S))
					{
#ifndef SATA_AUTO_FIFO_RST
						*sata_BlkCtrl_1 = TXSYNCFIFORST;	//reset sata TX  fifo
						*sata_BlkCtrl_1;
#endif
						// Reset on USB RX FIFO will propagate to DBUFF & 
						// it is not necssary to reset DBUFF again
						usb_rx_fifo_rst();
					}
					else
					{	// If CPU is source, F/W has to reset DBUFF.
						*dbuf_MuxCtrl = SEG_RESET;	//3609
						*dbuf_MuxCtrl;
					}

					*dbuf_MuxStatus = SEG_IDLE;
					// Disconnect both input & output port
					//DBUF_SegFree(segIndex);
					*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
					*dbuf_MuxInOut;
				}

				// Get Sata Status
				// wait for device is ready
					if (*sata_Status & ATA_STATUS_BSY)
				{
					DBG(("DO a bsy"));
					return;
				}

				if ((ctxt_site = SCM_CdbIndex) == CTXT_NULL)
				{
					// Local SATA command only
					//*sata_IntStatus_0 = DATATXDONE;
					tmp8 =  *sata_Status;
					if (tmp8 & ATA_STATUS_CHECK)
					{
						SCM_Fis_Error = *sata_Error;
						SCM_Fis_Staus = tmp8;

						SCM_Status = CCM_STATUS_ERROR;
					}
					else
					{
						SCM_Status = CCM_STATUS_GOOD;
					}
				}
				else
				{	// SATA Command has associated USB Command

					// select CTXT
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;

					//if (*ctxt_Status != CTXT_STATUS_XFER_DONE)
					if (SCTXT_Status != CTXT_STATUS_XFER_DONE)
					{
						//DBG(("wait USB rx done\n"));
	#if 0					
						dbuf_dbg(DIR_WRITE, 1);
						ERR(("usb_IntStatus:%BX\n", *usb_IntStatus_shadow_0));
						ERR(("usb_Msc0DOutCtrl:%BX\n", *usb_Msc0DOutCtrl));
						*usb_USB3StateSelect = 0x0A;
						ERR(("usb_USB3StateCtrl:%BX\n", *usb_USB3StateCtrl));
						
						ERR(("usb_Msc0IntStatus:%BX\n", *usb_Msc0IntStatus_0));

						while (1)
						{
						}	
	#endif
						return;
					}
					
					// break association from ctxt to scm
					SCTXT_CCMIndex = CCM_NULL;

					
#ifdef UAS_EN // for UAS+BOT only
					// break association from SCM to Ctxt
					SCM_CdbIndex = CTXT_NULL;
#endif

				#if 1
					if (tmp8 | val)
					{
						//DBG0(("\tsTS:%BX%BX\n", val, tmp8));
						hdd_err_2_sense_data(ERROR_CRC_ERROR);
					}
					else
				#endif
					{
						tmp8 =  *sata_Status;
					if (tmp8 & ATA_STATUS_CHECK)
					{
						val = *sata_Error;
						ctxt_FIS[FIS_STATUS] = tmp8;
						ctxt_FIS[FIS_ERROR] = val;

						//*ctxt_Status = CTXT_STATUS_ERROR;
						SCTXT_Status = CTXT_STATUS_ERROR;
						if (sobj_class == DEVICECLASS_ATA)
						{
						#ifdef ATA_PASS_THROUGTH
							if (SCTXT_Flag & SCTXT_ATA_PASS)
							{
								sata_return_tfr();
								hdd_ata_return_tfr();					
							}
							else
						#endif
							{
								// generate SCSI Sense code
								hdd_ata_err_2_sense_data();
							}
						}
					}
				#ifdef ATA_PASS_THROUGTH
					else if (SCTXT_Flag & SCTXT_FLAG_RET_TFR)
					{
						//val = *sata_Error;
						//ctxt_FIS[FIS_STATUS] = tmp8;
						//ctxt_FIS[FIS_ERROR] = val;
						sata_return_tfr();
						hdd_ata_return_tfr();
					}					
						else
					#endif
						{
							SCTXT_Status = CTXT_STATUS_GOOD;
						}
					//  send status back to USB Host
					//timer0_unload();
					}
					usb_rx_ctxt_send_status();
				}

				// everthing is ok for sata_isr\() to process DATATXDONE interrupt
				sobj_curScm = SCM_NULL;

				//sata_DetachSCM(1 << scm_site); //
				sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

				sobj_State = SATA_READY;
				sata_exec_next_scm();
			}

			*sata_FrameInt_0 = D2HFISI;
			*sata_IntStatus_0 = (DATATXDONE|FRAMEINTPEND);
			return;
		}

		// SATA non-NCQ command (No Data) 
		if (sata_frame_int_0 & D2HFISI)
		{			
//DBG0(("D2H "));
			if ((scm_site = sobj_curScm) != SCM_NULL)
			{
//DBG0(("v_s "));
				//*sata_CCMSITEINDEX = scm_site;
				SCM_INDEX = scm_site;

				sobj_curScm = SCM_NULL;

				//*sata_Ccm_cmdinten_0 = 0;

				// Get Sata Status
				tmp8 =  *sata_Status;

				if ((ctxt_site = SCM_CdbIndex) == CTXT_NULL)
				{
					// Local SATA command only
					*sata_FrameInt_0 = D2HFISI;

					if (tmp8 & ATA_STATUS_CHECK)
					{
						SCM_Fis_Error = *sata_Error;
						SCM_Fis_Staus = tmp8;
						SCM_Status = CCM_STATUS_ERROR;
					}
					else
					{
						SCM_Status = CCM_STATUS_GOOD;
					}

					return;
				}
				else
				{
//DBG0(("v_c %bx ", ctxt_site));
					// break association from SCCM/SCM to CTXT/SCTXT
					SCM_CdbIndex = CTXT_NULL;

					// SATA Command has associated USB Command
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;
					
					// break association from CTXT/SCTXT to SCCM/SCM
					SCTXT_CCMIndex = CCM_NULL;
					
					tmp8 =  *sata_Status;

					if (tmp8 & ATA_STATUS_CHECK)
					{
						val =  *sata_Error;

						ctxt_FIS[FIS_STATUS] = tmp8;
						ctxt_FIS[FIS_ERROR] = val;
						
						//FIXME: NO_MEDIA ODD go here, if not set, status keep PENDING
						SCTXT_Status = CTXT_STATUS_ERROR;
	
						if ((SCTXT_Flag & SCTXT_FLAG_B2S) == 0)
						{
							if (ctxt_CDB[0] == SCSI_SEND_DIAGNOSTIC)
							{
								hdd_err_2_sense_data(ERROR_SELFTEST_FAIL);
							}
							else
							{
								// generate SCSI Sense code
								hdd_ata_err_2_sense_data();
							}
						}
					}
					else
					{
					#ifdef ATA_PASS_THROUGTH
						if (SCTXT_Flag & SCTXT_FLAG_RET_TFR)
						{
							sata_return_tfr();
							hdd_ata_return_tfr();	
						}					
						else
					#endif
						{
							SCTXT_Status = CTXT_STATUS_GOOD;
							//if (SCTXT_Flag & SCTXT_FLAG_STANDBY)
							//{
							//	sobj_State = SATA_STANDBY;
							//}
						}
					}

					if (SCTXT_Flag & SCTXT_FLAG_U2B)
					{
//DBG0(("s st\n"));
						//usb_append_ctxt_que();
						usb_device_no_data();
					}
					else if (!(SCTXT_Flag & SCTXT_FLAG_B2S))
					{
//						usb_tx_ctxt_send_status(pCtxt);
						MSG(("D2HFISI with no Data between USB & SATA\n"));
					}
				}

				//sata_DetachSCM(1 << scm_site); //
				sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

				sobj_State = SATA_READY;
				sata_exec_next_scm();
			}
			*sata_FrameInt_0 = D2HFISI;

			// clear SATA Frame interrupt
			//*sata_IntStatus_0 = FRAMEINTPEND;
			#ifndef NO_RETURN
			return;
			#endif
		}

#if 1	//debug
		if ( sata_int_status_0 & CMDTXERR )
		{
			//DBG0(("CMDTXERR %bx\n", *sata_CTagXmtStat));
			*sata_IntStatus_0 = CMDTXERR;

			if ((scm_site = sobj_curScm) != SCM_NULL)
			{
				//*sata_CCMSITEINDEX = scm_site;
				SCM_INDEX = scm_site;

				if ((ctxt_site = SCM_CdbIndex) == CTXT_NULL)
				{
					SCM_Status = CCM_STATUS_ERROR;
					return;
				}
				else
				{
					// break association between Ctxt & Ccm
					SCM_CdbIndex = CTXT_NULL;

					// SATA Command has associated USB Command
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;
					
					//FIXME: place here make sense?
					SCTXT_CCMIndex = CCM_NULL;
					

					tmp8 =  *sata_Status;

					ctxt_FIS[FIS_STATUS] = ATA_STATUS_DRDY|ATA_STATUS_DSC|ATA_STATUS_CHECK;
					ctxt_FIS[FIS_ERROR] = ATA_ERR_ICRC;
						
					//FIXME: NO_MEDIA ODD go here, if not set, status keep PENDING
					SCTXT_Status = CTXT_STATUS_ERROR;
	
					if ((SCTXT_Flag & SCTXT_FLAG_B2S) == 0)
					{
						// generate SCSI Sense code
						hdd_ata_err_2_sense_data();
					}

					if (SCTXT_Flag & SCTXT_FLAG_U2B)
					{
						//usb_append_ctxt_que();
						usb_device_no_data();
					}
				}
				//sata_DetachSCM(1 << scm_site); //
				sobj_cFree1.udata.u_8[scm_site >> 3] |= i2bit[scm_site & 0x07];

				sobj_State = SATA_READY;
				sata_exec_next_scm();
			}
		}	//if ( sata_int_status_0 & CMDTXERR )
#endif
	}	// else  if (sobj_State == SATA_ACTIVE)
	else if (sobj_State == SATA_RESETING)
	{
		if (*sata_PhyInt & PHYRDYI)
		{
			// 	Enable D2HFIS Interrupt for Signature FIS
			*sata_FrameIntEn_0 |= D2HFISIEN;

			// clear SATA PHY Ready & Down interrupt
			*sata_PhyInt = (PHYDWNI|PHYRDYI);

			if (!(*sata_PhyStat & PHYRDY))
			{
				MSG(("S_R: Phy NOT READY%BX %BX\n", *sata_PhyStat, *sata_PhyInt));
				return;
			}
			// Disable SATA PHY Ready Interrupt
			// Enable SATA PHY Down Interrupt
			*sata_PhyIntEn &= (*sata_PhyIntEn & (~PHYRDYIEN)) | PHYDWNIEN;

			// clear SATA PHY Ready & Down interrupt
			//*sata_PhyInt = (PHYDWNI|PHYRDYI);

			// clear SATA PHY interrupt
			//*sata_IntStatus_0 = PHYINTPEND;

			// Enable SATA PHY Down Interrupt
			//*sata_PhyIntEn |= PHYDWNIEN;


			sobj_State = SATA_PHY_RDY;
			sobj_State_tout = SATA_PRDY_TOUT;
			if (sobj_init == 0)
				sobj_class = DEVICECLASS_NONE;

			//DBG0(("SATA RESETING:%BX %BX\n", *sata_IntStatus_0, *sata_FrameInt_0));
			//FIXME: reset interrupt again
			// clear all interuupt registers of SATA IP
			*sata_IntStatus_0 = 0xCC;
			//DBG0(("SATA RESETING2:%BX %BX\n", *sata_IntStatus_0, *sata_FrameInt_0));
		}
		else
		{
			ERR(("S_RESETING?:%BX %BX\n", *sata_IntStatus_0, *sata_FrameInt_0));
			*sata_IntStatus_0 = 0xFF;
		}
	}
	else if (sobj_State == SATA_PHY_RDY)
	{
		if (*sata_FrameInt_0 & D2HFISI)
		{
			tmp8 =  *sata_Status;

			// Disable D2HFIS Interrupt
			*sata_FrameIntEn_0 &= ~D2HFISIEN;

			// clear D2HFIS interrupt
			*sata_FrameInt_0 = D2HFISI;

			// clear SATA Frame interrupt
			//*sata_IntStatus_0 = FRAMEINTPEND;

	#ifdef USB_FAST_ENUM
			if (sobj_init)
	#endif
			{
				sata_get_class();
				sobj_State = SATA_READY;
				//DBG0(("SAT R\n"));
				// any pending SATA command ?
				sata_exec_next_scm();
			}
	#ifdef USB_FAST_ENUM
			else
			{
				if ((tmp8 & ATA_STATUS_BSY)  == 0 )
				{
					if ((tmp8 & ATA_STATUS_CHECK) == 0 )
					{
						sata_get_class();
						//DBG0(("DRDY\n"));
						sobj_State = SATA_DRV_RDY;

						//sobj_Timeout = 1;				// Enable SATA_DRV_RDY_TOUT
						return;
					}
					// reset timout value
					sobj_init_tout = 0;
				}
				else
				{
					ERR(("S Sts %bx\n", tmp8)); 
					// Enable D2HFIS Interrupt
					*sata_FrameIntEn_0 |= D2HFISIEN;
				}
			}
	#endif
		}
		else
		{
			ERR(("S_PhyRdy?:%BX %BX\n", *sata_IntStatus_0, *sata_FrameInt_0));
			*sata_IntStatus_0 = 0xFF;
		}
	}

#ifdef UAS_EN
	if (!bot_mode)
	{
		if (sobj_State == SATA_READY)
		{
			if ( *sata_IntStatus_0 & RXDATARLSDONE )
			{
				goto sata_isr_ncq_tr_done;
			}
		}
	}
#endif

	if (sata_int_status_0 & PHYINTPEND )
	{
		if (*sata_PhyInt & PHYDWNI)
		{
			if (bot_mode)
			{
				sata_abort_bot_io();
			}
		#ifdef UAS_EN
			else
			{
				sata_ncq_error_handling();
			}
		#endif

			sata_HardReset();
			sata_InitSataObj();
			return;
		}
	}

	if (*chip_IntStaus_0 & SATA0_INT)
	{
		if (sataIsrCnt++ < 2)
		{
			goto sata_isr_begin;
		}
		else
		{
			sataIsrCnt = 0;
			//ERR(("SI:sobj_State=%BX %BX %BX %BX\n", sobj_State,*sata_IntStatus_0, *sata_FrameInt_0, *sata_PhyInt));
			ERR(("SI:%BX %BX %BX\n", sobj_State, *sata_IntStatus_0, *sata_FrameInt_0));
#if 0
//#ifdef DEBUG_DP
			dump_reg();
			*usb_DevCtrlClr_0 = USB_ENUM;
			while (1)
				;
#endif
		}
	}

#if 0	
	else
	{
		if (*sata_FrameInt_0 & D2HFISI)
		{
			ERR(("unknow D2H FIS!\n"));
			// Disable D2HFIS Interrupt
			*sata_FrameIntEn_0 &= ~D2HFISIEN;

			// clear D2HFIS interrupt
			*sata_FrameInt_0 = D2HFISI;

			// clear SATA Frame interrupt
			//*sata_IntStatus_0 = FRAMEINTPEND;
		}
	}
#endif
	//DBG(("sata_isr done\n"));
}

/**************************************************************\
	sata_exec_internal_ccm()
		Execute a internal SATA non-NCQ command to SATA OBJ

	Input Global Parameter:
		scm_site: site # of SCM for SATA OBJ 
		sata_CCMSITEINDEX: site # of SCM for SATA OBJ

\*************************************************************/
u8 sata_exec_internal_ccm()
{
	//DBG(("SI "));

	//FIXME: assume always allocate TAG=0
	//scm_site = 0;

	// get SATA PRotocol from SCM
	//ccm_prot = SCM_prot & 0x0F;

	SCM_CdbIndex = CTXT_NULL;

	sata_exec_scm();
	while (1)
	{
		if ((SCM_prot == PROT_PIOIN) || (SCM_prot == PROT_DMAIN))
		{
			if ( *sata_IntStatus_0 & RXDATARLSDONE )
			{
				//FIXME: for DBG
#if 0				
				if ((*sata_TCnt_0|*sata_TCnt_1|*sata_TCnt_2) != 0)
				{
					DBG0(("TCNT not ZERO!\n"));
					//timer0_unload();
					*usb_DevCtrlClr_0 = USB_ENUM;
					dump_reg();
					while (1)
					{
					}
				}
#endif

				*sata_IntStatus_0 = RXDATARLSDONE|RXRLSDONE|DATARXDONE;	// Write 1 clear
				//if ((tmp8 = SCTXT_DbufIndex) != DBUF_SEG_NULL)
				if ((segIndex = SCM_SegIndex) != DBUF_SEG_NULL)
				{
					*dbuf_MuxSel = segIndex;		//SCTXT_DbufIndex;
					// Disconnect input port
					*dbuf_MuxInOut &= 0xf0;		//3609
					*dbuf_MuxInOut;

#ifndef SATA_AUTO_FIFO_RST
					*sata_BlkCtrl_1 = RXSYNCFIFORST;	//reset sata RX  fifo
					//*sata_BlkCtrl_1;
#endif
				}
				break;
			}
		}
		else if ((SCM_prot == PROT_PIOOUT) || (SCM_prot == PROT_DMAOUT))
		{
			if (*sata_IntStatus_0 & DATATXDONE )
			{
				//DBG(("sata_exec_ctxt: while, OUT done\n"));

				*sata_IntStatus_0 = DATATXDONE;	
#ifndef SATA_AUTO_FIFO_RST
				*sata_BlkCtrl_1 = TXSYNCFIFORST;  //reset sata TX fifo
				//*sata_BlkCtrl_1;
#endif
				break;
			}
		}
		else
		{
			if (*sata_FrameInt_0 & D2HFISI)
			{
				*sata_FrameInt_0= D2HFISI;

				// clear SATA Frame interrupt
				//*sata_IntStatus_0 = FRAMEINTPEND;
				
				*sata_Ccm_cmdinten_0 = 0;

				break;
			}
		}
#if 1
		if (*sata_IntStatus_0 & CMDTXERR )
		{
			//DBG0(("SI CMDTXERR %bx\n", *sata_CTagXmtStat));

			*sata_IntStatus_0 = CMDTXERR;

			sobj_State = SATA_NO_DEV;

			SCM_Status = CCM_STATUS_ERROR;
			return CCM_STATUS_ERROR;
		}
#endif
		//if (SCM_prot != PROT_ND)
		if (usb_active)
		{
			if ((tmp8 = *usb_IntStatus_shadow_0) & (HOT_RESET|WARM_RESET|USB_BUS_RST))
			{
#ifdef EQ_SETTING_TEST
				if(usbMode == CONNECT_USB3)
				{
					if(usb_30_bus_rst < 0xFF)
						usb_30_bus_rst++;
				}
				else
				{
					if(usb_20_bus_rst < 0xFF)
						usb_20_bus_rst++;
				}
#endif

//DBG0(("USB Rst %BX\n", tmp8));
#if 0
	#ifdef UAS_EN
				if (!bot_mode)
					uas_bus_reset();
				else
	#endif
					bot_usb_bus_reset();
#else
				sata_Reset(SATA_HARD_RST);				
#endif
				return CCM_STATUS_ERROR;
			}

		#if 0
			if ((*usb_IntStatus_shadow_1 & CTRL_INT) && (*usb_Ep0Ctrl & EP0_SETUP))
			{
				// in the SATA sync mode, there's possible that the USB setup is asserted
				usb_control();
				DBG0(("Set\n"));
			}
		#endif
		}

		if ((*sata_PhyStat & PHYRDY) == 0)
		{
			//DBG0(("S unplug\n"));
			return CCM_STATUS_ERROR;
		}
	}

	sobj_curScm = SCM_NULL;
	sobj_State = SATA_READY;

	sata_return_tfr();

	val = *sata_Status;
	//DBG(("icmd done %bx\n", val));
	
	{
		if (val & ATA_STATUS_CHECK)
		{
			SCM_Fis_Staus = val;
			SCM_Fis_Error = *sata_Error;
			SCM_Status = CCM_STATUS_ERROR;
			return CCM_STATUS_ERROR;
		}
		else
		{
			SCM_Status = CCM_STATUS_GOOD;
			return CCM_STATUS_GOOD;
		}
	}
}

/***********************************************************\
	sata_exec_dmae_RW_ctxt()
		Execute a SATA DMA Extended R/W command on SATA Obj 

	Input Global Parameter:
		ctxt_site: site # of CDB Context for BOT R/W command 
\***********************************************************/
u8 sata_exec_dmae_RW_ctxt()
{
	//FIXME: place here make sense?
	// BOT commmand always use #1 of SCM/CCM  
	SCTXT_CCMIndex = 1;
	scm_site = 1;

	//FIXME: assume always allocate TAG=1
	*sata_CCMSITEINDEX = 1;		// reserved CCM 1 for BOT Read/Write
	SCM_INDEX = 1;


	// set valid bit
	SCM_prot = *ctxt_SataProto >> 4;

	//AUTO_FIS no need to fill sata_Ccm_cmdinten
	//HW copy it from sata_SiteCmdIntEn_0~3
	//if (tmp8 = *ctxt_CCMcmdinten)
	//{
	//	*sata_Ccm_cmdinten_0 = tmp8;
	//}
	//*sata_Ccm_cmdinten_1 = 0;

	SCM_CdbIndex = ctxt_site;

//	SCM_Status = CCM_STATUS_PEND;

	SCM_SegIndex = SCTXT_DbufIndex;
	SCM_SegINOUT = SCTXT_DbufINOUT;
	
	//Start to copy FIS drom CDB context to SATA CCM
	*usb_Msc_FISDmaCtrl_0 = ctxt_site;
	
	//FIXME no need again
	//*usb_Msc_FISDmaCtrl_2 = MSC_FIS_ENABLE;

	//DBG0(("AUTO-FIS..."));
#if 1
	while (*usb_Msc_FISDmaCtrl_1 & MSC_FIS_BUSY)
	{
	}
#endif

	//DBG0(("Done\n"));
	//if (ccm_prot == PROT_PACKET_ND)
	//{
	//	*sata_SiteCmdIntEn_0 = 0;
	//}
	
	//ccm_prot = *ctxt_SataProto;
	sata_append_scm();

	hdd_start_act_led();

	//DBG(("\t %BX\n", *ctxt_No_Data));
	SCTXT_Status = CTXT_STATUS_PENDING;
	return CTXT_STATUS_PENDING;
}

/**************************************************\
 sata_exec_ctxt()
    Execute a SATA non-NCQ command to SATA OBJ

	Input Global Parameter:
		ctxt_site: site # of CDB Context
		SCTXT_INDEX: site # of CDB Context
		*host_ctxtmem_pt: site # of CDB Context
\**************************************************/
u8 sata_exec_ctxt()
{
#ifdef UAS_EN
	if ((bot_mode == 0) && (sobj_ncq_mode))
	{
		scm_site = sata_AllocateSCM();

		if (scm_site == SCM_NULL)
		{
			//SCM full 
			SCTXT_DbufIndex = DBUF_SEG_NULL;
			SCTXT_Status = CTXT_STATUS_ERROR;
			SCTXT_LunStatus = LUN_STATUS_TASK_SET_FULL;
			return CTXT_STATUS_ERROR;
		}
	}
	else
#endif
	{
		scm_site = 1;
		//DBG(("s_e_c %bx %bx %bx\n", SCTXT_Flag, SCTXT_DbufIndex, SCTXT_DbufINOUT));
	}
	// Select CCM/SCM
	*sata_CCMSITEINDEX = scm_site;
	SCM_INDEX = scm_site;

	//tmp8 = *ctxt_SataProto >> 4;
	SCM_prot = *ctxt_SataProto >> 4;
	val = *ctxt_CCMcmdinten;
	// check "ctxt_CCMcmdinten"
	if (val)
	{	//update "sata_SiteCmdIntEn_0" if non-zero
		*sata_SiteCmdIntEn_0 = val;
	}

	//SCTXT_Status = CTXT_STATUS_PENDING;
	SCTXT_CCMIndex = scm_site;


	//SCM_prot = tmp8;				// SATA Protocol
	SCM_CdbIndex = ctxt_site;
	SCM_Next = SCM_NULL;

	SCM_SegIndex = SCTXT_DbufIndex;
	SCM_SegINOUT = SCTXT_DbufINOUT;

	//Start of Auto FIS from CDB context to SATA CCM
	*usb_Msc_FISDmaCtrl_0 = ctxt_site;
	//*usb_Msc_FISDmaCtrl_2 = MSC_FIS_ENABLE;
#if 1
	while (*usb_Msc_FISDmaCtrl_1 & MSC_FIS_BUSY)
	{
	}
#endif

#if 1
	//dump_reg();	
	*sata_Ccm_xfercnt_0 = *ctxt_XferLen_0;
	*sata_Ccm_xfercnt_1 = *ctxt_XferLen_1;
	*sata_Ccm_xfercnt_2 = *ctxt_XferLen_2;
	*sata_Ccm_xfercnt_3 = *ctxt_XferLen_3;
	
#endif

	// append SCM to SATA non_ncq queue 
	sata_append_scm();

	SCTXT_Status = CTXT_STATUS_PENDING;
	SCTXT_LunStatus = LUN_STATUS_GOOD;

	// restore to zero
	*sata_SiteCmdIntEn_0 = 0;

	return CTXT_STATUS_PENDING;
}

/***************************************************\
 ata_exec_ncq_ctxt
   Execute a SATA NCQ command to seecified SATA OBJ

Input Global Parameter:
		ctxt_site: site # of CDB Context for UAS 
\**************************************************/
#ifdef UAS_EN
u8 sata_exec_ncq_ctxt()
{
	scm_site = sata_AllocateSCM();
	//DBG(("alloc:%BX\n", scm_site));

	if (scm_site == SCM_NULL)
	{
		//SCM full 
		//*ctxt_DBufIndex = DBUF_SEG_NULL;
#if 1
		SCTXT_Status = CTXT_STATUS_ERROR;
		SCTXT_LunStatus = LUN_STATUS_TASK_SET_FULL;
#endif
		return CTXT_STATUS_ERROR;
	}

	SCTXT_CCMIndex = scm_site;
	SCTXT_Status = CTXT_STATUS_PENDING;

	tmp8 = *ctxt_SataProto >> 4;

	// Select SCM
	*sata_CCMSITEINDEX = scm_site;		//
	
	*sata_SiteCmdIntEn_0 = DMASETUPIEN|SETDEVBITSIEN;
	SCM_INDEX = scm_site;

	SCM_prot = tmp8;				// SATA Protocol
	SCM_CdbIndex = ctxt_site;
	//*sata_Scm_site = scm_site;

	SCM_SegIndex = DBUF_SEG_NULL;
	//*sata_Scm_SegINOUT =0;

	//Start of Auto FIS drom CDB context to SATA CCM
	*usb_Msc_FISDmaCtrl_0 = ctxt_site;
	//*usb_Msc_FISDmaCtrl_2 = MSC_FIS_ENABLE;

#if 1
	while (*usb_Msc_FISDmaCtrl_1 & MSC_FIS_BUSY)
	{
	}
#endif

	// append SCM to SATA ncq queue 
	sata_append_ncq_scm(scm_site);

	hdd_start_act_led();

	SCTXT_Status = CTXT_STATUS_PENDING;
	SCTXT_LunStatus = LUN_STATUS_GOOD;

	//DBG(("site: %BX,\n", ctxt_site));
	return CTXT_STATUS_PENDING;
}
#endif

/****************************************\
	sata_HardReset
\****************************************/
void sata_HardReset()
{
	//DBG0(("s_Hst:%BX\n", sobj_State));

	if (sobj_State > SATA_READY)
	{
		Delay(1);
	}

	//	*sata_BlkCtrl |= (PROTBLKRST|TSPTBLKRST|PHYBLKRST);
#if 1
	if (!bot_mode)
	{
		*sata_BlkCtrl_1 |= SBLKRESET;
		Delay(1);
		sata_RegInit();

		*sata_FrameIntEn_0 = SETDEVBITSIEN|DMASETUPIEN;
		*sata_IntEn_0 = RXDATARLSDONEEN|DATATXDONEEN|FRAMEINTPENDEN|PHYINTPENDEN;
	}
#endif

	//spi_phy_wr_retry(PHY_SPI_SATA, 0x0D, 0x0B);
	//spi_phy_wr_retry(PHY_SPI_SATA, 0x0D, 0x00);

	// clear all interuupt registers of SATA IP 
	*sata_PhyInt = 0xFF;
	*sata_FrameInt_0 = 0xFF;
	*sata_FrameInt_1 = 0xFF;
	*sata_IntStatus_0 = 0xFF;
	*sata_IntStatus_1 = 0xFF;
	
	*sata_CurrTag = 0xFF;
	*sata_Status = 0xFF;

	// Disable SATA PHY not ready Interrupt
	// Enable SATA PHY ready Interrupt
	*sata_PhyIntEn = (*sata_PhyIntEn & ~PHYDWNIEN) | PHYRDYIEN;

	*sata_PhyCtrl_0 |= PHYRST;

	*sata_FrameIntEn_0 |= D2HFISIEN;

	*sata_SiteCmdIntEn_0 = 0;

	sobj_State = SATA_RESETING;
	sobj_State_tout = SATA_RESET_TOUT;
	
	//FIXME: force clear DATARXDONE after PHY RESET
	*sata_IntStatus_0 = DATARXDONE;
	
	//DBG0(("SR:%BX %BX\n", *sata_IntStatus_0, *sata_FrameInt_0));
	//Delay(1);
	//DBG0(("SR:%BX %BX\n", *sata_IntStatus_0, *sata_FrameInt_0));
}

/****************************************\
	sata_Reset
\****************************************/
void sata_Reset(u8 rst_cmd)
{
	switch (rst_cmd)
	{
		case USB_SATA_RST:
			if (sobj_State > SATA_READY)
			{	// sata obj is not in SATA_ACTIVE, SATA_NCQ_FLUSH, or SATA_NCQ_ACTIVE state
				sata_HardReset();
				sata_InitSataObj();
				return;
			}
			sata_InitSataObj();
			return;

		case SATA_HARD_RST:
			sata_HardReset();

			//sata_abort_all_usb_io();

			sata_InitSataObj();
			break;

		case SATA_SOFT_RST:
			*sata_DEVCTRL = DEVICE_CRTL_SRST;
			//Delay(1);
			// 	clear SATA Frame Interrupt
			*sata_FrameInt_0 = 0xff;
			*sata_FrameInt_1 = 0xff;

			// clear SATA interrupt
			*sata_IntStatus_0 = 0xff;
			*sata_IntStatus_1 = 0xff;
			// 	Enable D2HFIS Interrupt for Signature FIS
			*sata_FrameIntEn_0 |= D2HFISIEN;
			sobj_State = SATA_PHY_RDY;
			sobj_State_tout = SATA_PRDY_TOUT;

			*sata_DEVCTRL = 0;

			//sata_abort_all_usb_io();

			sata_InitSataObj();

			break;
	}
}

void sata_get_class()
{
	if ((*sata_SectCnt == 0x01) &&
		(*sata_LbaL == 0x01))
	{
		if ((*sata_LbaM == 0x14) &&
			(*sata_LbaH == 0xEB))
		{	// ATAPI device
			sobj_class = DEVICECLASS_ATAPI;
			DevicesFound = 1;
			//DBG(("Atapi\n"));
			return;
		}
		else if ((*sata_LbaM == 0x00) &&
				 (*sata_LbaH == 0x00))
		{
			sobj_class = DEVICECLASS_ATA;
			DevicesFound = 1;
//								sata_init = 1;
			//DBG(("ATA\n"));
			return;
		}
	}
}

/****************************************\
	scan_sata_device
\****************************************/
void scan_sata_device()
{
	//u8 ii, rty;
	u8 jj;
	u8 retrycount;
	//u8 count;

	//only power on allow longer time for sata2 phy checking 
	//sata2_retry_count = 5;		

	//make sure  turn on HDD power
	//reg_w32(GPIO_CTRL_DO, reg_r32(GPIO_CTRL_DO)|GPIO_HDDPOWER_SWITCH);	// turn off HDD power
	
	MSG(("scan sata\n"));

	DevicesFound = 0;
	retrycount = 0;
	//rty = 0;
	//retry 4 times
//retry:	
       //make sure this flag is set to 0xff
	*sata_Status = 0xFF;
	*sata_PhyInt = 0xFF;

	*sata_FrameIntEn_0 &= ~D2HFISIEN;
	*sata_FrameInt_0 = 0xFF;
	*sata_FrameInt_1 = 0xFF;
	*sata_IntStatus_0 = 0xFF; // clr 
	*sata_IntStatus_1 = 0xFF; // clr 
	
	sobj_class = DEVICECLASS_UNKNOWN;

#if 1
//	for (count=0; count<2; count++)
	{
		//*sata_PhyCtrl_0	&= ~(PHYPWRUP|PHYGEN2|PHYGEN1);
		//check gen1 or gen2
		//*sata_PhyCtrl_0	|= (PHYPWRUP|PHYGEN2|PHYGEN1);
#if 0		
		*sata_PhyCtrl_0	&= ~(PHYGEN2|PHYGEN1);
		*sata_PhyCtrl_3	&= ~PHYPWRUP;
		
		//FIXME: open G1 and G2 in chip, sisable G1 for DBG
		*sata_PhyCtrl_0	|= (PHYGEN2|PHYGEN1);
		*sata_PhyCtrl_3	|= PHYPWRUP;

		Delay(1);
#endif
_check_SATA_PHY_RDY:
	      //these loop take about 4 seconds (4 loop)
		for (jj = 0; jj < 4; jj++)
		{
			//SATA PHY reset
			sata_HardReset();

			// check if SATA Phy ready (OOB passed)
			for (i8= 0; i8 < 250; i8++)
			{	
				Delay(4);	
				if (*sata_PhyStat & PHYRDY)
				{
					MSG(("Sata phy ready\n"));
					goto check_sata;
				}
			}		
		}	
	}
	ERR(("\nSATA2 NOT READY\n"));
#endif	
//SATA1_PHY_CHECK:
    //make sure phy down
	//force to gen1
	//*sata_PhyCtrl_0	&= ~(PHYPWRUP|PHYGEN2|PHYGEN1);
	//*sata_PhyCtrl_0	|= (PHYPWRUP|PHYGEN1);
	//*sata_PhyCtrl_0	&= ~(PHYGEN2|PHYGEN1);
	*sata_PhyCtrl_3	= 0;
	
	*sata_PhyCtrl_0	= PHYGEN1;
	*sata_PhyCtrl_3	= PHYPWRUP;
	
	Delay(20);
	   
	for (jj = 0; jj < 10; jj++)
	{
		//SATA PHY reset
		sata_HardReset();

		// check if SATA Phy ready (OOB passed)
		for (i8= 0; i8 < 250; i8++)
		{	
			Delay(4);	
			if (*sata_PhyStat & PHYRDY)
			{
				MSG(("S Gen1 rdy\n"));
				goto check_sata;
			}
		}
		MSG(("."));
	}
	
	ERR(("SGen1 NRDY\n"));
	sobj_State = SATA_RESETING;
	return;

check_sata:

	sobj_State = SATA_PHY_RDY;

	MSG(("Chk FIS34\n"));

	sobj_class = DEVICECLASS_NONE;
	{
		// wait 2 seconds for FIS34 from SATA device
		
		for (tmp16= 0; tmp16 < 2000; tmp16++)
		{
			val = *sata_Status;
			//if ((val = *sata_Status) != 0xFF)
			{	
				// RX FIS 34
				// chk Busy Bit of TFR Status Register
				if ((val & ATA_STATUS_BSY) == 0)
				{	
					// chk Error Bit of TFR Status Register
					if ((val & ATA_STATUS_CHECK) == 0)
					{
						sata_get_class();
						sobj_State = SATA_READY;
					}
					goto exit;
				}
				
				if ((*sata_PhyStat & PHYRDY) == 0)
				{
					ERR(("PHY NRDY\n"));
					goto _check_SATA_PHY_RDY;
				}
			}

			//MSG(("."));
			Delay(1);
		}
		retrycount ++;
	}

	if((sobj_class == DEVICECLASS_NONE) && ( retrycount < 13))
		goto _check_SATA_PHY_RDY;
	if ( retrycount < 13)
	{
MSG(("\nOK!\n"));

		goto exit;
	}

#if 1
	*sata_FrameIntEn_0 |= D2HFISIEN;
	MSG(("Failed!\n"));
	return;
#else
	rty++;
	
    if(rty > 4)
   	{
		MSG(("\nScan Failed!\n"));
   	}
	else goto retry;
#endif

exit:
	*sata_FrameIntEn_0 &= ~D2HFISIEN;
	*sata_PhyInt = 0xFF;
	*sata_FrameInt_0 = 0xFF;
	*sata_FrameInt_1 = 0xFF;
	*sata_IntStatus_0 = 0xFF; // clr 
	*sata_IntStatus_1 = 0xFF; // clr 
		
	sobj_State = SATA_READY;

	// assume SATA device w/o NCQ
	return;
}

/****************************************\
	sata_RegInit
\****************************************/
void sata_RegInit()
{
	//DBG(("sata_init()\n"));

	*sata_PhyCtrl_0	= PHYGEN2|PHYGEN1;
	*sata_HoldLv = 0x40;
	if (usb_active)
	{
		if (usbMode == CONNECT_USB1)
		{
			*sata_HoldLv = 0x60;
		}
	}

	*sata_CurrTag	= 0xFF;
	*sata_EXQCtrl	= (EXQHSTRT|EXQNSTRT);
	//*sata_IntEn_0 	= 0x00;
	//*sata_IntEn_1 	= 0x00;
	
//	sata_CCMDIntEn = 0x0;

	// Disable auto reset of SATA BLK  TXSYNCFIFO & RXSYNCFIFO
#ifndef SATA_AUTO_FIFO_RST
	*sata_BlkCtrl_2  &= ~(DTXDDONEDIS|DRXSDONEDIS);
	//*sata_BlkCtrl_2;
#endif

#ifdef NCQ
	// if set BIT5, HW auto fill CurrTag with RXTag while CurrTag is 0xFF when HD send DMA Setup
	
	//FrameIntEn must enable, even for polling usage.
	*sata_FrameIntEn_0 = (SETDEVBITSIEN | DMASETUPIEN);
#endif //NCQ

	//*sata_Ccm_prot_3 = CCM_V;
	
	//set BIT_19 (not in spec)
	//*sata_PhyCtrl_2 |= 0x08;

	//if AUTO-FIS, fill bot ccm with 0
	*sata_SiteCmdIntEn_0 = 0;
	*sata_SiteCmdIntEn_1 = 0;
	*sata_SiteCmdIntEn_2 = 0;
	*sata_SiteCmdIntEn_3 = 0;

	*sata_Rsvd168	|=	(BIT_3|BIT_2|BIT_1|BIT_0); // fowllowing ASIC's instruction in 12/29/10 for compliance test

#if 1
	tmp8 = spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
	//DBG0(("sata_RegInit %BX %BX\n", tmp8, *sata_PhyCtrl_3));
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8|PD_RX);

	*sata_PhyCtrl_3 = PHYPWRUP;

	// power up SATA DRVR_T_MUX, SATA TX, & SATA RX  	
	tmp8 &= (~(PD_SATA_PLL|PD_DRVR_T_MUX|PD_TX|PD_RX));
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);
#else
	*sata_PhyCtrl_3 = PHYPWRUP;
#endif
}

void sata_set_cfree()
{
		sobj_cFree1.udata.u_8[1] = 0;
		sobj_cFree1.udata.u_8[2] = 0;
		sobj_cFree1.udata.u_8[3] = 0;

		tmp8 = sobj_qdepth;
		if (tmp8 < 8)
		{
			sobj_cFree1.udata.u_8[0] = i2mbit[tmp8];
		}
		else
		{
			sobj_cFree1.udata.u_8[0] = 0xFF;
			tmp8 -= 8;
			if (tmp8 < 8)
			{
				sobj_cFree1.udata.u_8[1] = i2mbit[tmp8];
			}
			else
			{
				sobj_cFree1.udata.u_8[1] = 0xFF;
				tmp8 -= 8;
				if (tmp8 < 8)
				{
					sobj_cFree1.udata.u_8[2] = i2mbit[tmp8];
				}
				else
				{
					sobj_cFree1.udata.u_8[2] = 0xFF;
					sobj_cFree1.udata.u_8[3] = i2mbit[tmp8-8];
				}
			}
		}
		
		//sobj_cFree1.udata.u_8[0] = sobj_default_cFree1.udata.u_8[0];				//bbit;
		//sobj_cFree1.udata.u_8[1] = sobj_default_cFree1.udata.u_8[1];				//bbit;
		//sobj_cFree1.udata.u_8[2] = sobj_default_cFree1.udata.u_8[2];				//bbit;
		//sobj_cFree1.udata.u_8[3] = sobj_default_cFree1.udata.u_8[3];				//bbit;
}

void sata_InitSataObj()
{
	//u8 scm_site;
	//DBG0(("sata_InitSataObj:%LX\n", sobj_default_cFree1.udata.u_32));

	//FIXME
	for(scm_site=0; scm_site<32; scm_site++)
	{	// select SCM
		SCM_INDEX = scm_site;

		SCM_CdbIndex = CTXT_NULL;
		SCM_Next = SCM_NULL;
	}
	sobj_que = SCM_NULL;
	sobj_ncq_que = SCM_NULL;
	sobj_curScm = SCM_NULL;

	//sobj_cFree1.udata.u_8[0] = sobj_default_cFree1.udata.u_8[0];
	//sobj_cFree1.udata.u_8[1] = sobj_default_cFree1.udata.u_8[1];
	//sobj_cFree1.udata.u_8[2] = sobj_default_cFree1.udata.u_8[2];
	//sobj_cFree1.udata.u_8[3] = sobj_default_cFree1.udata.u_8[3];
	sata_set_cfree();

	sobj_sBusy1.udata.u_8[0] = 0;
	sobj_sBusy1.udata.u_8[1] = 0;
	sobj_sBusy1.udata.u_8[2] = 0;
	sobj_sBusy1.udata.u_8[3] = 0;

	hdd_post_write = 0;

}


#ifdef PWR_SAVING

void sata_pll_pwr_up()
{	
// Power-up SATA PLL, SATA TX, & SATA RX
	// power up SATA PLL
	tmp8 = spi_phy_rd(PHY_SPI_SATA, 0x0D);
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8 & (~(PD_SATA_PLL)));

	// Dealy 1ms for SATA PLL ready
	//Delay(1);	

	// power up SATA DRVR_T_MUX, SATA TX, & SATA RX  	
	tmp8 &= (~(PD_SATA_PLL|PD_DRVR_T_MUX|PD_TX|PD_RX));

	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);

	// Dealy 1ms
	Delay(1);	

	// Disable ASIC Reference Clock
	*cpu_Clock_3 &= ~ASIC_PLL_BYPASS; // use 
	cpu_ref_clk = 0;

	#ifdef DBG_FUNCTION
		uart_init();
	#endif

	//DBG(("s_pl_on %BX\n", *cpu_Clock_2));

	// restore SATA RX/TX Impendence
	spi_phy_wr(PHY_SPI_SATA, 0x00, 0x48);
#else
	// power up SATA TX, & SATA RX  	
	tmp8 =	(~(PD_TX|PD_RX)) & spi_phy_rd(PHY_SPI_SATA, 0x0D);
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);
#endif
}

void sata_pwr_up()
{
	//DBG0(("s_pwr_up\n")); 
#if 1
	// Power on SATA HDD
	//if (DrvPwrMgmt)
	{
		power_up_hdd();
		Delay(10);
	}
#endif

	// Power-up SATA PLL, SATA TX, & SATA RX
	sata_pll_pwr_up();

	// Power UP SATA PHY
#if 0
	*sata_PhyCtrl_3 = PHYPWRUP;
	*sata_PhyIntEn = (*sata_PhyIntEn & ~PHYDWNIEN) | PHYRDYIEN;
#else
	sata_RegInit();
	*sata_FrameIntEn_0 |= D2HFISIEN;
	*sata_PhyIntEn = PHYRDYIEN;
	if (!bot_mode)
	{
		*sata_FrameIntEn_0 |= SETDEVBITSIEN|DMASETUPIEN;
	}
	*sata_IntEn_0 = RXDATARLSDONEEN|DATATXDONEEN|FRAMEINTPENDEN|PHYINTPENDEN;
#endif

	sobj_State = SATA_RESETING;
	sobj_State_tout = SATA_RESET_TOUT;
	//sobj_Timeout = 1;				// Enable SATA_PHY_RDY_TOUT 
	
	//hdd_post_write = 0;

	hdd_on_led();
}

#ifdef PWR_SAVING
void sata_pwr_down()
{
	//DBG(("s_pwr_dn\n"));
	if ((*cpu_Clock_2 & SATA0_PLL_RDY) == 0)
	{
		return;
	}

#if 0
	// Clear SATA Phy Interrupt
	*sata_PhyInt = 0xff;

	// Disabe SATA PHY Interrupt
	*sata_PhyIntEn = 0x00;

	// Clear SATA Interrupt
	*sata_IntStatus_0 = 0xFF;
	*sata_IntStatus_1 = 0xFF;

	// Disable SATA Interrupt
	*sata_IntEn_0 = 0x00;
	*sata_IntEn_1 = 0x00;

	// Power down SATA PHY
	*sata_PhyCtrl_3 = 0;		//*sata_PhyCtrl_3 &= (~PHYPWRUP);
	*sata_PhyCtrl_3;
#else
	*sata_BlkCtrl_1 = SBLKRESET;
	*sata_BlkCtrl_1;
#endif

	// Power off SATA HDD
	#ifdef POW_MGMT
	if (DrvPwrMgmt)
	#endif
		power_down_hdd();

#ifdef ASIC_CONF
	// Enable ASIC Reference Clock
	*cpu_Clock_3 |= ASIC_PLL_BYPASS; // use the external crystall clock, it's 25M in current PCBA	

	cpu_ref_clk = 1;
	#ifdef DBG_FUNCTION
		uart_init();
	#endif

	// 100ms tick interrupt
	//init_timer0_interrupt(100, CLOCK_DEFAULT);

	// Power-down SATA PLL, SATA TX, & SATA RX
	// Power-down SATA DRVR_T_MUX, SATA PLL, SATA TX, & SATA RX
	tmp8 =	(PD_SATA_PLL|PD_DRVR_T_MUX|PD_TX|PD_RX) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);	// SATA PLL power


#else
	// Power-down SATA TX, & SATA RX
	tmp8 =	(PD_TX|PD_RX) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);	// SATA PLL power	
#endif

	hdd_post_write = 0;

	sobj_State = SATA_PWR_DWN;
	//sobj_init = 0;
}
#endif
