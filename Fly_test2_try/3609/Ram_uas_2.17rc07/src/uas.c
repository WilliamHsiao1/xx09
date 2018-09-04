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
 * uas.c		2010/08/15	winston 	Initial version
 *				2010/10/06	Ted			VBUS OFF detect modify
 * 				2010/11/20	Odin		AES function
 *
 *****************************************************************************/

#define UAS_C

#include "general.h"

#ifdef UAS_EN

void uas_status_dead_time_out();

/****************************************\
   uas_init()
\****************************************/
void uas_init()
{
	bot_mode = 0;

	dbuf_uas_mode();

	hdd_post_write = 0;

	usb_curCtxt = CTXT_NULL;
	usb_ctxt_que =  CTXT_NULL;
	usb_post_dout_ctxt = CTXT_NULL;
	usb_reenum_flag = 0;

	uas_ci_paused = 0;
	uas_ci_paused_ctxt = CTXT_NULL;

#ifdef STANDBY_TIMER
	hdd_tick = 0;
	hdd_standby_tick = 0;
#endif

#if 1
	hdd_que_ctxt_site = CTXT_NULL;
#endif

	hdd_que_dout_ctxt = CTXT_NULL;

	for (ctxt_site = 0; ctxt_site < 32; ctxt_site++)
	{
		SCTXT_INDEX = ctxt_site;
		SCTXT_Tag = 0;
		SCTXT_CCMIndex = SCM_NULL;
		//SCTXT_Status = CTXT_STATUS_FREE;
	}

#ifdef UAS_OVERLAPPED_TAG
	uas_cmd_tag.udata.u_8[0] = 0;
	uas_cmd_tag.udata.u_8[1] = 0;
	uas_cmd_tag.udata.u_8[2] = 0;
	uas_cmd_tag.udata.u_8[3] = 0;

	#if 0
		uas_func_tag.udata.u_8[0] = 0;
		uas_func_tag.udata.u_8[1] = 0;
		uas_func_tag.udata.u_8[2] = 0;
		uas_func_tag.udata.u_8[3] = 0;
	#endif
#endif

	uas_tur_tsf_cnt = 0;

	//	xp2tdisk	= 0;
#ifdef UAS_TOUT
	timerCnt = 0;
	timerExpire = 0;
#endif
}

/****************************************\
	uas_device_no_data()
		Dn

	Input Global Parameter:
		ctxt_site: site # of CDB Context for UAS 
\****************************************/
void uas_device_no_data()
{
//DBG0(("ua_n_d\n"));
	
	SCTXT_Flag |= (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
	//*ctxt_Next = CTXT_NULL;
#if 0
	*ctxt_XferLen_0 = 0;
	*ctxt_XferLen_1 = 0;
	*ctxt_XferLen_2 = 0;
	*ctxt_XferLen_3 = 0;
#endif
		
	SCTXT_DbufIndex = DBUF_SEG_NULL;

	//*ctxt_No_Data = (MSC_TX_CTXT_NODATA);
	usb_append_ctxt_que();

	return;
}

/****************************************\
	uas_device_data_in()
		Di or Dn

	Input Global Parameter:
		byteCnt:
		ctxt_site: site # of CDB Context for UAS 

\****************************************/
void uas_device_data_in()
{
	//u8  *p8;

	SCTXT_Flag |= (SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
//	*ctxt_Control |= CTXT_CTRL_DIR;			// use TX Context Register
	//*ctxt_Next = CTXT_NULL;

	*ctxt_XferLen_0 = byteCnt;
	*ctxt_XferLen_1 = byteCnt >> 8;
	*ctxt_XferLen_2 = 0;			//byteCnt >> 16;
	*ctxt_XferLen_3 = 0;			//byteCnt >> 24;

	SCTXT_Status = CTXT_STATUS_GOOD;
	SCTXT_LunStatus = LUN_STATUS_GOOD;
	//DBG("ctxt: %bx, len: %x", ctxt, byteCnt);

	// Hi
	// input port of TX buffer Seg 2  is CPU Write
	// Output port of TX buffer Seg 2  is USB Read
	
	// Select DBUF Segment
	*dbuf_MuxSel = DBUF_SEG_C2U;
	// set Input port only
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_CPU_W_PORT;
	sz16 = byteCnt;
#ifdef USB2_DI_PATCH	// USB2.0 patch
	if (usbMode == CONNECT_USB2)
	{	// ceiling to mutiple of 8
		if ((16 > sz16) && (sz16 > 8) )
		{
			sz16 = 16;
		}
	}
#endif

	pU8 = mc_buffer;
	for (; sz16 != 0; sz16--)
	{
		*dbuf_DataPort = *pU8++;
	}
					
	//DBG("Segment DONE!\t");
	*dbuf_MuxCtrl = SEG_DONE;
	*dbuf_MuxCtrl;

	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;			

	SCTXT_DbufIndex = DBUF_SEG_C2U;
//		SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_CPU_W_PORT;
	SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_NULL;

	//*ctxt_No_Data = 0;

	usb_append_ctxt_que();
		
//		DBG("ok\n");
	return;
}

/****************************************\
   Do or Dn
\****************************************/
void uas_device_data_out()
{
	SCTXT_Flag |= SCTXT_FLAG_U2B;
	//*ctxt_Control = 0;			// use RX Context Register
	//SCTXT_Next = CTXT_NULL;

	*ctxt_XferLen_0 = byteCnt;
	*ctxt_XferLen_1 = byteCnt >> 8;
	*ctxt_XferLen_2 = 0;
	*ctxt_XferLen_3 = 0;

	SCTXT_Status = CTXT_STATUS_PENDING;
	//*ctxt_LunStatus = LUN_STATUS_GOOD;

	// USB Data Out

	// input port of RX buffer Seg 4  is CPU Read
	// Output port of RX buffer Seg 4  is USB Write
	SCTXT_DbufIndex = DBUF_SEG_U2C;
	//SCTXT_DbufINOUT = (TX_DBUF_CPU_R_PORT << 4) | TX_DBUF_USB_W_PORT; 
	SCTXT_DbufINOUT = (TX_DBUF_NULL << 4) | TX_DBUF_USB_W_PORT; 

	//*ctxt_No_Data = 0;
	if (sobj_init && (sobj_State > SATA_READY))
	{
		hdd_que_dout_ctxt = ctxt_site;
		hdd_que_dout_tout = HDD_UAS_QUE_TOUT;
//DBG0(("\tDO Bsy\n"));
		return;
	}
	usb_append_ctxt_que();
}

/****************************************\
   uas_bus_reset
\****************************************/
void uas_bus_reset()
{
	MSG(("uas_brst\n"));
	//DBG0(("s %bx\n", sobj_State));

#if 0	
					*usb_DevCtrlClr_0 = USB_ENUM;
					dump_reg();
					while (1)
					{
					}
#endif
	usbMode = CONNECT_UNKNOWN;

	*usb_Msc0IntStatus_0 = 0xFF;	//MSC_TX_DONE_INT|MSC_RX_DONE_INT|BOT_RST_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT;
	*usb_Msc0IntStatus_1 = 0xFF;		//MSC_CONFIG_INT;
	*usb_Msc0IntStatus_2 = 0xFF;

	*usb_IntStatus_0 = HOT_RESET|WARM_RESET|USB_BUS_RST;
	//*usb_IntStatus_1 = CDB_AVAIL_INT|CTRL_INT|MSC0_INT;

	if (sobj_State > SATA_PWR_DWN)
		sata_Reset(USB_SATA_RST);

	dbuf_Reset();

	if (sobj_State > SATA_PWR_DWN)
	{
		*sata_BlkCtrl_1 = (RXSYNCFIFORST | TXSYNCFIFORST);	//reset sata TX  fifo
		*sata_BlkCtrl_1;

		//3609 reset sata_SiteCmdIntEn to 0
		*sata_SiteCmdIntEn_0 = 0;
	}

	*usb_Ep0CtrlClr = EP0_HALT;
	*usb_Msc0DICtrlClr = MSC_DIN_DONE | MSC_CSW_RUN | MSC_DI_HALT | MSC_TXFIFO_RST;
	*usb_Msc0DICtrlClr;
	*usb_Msc0DOutCtrlClr = MSC_DOUT_RESET | MSC_DOUT_HALT | MSC_RXFIFO_RESET ;
	*usb_Msc0DOutCtrlClr;
	*usb_RecoveryError = 0x00;                       //for usb hot rest
	//spi_phy_wr_retry(PHY_SPI_U3PCS, 0x12, spi_phy_rd(PHY_SPI_U3PCS, 0x12) | BIT_7);

//	*sata_BlkCtrl |= (RXSYNCFIFORST | TXSYNCFIFORST);	//reset sata TX  fifo
//	*sata_BlkCtrl;

	switch_regs_setting(NORMAL_MODE);
	
	usb3_u2_enable = 0;
	usb3_function_suspend = 0;
	usb3_device_U2_state = 0;
#ifdef USB3_TX_DBG
	count_recvry_en = 1;
	recovery_count = 0;
#endif

	uas_init();
}

/****************************************\
	uas_push_ctxt_site_to_free_fifo()
		ctxt_site: site # of CDB Context for UAS command
\****************************************/
void uas_push_ctxt_site_to_free_fifo()
{
	*usb_CtxtFreeFIFO = ctxt_site;
	*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1

	SCTXT_INDEX = ctxt_site;
	//DBG0(("p f FIFO %BX %BX\n", ctxt_site, SCTXT_Tag));
	if (SCTXT_CCMIndex != SCM_NULL)
	{	// break association between ctxt & SCM
		SCM_INDEX = SCTXT_CCMIndex;
		SCM_CdbIndex = CTXT_NULL;

		SCTXT_CCMIndex = SCM_NULL;
	}

	SCTXT_Next = CTXT_NULL;
#ifdef UAS_OVERLAPPED_TAG
	if (SCTXT_Tag)
	{
		uas_cmd_tag.udata.u_8[(SCTXT_Tag >> 3) & 0x03] &= ~(1 << (SCTXT_Tag & 0x7));
	}
#endif
	SCTXT_Tag = 0;
}

#if 0
bit uas_abort_ctxt(u8 ctxt)
{
	u8 qPrev;

	ctxt_site = ctxt;
	//*host_ctxtmem_ptr = ctxt_site;
	SCTXT_INDEX = ctxt_site;

	if ((uas_ci_paused))
	{
		if (ctxt_site == uas_ci_paused_ctxt)
		{
			uas_ci_paused = 0;
			uas_ci_paused_ctxt = CTXT_NULL;
		}
	}

	if (ctxt_site == usb_ctxt_que)
	{
		usb_post_dout_ctxt = CTXT_NULL;
	}

	if (ctxt_site == usb_post_dout_ctxt)
	{
		usb_post_dout_ctxt = CTXT_NULL;
	}

	if (usb_curCtxt == ctxt_site)
	{
		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{
			*usb_Msc0DICtrl = MSC_DATAIN_RESET;
		}
		else
		{
		#if 0
			*usb_Msc0DOutCtrl = MSC_DOUT_RESET;
		#else
			*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
			//*usb_Msc0DOutCtrl;

			*usb_Msc0DOutCtrlClr = MSC_DOUT_HALT;
			//*usb_Msc0DOutCtrlClr;

			*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET;
			//*usb_Msc0DOutCtrlClr;


			*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT;
			*usb_Msc0IntStatus_0;
		#endif
		}

		//*usb_CtxtFreeFIFO = ctxt_site;
		//*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
		uas_push_ctxt_site_to_free_fifo();
		usb_curCtxt = CTXT_NULL;
		usb_exec_que_ctxt();
		return 0;
	}
	else if (usb_ctxt_que != CTXT_NULL)
	{
		if (ctxt_site == usb_ctxt_que)
		{
			SCTXT_INDEX = usb_ctxt_que;
			usb_ctxt_que = SCTXT_Next;

			//*usb_CtxtFreeFIFO = ctxt_site;
			//*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
			uas_push_ctxt_site_to_free_fifo();
			return 0;
		}
		else
		{
			qPrev = usb_ctxt_que;
			SCTXT_INDEX = usb_ctxt_que;
			tmp8 = SCTXT_Next; 
			while (tmp8 != CTXT_NULL)
			{
				if (ctxt_site == tmp8)
				{	// un-queue ctxt from usb_ctxt_que
					SCTXT_INDEX = tmp8;
					tmp8 = SCTXT_Next;

					SCTXT_INDEX = qPrev;
					SCTXT_Next = tmp8;
					//*usb_CtxtFreeFIFO = ctxt_site;
					//*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
					uas_push_ctxt_site_to_free_fifo();
					return 0;
				}
			}
		}
	}

	SCTXT_INDEX = ctxt_site;
	if (SCTXT_Tag)
	{
		uas_push_ctxt_site_to_free_fifo();
		return 0;
	}
	return 1;
}
#endif

#if 0
bit uas_abort_ctxt_w_status(u8 ctxt)
{
	//u8 qPrev;

	ctxt_site = ctxt;
	*host_ctxtmem_ptr = ctxt_site;
	SCTXT_INDEX = ctxt_site;

	if (SCTXT_Tag == 0)
		return 1;

	if ((uas_ci_paused))
	{
		if (ctxt_site == uas_ci_paused_ctxt)
		{
			uas_ci_paused = 0;
			uas_ci_paused_ctxt = CTXT_NULL;
		}
	}

	if (usb_curCtxt == ctxt_site)
	{
		if (SCTXT_Status > CTXT_STATUS_XFER_DONE)
		{
			if (SCTXT_Flag & SCTXT_FLAG_DIN)
			{
				*usb_Msc0DICtrl = MSC_DATAIN_RESET;
			}
			else
			{
			#if 0
				*usb_Msc0DOutCtrl = MSC_DOUT_RESET;
			#else
				*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
				//*usb_Msc0DOutCtrl;

				*usb_Msc0DOutCtrlClr = MSC_DOUT_HALT;
				//*usb_Msc0DOutCtrlClr;

				*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET;
				//*usb_Msc0DOutCtrlClr;


				*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT;
				*usb_Msc0IntStatus_0;
			#endif
			}
			if (SCTXT_DbufIndex != DBUF_SEG_NULL)
			{
				DBUF_SegFree(SCTXT_DbufIndex);
				SCTXT_DbufIndex = DBUF_SEG_NULL;
			}


			hdd_err_2_sense_data(ERROR_DATA_PHASE);
			SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
			usb_exec_tx_ctxt();
			return 0;
		}
		else if (SCTXT_Status == CTXT_STATUS_XFER_DONE)
		{
			hdd_err_2_sense_data(ERROR_DATA_PHASE);
		#if 1
			if (SCTXT_Flag & SCTXT_FLAG_DIN)
			{
				usb_tx_ctxt_send_status();
			}
			else
			{
				usb_rx_ctxt_send_status();
			}
		#else
			uas_send_sense_iu();
		#endif
		}
		return 0;

	}
	else if (usb_ctxt_que != CTXT_NULL)
	{
		if (ctxt_site == usb_ctxt_que)
		{
			//SCTXT_INDEX = usb_ctxt_que;

			hdd_err_2_sense_data(ERROR_DATA_PHASE);

			SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
			if (SCTXT_DbufIndex != DBUF_SEG_NULL)
			{
				DBUF_SegFree(SCTXT_DbufIndex);
				SCTXT_DbufIndex = DBUF_SEG_NULL;
			}
			return 0;
		}
		else
		{
			SCTXT_INDEX = usb_ctxt_que;
			tmp8 = SCTXT_Next; 
			while (tmp8 != CTXT_NULL)
			{
				SCTXT_INDEX = tmp8;

				if (ctxt_site == tmp8)
				{
					hdd_err_2_sense_data(ERROR_DATA_PHASE);

					SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
					if (SCTXT_DbufIndex != DBUF_SEG_NULL)
					{
						DBUF_SegFree(SCTXT_DbufIndex);
						SCTXT_DbufIndex = DBUF_SEG_NULL;
					}
					return 0;
				}
				// check next ctxt
				tmp8 = SCTXT_Next; 
			}
		}
	}

	{
		SCTXT_INDEX = ctxt_site;
		hdd_err_2_sense_data(ERROR_DATA_PHASE);
		uas_device_no_data();
		return 0;
	}
}
#endif

#if 1
bit uas_abort_task(task_tag)
{
	if (uas_ci_paused)
	{
		SCTXT_INDEX = uas_ci_paused_ctxt;
		if (SCTXT_Tag == task_tag)
		{
			uas_ci_paused = 0;
		}
	}

	if (hdd_que_ctxt_site != CTXT_NULL)
	{
		SCTXT_INDEX = hdd_que_ctxt_site;
		if (SCTXT_Tag == task_tag)
		{
			ctxt_site = hdd_que_ctxt_site;
			uas_push_ctxt_site_to_free_fifo();
			hdd_que_ctxt_site = CTXT_NULL;
			return 0;
		}
	}

	if (hdd_que_dout_ctxt != CTXT_NULL)
	{
		SCTXT_INDEX = hdd_que_dout_ctxt;
		if (SCTXT_Tag == task_tag)
		{
			ctxt_site = hdd_que_dout_ctxt;
			uas_push_ctxt_site_to_free_fifo();
			hdd_que_dout_ctxt = CTXT_NULL;
			return 0;
		}
	}

	if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
	{
		SCTXT_INDEX = ctxt_site;

		if (SCTXT_Tag == task_tag)
		{
			if (usb_post_dout_ctxt == usb_curCtxt)
			{
				usb_post_dout_ctxt = CTXT_NULL;
			}

			if (SCTXT_Flag & SCTXT_FLAG_DIN)
			{
				*usb_Msc0DICtrl = MSC_DATAIN_RESET;
			}
			else
			{
			#if 1
				usb_rst_do();
			#else
				*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
				//*usb_Msc0DOutCtrl;

				*usb_Msc0DOutCtrlClr = MSC_DOUT_HALT;
				//*usb_Msc0DOutCtrlClr;

				*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET;
				//*usb_Msc0DOutCtrlClr;


				*usb_Msc0StatCtrl = MSC_STAT_HALT;
				//*usb_Msc0StatCtrl;

				*usb_Msc0StatCtrlClr = MSC_STAT_HALT;
				//*usb_Msc0StatCtrlClr;

				*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT;
				*usb_Msc0IntStatus_0;
				DBG(("Do Rst %bx\n", *usb_Msc0IntStatus_0));
			#endif
			}

			//*usb_CtxtFreeFIFO = usb_curCtxt;
			//*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
			uas_push_ctxt_site_to_free_fifo();
			usb_curCtxt = CTXT_NULL;
			usb_exec_que_ctxt();
			return 0;
		}
	}

	if ((ctxt_site = usb_ctxt_que) != CTXT_NULL)
	{
		u8 qPrev = CTXT_NULL;

		while (ctxt_site != CTXT_NULL)
		{
			SCTXT_INDEX = ctxt_site;

			if (SCTXT_Tag == task_tag)
			{	// find Ctxt with matched Tag
				if (qPrev == CTXT_NULL)
				{	// ctxt is head of que
					usb_ctxt_que = SCTXT_Next;
				}
				else
				{	// un-queue ctxt from usb_ctxt_que
					tmp8 = SCTXT_Next;

					SCTXT_INDEX = qPrev;
					SCTXT_Next = tmp8;
				}
				uas_push_ctxt_site_to_free_fifo();
				return 0;
			}
			ctxt_site = SCTXT_Next;
		}	// while
	}
}
#endif

void uas_abort_all()
{
	uas_ci_paused = 0;
	uas_ci_paused_ctxt = CTXT_NULL;

	if (hdd_que_ctxt_site != CTXT_NULL)
	{
			ctxt_site = hdd_que_ctxt_site;
			uas_push_ctxt_site_to_free_fifo();
			hdd_que_ctxt_site = CTXT_NULL;
	}

	if (hdd_que_dout_ctxt != CTXT_NULL)
	{
			ctxt_site = hdd_que_dout_ctxt;
			uas_push_ctxt_site_to_free_fifo();
			hdd_que_dout_ctxt = CTXT_NULL;
	}


	if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
	{
		//*host_ctxtmem_ptr = ctxt_site;
		SCTXT_INDEX = ctxt_site;

		if (ctxt_site == usb_post_dout_ctxt)
		{
			usb_post_dout_ctxt = CTXT_NULL;
		}

		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{
			*usb_Msc0DICtrl = MSC_DATAIN_RESET;
			*usb_Msc0DICtrl;
		}
		else
		{
			if (SCTXT_DbufIndex != DBUF_SEG_NULL)
			{
				DBUF_SegFree(SCTXT_DbufIndex);
			}
		#if 0
			*usb_Msc0DOutCtrl = MSC_DOUT_RESET;
		#else
			*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
			*usb_Msc0DOutCtrl;

			*usb_Msc0DOutCtrlClr = MSC_DOUT_HALT;
			*usb_Msc0DOutCtrlClr;

			*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET;
			*usb_Msc0DOutCtrlClr;


			*usb_Msc0StatCtrl = MSC_STAT_HALT;
			*usb_Msc0StatCtrl;

			*usb_Msc0StatCtrlClr = MSC_STAT_HALT;
			*usb_Msc0StatCtrlClr;

			*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT;
			//DBG0(("Do Rst %bx\n", *usb_Msc0IntStatus_0));
		#endif
		}

		//*usb_CtxtFreeFIFO = ctxt_site;
		//*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
		uas_push_ctxt_site_to_free_fifo();
		usb_curCtxt = CTXT_NULL;
	}

	if ((ctxt_site = usb_ctxt_que) != CTXT_NULL)
	{
		while (ctxt_site != CTXT_NULL)
		{
			SCTXT_INDEX = ctxt_site;

			tmp8 = SCTXT_Next;

			//*usb_CtxtFreeFIFO = ctxt_site;
			//*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
			uas_push_ctxt_site_to_free_fifo();
			ctxt_site = tmp8;
		}


		usb_ctxt_que = CTXT_NULL;
	}

#ifdef UAS_OVERLAPPED_TAG
	for (ctxt_site = 0; ctxt_site < 32; ctxt_site++)
	{
		SCTXT_INDEX = ctxt_site;
		if (SCTXT_Tag)
		{
			uas_push_ctxt_site_to_free_fifo();
		}
	}

#endif
	// flush USB Ctxt Avail FIFO back to CTXT FREE FIFO
	while (1)
	{
		if (*usb_IntStatus_1 & CDB_AVAIL_INT)
		{
			ctxt_site = *usb_CtxtAvailFIFO;
			*usb_CtxtFreeFIFO = ctxt_site;
			*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1

			continue;
		}
		break;
	}
}

/****************************************\
	uas_send_response_iu
\****************************************/
void uas_send_response_iu(u8 response_code, u8 ITag)
{
	if (ITag == 0)
		return;

	//make sure response run is not set
	if (*usb_Msc0StatCtrl & MSC_STAT_RESP_RUN)
	{
		if (*usb_Msc0Tag_0 == ITag)
		{
			response_code = OverlappedTagAttempted;
		}
		*usb_Msc0StatCtrlClr = MSC_STAT_RESP_RUN;  //clear it
	}

	uas_riu_Tag = ITag;

	*usb_Msc0Tag_0 = uas_riu_Tag;		//ITag;
	//*usb_Msc0Tag_1 = 0;
	//*usb_Msc0Response_0 = 0;
	//*usb_Msc0Response_1 = 0;
	//*usb_Msc0Response_2 = 0;
	*usb_Msc0Response_3 = response_code;

	//DBG(("Res %BX\n", *usb_Msc0Response_3));

	//now send it
	*usb_Msc0StatCtrl = MSC_STAT_RESP_RUN;
	uas_st_tout_cnt = 10;
}

void uas_fetch_TIU()
{
	EAL = 0;
	//COPYU16_REV_ENDIAN_X2D(usb_Msc0TaskITag_0, &initiator_Tag);
	tiu_initiator_Tag = *usb_Msc0TaskITag_0;
	//COPYU16_REV_ENDIAN_X2D(usb_Msc0TaskTTag_0, &task_Tag);

	//task_tag0 = *usb_Msc0TaskTTag_0;
	tiu_task_tag0 = CACHE_BYTE2;

	tiu_func = *usb_Msc0TaskFunc;
	//tmp8 = *usb_Msc0TaskFlag;
	tiu_flag = CACHE_BYTE2;
	EAL = 1;

	//tiu_lun0 = *usb_Msc0TaskLun;
	*usb_Msc0IntStatus_1 = MSC_TASK_AVAIL_INT;
}

/****************************************\
	UAS_Task_Manage_IU
\****************************************/
void UAS_Task_Manage_IU()
{
	u8	response_code;

	//DBG(("TMIU\n"));

	uas_fetch_TIU();

tiu_again:
	//check if error receive this task
	if (tiu_flag & TASK_LUN_ERR)
	{	// 4.53
		response_code = IncorrectLUN;
	}
	else
	{
#if 0	// debug
		DBG0(("T_IU %bx\n", tiu_func));

		dump_reg();
		*usb_DevCtrlClr_0 = USB_ENUM;
		while (1)
		{
		}
#endif

		val = tiu_initiator_Tag;
		if (val == 0)
		{
			return;
		}
#ifdef UAS_OVERLAPPED_TAG
		// Is tag already in use by another command ?
		i8 = (val >> 3) & 0x03;
		tmp8 = 1 << (val & 0x07);

		//if ((uas_cmd_tag.udata.u_8[i8] & tmp8) ||
		//	(uas_func_tag.udata.u_8[i8] & tmp8))
		if (uas_cmd_tag.udata.u_8[i8] & tmp8)
		{
			//OVERLAPPED TAG ATTEMPTED
			//	abt_itag[0] = *(((u8 *)(&tmp16)) + 1);
			//	abt_itag[1] = *(((u8 *)(&tmp16)) + 0);
			MSG(("T>C %BX\n", val));
			//MSG(("%BX%BX%BX%BX\n", uas_cmd_tag.udata.u_8[3], uas_cmd_tag.udata.u_8[2], uas_cmd_tag.udata.u_8[1], uas_cmd_tag.udata.u_8[0]));
			//a) abort all commands received
			//b) abort all task management functions received
			//uas_target_reset();
			sata_abort_all();
			uas_abort_all();

			//if (*usb_Msc0IntStatus_1 & MSC_TASK_AVAIL_INT)
			//	*usb_Msc0IntStatus_1 = MSC_TASK_AVAIL_INT;
			//c) Service Response with "OVERLAPPED TAG ATTEMPTED"
			//		SFUNC_INDEX = 0x10;
			//		SFUNC_NEXT = SFUNC_NULL;
			//		SFUNC_TAG0 = abt_itag[0];
			//		SFUNC_TAG1 = abt_itag[1];
			//		SFUNC_RESPONCE = (OverlappedTagAttempted) ;			// INVALID INFORMATION UNIT

			// append "Func Context" to response queue 
			//		uas_append_response_que(0x10);
			response_code = OverlappedTagAttempted;
										
			goto send_riu;
		}
		else if (uas_riu_Tag == val)
		{
			MSG(("T>T %BX\n", val));
			//MSG(("%BX%BX\n", uas_func_tag.u16data.u_8[1], uas_func_tag.u16data.u_8[0]));

			//a) abort all commands received
			//b) abort all task management functions received
			//uas_target_reset();
			sata_abort_all();
			uas_abort_all();
			//c) Service Response with "OVERLAPPED TAG ATTEMPTED"
			//		SFUNC_INDEX = 0x10;
			//		SFUNC_NEXT = SFUNC_NULL;
			//		SFUNC_TAG0 = abt_itag[0];
			//		SFUNC_TAG1 = abt_itag[1];
			//		SFUNC_RESPONCE = (OverlappedTagAttempted) ;			// INVALID INFORMATION UNIT

			//if (*usb_Msc0IntStatus_1 & MSC_TASK_AVAIL_INT)
			//	*usb_Msc0IntStatus_1 = MSC_TASK_AVAIL_INT;

			// append "Func Context" to response queue 
			//		uas_append_response_que(0x10);
			response_code = OverlappedTagAttempted;
										
			goto send_riu;
		}
#endif	//#ifdef UAS_OVERLAPPED_TAG
		response_code = (TaskManageComplete) ;			// TASK MANAGEMENT FUNCTION COMPLETE
		switch (tiu_func)
		{
			case  ABORT_TASK:
				//DBG(("abt\n"));
				//if (((tmp8 = *usb_Msc0TaskTTag_0) >= 0x20) ||
				//	(*usb_Msc0TaskTTag_1 != 0))
				//{
				//	response_code = (TaskManageFunctionFailed) ;			// 
				//}
				//else
				{
					tmp8 = tiu_task_tag0;
					if (tmp8 == 0)
					{
						//response_code = (InvalidInformationUnit) ;
						response_code = TaskManageFunctionFailed;
						break;
					}
			#ifdef UAS_OVERLAPPED_TAG
					i8 = (tmp8 >> 3) & 0x3;
					tmp8 = 1 << (tmp8 & 0x7);
					if ((uas_cmd_tag.udata.u_8[i8] & tmp8) == 0)
					{	//cmd was not in the task set
						break;
					}
					uas_cmd_tag.udata.u_8[i8] &= ~tmp8;
			#endif

#if 0 //debug
					if (sata_abort_task(tiu_task_tag0))
					{
						//tmp8 = *usb_Msc0TaskTTag_0;
						if 	(uas_abort_task(tiu_task_tag0))
						{
							response_code = (TaskManageFunctionFailed) ;			// TASK MANAGEMENT FUNCTION FAILED
						}
					}
#else
					if 	(uas_abort_task(tiu_task_tag0))
					{

						//response_code = (TaskManageFunctionFailed) ;			// TASK MANAGEMENT FUNCTION FAILED
					}
#endif
				}
				break;

			case  ABORT_TASK_SET:
				MSG(("ATS\n"));
				goto abort_all;

			case  CLEAR_TASK_SET:
				MSG(("CTS\n"));
				goto abort_all;
		
			case  LOGICAL_UNIT_RESET:
				MSG(("LRst\n"));
				goto abort_all;

			case  I_T_NEXUS_RESET:	
				MSG(("I_T_RST\n"));
abort_all:
			//#ifdef DEBUG_DP
			#if 0
				dump_reg();
				*usb_DevCtrlClr_0 = USB_ENUM;

				while (1) ;
			#endif
				sata_abort_all();
				uas_abort_all();
				uas_init();
				break;

			case  CLEAR_ACA:
			case  QUERY_TASK:
			case  QUERY_TASK_SET:
			case  QUERY_ASYC_EVENT:
				break;
			default:
				// 4.52
				//DBG(("b tiu %bx\n", tiu_func));
				response_code = (TaskManageNotSupported) ;			// TASK MANAGEMENT FUNCTION NOT SUPPORTED
				break;
		} // switch
	}

send_riu:
	tiu_func = 0;
    //make sure response run is not set
	if (*usb_Msc0StatCtrl & MSC_STAT_RESP_RUN)
	{
#if 1
		if (*usb_Msc0Tag_0 == tiu_initiator_Tag)
		{
			response_code = OverlappedTagAttempted;
		#if 0
			*usb_Msc0StatCtrlClr = MSC_STAT_RESP_RUN;  //clear it
			if ((tmp8 = *usb_Msc0StatCtrlClr) & MSC_STAT_FLOW_CTRL)
			{
				*usb_Msc0StatCtrlClr = MSC_STAT_FLOW_CTRL;
			}
		#else
			*usb_Msc0StatCtrl = MSC_STAT_RESET;

			//DBG(("\tT>T %BX\n", *usb_Msc0StatCtrl));
			//EAL = 0;
			//*usb_USB3StateSelect = 0x0B;		// MSC0 STATUS Control State Machine
			//tmp8 = *usb_USB3StateCtrl;
			//*usb_USB3StateSelect = 0x00;		// Link Training and Status State Machine (LTSSM)
			//EAL = 1;
			//DBG0(("\t%BX\n", tmp8));

		#endif
			goto send_riu_2;
		}
#endif
		tmp16=0;
		while (1)
		{
			if (*usb_Msc0IntStatus_1 & MSC_RESP_DONE_INT)
			{
				//clear the task bit 
				*usb_Msc0IntStatus_1 = MSC_RESP_DONE_INT;
				if ((tmp8 = *usb_Msc0StatCtrlClr) & MSC_STAT_FLOW_CTRL)
				{
					*usb_Msc0StatCtrlClr = MSC_STAT_FLOW_CTRL;
				}
				//uas_st_tout_cnt = 0;
				#ifdef UAS_OVERLAPPED_TAG
						uas_riu_Tag = 0;
				#endif
				break;
			}
			if (*usb_Msc0IntStatus_1 & MSC_ST_TIMEOUT_INT)
			{
				uas_status_dead_time_out();
			}
			if ((tiu_func == 0) && (*usb_Msc0IntStatus_0 & MSC_TASK_AVAIL_INT))
			{
				uas_fetch_TIU();
			}
			if ((++tmp16) == 0)
			{
				*usb_Msc0StatCtrlClr = MSC_STAT_RESP_RUN;  //clear it
				break;
			}
		}  //while (1)
	}
send_riu_2:
	uas_riu_Tag = tiu_initiator_Tag;
	*usb_Msc0Tag_0 = uas_riu_Tag;
	//*usb_Msc0Tag_1 = initiator_Tag >> 8;

	//load response code
	//*usb_Msc0Response  |= code << 24;
	//*usb_Msc0Response  |=  code << 16; ??
	//*usb_Msc0Response  |=  additional_code;
	*usb_Msc0Response_3 = response_code;
	// change dead timeout for RIU 120 * 100us = 12ms
	*usb_DeadTimeout_0 = 120 & 0xff;
	*usb_DeadTimeout_1 = 120 >> 8;

	//now send it
	*usb_Msc0StatCtrl = MSC_STAT_RESP_RUN;
	//DBG(("\tStC %BX\n", *usb_Msc0StatCtrl));
	uas_st_tout_cnt = 10;
	if (tiu_func)
	{
		goto tiu_again;
	}
}

#if 1
/****************************************\
	uas_send_sense_iu
\****************************************/
void uas_send_sense_iu()
{
//DBG0(("IU:%BX\n", ctxt_site));
	*host_ctxtmem_ptr = ctxt_site;
	SCTXT_INDEX = ctxt_site;

	tmp8 = 0;		// sense data length is zero by default
	// check status byte
    if (SCTXT_Status != CTXT_STATUS_GOOD)
    {
		if (SCTXT_LunStatus == LUN_STATUS_CHKCOND)
		{	//check condition
			//DBG0(("B: %BX, OP:%BX\n", *host_ctxtmem_ptr, CmdBlk(0)));
			xmemset(uas_senseBuffer, 0, 24);
			uas_senseBuffer[7] = 10;		// additional Sense Length
			//DBG0(("B1:SB[0]:%BX, [7]:%BX\n", uas_senseBuffer[0], uas_senseBuffer[7]));
			tmp8 = 18;
		#ifdef ATA_PASS_THROUGTH
			if (ctxt_CDB[16] == 22)
			{	// extended Descriptor
				uas_senseBuffer[0] = 0x72;							// response code
				uas_senseBuffer[1] = SCSI_SENSE_RECOVERED_ERROR;	// SENSE KEY
				//uas_senseBuffer[2] = 0x00;						// ASC:ATA PASS THROUGH INFORMATION AVAILABLE
				uas_senseBuffer[3] = 0x1D;							// ASCQ
				uas_senseBuffer[7] = 14;							// additional Sense Length

				extended_descriptor = 0;
				if (ctxt_CDB[0] == SCSI_ATA_PASS_THR16)
				{
					extended_descriptor = ctxt_CDB[1] & 0x01;		// extend bit
				}
				hdd_return_descriptor_sense(&uas_senseBuffer[8]);
				tmp8 = 22;
			}
			else //if (ctxt_CDB[16] == 18)
		#endif
			{
				uas_senseBuffer[0] = 0x70;
				uas_senseBuffer[2] =  ctxt_CDB[17];	// sense code
				uas_senseBuffer[12] = ctxt_CDB[18];	// ASC
				uas_senseBuffer[13] = ctxt_CDB[19];	// ASCQ
			}
		}
	}
	else  
	{
		////*usb_Msc0DIStatus = LUN_STATUS_GOOD; 
		//SCTXT_LunStatus = LUN_STATUS_GOOD; 
	}
		  
#ifdef DBG_FUNCTION
	if (tmp8)
	{
		//DBG0(("Sen %BX %BX", uas_senseBuffer[2], uas_senseBuffer[12]));
	}
#endif

	if (SCTXT_Flag & SCTXT_FLAG_DIN)
	{
		*usb_Msc0DIStatus = SCTXT_LunStatus; 

		// Reseet Sense Data Pointer
		*usb_Msc0TxSenseAddress = 0;

		if (tmp8)
		{
			pU8 = uas_senseBuffer;
		
			//copy sense data 
			for (; tmp8 != 0 ; tmp8--)
			{
				//DBG0(("%BX ", *pU8));
				*usb_Msc0TxSensePort_0 = *pU8++;
			}
#if 0	// dbg
			*usb_Msc0TxSenseAddress = 0;
			for (i8=0; i8 < 20 ; i8++)
			{
				DBG0(("%BX ", *usb_Msc0TxSensePort_0));
				;
			}
			DBG0(("\n"));
#endif
		}


		//this instruction will cause hardware to send sense IU			
		*usb_Msc0StatCtrl = MSC_STAT_TXSENSE_RUN;
		uas_st_tout_cnt = 10;
	}
	else
	{
		*usb_Msc0DOStatus = SCTXT_LunStatus;

		// Reseet Sense Data Pointer
		*usb_Msc0RxSenseAddress = 0;
		if (tmp8)
		{
			pU8 = uas_senseBuffer;
		
			//copy sense data 
			for (; tmp8 != 0 ; tmp8--)
			{
				//DBG0(("%BX ", *pU8));
				*usb_Msc0RxSensePort_0 = *pU8++;
			}
		}
		*usb_Msc0StatCtrl = MSC_STAT_RXSENSE_RUN;
		uas_st_tout_cnt = 10;
	}
}
#else

/****************************************\
	uas_din_send_sense_iu
\****************************************/
void uas_din_send_sense_iu()
{
//DBG0(("IU:%BX\n", ctxt_site));
		*host_ctxtmem_ptr = ctxt_site;
		SCTXT_INDEX = ctxt_site;

		// Reseet Sense Data Pointer
		*usb_Msc0TxSenseAddress = 0;

		// check status byte
      	if (SCTXT_Status != CTXT_STATUS_GOOD)
      	{
			//u8  *pU8;

			*usb_Msc0DIStatus = SCTXT_LunStatus; 

			if (SCTXT_LunStatus == LUN_STATUS_CHKCOND)
			{	//check condition
				//DBG0(("B: %BX, OP:%BX\n", *host_ctxtmem_ptr, CmdBlk(0)));
				xmemset(uas_senseBuffer, 0, 24);
				uas_senseBuffer[0] = 0x70;
				uas_senseBuffer[7] = 10;		// additional Sense Length
				//DBG0(("B1:SB[0]:%BX, [7]:%BX\n", uas_senseBuffer[0], uas_senseBuffer[7]));
				tmp8 = 18;
		#ifdef ATA_PASS_THROUGTH
				if (ctxt_CDB[16] == 22)
				{	// extended Descriptor
					uas_senseBuffer[0] = 0x72;							// response code
					uas_senseBuffer[1] = SCSI_SENSE_RECOVERED_ERROR;	// SENSE KEY
					//uas_senseBuffer[2] = 0x00;						// ASC:ATA PASS THROUGH INFORMATION AVAILABLE
					uas_senseBuffer[3] = 0x1D;							// ASCQ
					uas_senseBuffer[7] = 14;							// additional Sense Length

					extended_descriptor = 0;
					if (ctxt_CDB[0] == SCSI_ATA_PASS_THR16)
					{
						extended_descriptor = ctxt_CDB[1] & 0x01;		// extend bit
					}
					hdd_return_descriptor_sense(&uas_senseBuffer[8]);
					tmp8 = 22;
				}
				else //if (ctxt_CDB[16] == 18)
		#endif
				{
					uas_senseBuffer[2] =  ctxt_CDB[17];	// sense code
					uas_senseBuffer[12] = ctxt_CDB[18];	// ASC
					uas_senseBuffer[13] = ctxt_CDB[19];	// ASCQ
				}

				pU8 = uas_senseBuffer;
		
				//copy20 byte sense data 
				for (; tmp8 != 0 ; tmp8--)
				{
					//DBG0(("%BX ", *pU8));
					*usb_Msc0TxSensePort_0 = *pU8++;
				}
				//DBG0(("\n"));

				// copy 2 more bytes if needed
				//if (ctxt_CDB[16] == 22)
				//{
				//	*usb_Msc0TxSensePort_0 = *pU8++;
				//	*usb_Msc0TxSensePort_0 = *pU8++;
				//	*usb_Msc0TxSensePort_0 = *pU8++;
				//	*usb_Msc0TxSensePort_0 = *pU8;
				//}
	#if 0
				*usb_Msc0TxSenseAddress = 0;
				for (i8=0; i8 < 20 ; i8++)
				{
					DBG0(("%BX ", *usb_Msc0TxSensePort_0));
					;
				}
				DBG0(("\n"));
	#endif
			}
		}
		else  
		{
			*usb_Msc0DIStatus = LUN_STATUS_GOOD; 
		}
		  		
		//this instruction will cause hardware to send sense IU			
		*usb_Msc0StatCtrl = MSC_STAT_TXSENSE_RUN;
		uas_st_tout_cnt = 10;
}

/****************************************\
	uas_dout_send_sense_iu
\****************************************/
void uas_dout_send_sense_iu()
{
		//DBG("do_sense_iu\n");
		DBG(("OUTIN:%BX\n", SCTXT_INDEX));

		// Reseet Sense Data Pointer
		*usb_Msc0RxSenseAddress = 0;
		
		// check status byte
      	if (SCTXT_Status != CTXT_STATUS_GOOD)
		{
			//u8  *pU8;

			*usb_Msc0DOStatus = SCTXT_LunStatus; 

			if (SCTXT_LunStatus == LUN_STATUS_CHKCOND)
			{	//check condition
				xmemset(uas_senseBuffer, 0, 24);
				uas_senseBuffer[0] = 0x70;
				uas_senseBuffer[7] = 10;		// additional Sense Length

				tmp8 = 18;
			#ifdef ATA_PASS_THROUGTH
				if (ctxt_CDB[16] == 22)
				{	// extended Descriptor
					uas_senseBuffer[0] = 0x72;		// response code
					uas_senseBuffer[1] = SCSI_SENSE_RECOVERED_ERROR;	// SENSE KEY
					//uas_senseBuffer[2] = 0x00;						// ASC:ATA PASS THROUGH INFORMATION AVAILABLE
					uas_senseBuffer[3] = 0x1D;							// ASCQ
					uas_senseBuffer[7] = 14;		// additional Sense Length

					extended_descriptor = 0;
					if (ctxt_CDB[0] == SCSI_ATA_PASS_THR16)
					{
						extended_descriptor = ctxt_CDB[1] & 0x01;		// extend bit
					}
					hdd_return_descriptor_sense(&uas_senseBuffer[8]);
					tmp8 = 22;
				}
				else //if (ctxt_CDB[16] == 18)
			#endif
				{
					uas_senseBuffer[2] =  ctxt_CDB[17];	// sense code
					uas_senseBuffer[12] = ctxt_CDB[18];	// ASC
					uas_senseBuffer[13] = ctxt_CDB[19];	// ASCQ
				}

				pU8 = (uas_senseBuffer);
		
				//copy20 byte sense data 
				for (; tmp8 != 0 ; tmp8--)
				{
					*usb_Msc0RxSensePort_0 = *pU8++;
				}
				// copy 2 more bytes if needed
				//if (ctxt_CDB[16] == 22)
				//{
				//	*usb_Msc0RxSensePort_0 = *pU8++;
				//	*usb_Msc0RxSensePort_0 = *pU8++;
				//}
			}
		}
		else  
		{
			*usb_Msc0DOStatus = LUN_STATUS_GOOD; 	
		}
			
		//this instruction will cause hardware to send sense IU			
		*usb_Msc0StatCtrl = MSC_STAT_RXSENSE_RUN;
		uas_st_tout_cnt = 10;
}
#endif

#ifdef INTEL_SEQNUM_PATCH
void chk_condition_action_for_Intel_UAS(void)
{
#if 0
	if (intel_SeqNum_monitor)
	{ 
		// if there's clear_feature halt following, it's intel host
		intel_SeqNum_monitor |= INTEL_SEQNUM_CHECK_CONDITION;
	}
	if (intel_host_flag)
#endif
	{
		for (i8 = 0; i8 < 100; i8++)
		{
			if ( *usb_Ep0Ctrl & EP0_SETUP)
			{
				usb_control();
				break;
			}
			Delay10us(1);
		}
	}
}
#endif

void uas_tx_sense_done()
{
	//DBG(("usb_msc_isr: tx sense done\n"));
								
	*usb_Msc0IntStatus_1 = MSC_TXSENSE_DONE_INT;
	*usb_Msc0StatCtrlClr = MSC_STAT_FLOW_CTRL;
	if ((uas_ci_paused) /*&& (pCtxt)*/)
	{
		if (usb_curCtxt == uas_ci_paused_ctxt)
		{
			uas_ci_paused = 0;			
		}
	}						
	if (usb_curCtxt != CTXT_NULL)
	{
		SCTXT_INDEX = usb_curCtxt;
	#ifdef UAS_OVERLAPPED_TAG
		if (SCTXT_Tag)
		{
			uas_cmd_tag.udata.u_8[(SCTXT_Tag >> 3) & 0x03] &= ~(1 << (SCTXT_Tag & 0x07));
		}
		else
		{
			//DBG0(("SCTXT_Tag 0\n"));
		#ifdef DEBUG_DP
		//#if 0
			dump_reg();
		#endif
		}
	#endif
		SCTXT_Tag = 0;
	#ifdef INTEL_SEQNUM_PATCH
		if (SCTXT_Status == CTXT_STATUS_ERROR)
		{
			MSG(("chk con 0\n"));
			chk_condition_action_for_Intel_UAS();
		}
	#endif					
	}
	usb_curCtxt = CTXT_NULL;
	// check pending Ctxt queue for TX(DI) Context ????
	usb_exec_que_ctxt();
}

void uas_rx_sense_done()
{
	*usb_Msc0IntStatus_0 = MSC_RXSENSE_DONE_INT;
	*usb_Msc0StatCtrlClr = MSC_STAT_FLOW_CTRL;
					
	if ((uas_ci_paused) /*&& (pCtxt)*/)
	{
		if (usb_curCtxt == uas_ci_paused_ctxt)
		{
			uas_ci_paused = 0;
			
		}
	}
	if (usb_curCtxt != CTXT_NULL)
	{
		SCTXT_INDEX = usb_curCtxt;
	#ifdef UAS_OVERLAPPED_TAG
		if (SCTXT_Tag)
			uas_cmd_tag.udata.u_8[(SCTXT_Tag >> 3) & 0x03] &= ~(1 << (SCTXT_Tag & 0x07));
	#endif
		SCTXT_Tag = 0;
	#ifdef INTEL_SEQNUM_PATCH
		if (SCTXT_Status == CTXT_STATUS_ERROR)
		{
			MSG(("chk con 1\n"));
			chk_condition_action_for_Intel_UAS();
		}
	#endif					
	}
	usb_curCtxt = CTXT_NULL;

	usb_exec_que_ctxt();
}

/*********************************************************************\
	uas_chk_TSFull


\*********************************************************************/
#if 1
bit uas_chk_TSFull()
{
	{
		//if ((SCTXT_Tag == 0x01) && (usb_curCtxt == CTXT_NULL))
		{
			//*usb_USB3Option_1 &= ~USB3_ENABLE_FLOW_PRIME;
		#ifdef DBG_FUNCTION
			val = *usb_Msc0CtxtUsed_0;
		#endif
			//if (val <= MAX_UAS_TAG_ID)
			{
				//Delay10us(60);
				//if (*usb_Msc0CtxtUsed_0 > val)
				{
					//DBG(("TSF %BX %BX\n", val, *usb_Msc0CtxtUsed_0));
					for (i8 = 0; i8 < 255; i8++)
					{
						if (*usb_Msc0CtxtUsed_0 > MAX_UAS_TAG_ID)
						{
							//uas_ctxt_used = *usb_Msc0CtxtUsed_0;
		UART_CH('@');
							EAL = 0;
							tmp16 = 3000;
							while(1)
							{
								if (*usb_Msc0IntStatus_1 & MSC_TSFULL_DONE_INT )
								{
									//val = *usb_Msc0StatInXferStat_1;
									break;
								}
								if (*usb_Msc0IntStatus_1 & MSC_TSFULL_DONE_INT )
								{
									//val = *usb_Msc0StatInXferStat_1;
									break;
								}
								if (*usb_Msc0IntStatus_1 & MSC_TSFULL_DONE_INT )
								{
									//val = *usb_Msc0StatInXferStat_1;
									break;
								}
								if (*usb_Msc0IntStatus_1 & MSC_TSFULL_DONE_INT )
								{
									//val = *usb_Msc0StatInXferStat_1;
									break;
								}
								if (*usb_Msc0IntStatus_1 & MSC_TSFULL_DONE_INT )
								{
									//val = *usb_Msc0StatInXferStat_1;
									break;
								}
								if (*usb_Msc0IntStatus_1 & MSC_TSFULL_DONE_INT )
								{
									//val = *usb_Msc0StatInXferStat_1;
									break;
								}
								if ((--tmp16) == 0)
								{
									//DBG(("TSF tout\n"));
									EAL = 1;
									return 1;
								}
							} // while()
							//if (*usb_Msc0IntStatus_1 & MSC_TSFULL_DONE_INT)
							{
								*usb_Msc0StatCtrl = MSC_STAT_RESET;
								EAL = 1;
								//DBG0(("TSF D %BX\n", val));
								*usb_Msc0IntStatus_1 = MSC_TSFULL_DONE_INT;
								//MSG(("Cus %bx, %bx\n", (u8)uas_ctxt_used, i8));
								return 1;
							}
						}
						Delay10us(6);
					}	//for (i8 = 0; i8 < 255; i8++)
					//DBG(("Tag %bx site %bx ctxtU %bx\n", SCTXT_Tag, ctxt_site, *usb_Msc0CtxtUsed_0));	
					//DBG0(("TSF F %BX %BX\n", val, *usb_Msc0CtxtUsed_0));
				}
			}
			//*usb_USB3Option_1 |= USB3_ENABLE_FLOW_PRIME;
		}
	}
	return 0;
}
#endif

#ifdef UAS_DI_TOUT
void uas_xfer_tout()
{
	if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
	{
		SCTXT_INDEX = ctxt_site;
		*host_ctxtmem_ptr = ctxt_site;

		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{	// set Done Bit
			*usb_Msc0DICtr = MSC_DIN_DONE;
			*usb_Msc0DICtr;
		}

		if (usb_post_dout_ctxt == usb_curCtxt)
		{
			usb_post_dout_ctxt = CTXT_NULL;
		}

		if (SCTXT_DbufIndex != DBUF_SEG_NULL)
		{
			*dbuf_MuxSel = SCTXT_DbufIndex;
			SCTXT_DbufIndex = DBUF_SEG_NULL;
			*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
			*dbuf_MuxCtrl = SEG_RESET;

		}

		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{
			*usb_Msc0DICtrl = MSC_DATAIN_RESET;
		}
		//else
		//{
		//	*usb_Msc0DOutCtrl = MSC_DOUT_RESET;
		//}

		if (SCTXT_CCMIndex != SCM_NULL)
		{	// break association between ctxt & SCM
			SCM_INDEX = SCTXT_CCMIndex;
			SCM_CdbIndex = CTXT_NULL;
			SCTXT_DbufIndex = DBUF_SEG_NULL;
			SCTXT_CCMIndex = SCM_NULL;
		}
		hdd_err_2_sense_data(ERROR_DATA_PHASE);
		SCTXT_Flag = (SCTXT_FLAG_NO_DATA|SCTXT_FLAG_DIN|SCTXT_FLAG_U2B);
		usb_exec_tx_ctxt();
	}
}
#endif
void uas_status_dead_time_out()
{
//st_flow_tout:
							*usb_Msc0IntStatus_1 = MSC_ST_TIMEOUT_INT;
							tmp8 = *usb_Msc0StatCtrl;
		//DBG(("mSI %BX ", tmp8));

							if ((tmp8 & (STATUS_ACKE|STATUS_ACKR)) && 
								(tmp8 & (MSC_STAT_RESP_RUN|MSC_STAT_TXSENSE_RUN|MSC_STAT_RXSENSE_RUN)) )
							{
								//DBG(("mStCt\n"));
								//tmp8 &= MSC_STAT_HOST_WAIT;
								//if ((tmp8 == STATUS_ACKE) || (tmp8 == STATUS_ACKR))
								{
									if (uas_st_tout_cnt == 0)
										goto si_flow_ctrl;

									uas_st_tout_cnt--;
									if (uas_st_tout_cnt == 0)
									{
										if (tmp8 & (MSC_STAT_TXSENSE_RUN|MSC_STAT_RXSENSE_RUN))
										{
											*usb_Msc0StatCtrl = MSC_STAT_HALT;

											*usb_Msc0StatCtrlClr = MSC_STAT_HALT;
											*usb_Msc0StatCtrlClr;
											//if (usb_curCtxt != CTXT_NULL)
											{
												ctxt_site = usb_curCtxt;
												uas_push_ctxt_site_to_free_fifo();
												usb_curCtxt = CTXT_NULL;
											}
											//abort all commands in the task set and all commands that 
											//the target port receives until the UAS target port is able to terminate a command 
											//with CHECK CONDITION status 
											//with the sense key set to UNIT ATTENTION 
											//with the additional sense code set to COMMANDS CLEARED BY DEVICE SERVER
											sata_abort_all();
											uas_abort_all();
											uas_init();
										}
										else //if (tmp8 & MSC_STAT_RESP_RUN )
										{
										#if 0
											*usb_Msc0StatCtrlClr = MSC_STAT_RESP_RUN;  //clear it
										#else
											*usb_Msc0StatCtrl = MSC_STAT_RESET;
										#endif
											uas_riu_Tag = 0;
										}
										//else
										//{
										//	*usb_Msc0StatCtrl = MSC_STAT_RESET;	
										//}
									}
									else
									{
si_flow_ctrl:
										*usb_Msc0StatCtrl = MSC_STAT_FLOW_CTRL;
										//DBG0(("SI_F\n"));
										//*host_ctxtmem_ptr = usb_curCtxt;
										//mem_dump(ctxt_CDB, 10);	
									#ifdef UAS_TOUT
										uas_dead_time_out++;
										if (uas_dead_time_out > 200)
										{
											dump_reg();
											*usb_DevCtrlClr_0 = USB_ENUM;
	
											while (1)
											{
											}
										}
									#endif	//	#ifdef UAS_TOUT
									}
								}
	
							}
							else
							{
								*usb_Msc0StatCtrlClr = MSC_STAT_FLOW_CTRL;// clear the Flow control
							}
}

/****************************************\
	usb_uas
\****************************************/
void usb_uas()
{
	//MSG(("UAS %BX\n", sobj_State));

	// xlate SCSI Read/Wite command to SATA NCQ command 
	// ATA NCQ command
	//*usb_Msc0Lun_SAT_0 = (*usb_Msc0Lun_SAT_0 & ~SAT_CMD) | SATA_NCQ_CMD;
	//*usb_DevCtrl |=	EXIT_SS_INACTIVE;
	//if (revision_a91 == 0)
		*usb_CoreCtrl_3 |=  USB3_BURST_MAX_RX;

	//curMscMode = MODE_UAS;
	uas_init();
	MSG(("UAS %BX\n", sobj_State));

#if 1	// 
	#ifdef REJECT_U1U2
		//*usb_DevCtrlClr_1 = USB3_U1_REJECT;	
		//*usb_DevCtrlClr_2 = USB3_U2_REJECT;	
		u1U2_reject_state = U1U2_ACCEPT;
		U1U2_ctrl_timer = 0;
	#endif
#endif
	// Enable "Setup Device Request" interrupt of EP 0
	*usb_Ep0IntEn = EP0_SETUP;

	// Enable MSCn_INTEN register
	*usb_Msc0IntEn_0 = (MSC_RXSENSE_DONE_INT|MSC_TX_DONE_INT|MSC_RX_DONE_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT);
	//*usb_Msc0IntEn_1 = MSC_TASK_AVAIL_INT|MSC_RESP_DONE_INT|MSC_TXSENSE_DONE_INT;
	*usb_Msc0IntEn_1 = MSC_TASK_AVAIL_INT|MSC_RESP_DONE_INT|MSC_TXSENSE_DONE_INT|MSC_ST_TIMEOUT_INT|MSC_DI_TIMEOUT_INT|MSC_DO_TIMEOUT_INT;

	// Enable MSCn_INTEN register
	*usb_IntEn_0 = HOT_RESET|WARM_RESET|USB_BUS_RST;
	*usb_IntEn_1 = CDB_AVAIL_INT|MSC0_INT|CTRL_INT;

	*sata_FrameIntEn_0 |= SETDEVBITSIEN|DMASETUPIEN;

	*sata_IntEn_0 = RXDATARLSDONEEN|DATATXDONEEN|FRAMEINTPENDEN|PHYINTPENDEN;

#if 0
	// 1111 * 100us = 111.1.0ms
	*usb_DeadTimeout_0 = 1111 & 0xff;
	*usb_DeadTimeout_1 = 1111 >> 8;
#else
	// 1280 * 100us = 128ms
	*usb_DeadTimeout_0 = 1280 & 0xff;
	*usb_DeadTimeout_1 = 1280 >> 8;
#endif

	*usb_LinDBG_0 |= DISCARD_ZERO_LENGTH_QUALIFIER;

	if(usbMode == CONNECT_USB3)
	{
		*usb_DevCtrl_1 = USB2_FORCE_SUSP;
		//*usb_DevCtrlClr_2 =	USB2_PLL_FREERUN;	// disable U2 PLL Freerun
	}
	else
	{
		//if (revision_a61)
		{	// power down USB3.0 Phy
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|HW_RESTART_VREG_DISABLE|HW_RESTART_PLL_DISABLE|PD_USB3_TX|PD_USB3_RX|PD_VREG|PD_USB3_PLL);
			//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|PD_USB3_TX|PD_USB3_RX);
		}
	}

	#ifdef UAS_TOUT
		timerExpire = 0;
	#endif

	#ifdef UAS_TOUT
		uas_dead_time_out =0;
	#endif

	while(1)
	{
//begin:
	#ifdef UAS_TOUT
		if (timerExpire == 1 )
		{
			dump_reg();
			sata_Reset(SATA_HARD_RST);
			*usb_DevCtrlClr_0 = USB_ENUM;

			while (1)
			{
			}			
		}
	#endif

		if (USB_VBUS_OFF())
		{
#ifdef DBG_FUNCTION
			EAL = 0;
#endif
			//DBG(("UV off\n"));
#ifdef PWR_SAVING
			//usb20_clock_enable();
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|PD_USB3_TX|PD_USB3_RX);
#endif
			usb_active = 0;
			return;
		}

		if (usb_active == 0)
			return;

		if((tmp8 = *cpu_wakeup_ctrl_0) & (CPU_USB_SUSPENDED))
		{
		#ifdef USB2_L1_PATCH		
			if (tmp8 & CPU_USB2_L1_SLEEP)
			{
				//DBG(("U L1\n"));
				//*cpu_wakeup_ctrl_0 = CPU_USB2_L1_SLEEP;

				while (1)
				{
					if (USB_VBUS_OFF())
					{
						EAL = 0;
						spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|PD_USB3_TX|PD_USB3_RX);
						EAL = 1;
						//DBG0(("\tH VB OF1\n"));
						//bot_usb_bus_reset();
						usb_active = 0;
						return; //	power down funciton implement in usb_main()
					}
					if (*cpu_wakeup_ctrl_0 & CPU_USB_UNSUSPENED)
					{
						usb_rej_L1_in_io_transfer();

						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;
						*usb_IntStatus_0 = USB_SUSPEND;
						
						//DBG0(("\tRes\n"));

						//goto begin;
						break;
					}			
					if ((*usb_IntStatus_shadow_0 & (USB_BUS_RST |WARM_RESET)))
					{
						EAL = 0;
						spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
						// Enable USB2.0 PLL
						//*usb_DevCtrl_2 = USB2_PLL_FREERUN;
						EAL = 1;
#ifdef EQ_SETTING_TEST
						if(usbMode = CONNECT_USB3)
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

						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED;
						*usb_IntStatus_0 = USB_SUSPEND;
						//Delay(1);
						if ((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED) == 0)
						{
							//DBG0(("abnormal wake\n"));
							break;
						}
					}
				}
			}
			else
		#endif
			{
				if (usb_suspend(0))
					return;
			}
		}	//if((tmp8 = *cpu_wakeup_ctrl_0) & (CPU_USB_SUSPENDED))		

/////////////end of check suspend

		if (*chip_IntStaus_0 & USB_INT)
		{
			// USB 2.0 BUS_RST--------------
			// USB 3.0 Hot or Warm Reset--------------
			tmp8 = *usb_IntStatus_shadow_0;
			if (tmp8 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
			{
#ifdef ASIC_CONF
				EAL = 0;
				//if (revision_a61)
				{
					// Enable USB3.0 PLL
					spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
				}
				EAL = 1;
#endif
				// Enable USB2.0 PLL
				//*usb_DevCtrl_2 = USB2_PLL_FREERUN;
				*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
#ifdef EQ_SETTING_TEST
				if(usbMode = CONNECT_USB3)
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

				uas_bus_reset();
				return;
			}

			usb_int_Stat_1 = *usb_IntStatus_shadow_1;
			if (usb_int_Stat_1 & CTRL_INT)
			{	
				if (*usb_Msc0IntStatus_1 & MSC_TXSENSE_DONE_INT) 
				{
					uas_tx_sense_done();
				}
				else if (*usb_Msc0IntStatus_0 & MSC_RXSENSE_DONE_INT)
				{
					uas_rx_sense_done();
				}

				// EP0_SETUP----------------
				if (*usb_Ep0Ctrl & EP0_SETUP)
				{	
				   //check_usb_mode();

					//DBG(("U ep_stp\n"));
					usb_control();
				}
			}

			/****************************************\
				USB Command IU 
			\****************************************/

			//don't enter u1 u2 when there is command
	#if 1
			if ((uas_ci_paused == 0) &&
				((sobj_cFree1.udata.u_8[0] | sobj_cFree1.udata.u_8[1]|sobj_cFree1.udata.u_8[2]|sobj_cFree1.udata.u_8[3]) != 0) &&
				(usb_int_Stat_1 & CDB_AVAIL_INT))
			{
			#ifdef USB2_L1_PATCH
				if (usbMode == CONNECT_USB2)
				{
					usb_rej_L1_in_io_transfer();
				}
			#endif

				if (*usb_Msc0IntStatus_1 & MSC_TXSENSE_DONE_INT) 
				{
					uas_tx_sense_done();
				}
				else if (*usb_Msc0IntStatus_0 & MSC_RXSENSE_DONE_INT)
				{
					uas_rx_sense_done();
				}

#ifdef REJECT_U1U2
				if (u1U2_reject_state == U1U2_ACCEPT)
				{
					tmp8 = *usb_USB3LTSSM_0;
					*usb_DevCtrl_1 = USB3_U1_REJECT;
					*usb_DevCtrl_2 = USB3_U2_REJECT;
					if ((tmp8 == LTSSM_U1) || (tmp8 == LTSSM_U2))
					{
						*usb_DevCtrl_1 = USB3_U1U2_EXIT;
					}
					u1U2_reject_state = U1U2_REJECT;	
					U1U2_ctrl_timer = U1U2_REJECT_TIME;
				}
#endif

				//don't enter u1 u2 when there is command
//				*usb_DevCtrl_1 = USB3_U1_REJECT;
//				*usb_DevCtrl_2 = USB3_U2_REJECT;
				
				ctxt_site = *usb_CtxtAvailFIFO;
				*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;
				SCTXT_Flag = 0;

			#if 1
				val = *ctxt_Flag;
				if (val & CTXT_FLAG_SIG_ERR)
				{	//Tag field of UAS IU is zero
					//Ignore it by return it back to Free FIFO 
					*usb_CtxtFreeFIFO = ctxt_site;
					*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
					goto chk_msc_int;
				}
				if (val & CTXT_FLAG_INVALID_IU)
				{	// 4.36 & 4.51
					uas_send_response_iu(InvalidInformationUnit, *ctxt_ITag_0);
					*usb_CtxtFreeFIFO = ctxt_site;
					*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
					goto chk_msc_int;
				}
			#endif

				val = *ctxt_ITag_0;
				if (val == 0)
				{
					//DBG0(("Inv Tag\n"));
					*usb_CtxtFreeFIFO = ctxt_site;
					*usb_Msc0LunCtxtUsed_0 = 0xFF;	// -1
					goto chk_msc_int;
				}
				//SCTXT_Tag = val;

		#ifdef UAS_OVERLAPPED_TAG
				// Is tag already in use by another command ?
				tmp8 = 1 << (val & 0x07);
				i8 = (val >> 3) & 0x03;
				if (uas_cmd_tag.udata.u_8[i8] & tmp8)
				{
					// save current Ctxt
					save_ctxt_site = ctxt_site;

					//a) abort all task management functions received
					//uas_abort_all_task_manag();
					MSG(("COv %BX %BX", *ctxt_ITag_0, SCTXT_Tag));
					MSG((" %BX%BX%BX%BX\n", uas_cmd_tag.udata.u_8[3], uas_cmd_tag.udata.u_8[2], uas_cmd_tag.udata.u_8[1], uas_cmd_tag.udata.u_8[0]));
					sata_abort_all();
					uas_abort_all();

					// restore current Ctxt
					ctxt_site = save_ctxt_site;
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;
					SCTXT_Tag = *ctxt_ITag_0;

					//b) respond to the overlapped command with ABORTED COMMAND and ASC(OVERLAPPED COMMANDS ATTEMPTED)
					hdd_err_2_sense_data(ERROR_OVERLAPPED_CMD);
					uas_device_no_data();
					goto chk_msc_int;
				}
				// Is tag already in use by another a task management function ?
				else if (uas_riu_Tag == val)
				{
					SCTXT_Tag = val;

					// save tag
					save_tag = val;

					MSG(("c>T %BX\n", val));
					//a) abort all commands received
					//uas_abort_all_task_set();
					sata_abort_all();
					uas_abort_all();

					//b) abort all task management functions received
					//uas_abort_all_task_manag();

					//c) requesting that the target port set the RESPONSE CODE field set to "OVERLAPPED TAG ATTEMPTED"
					uas_send_response_iu(OverlappedTagAttempted, save_tag);

					goto chk_msc_int;
				}
				uas_cmd_tag.udata.u_8[i8] |= tmp8;
		#endif	//#ifdef UAS_OVERLAPPED_TAG

				SCTXT_Tag = *ctxt_ITag_0;

				CmdBlk(0) = ctxt_CDB[0];
				//MSG(("CIU:%bx\n", CmdBlk(0)));
				//MSG(("CIU:%bx %bx %bx\n", CmdBlk(0), ctxt_site, *ctxt_ITag_0));
				//MSG(("C:%bx\n", CmdBlk(0)));

				//SCTXT_DbufIndex = DBUF_SEG_NULL;
				//SCTXT_Status = CTXT_STATUS_PENDING;
				SCTXT_Status = CTXT_STATUS_GOOD;
				SCTXT_LunStatus = LUN_STATUS_GOOD;
				//SCTXT_CCMIndex = SCM_NULL;

start_scsi_cmd:
				switch (CmdBlk(0))
				{
					case SCSI_READ6:
					case SCSI_READ10:
					case SCSI_READ12:
					case SCSI_READ16:
					{
						/****************************************\
						 SCSI_READ Commands
						\****************************************/
						if (*ctxt_SatFailReason & (SAT_LBA_OVRN|SAT_LBA_ERR|SAT_SECCNT_ERR))
						{
							//DBG(("Bad Rd cmd %bx\n", *ctxt_SatFailReason));
							//*ctxt_Next = CTXT_NULL;
							hdd_err_2_sense_data(ERROR_ILL_CDB);
							uas_device_no_data();
							break;
						}

						// check SATA Transfer block count
						if (sobj_ncq_mode)
						{
							if ((ctxt_FIS[FIS_FEATURE] == 0) && (ctxt_FIS[FIS_FEATURE_EXP] == 0))
							{// the sector_CNT is in Feature & Feature_EXP byte of FIS27 for FPDMA
							#if 0
								mem_dump(ctxt_FIS, 16);
							#endif							
								goto uas_nodata;
							}
						}
						else
						{
							if ((ctxt_FIS[FIS_SEC_CNT] == 0) && (ctxt_FIS[FIS_SEC_CNT_EXP] == 0))
							{// the sector_CNT is in Feature & Feature_EXP byte of FIS27 for FPDMA
							#if 0
								mem_dump(ctxt_FIS, 16);
							#endif							
								goto uas_nodata;
							}
						}

	#if 1
						if (sobj_State < SATA_STANDBY)
						{
						#ifdef PWR_SAVING
							if (sobj_State == SATA_PWR_DWN)
							{
								//sata_pwr_up();
								hdd_err_2_sense_data(ERROR_UA_NOT_READY_INIT);
								uas_device_no_data();
								break;
							}
						#endif
							//DBG(("RD %bx %bx\n", sobj_State, sobj_State_tout));

							hdd_que_ctxt_site = ctxt_site;
							hdd_que_ctxt_tout = HDD_UAS_QUE_TOUT;					// 

							uas_ci_paused = 1;
							uas_ci_paused_ctxt = ctxt_site;
							break;
						}
	#else
						if (hdd_chk_drv_state())
						{
							DBG0(("RD %bx %bx\n", sobj_State, sobj_State_tout));
							uas_ci_paused = 1;
							uas_ci_paused_ctxt = ctxt_site;
							break;
						}
	#endif	//ifdef PWR_SAVING
						
						if (uas_ci_paused)
						{
							//DBG0(("\trd\n"));
							uas_ci_paused = 0;
						}

						if (sobj_ncq_mode)
						{
							SCTXT_Flag |= (SCTXT_FLAG_NCQ|SCTXT_FLAG_DIN);			// Data-in
							//*ctxt_CCMcmdinten = 0; 
							//*ctxt_Status = CTXT_STATUS_PENDING;
							if (sata_exec_ncq_ctxt() == CTXT_STATUS_ERROR)
							{	// sense Sense IU if error return 
								uas_ci_paused = 1;
								uas_ci_paused_ctxt = ctxt_site;
								uas_device_no_data();
								break;
							}
						}
						else
						{	// SATA (non ncq) DMAE-IN command
							SCTXT_Flag |= SCTXT_FLAG_DIN;
							SCTXT_DbufIndex	= DBUF_SEG_S2U;
							SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) |  TX_DBUF_SATA0_W_PORT;

							//*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 
							uas_ci_paused = 1;
							uas_ci_paused_ctxt = ctxt_site;

							sata_exec_dmae_RW_ctxt();
						}
						break;
					}
				
					

					case SCSI_WRITE6:
					case SCSI_WRITE10:
					case SCSI_WRITE12:
					case SCSI_WRITE16:
					{
						/****************************************\
						 SCSI_WRITE Commands
						\****************************************/
						if (*ctxt_SatFailReason & (SAT_LBA_OVRN|SAT_LBA_ERR|SAT_SECCNT_ERR))
						{
							//DBG(("Bad Wr cmd %bx\n", *ctxt_SatFailReason));
							//*ctxt_Next = CTXT_NULL;
							hdd_err_2_sense_data(ERROR_ILL_CDB);
							uas_device_no_data();
							break;
						}

						if (sobj_ncq_mode)
						{
							if ((ctxt_FIS[FIS_FEATURE] == 0) && (ctxt_FIS[FIS_FEATURE_EXP] == 0))
							{// the sector_CNT is in Feature & Feature_EXP byte of FIS27 for FPDMA
								goto uas_nodata;
							}
						}
						else
						{
							if ((ctxt_FIS[FIS_SEC_CNT] == 0) && (ctxt_FIS[FIS_SEC_CNT_EXP] == 0))
							{// the sector_CNT is in Feature & Feature_EXP byte of FIS27 for FPDMA
							#if 0
								mem_dump(ctxt_FIS, 16);
							#endif							
	uas_nodata:
								//DBG(("_nodata\n"));
								//SCTXT_Status = LUN_STATUS_GOOD;
								SCTXT_LunStatus = LUN_STATUS_GOOD;
								uas_device_no_data();
								break;
							}
						}

	#if 1
						if (sobj_State < SATA_STANDBY)
						{
						#ifdef PWR_SAVING
							if (sobj_State == SATA_PWR_DWN)
							{
								//sata_pwr_up();
								hdd_err_2_sense_data(ERROR_UA_NOT_READY_INIT);
								uas_device_no_data();
								break;
							}
						#endif
							//DBG(("WR %bx %bx\n", sobj_State, sobj_State_tout));

							hdd_que_ctxt_site = ctxt_site;
							hdd_que_ctxt_tout = HDD_UAS_QUE_TOUT;					// 

							uas_ci_paused = 1;
							uas_ci_paused_ctxt = ctxt_site;
							break;
						}
	#else
						if (hdd_chk_drv_state())
						{
							uas_ci_paused = 1;
							uas_ci_paused_ctxt = ctxt_site;
							break;
						}
	#endif	

						if (uas_ci_paused)
						{
							//DBG0(("\twr\n"));
							uas_ci_paused = 0;
						}

						hdd_post_write = 1;
						if (sobj_ncq_mode)
						{
							SCTXT_Flag |= SCTXT_FLAG_NCQ;			// NCQ & Data-Out
							//*ctxt_Index = DBUF_SEG_NULL;
							//*ctxt_CCMcmdinten = 0; 
							//*ctxt_Status = CTXT_STATUS_PENDING;
		//					*ctxt_LunStatus = LUN_STATUS_GOOD;
							if (sata_exec_ncq_ctxt() == CTXT_STATUS_ERROR)
							{	// sense Sense IU if error return 
								uas_ci_paused = 1;
								uas_ci_paused_ctxt = ctxt_site;
								uas_device_no_data();
								break;
							}
						}
						else
						{	// SATA (non ncq) DMAE-Out command
							SCTXT_DbufIndex = DBUF_SEG_U2S;
							SCTXT_DbufINOUT = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_USB_W_PORT;

							//*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 
							uas_ci_paused = 1;
							uas_ci_paused_ctxt = ctxt_site;

							sata_exec_dmae_RW_ctxt();
						}
						break;
					}
					
					/****************************************\
					other commands
					READ6, WRITE6, VERIFY, INQUIRY, READ_CAPACITY,
					REQUEST_SENSE, MODE_SENSE6, MODE_SELECT6, WRITE_BUFFER10, READ_BUFFER10,
					TEST_UNIT_READY, SYNCHRONIZE_CACHE, REZERO, PREVENT_MEDIUM_REMOVAL,
					START_STOP_UNIT, and FORMAT_UNIT
					\****************************************/

					default:
						CmdBlk(1) = ctxt_CDB[1];
						CmdBlk(2) = ctxt_CDB[2];
						CmdBlk(3) = ctxt_CDB[3];
						CmdBlk(4) = ctxt_CDB[4];
						CmdBlk(5) = ctxt_CDB[5];
						CmdBlk(6) = ctxt_CDB[6];
						CmdBlk(7) = ctxt_CDB[7];
						CmdBlk(8) = ctxt_CDB[8];
						CmdBlk(9) = ctxt_CDB[9];
						CmdBlk(10) = ctxt_CDB[10];
						CmdBlk(11) = ctxt_CDB[11];
						CmdBlk(12) = ctxt_CDB[12];
						CmdBlk(13) = ctxt_CDB[13];
						CmdBlk(14) = ctxt_CDB[14];
						CmdBlk(12) = ctxt_CDB[12];
						CmdBlk(13) = ctxt_CDB[13];
						CmdBlk(14) = ctxt_CDB[14];
						CmdBlk(15) = ctxt_CDB[15];

						hdd_StartAtaCmd();
				}	// switch (pCtxt->CTXT_CDB[0])
				//*usb_IntStatus_1 = CDB_AVAIL_INT|MSC0_INT;
			}	//	if (((uas_ci_paused == 0) && (*usb_IntStatus & CDB_AVAIL_INT))
	#endif

	#if 1
chk_msc_int:
			if (usb_int_Stat_1 & MSC0_INT)
			{
				//u8	 msc_int_status_1;
				EAL = 0;
				tmp8 = *usb_Msc0IntStatus_0;
				msc_int_status_1 = CACHE_BYTE1;
				EAL = 1;

				if (tmp8 & MSC_RXSENSE_DONE_INT)
				{
					uas_rx_sense_done();
				}
				else if (tmp8 & (MSC_TX_DONE_INT|MSC_RX_DONE_INT))
				{
					usb_msc_isr();
				}

				//msc_int_status_1 = *usb_Msc0IntStatus_1;

				if (msc_int_status_1 & MSC_RESP_DONE_INT)
				{
					//clear the task bit 
					*usb_Msc0IntStatus_1 = MSC_RESP_DONE_INT;
					if ((tmp8 = *usb_Msc0StatCtrlClr) & MSC_STAT_FLOW_CTRL)
					{
						*usb_Msc0StatCtrlClr = MSC_STAT_FLOW_CTRL;
					}
					// After RIU done, restore Dead timeout to 1280 * 100us = 128ms
					*usb_DeadTimeout_0 = 1280 & 0xff;
					*usb_DeadTimeout_1 = 1280 >> 8;
					#ifdef UAS_OVERLAPPED_TAG
							uas_riu_Tag = 0;
					#endif
				}

				if (msc_int_status_1 & MSC_TXSENSE_DONE_INT) 
				{	// 	UAS Sense IU Transfer thru TX(DI) context  has completed.
					uas_tx_sense_done();
				}

				//check if management_IU posted
				if (msc_int_status_1 & MSC_TASK_AVAIL_INT)
				{
					{
						//management IU need to take care
						UAS_Task_Manage_IU();
	//					UAS_Send_Sense_IU();
					}
				}

				if (msc_int_status_1 & (MSC_ST_TIMEOUT_INT|MSC_DI_TIMEOUT_INT|MSC_DO_TIMEOUT_INT))
				{
					if (usb_curCtxt == CTXT_NULL)
					{
						if  ((msc_int_status_1 & MSC_ST_TIMEOUT_INT) && (*usb_Msc0StatCtrl & MSC_STAT_RESP_RUN))
						{
#if 1
							uas_status_dead_time_out();
#else
							goto st_flow_tout;
#endif
						}
						
						*usb_Msc0IntStatus_1 = MSC_ST_TIMEOUT_INT|MSC_DI_TIMEOUT_INT|MSC_DO_TIMEOUT_INT;
					}
					else
					{
						if (msc_int_status_1 & MSC_ST_TIMEOUT_INT)
						{
#if 1
							uas_status_dead_time_out();
#else
st_flow_tout:
							*usb_Msc0IntStatus_1 = MSC_ST_TIMEOUT_INT;
							tmp8 = *usb_Msc0StatCtrl;
		DBG(("mStCt %BX ", tmp8));

							if ((tmp8 & (STATUS_ACKE|STATUS_ACKR)) && 
								(tmp8 & (MSC_STAT_RESP_RUN|MSC_STAT_TXSENSE_RUN|MSC_STAT_RXSENSE_RUN)) )
							{
								DBG(("mStCt\n"));
								//tmp8 &= MSC_STAT_HOST_WAIT;
								//if ((tmp8 == STATUS_ACKE) || (tmp8 == STATUS_ACKR))
								{
									if (uas_st_tout_cnt == 0)
										goto si_flow_ctrl;

									uas_st_tout_cnt--;
									if (uas_st_tout_cnt == 0)
									{
										if (tmp8 & (MSC_STAT_TXSENSE_RUN|MSC_STAT_RXSENSE_RUN))
										{
											*usb_Msc0StatCtrl = MSC_STAT_HALT;

											*usb_Msc0StatCtrlClr = MSC_STAT_HALT;
											*usb_Msc0StatCtrlClr;
											//if (usb_curCtxt != CTXT_NULL)
											{
												ctxt_site = usb_curCtxt;
												uas_push_ctxt_site_to_free_fifo();
												usb_curCtxt = CTXT_NULL;
											}
											//abort all commands in the task set and all commands that 
											//the target port receives until the UAS target port is able to terminate a command 
											//with CHECK CONDITION status 
											//with the sense key set to UNIT ATTENTION 
											//with the additional sense code set to COMMANDS CLEARED BY DEVICE SERVER
											sata_abort_all();
											uas_abort_all();
											uas_init();
										}
										else //if (tmp8 & MSC_STAT_RESP_RUN )
										{
										#if 0
											*usb_Msc0StatCtrlClr = MSC_STAT_RESP_RUN;  //clear it
										#else
											*usb_Msc0StatCtrl = MSC_STAT_RESET;
										#endif
											uas_riu_Tag = 0;
										}
										//else
										//{
										//	*usb_Msc0StatCtrl = MSC_STAT_RESET;	
										//}
									}
									else
									{
si_flow_ctrl:
										*usb_Msc0StatCtrl = MSC_STAT_FLOW_CTRL;
										DBG0(("SI_F\n"));
										//*host_ctxtmem_ptr = usb_curCtxt;
										//mem_dump(ctxt_CDB, 10);	
									#ifdef UAS_TOUT
										uas_dead_time_out++;
										if (uas_dead_time_out > 200)
										{
											dump_reg();
											*usb_DevCtrlClr_0 = USB_ENUM;
		
											while (1)
											{
											}
										}
									#endif	//	#ifdef UAS_TOUT
									}
								}	
							}
							else
							{
								*usb_Msc0StatCtrlClr = MSC_STAT_FLOW_CTRL;// clear the Flow control
							}
#endif
						}

						if (msc_int_status_1 & MSC_DI_TIMEOUT_INT)
						{
							tmp8 = *usb_Msc0DIXferStatus & MSC_DATAIN_HOST_WAIT;
							val = CACHE_BYTE0;		//	read usb_Msc0DICtrl	
							*usb_Msc0IntStatus_1 = MSC_DI_TIMEOUT_INT;
							//DBG(("DI_To %BX ", tmp8));

							// chk Host_Wait
							//if ((tmp8 >= ACKE) && (tmp8 != ACK1))
							if (tmp8)
							{
							#ifdef UAS_DI_TOUT
								uas_xfer_to_cnt++;
								if (uas_xfer_to_cnt > 100)
								{
									uas_xfer_tout();
								}
								else
							#endif
								{
									//if ((*usb_Msc0DICtrl & MSC_TXFIFO_EMPTY) == 0)
									{
										//uas_di_dump();
										//DBG0(("SI %BX\n", *usb_Msc0StatCtrl));
										if ((val & MSC_DIN_FLOW_CTRL) == 0)
										{	// read Host_Wait & Flow_Ctrl again
												tmp8 = *usb_Msc0DIXferStatus & MSC_DATAIN_HOST_WAIT;
												val = CACHE_BYTE0;		//	read usb_Msc0DICtrl	
												if (tmp8 == 0)
													goto inv_Host_wait;
										}

										if (val & MSC_DIN_FLOW_CTRL)
										{
												//DBG0(("di %BX %BX\n", tmp8, val));
												// Reset Host Wait
												*usb_Msc0DIXferStatus = 0;
										}
										else
										{	// Set Flow_Ctrl
												//DBG0(("DI_F %BX %BX\n", tmp8, *usb_Msc0DICtrl));
												*usb_Msc0DICtrl = MSC_DIN_FLOW_CTRL;
										}
									}
								}
							}
							else
							{	// invalid_Host_Wait
inv_Host_wait:
								//DBG0(("DI %BX %BX\n", tmp8, val));
								//*usb_Msc0DIXferStatus = 0;
							}
						}	// if (msc_int_status_1 & MSC_DI_TIMEOUT_INT)
					
						if (msc_int_status_1 & MSC_DO_TIMEOUT_INT)
						{
							tmp8 = *usb_Msc0DOXferStatus;
							val = CACHE_BYTE0;		//	read usb_Msc0DOCtrl	
							*usb_Msc0IntStatus_1 = MSC_DO_TIMEOUT_INT;
							//DBG0(("DO_To %bx ", tmp8));
						//#ifdef DEBUG_DP
						#if 0
							uas_timeout++;
							if (uas_timeout > 16)
							{
								dump_reg();
								*usb_DevCtrlClr_0 = USB_ENUM;
								while (1);
							}
						#endif
							//if ((tmp8 & MSC_DATAOUT_HOST_WAIT) == DPR)
							if ((tmp8 & MSC_DATAOUT_HOST_WAIT) >= DPE)
							{
								//tmp8 = *usb_Msc0DOXferStatus;
								*usb_Msc0DOutCtrl = MSC_DOUT_FLOW_CTRL;
								//DBG(("DO_TO C %bx\n", tmp8));
							}
							else
							{
								//DBG0(("%BX\n", *usb_Msc0DOutCtrl));
							}
						}
					}
				}	
			}	// if ((usb_int_Status & MSC0_INT)
	#endif
		}	//		if (*chip_IntStaus_0 & USB_INT)

		if (*chip_IntStaus_0 & SATA0_INT)
		{
			sata_isr();

	#ifdef USB_FAST_ENUM
			if (sobj_init == 0)
			{
				if (sobj_State == SATA_DRV_RDY)
				{
					if (sobj_class == org_sobj_class)
					{
						if (ata_fast_enum_init())
							return;
					}
					else
					{
						// Not ATA Device
						usb_wrong_sata_device();
						return;
					}
				}
			}
	#endif
			if (hdd_que_ctxt_site != CTXT_NULL)
			{
				if ((sobj_State == SATA_STANDBY) ||(sobj_State == SATA_READY))
				{
					ctxt_site = hdd_que_ctxt_site;
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;

					hdd_que_ctxt_site = CTXT_NULL;
					CmdBlk(0) = ctxt_CDB[0];
					goto start_scsi_cmd;
				}
			}
			else if (hdd_que_dout_ctxt != CTXT_NULL)
			{
				if ((sobj_State == SATA_READY) || (sobj_State == SATA_STANDBY))
				{
//DBG(("\tStart DO\n"));
					ctxt_site = hdd_que_dout_ctxt;
					hdd_que_dout_ctxt = CTXT_NULL;

					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;

					usb_append_ctxt_que();
				}
			}
		}

	#if 0
		if(usbMode == CONNECT_USB3)
		{
			if ((*usb_DevCtrl_1 & USB2_FORCE_SUSP) == 0)
			{
				*usb_DevCtrl_1 = USB2_FORCE_SUSP;
			}
		}
	#endif

	#ifdef USB_FAST_ENUM
		if (sobj_State == SATA_DRV_RDY)
		{
			#ifdef USB2_L1_PATCH
			if (usbMode == CONNECT_USB2)
			{
				usb2_L1_reject_timer = 10;
			}
			#endif
			if (ata_fast_enum_init())
				return;
		}
		else
	#endif
		if (sobj_State >= SATA_STANDBY)
		{
			if ((*sata_PhyStat & PHYRDY) == 0)
			{
				if (sobj_State > SATA_READY)
				{
					//DBG0(("S UP\n"));
					sobj_State = SATA_RESETING;
					usb_active = 0;
					return;
				}
				else
				{
					//tmp8 = vendor;
					sata_Reset(SATA_HARD_RST);
				}
			}

		}

	#ifdef REJECT_U1U2
		if (sobj_State > SATA_READY)
		{
			U1U2_ctrl_timer = U1U2_REJECT_TIME;
		}
	#endif
		if (usb_post_dout_ctxt != CTXT_NULL)
		{
			ctxt_site = usb_post_dout_ctxt;
			*host_ctxtmem_ptr = ctxt_site;
			SCTXT_INDEX = ctxt_site;

			usb_post_dout_ctxt = CTXT_NULL;
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

#if 0
			if ((ctxt_site != usb_curCtxt))
			{
				DBG0((" usb_curCtxt != usb_post_dout_ctxt\n"));
			}
#endif
			hdd_post_data_out();
		}

		if (hdd_tick)
		{
			hdd_tick_isr();
			hdd_tick = 0;
			if (usb_active == 0)
				return;
			if (hdd_que_ctxt_site != CTXT_NULL)
			{
				if (hdd_que_ctxt_tout)
				{
					hdd_que_ctxt_tout--;
					if (hdd_que_ctxt_tout == 0)
					{
						ctxt_site = hdd_que_ctxt_site;
						*host_ctxtmem_ptr = ctxt_site;
						SCTXT_INDEX = ctxt_site;

						hdd_que_ctxt_site = CTXT_NULL;

		//DBG0(("HD tou %bx %bx\n", sobj_State, sobj_State_tout));
						// Time out
						if (sobj_State > SATA_READY)
						{
							hdd_err_2_sense_data(ERROR_ILL_CDB);
						}
						else
						{
							hdd_err_2_sense_data(ERROR_UA_BECOMING_READY);
						}

						usb_device_no_data();
					}
				}
			}
			else if (hdd_que_dout_ctxt != CTXT_NULL)
			{
				if (hdd_que_dout_tout)
				{
					hdd_que_dout_tout--;
					if (hdd_que_dout_tout == 0)
					{
						ctxt_site = hdd_que_dout_ctxt;
						hdd_que_dout_ctxt = CTXT_NULL;

						*host_ctxtmem_ptr = ctxt_site;
						SCTXT_INDEX = ctxt_site;


		//DBG0(("DO tou %bx\n", sobj_State));
						// Time out
						hdd_err_2_sense_data(ERROR_UA_NOT_READY);
						usb_device_no_data();
					}
				}
			}
			if (sobj_init == 0)
			{
				if (sobj_init_tout)
				{
					sobj_init_tout--;
				}
				else
				{
					sobj_init = 1;
					sobj_State = SATA_NO_DEV;
					usb_active = 0;
					return;
				}
			}
		}	// if (hdd_tick)
	} // End of while loop
}
#endif
