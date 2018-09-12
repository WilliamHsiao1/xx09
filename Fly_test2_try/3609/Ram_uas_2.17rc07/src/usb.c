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
 * 3610		2010/04/27	Odin		USB2.0 BOT Debugging, 
 *                                     check sata status when wait_for_done
 *                                     setup "usb_Msc0Residue" at CSW command
 * 3607		2010/11/20	Odin		AES function
 * 3609		2010/12/21	Ted			Remove AES function 
 *
 *****************************************************************************/
 
#define USB_C

#include	"general.h"



/****************************************\
	UsbExec_Init
\****************************************/
void UsbExec_Init(void)
{
	//DBG(("Exec_Init\n"));

	//set USB23_DETECT_flag at initial. by Ted:2010/12/09
	//USB23_DETECT_flag = 1;		
	
	InitDesc();
	if (cfa_active == 0)
	{
		sata_InitSataObj();
	}

	//usb_reenum_flag = 0;

#if 1
	dbuf_Reset();
	//dbuf_bot_reset();
#else
	*dbuf_MuxSel = 0;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxSel = 1;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxSel = 2;
	*dbuf_MuxCtrl = SEG_RESET;
#endif		

	*usb_Msc0IntStatus_0 = 0xff;
	*usb_Msc0IntStatus_1 = 0xff;
	*usb_Msc0IntStatus_2 = 0xff;

	*usb_IntStatus_0 = 0xff;
	*usb_IntStatus_1 = 0xff;
	*usb_IntStatus_2 = 0xff;

	//*usb_CoreCtrl_1 |= VBUS_RESET_EN;
	
	//*usb_DeviceRequestEn_0 = 0xFFF7;
	//*usb_DeviceRequestEn_1 = 0xFF;

#if 0
	pTmp = (u8*)(HOST_CONTEXT_ADDR);
	 
	for(j=0; j<64; j++)
	{
		*pTmp = 0x55;
		pTmp++;
	}
	 
	pTmp = (u8*)(HOST_CONTEXT_ADDR);
	DBG(("addr:%X\n", pTmp));
#endif
#if 0	
	DBG(("*****************************************\n"));
	for(i=0; i<2; i++)
	{
		tmp8 = *pTmp;
		pTmp++;	
	    DBG(("%BX ", tmp8));
	}
	//DBG(CmdBlk(0));
	DBG(("******************************************\n"));
#endif
			
	usb_power_on_flag = 1;

#ifdef DEBUG_LTSSM
	usb3_ts1_timer = 90;
#else
	usb3_ts1_timer = 8;
#endif

#ifdef HW_CAL_SSC
	start_HW_cal_SSC = 1;
#endif

	usb3_test_mode_enable = 0;
	usbMode = CONNECT_UNKNOWN;
	usb3_u2_enable = 0;
	usb3_function_suspend = 0;
	usb3_device_U2_state = 0;
	usb3_function_suspend = 0;
	usb3_device_U2_state = 0;
	usb3_U2_inactivity_timer = 0;
#ifdef USB3_TX_DBG	
	recovery_count = 0;
	count_recvry_en = 1;
#endif


}

/****************************************\
	usb_reenum
\****************************************/
void usb_reenum()
{
	*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
	*usb_DevCtrlClr_0 = USB_ENUM;
	
	usb_reenum_flag = 0;
	fast_enum_usb_flag = 0;
	usb_BusPwr_FastEnum = 0;
	InitDesc();

	*usb_DevCtrl_1 = USB_CORE_RESET;
	MSG(("Renu\n"));
	Delay(600);
	*usb_DevCtrl_0 = USB_ENUM;
}

/****************************************\
	usb_rx_fifo_rst
\****************************************/
void usb_rx_fifo_rst()
{
	*usb_Msc0DOutCtrlClr= MSC_RXFIFO_RESET;
	*usb_Msc0DOutCtrlClr;

}
#ifdef UAS_EN

void usb_rst_do()
{
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
		//DBG(("Do Rst %bx\n", *usb_Msc0IntStatus_0));
}
#endif
/****************************************\
	usb_tx_ctxt_send_status
\****************************************/
void usb_tx_ctxt_send_status()
{
#ifdef UAS_EN
	if (!bot_mode)
	{
		//uas_din_send_sense_iu();
		uas_send_sense_iu();
		#ifdef UAS_TOUT
			timerCnt = 0;
		#endif
	}
	else
#endif
	{
		// send BOT csw
		*usb_Msc0DIStatus = SCTXT_Status;
		*usb_Msc0DICtrl = MSC_CSW_RUN;

		#ifdef BOT_TOUT
			timerCnt = 0;
		#endif
	}
}

/*********************************************************************\
	usb_exec_tx_ctxt

		ctxt_site: site # of CDB Context for UAS/BOT command
		*host_ctxtmem_ptr: site # of CDB Context for UAS/BOT command
		SCTXT_INDEX: site # of CDB Context scratch for UAS/BOT command
\*********************************************************************/
#if 1
void usb_exec_tx_ctxt()
{

	//val = *ctxt_No_Data;

	// sets up the USB Transfer by loading the MSCn TX Context Register  
	//DBG(("set TxCtxt valid\n"));
	
	//DBG0(("TV:%BX, %BX\n", ctxt_site | (SCTXT_Flag & SCTXT_FLAG_NO_DATA), SCTXT_DbufIndex));
	*usb_Msc0TxCtxt_0 = ctxt_site | (SCTXT_Flag & SCTXT_FLAG_NO_DATA);	//3609

	// Set up Dbuf if necessary
	if (SCTXT_DbufIndex != DBUF_SEG_NULL )
	{
		*dbuf_MuxSel = SCTXT_DbufIndex;
		*dbuf_MuxInOut = (SCTXT_DbufINOUT & 0xF0) | (*dbuf_MuxInOut & 0x0F); 
	}

#if 0
	// special case for Case 4
	if (ctxt_PhaseCase_0 & BIT_4)
	{	// case 4
		if ((*ctxt_XferLen_0 | *ctxt_XferLen_1  | *ctxt_XferLen_2 | *ctxt_XferLen_3) == 0)
			*usb_Msc0DICtrl = MSC_DIN_FLOW_CTRL;
	}
#endif

	// current Active CDB context for USB Core 
	usb_curCtxt = ctxt_site;	

#ifdef UAS_DI_TOUT
	uas_xfer_to_cnt = 0;
#endif

#ifdef UAS_TOUT
	uas_dead_time_out = 0;
#endif

	//if ((*ctxt_XferLen_0 | *ctxt_XferLen_1  | *ctxt_XferLen_2 | *ctxt_XferLen_3) == 0)
	if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
	{
		//DBG0(("un\n"));
		usb_tx_ctxt_send_status();
		return;
	}

		#ifdef UAS_TOUT
	if (!bot_mode)
	{
		if (SCTXT_Flag & SCTXT_FLAG_NCQ)
		{
			timerCnt = 80;			// 8 sec
		}
	}
#endif
}
#endif

/****************************************\
	usb_rx_ctxt_send_status

		ctxt_site: site # of CDB Context for UAS/BOT command
		*host_ctxtmem_ptr: site # of CDB Context for UAS/BOT command
\****************************************/
void usb_rx_ctxt_send_status()
{
	//3609 reset sata_SiteCmdIntEn to 0 after command done
	
#ifdef UAS_EN
	if (!bot_mode)
	{
		//uas_dout_send_sense_iu();
		uas_send_sense_iu();
		#ifdef UAS_TOUT
			timerCnt = 0;
		#endif
	}
	else
#endif
	{
		*usb_Msc0DIStatus = SCTXT_Status;
		*usb_Msc0DICtrl = MSC_CSW_RUN;

		#ifdef BOT_TOUT
			timerCnt = 0;
		#endif
	}
}




/****************************************\
	usb_exec_rx_ctxt

		ctxt_site: site # of CDB Context for UAS/BOT command
		*host_ctxtmem_ptr: site # of CDB Context for UAS/BOT command
		SCTXT_INDEX: site # of Scratch CDB Context(SCTXT) for UAS/BOT command
\****************************************/

void usb_exec_rx_ctxt()
{
	// sets up the USB Transfer by loading the MSCn RX Context Register
//	*host_ctxtmem_ptr = ctxt_site;
//	SCTXT_INDEX = ctxt_site;
	//DBG(("RxCtxt: %BX\n", ctxt_site | (SCTXT_Flag & SCTXT_FLAG_NO_DATA)));
	*usb_Msc0RxCtxt_0 = ctxt_site | (SCTXT_Flag & SCTXT_FLAG_NO_DATA) ;


	// current Active CDB context for USB Core 
	usb_curCtxt = ctxt_site;	

	if (((SCTXT_DbufIndex) != DBUF_SEG_NULL)  )
	{
		*dbuf_MuxSel = SCTXT_DbufIndex;
		*dbuf_MuxInOut = SCTXT_DbufINOUT; 
	}
#ifdef UAS_TOUT
	uas_dead_time_out = 0;
#endif

#ifdef UAS_TOUT
	if (!bot_mode)
	{
		if (SCTXT_Flag & SCTXT_FLAG_NCQ)
		{
			timerCnt = 80;			// 8 sec
		}
	}
#endif

}


/****************************************\
	usb_exec_rx_que_ctxt
\****************************************/
#ifdef UAS_EN
void usb_exec_que_ctxt()
{

	//DBG(("usb_exec_que_ctxt\n"));
	if ((ctxt_site = usb_ctxt_que) != CTXT_NULL)
	{	// un-queue CDB context from pending que for USB RX Contxt
		// and initiate USB xfer for CDB context 
//DBG0(("e_q_ct %bx\n", ctxt_site));

		*host_ctxtmem_ptr = ctxt_site;
		SCTXT_INDEX = ctxt_site;
		//DBG0(("1 DQ:%BX\n", ctxt_site));


		usb_ctxt_que = SCTXT_Next;
		SCTXT_Next = CTXT_NULL;
		//DBG0(("2 DQ:%BX\n", usb_ctxt_que));

		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{	// Data-in
			usb_exec_tx_ctxt();
		}
		else
		{
			usb_exec_rx_ctxt();
		}
	}
}
#endif



/*******************************************************************\
	usb_append_ctxt_que

	Input Global Parameter:
		ctxt_site: site # of CDB Context for UAS/BOT command
		*host_ctxtmem_ptr: site # of CDB Context for UAS/BOT command
\*******************************************************************/
#if 1
void usb_append_ctxt_que()
{
	u8	qCtxt;
	
#if 0
DBG0(("a_c %bx %bx\n", usb_curCtxt, ctxt_site));
if (usb_curCtxt == ctxt_site)
{
	//DBG0(("%bx %bx %bx %bx\n", *((u8 idata *)(SP)), *((u8 idata *)(SP+1)), *((u8 idata *)(SP+2)), *((u8 idata *)(SP+3)) ));
				dump_reg();
				*usb_DevCtrlClr_0 = USB_ENUM;
				while (1)
				{
				}
}
#endif
	// if there is no active CDB context for RX Context register
	// 
	if (usb_curCtxt == CTXT_NULL)
	{
		//*host_ctxtmem_ptr = ctxt_site;

	#ifdef UAS_EN
		//#ifdef DEBUG_DP
		#if 0
			uas_timeout++;
		#endif
	#endif
		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{	// Data-in
			usb_exec_tx_ctxt();
		}
		else
		{
			usb_exec_rx_ctxt();
		}
		return;
	}
	//DBG(("append to ctxt que %bx %bx %bx\n", usb_curCtxt, usb_ctxt_que, ctxt_site));
	{
		SCTXT_Next = CTXT_NULL;
		// pending que for USB TX Contxt
		if ((qCtxt = usb_ctxt_que) == CTXT_NULL)
		{
			//DBG0(("append to ctxt que, null, %bx\n", ctxt_site));
			//DBG0(("A2QN,%bx\n", ctxt_site));
			usb_ctxt_que = ctxt_site;
		}
		else
		{
			//DBG0(("A2Q,%bx %bx\n", usb_ctxt_que, ctxt_site));
			//*host_ctxtmem_ptr = qCtxt;
			SCTXT_INDEX = qCtxt;

			while (SCTXT_Next != CTXT_NULL)
			{
				qCtxt = SCTXT_Next;
				//*host_ctxtmem_ptr = qCtxt;
				SCTXT_INDEX = qCtxt;
			}
			// find the last CTXT in the USB Pending Queue
			// Append CTXT into Queue
			SCTXT_Next = ctxt_site;
		}
	}
}
#endif

/****************************************\
	usb_data_out_done
\****************************************/
void usb_data_out_done()
{
	usb_post_dout_ctxt = ctxt_site;
}

/****************************************\
  usb_device_no_data
   Dn
\****************************************/
#if 1
void usb_device_no_data()
{
//DBG0(("u_n_d\n"));

	SCTXT_CCMIndex = SCM_NULL;
	//*ctxt_DBufIndex = DBUF_SEG_NULL;
	SCTXT_DbufIndex = DBUF_SEG_NULL;
    
#ifdef UAS_EN
	if (!bot_mode)
	{
	    DBG(("uas_device_no_data\n"));
		uas_device_no_data();
	}
	else
#endif
	{
	    DBG(("bot_device_no_data\n"));
		bot_device_no_data();
	}
}
#endif

/****************************************\
	usb_device_data_in()
		Di or Dn

	Input Global Parameter:
		byteCnt:
		ctxt_site: site # of CDB Context for UAS 
\****************************************/
void usb_device_data_in()
{
	//DBG("data in %X\n", byteCnt);
	SCTXT_CCMIndex = SCM_NULL;
    DBG(("usb_device_data_in \n"));
#ifdef UAS_EN
	if (!bot_mode)
	{
	    DBG(("uas_device_data_in \n"));
		uas_device_data_in();
	}
	else
#endif
	{
	    DBG(("bot_device_data_in \n"));
		bot_device_data_in();
	}
}


/****************************************\
	usb_device_data_out()
		Do or Dn

	Input Global Parameter:
		byteCnt:
		ctxt_site: site # of CDB Context for UAS 
\****************************************/
void usb_device_data_out()
{
	//printf("data out %X\n", byteCnt);
	SCTXT_CCMIndex = SCM_NULL;

	if (byteCnt > MC_BUFFER_SIZE)
	{	// fails  
		hdd_err_2_sense_data(ERROR_ILL_CDB);
		usb_device_no_data();
		return;
	}
#ifdef UAS_EN
	if (!bot_mode)
	{
		uas_device_data_out();
	}
	else
#endif
	{
		bot_device_data_out();
	}

}

void usb_device_data_out2()
{
	//printf("data out %X\n", byteCnt);
	SCTXT_CCMIndex = SCM_NULL;

	if (byteCnt > MC_BUFFER_SIZE2)
	{	// fails  
		hdd_err_2_sense_data(ERROR_ILL_CDB);
		usb_device_no_data();
		return;
	}
#ifdef UAS_EN
	if (!bot_mode)
	{
		uas_device_data_out();
	}
	else
#endif
	{
		bot_device_data_out();
	}

}

#if 0
void usb_set_halt_if_dout_flow_ctrl()
{
	
	if ((*usb_Msc0DOutCtrl & 0xE1) == 0xA0)
	{
		if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
		{
			SCTXT_INDEX = ctxt_site;
			if (SCTXT_Status != CTXT_STATUS_HALT)
			{
				SCTXT_Status = CTXT_STATUS_HALT;
				*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
			}
		}
	}
}
#endif
#ifdef USB3_TX_DBG
void usb_rst_usb3_tx_FIFO(void)
{
	//if ((polling_failed_count) ||(recovery_count > 3))
	{
		// following hardware's sequence, shall reset the TX FIFO when SSC is off
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_02, 0x4A); // power down USB3 SSC
		// reset TX FIFO
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, EN_SSC_LOOP);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x00);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, EN_SSC_LOOP);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x00);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, EN_SSC_LOOP);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x00);

		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, EN_SSC_LOOP);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x00);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, EN_SSC_LOOP);
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x00);
		//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_02, 0x42); // power on USB3 SSC
		recovery_count = 0;
		UART_CH('@');
	}
}
#endif



#if 0
bit USB_VBUS_OFF()
{
	if ((*cpu_wakeup_ctrl_0 & CPU_USB_VBUS) == 0)
	{
DBG0(("VBUS/GPIO2 Off\n"));
		return 1;
	}
	else
	{
		return 0;
	}

}
#endif


#ifdef PWR_SAVING
void	usb30_clock_disable(void)
{
	// Power Down "CDR Limdet Detector"
	//tmp8 = 0x02 | spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05);	
	//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05, tmp8);	

	#ifdef RTEM_2_AGND
	if (usb_self_power == 0)
	{
		// en_rtem2agnd, enable rx common mode voltage and agnd
		tmp8 = 0x01 | spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A);	
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A, tmp8);	
	}
	#endif

	// disable tx_no_sync
	//tmp8 = (~0x01) & spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0B);	
	//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0B, tmp8);

	// 3. turn off usb3 phy
	//tmp8 = (PD_USB3_TX|PD_USB3_RX||PD_VREG|PD_USB3_PLL) | spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);	
	//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, tmp8);
	//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, PD_USB3_RX_EN|PD_USB3_TX_EN|PD_USB3_TX|PD_USB3_RX|PD_VREG|PD_USB3_PLL);
	if (revision_a61)
	{
		if (usb_self_power == 0)
		{
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE|PD_USB3_RX|PD_VREG|PD_USB3_PLL);
		}
		else
		{
			//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE|PD_USB3_RX|PD_USB3_PLL);
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE|PD_USB3_RX);
		}
	}
	else
	{
		//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, PD_USB3_RX_EN|PD_USB3_TX_EN|PD_USB3_TX|PD_USB3_RX|PD_VREG|PD_USB3_PLL);
		spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, PD_USB3_RX_EN|PD_USB3_RX);
	}
}
#endif	//#ifdef PWR_SAVING

#ifdef PWR_SAVING
void usb_clock_disable(void)
{
		// 2. Use REFCLK as source for USBCLK
		*cpu_Clock_3 |= USBCLK_SELECT;

		// 3. turns off USB2.0 PLL
		//if (usb_active == 0)
		//{
		//	*usb_DevCtrl_1 = USB2_FORCE_SUSP;
		//}
		//else
		{
			if ((usbMode == CONNECT_USB2) || (usbMode == CONNECT_USB1))
			{
				//*usb_DevCtrlClr_2 =	USB2_PLL_FREERUN;	// disable U2 PLL Freerun
			}
			else
			{
				*usb_DevCtrl_1 = USB2_FORCE_SUSP;
			}
		}

		// 3. turn off usb3 phy
		usb30_clock_disable();				
}
#endif //#ifdef  PWR_SAVING

#ifdef PWR_SAVING
void	usb30_clock_enable(void)
{
	// enable tx_no_sync
	//tmp8 = 0x01 | spi_phy_rd(PHY_SPI_U3PMA, 0x0B);	
	//spi_phy_wr_retry(PHY_SPI_U3PMA, 0x0B, tmp8);	
	
	// Power Up "CDR Limit detector"
	//tmp8 = (~0x02) & spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05);	
	//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05, tmp8);	

#ifdef RTEM_2_AGND
	if (usb_self_power == 0)
	{	// en_rtem2agnd, disable rx common mode voltage and agnd
		tmp8 = (~0x01) & spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A);	
		spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A, tmp8);	
	}
#endif

	//turn on Usb3 phy PLL & VREG
	//tmp8 = (~(PD_USB3_RX|PD_USB3_TX|PD_VREG|PD_USB3_PLL)) & spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
	//tmp8 = (~(PD_VREG|PD_USB3_PLL)) & spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
	//spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, tmp8);
#if 0
	if (revision_a61)
	{
		//spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|PD_USB3_TX|PD_USB3_RX);
	}
	else
	{
		spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, PD_USB3_RX_EN|PD_USB3_TX_EN|PD_USB3_TX|PD_USB3_RX);
	}

	while (1)
	{
		if ((*cpu_Clock_2 & (USB3_PLL_RDY)))
		{
			break;
		}
	}	

	//turn on Usb3 phy TX & RX 
	if (revision_a61)
	{
		spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
	}
	else
	{
		spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, 0);
		//tmp8 = (~(PD_USB3_RX|PD_USB3_TX)) & spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
		//spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, tmp8);
	}
	MSG(("U3 P R\n"));
#else
	// delay 100us
	//Delay10us(10);

	spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
	//DBG(("U3 P R\n"));
	//while (1)
	//{
	//	if ((*cpu_Clock_2 & (USB3_PLL_RDY)))
	//	{
	//		break;
	//	}
	//}	
#endif
}
#endif	//##ifdef PWR_SAVING

#ifdef PWR_SAVING
void	usb_clock_enable(void)
{	
	if ((*cpu_Clock_2 & USB2_PLL_RDY) == 0)
	{
		// Use REFCLK as source for USBCLK
		*cpu_Clock_3 |= USBCLK_SELECT;

		// enable U2 PLL
#if 1
		*usb_DevCtrl_2 =	USB2_PLL_FREERUN;
		*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;	
#else
		if (usb_active == 0)
		{
			*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
		}
		else
		{
			if (usbMode == CONNECT_USB2)
			{
				*usb_DevCtrl_2 = USB2_PLL_FREERUN;	// En U2 PLL Freerun
			}
			else
			{
				*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
			}
		}
#endif
	}
	else
	{
		// enable U2 PLL Freerun
		*usb_DevCtrl_2 =	USB2_PLL_FREERUN;
		*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;	
	}

	// Enable USBCLK
	//*cpu_Clock_3 &=	~USBCLK_DSBL;

	// enable tx_no_sync
#if 0
	tmp8 = 0x01 | spi_phy_rd(PHY_SPI_U3PMA, 0x0B);	
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x0B, tmp8);	
#endif	

	//turn on Usb3 PLL ony
	//tmp8 = (~(PD_USB3_RX|PD_USB3_TX|PD_VREG|PD_USB3_PLL)) & spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
	//spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, tmp8);
	if (revision_a61)
	{
		spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, (HW_RESTART_USB2_ENABLE|PD_USB3_RX|PD_USB3_TX));
	}
	else
	{
		//spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, (PD_USB3_RX|PD_USB3_TX));
		spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, PD_USB3_RX);
	}

	 // Power Up "CDR Limit detector"
	//tmp8 = (~0x02) & spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05);	
	//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05, tmp8);	

	// en_rtem2agnd, disable rx common mode voltage and agnd
#ifdef RTEM_2_AGND
		if (usb_self_power == 0)
		{	// en_rtem2agnd, disable rx common mode voltage and agnd
			tmp8 = (~0x01) & spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A);	
			spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A, tmp8);	
		}
#endif


#if 0
	// Power up SATA PLL
	tmp8 =  (~PD_SATA_PLL) & spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);
#endif

	// Enable USB3.0 TX & RX after USB3.0 PLL is ready
	if (revision_a61)
	{
		spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, (HW_RESTART_USB2_ENABLE));
	}
	else
	{
		spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, 0);
	}

	// 11. wait for USBx_PLL_READY is asserted in the CPU Clock Register
	while (1)
	{
		if ((*cpu_Clock_2 & (USB2_PLL_RDY | USB3_PLL_RDY)) == (USB2_PLL_RDY | USB3_PLL_RDY))
		{
			break;
		}
	}	

//	*switch_regulator_reg = *switch_regulator_reg & ~0x07; 	// set back switching regulator

	// 12. resets the USBCLK_SELECT signal to switch the source to the USB PLL
	*cpu_Clock_3 &= ~USBCLK_SELECT;

	//DBG(("w U PLL"));
	//DBG(("->R\n"));
}
#endif	//#ifdef PWR_SAVING

#ifdef PWR_SAVING
void	usb20_clock_enable(void)
{
	//DBG(("u2_c_e %bx\n", *cpu_Clock_2));
	// Use REFCLK as source for USBCLK
	*cpu_Clock_3 |= USBCLK_SELECT;

	// enable U2 PLL Freerun
	*usb_DevCtrl_2 =	USB2_PLL_FREERUN;
	*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;	

	// 11. wait for USBx_PLL_READY is asserted in the CPU Clock Register
	MSG(("w U2 PL"));
	while (1)
	{
		if (*cpu_Clock_2 & USB2_PLL_RDY)
		{
			break;
		}
	}	

//	*switch_regulator_reg = *switch_regulator_reg & ~0x07; 	// set back switching regulator

	MSG(("->R\n"));
	// 12. resets the USBCLK_SELECT signal to switch the source to the USB PLL
	*cpu_Clock_3 &= ~USBCLK_SELECT;
}
#endif	// #ifdef PWR_SAVING

/****************************************\
	usb_suspend_wait
	return:
		1: vbus off
		0: USB unsuspended
		2: wait USB unsuspended time Out
\****************************************/
u8 usb_suspend_wait(u16 timeout)
{
 		for (tmp16 = 0; tmp16 < timeout; tmp16++)
		{
			if (USB_VBUS_OFF())
			{
				//DBG0(("\tV of\n"));
#ifdef PWR_SAVING
				//usb20_clock_enable();
#endif
				usb_active = 0;
				//bot_usb_bus_reset();
				return 1;
			}
			if (*cpu_wakeup_ctrl_0 & CPU_USB_UNSUSPENED)
			{
					*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;
					//DBG0(("\tRes %bx %bx\n", *usb_CMgr_Status_shadow, *usb_USB3StateCtrl));
					return 0;
			}

			if ((usb_power_on_flag == 1) && (*usb_USB3LTSSM_0_shadow & LTSSM_U0))
			{
				goto chk_susp;
			}

			if (*chip_IntStaus_0 & USB_INT)
			{
				if (!revision_a61)
				{
					tmp8 = *usb_IntStatus_0;
					val = *usb_IntStatus_1;
				}
				else
				{
					tmp8 = *usb_IntStatus_shadow_0;
					val = *usb_IntStatus_shadow_1;
				}
				if ((tmp8 & (USB_BUS_RST |HOT_RESET)) || (val & CTRL_INT))
				{
chk_susp:
					*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED;
					*usb_IntStatus_0 = USB_SUSPEND;
					Delay(1);
					if ((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED) == 0)
					{
						//DBG0(("ab wa\n"));
						return 0;
					}
				}

			}
			if (sobj_State >= SATA_DRV_RDY)
			{
				if ((*sata_PhyStat & PHYRDY) == 0)
				{
					sata_Reset(SATA_HARD_RST);
				}
			}

			if ((sobj_State > SATA_PWR_DWN)  && (*chip_IntStaus_0 & SATA0_INT))
			{
				sata_isr();
			}
			Delay(1);

		}
		return 2;
}

/****************************************\
	usb_suspend
	return:
		1: vbus off
		0: USB unsuspended
\****************************************/
bit usb_suspend(bit fast_flag)
{
	EAL = 0;
#ifdef ASIC_CONF
	//if (revision_a61)
	{
		spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
	}

#endif	
	*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
	EAL = 1;
    // there is a condition that happen in usb2 host that cause the unsuspend
    // pending even when there is no unsuspend
    //clear it first, Can we missing the real unsuspend?, possibilty is real low.
	*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;

	// check "USB Connection Manager Status"
	tmp8 = *usb_CMgr_Status_shadow;
	//if ((tmp8 & (CMGR_USB3_MODE|CMGR_USB2_MODE)) == 0)
	if (tmp8 == CMGR_IDLE)
	{	// waiting for USB Connection
		usbMode = CONNECT_UNKNOWN;
	}

	//DBG0(("Su %bx\n", *usb_CMgr_Status_shadow));
//	DBG0(("Su %bx\n", *cpu_wakeup_ctrl_0));
	//DBG0(("Su %bx\n", *cpu_wakeup_ctrl_0));

	usb3_function_suspend = 0;

	#ifdef ATAPI
		 if (sobj_class == DEVICECLASS_ATAPI)
		 {
		 	tmp8 = usb_suspend_wait(3000);
			if (tmp8 != 2)
			{
				return tmp8;
			}
		 }
		else if (sobj_class == DEVICECLASS_ATA)
	#endif
		{
			if (sobj_State == SATA_READY)
			{		
				//DBG0(("\tATA Flush\n"));
				ata_ExecNoDataCmd(ATA6_FLUSH_CACHE_EXT, 0);
				hdd_post_write = 0;
			}

			hdd_led_going_off();
			led_on = 0;
			led_state = LED_OFF;

			// wait for 10s
			tmp16 = 10000;		// 2s
			if (fast_flag)
			{
				tmp16 = 500;	//500ms
			}
			tmp8 = usb_suspend_wait(tmp16);
			
			if (tmp8 != 2)
			{
				if (tmp8 == 0)
				{
					hdd_on_led();
				}
				return tmp8;
			}

		#ifdef POW_MGMT
			if (PwrMgmt && (sobj_State == SATA_READY))
		#else
			if ((sobj_State == SATA_READY))
		#endif
			{
	//DBG(("Sp D\n"));
				if (ata_ExecNoDataCmd(ATA6_STANDBY_IMMEDIATE, 0) == CCM_STATUS_GOOD)
				{
					sobj_State = SATA_STANDBY;
				}
			}			

			//tmp16 = 8000;		// 8s
			//if (fast_flag)
			{
				tmp16 = 10;	// 10ms
			}
			tmp8 = usb_suspend_wait(tmp16);
			if (tmp8 != 2)
			{
				return tmp8;
			}
		}

		ET0 = 0;
//		hdd_led_going_off();
		xtimer_disable();

	#ifdef PWR_SAVING
		if (sobj_State > SATA_PWR_DWN)
		{
//DBG(("S PD\n"));
			sata_pwr_down();
		}
	#endif


		*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED;

#if 0
		if (USB_VBUS_OFF())
		{
			DBG0(("B vb of\n"));
			usb_active = 0;
			//bot_usb_bus_reset();
			return 1;
		}
#endif

//DBG0(("Su wait3 \n"));
			
#ifdef PWR_SAVING
	#if 1		
		// debug
		usb_clock_disable();
	#else
		if (usbMode == CONNECT_USB3)
		{
			//*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
			*usb_DevCtrl_1 = USB2_FORCE_SUSP;
		}
		else
		{
			*usb_DevCtrlClr_2 =	USB2_PLL_FREERUN;	// disable U2 PLL Freerun
		}
		usb30_clock_disable();
	#endif
#if 1

		if (usb_self_power == 0)
		{
			//if ((globalNvram.USB_ATTR & 0x40) == 0)
			{	// USB BUS Power device
				//DBG0(("u_s %bx %bx\n", *cpu_Clock_2, *usb_DevCtrl_2));

				//spi_phy_wr(PHY_SPI_SATA, 0x00, 0x01);
				// USB Bus Power device
				// 4. turns OFF the USBCLK.
				*cpu_Clock_3 |=	USBCLK_DSBL;

		//DBG0(("Su %BX\n", *cpu_Clock_3));
				#if 0
					dump_phy();
				#endif
					//Delay(10);
					// Enable hardware reset switch regulator
					*fw_temp_3 &= ~DIS_AUTO_RST_SW;

		//DBG(("Su %BX\n", *swreg_ctrl_1));
					// set switching regulator to low voltage for power saving
					// 111: 1.14V
					// 000: 1.08V
					// 001: 1.02V
					// 010: 0.96V
					// 011: 0.90V
					// 100: 0.84V
					// 101: 0.78V
					// 110: 0.72V
				#ifdef RTEM_2_AGND
					*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x02; 	 //  0.96v
				#else
					*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x04; 	 //  0.84v
				//*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x03; 	 //  0.90v
				//*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x02; 	 //  0.96v
				#endif

					// 6. STOP_REFCLK control
					*cpu_Clock_3 |= (STOP_REFCLK|XTAL_DISABLE);

					// disable hardware reset switch regulator
					*fw_temp_3 |= DIS_AUTO_RST_SW;
				*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x01; // set the switch regulator's voltage to 1.02V
				#if 1
					usb30_clock_enable();
					//DBG(("USB Up %bx\n", *cpu_Clock_2));
					if ((*cpu_Clock_2 & USB2_PLL_RDY) == 0)
					{
						for (i8 = 0; i8 < 255; i8++)
						{
							Delay10us(1);
							if ((*cpu_Clock_2 & USB2_PLL_RDY))
							{
								//DBG(("i8 %bx\n", i8+1));
								goto u2_free_run;
							}

						}
						//DBG(("U2 C Tout\n"));
						// Use REFCLK as source for USBCLK
						*cpu_Clock_3 |= USBCLK_SELECT;
					}
u2_free_run:
					// enable U2 PLL Free running
					*usb_DevCtrl_2 =	USB2_PLL_FREERUN;
					*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;	

					// disable REFCLK as source for USBCLK
					*cpu_Clock_3 &= ~USBCLK_SELECT;
				#else

					DBG0(("USB Up %bx%bx\n", *cpu_Clock_3, *cpu_Clock_2));
				#ifdef PWR_SAVING
					// enable U2 PLL Freerun
					usb_clock_enable();
				#endif
				#endif
				//	spi_phy_wr(PHY_SPI_SATA, 0x00, 0X48);
					//Delay(1);

					*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;
					goto usb_wakeup;
			}
		}	//if (usb_self_power == 0)
#endif
#endif	//#ifdef PWR_SAVING


//DBG0(("Su wait \n"));
#define prev_ck3 val

		if (usbMode != CONNECT_USB3)
		{
			prev_ck3 = *cpu_Clock_3;
			//DBG(("C3 %BX\n", prev_ck3));
		}

		while (1)
		{
			if (usbMode != CONNECT_USB3)
			{
				tmp8 = 	*cpu_Clock_3;
				if (tmp8 != prev_ck3)
				{
					prev_ck3 = tmp8;
					//DBG0(("\tC3 %BX\n", prev_ck3));
					//*usb_DevCtrl_2 =	USB2_PLL_FREERUN;	// Enable U2 PLL Freerun
					*cpu_wakeup_ctrl_2 = USB_WAKEUP_REQ;
				}
			}
			if (USB_VBUS_OFF())
			{
#ifdef PWR_SAVING
				usb_clock_enable();
#endif
				//DBG(("v of\n"));
				usb_active = 0;
					//bot_usb_bus_reset();
				return 1;
			}
			//if (revision_a61)
			{
				if (*usb_IntStatus_shadow_0 & USB_BUS_RST)
				{
					//DBG(("USB2 BRst\n"));
#ifdef PWR_SAVING
					usb_clock_enable();
#endif
					*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED;
					*usb_IntStatus_0 = USB_SUSPEND;
					//DBG(("clr %bx, %bx\n", *cpu_wakeup_ctrl_0, *usb_IntStatus_shadow_0));
					Delay(1);
					//DBG(("%bx, %bx\n", *cpu_wakeup_ctrl_0, *usb_IntStatus_shadow_0));	
					if ((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED) == 0)
					{
						//DBG0(("ab USB2 Wk\n"));
						break;
					}
				}
			}
			if (*cpu_wakeup_ctrl_0 & CPU_USB_UNSUSPENED)
			{
				//7. Resume/Wakeup Signaling is detected on the USB Bus.
				//   USB_SUSPEND is negated by the Link and the Clock Management logic resets USBCLK_DISABLE=0 and CPUCLK_DIV=Fh. 
				//   USBCLK and CPUCLK are now driven by REFCLK. 
				//   The CPU is alive and can detect remote wakeup with USB_UNSUPENDED in the CPU Wakeup Control Register (0x1C).
#ifdef PWR_SAVING
				if (usbMode == CONNECT_USB3)
				{
					usb30_clock_enable();
					//dump_phy();
				}
#endif
				*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;
				//DBG0(("\tBW\n"));
				break;
			}
		}	// while (1)

#undef prev_ck3

//DBG0(("Su wake \n"));
usb_wakeup:
//DBG(("\tWake\n"));

#if 1	// debug
		if (usbMode == CONNECT_USB3)
		{
			if (*usb_CMgr_Status_shadow & B_BIT_5)
			{	//still USB3.0, so turn off USB2.0 PLL
				*usb_DevCtrl_1 = USB2_FORCE_SUSP;
			}
			else
			{	// not USB3.0
				usb_inactive_count = 0;
			}

//			*usb_DevCtrl_1 = USB2_FORCE_SUSP;
//			//*usb_DevCtrlClr_2 =	USB2_PLL_FREERUN;	// disable U2 PLL Freerun
			//*usb_USB3StateSelect = 0x00;
			temp_USB3StateCtl = *usb_USB3StateCtrl;
			//DBG0(("\tL %BX\n", temp_USB3StateCtl));

			ET0 = 1;			//Enable Timer 0 interrupt
		}
		else
		{
			*usb_DevCtrl_2 =	USB2_PLL_FREERUN;	// Enable U2 PLL Freerun
		}
#endif

		if (*usb_DevState_shadow & (USB_CONFIGURED))
		{
			sata_pwr_up();
		}

		//hdd_on_led();
		xtimer_setup();

		//timer0_setup();

#ifdef STANDBY_TIMER
		hdd_tick = 0;
		hdd_standby_tick = 0;
#endif
		return 0;
}

/****************************************\
	usb_msc_isr
\****************************************/
void usb_msc_isr()
{
	//u8 Msc0Int_0 = *usb_Msc0IntStatus_0;

	//DBG(("msc_isr\n"));

	val = *usb_Msc0IntStatus_0;
	if (val & MSC_TX_DONE_INT)
	{	//  USB command with Data-in
#ifdef DBG_ODD_U2
			if (dbg_flag)
			{
				//DBG0(("vUtxd\n"));
			}
#endif	
	
			//DBG(("tx done\n"));
#if 0
			if (*usb_Msc0DIXferStatus & 0x0F)
			{
				if ((*usb_Msc0DIXferStatus & 0x0F) != 0x05)
				{
					ERR(("TXRsn:%BX %BX\n", *usb_Msc0DIXferStatus, CmdBlk(0)));
				}
			}
#endif

			// if HALT asserted wait HALT clear...
			if (val & MSC_DIN_HALT_INT)
			{
				if ((*usb_Msc0DICtrl & MSC_DI_HALT))
				{
					goto msc_isr_check_rx;
				}
			}
			
			if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
			{
				*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;

				//DBG(("msc_isr: ctxt\n"));
				if (SCTXT_CCMIndex != SCM_NULL)
				{
					//DBG(("msc_isr: goto msc_isr_check_rx\n"));
					goto msc_isr_check_rx;
				}
//				DBG("CTXT_ccmIndex:%BX\n", *ctxt_ccmIndex);

				*usb_Msc0IntStatus_0 = MSC_TX_DONE_INT|MSC_DIN_HALT_INT;
			
#if 1
				if (bot_mode)
				{
					*usb_Msc0Residue_0 = *usb_Msc0TxXferCount_0;			// for BOT only
					*usb_Msc0Residue_1 = *usb_Msc0TxXferCount_1;			// for BOT only
					*usb_Msc0Residue_2 = *usb_Msc0TxXferCount_2;			// for BOT only
					*usb_Msc0Residue_3 = *usb_Msc0TxXferCount_3;			// for BOT only
				}
#endif

#if 0
				if (*ctxt_CCMIndex != SCM_NULL)
				{
					if (*chip_IntStaus & SATA0_INT)
					{
						sata_isr();
					}

					if (*ctxt_CCMIndex != SCM_NULL)
					{
						DBG(("TxDone: SCTXT_ccmIndex:%bx", SCTXT_CCMIndex));
					}

				}
#endif
				if ((SCTXT_DbufIndex) != DBUF_SEG_NULL)
				{
#ifdef DBG_ODD_U2
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
				
					*dbuf_MuxSel = SCTXT_DbufIndex;
					*dbuf_MuxCtrl = SEG_RESET;
					*dbuf_MuxCtrl;

					*usb_Msc0DICtrlClr	= MSC_TXFIFO_RST;
					*usb_Msc0DICtrlClr;

					*dbuf_MuxCtrl = SEG_RESET;
					*dbuf_MuxCtrl;
					
						
#if 0
					if (((dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count & 0xF8000000) != 0xC0000000) ||
						((dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count & 0xF8000000) != 0xC0000000))
					{
						ERR(("USB TX error!\n"));
						ERR(("SATA_W:%LX\n", dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count));
						ERR(("USB_R:%LX\n", dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count));
						DBG(("sata_DataRxStat:%bX\n", *sata_DataRxStat));
					}
#endif
					
					*dbuf_MuxStatus = SEG_IDLE;
					*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
					*dbuf_MuxInOut;

//#ifdef USB2_DI_PATCH
#ifdef DBG_ODD_U2
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
				
//				usb_curCtxt = CTXT_NULL;

//				if (SCTXT_Status == CTXT_STATUS_PENDING)
//				{
//					SCTXT_Status = CTXT_STATUS_PHASE;
//				}

//				if ((*usb_Msc0DICtrl & MSC_DI_HALT) == 0)
				{
//					DBG(("no halt\n"));

#ifdef UAS_EN
					if ((*ctxt_SataProto & 0xc0) == 0x80)
					{	// // send status Back if it is NCQ Protocol
						//DBG(("sense IU for NCQ, site: %BX\n", ctxt_site));
						if (SCTXT_Status >= CTXT_STATUS_PENDING)
						{
							SCTXT_Status = CTXT_STATUS_GOOD;
						}
						//uas_din_send_sense_iu();
						uas_send_sense_iu();

						*usb_Msc0DICtrlClr = MSC_DIN_FLOW_CTRL;
						*usb_Msc0DIXferStatus = 0; 
					}
					else
#endif
					{
//						if (SCTXT_Status != CTXT_STATUS_PENDING)
//						{
//							if (reg_r32(chip_IntStaus) & SATA0_INT)
//								sata_isr();
//						}
						//DBG(("status response %BX\n", SCTXT_Status));
#ifdef DBG_ODD_U2
						if (dbg_flag)
						{
							//DBG0(("vu_Ss %bx\n", SCTXT_Status));
						}
#endif	

						if (SCTXT_Status != CTXT_STATUS_PENDING)
						{
							{	// send csw
								//3609
								//timer0_unload();
								usb_tx_ctxt_send_status();
								//*usb_Msc0DIStatus = *ctxt_Status;
								//*usb_Msc0DICtrl = MSC_CSW_RUN;
							}
						}
					}
					*usb_Msc0IntStatus_0 = MSC_DIN_HALT_INT;
				}
#if 0
				else
				{

					DBG(("TxDone: Halt\n"));
				}
#endif
			}
			//else
			//{
			//	DBG(("usb_curCtxt is NULL\n"));
			//}
	}	// if (Msc0Int & MSC_TX_DONE_INT)

	else if (val & MSC_DIN_HALT_INT)
	{
#ifdef DBG_ODD_U2
		if (dbg_flag)
		{
			//DBG0(("vUHALTIN\n"));
		}
#endif	
	
		//DBG(("usb_msc_isr: halt in\n"));

		if ((*usb_Msc0DICtrl & MSC_DI_HALT))
			goto msc_isr_check_rx;

		*usb_Msc0IntStatus_0 = MSC_DIN_HALT_INT;
		if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
		{
			SCTXT_INDEX = ctxt_site;

#ifdef UAS_EN
			if (bot_mode)
#endif			
			{
				//DBG(("halt in: usb_Msc0TxXferCount:%LX\n", *usb_Msc0TxXferCount));
				//DBG(("CTXT_XFRLENGTH:%BX%BX%BX%BX\n", *ctxt_XferLen_3,*ctxt_XferLen_2,*ctxt_XferLen_1, *ctxt_XferLen_0 ));
				//case 4
				*usb_Msc0Residue_0 = *ctxt_XferLen_0;			// for BOT only
				*usb_Msc0Residue_1 = *ctxt_XferLen_1;			// for BOT only
				*usb_Msc0Residue_2 = *ctxt_XferLen_2;			// for BOT only
				*usb_Msc0Residue_3 = *ctxt_XferLen_3;			// for BOT only
			}
			
			if ((*usb_Msc0DICtrl & MSC_DI_HALT) == 0)
			{		
					{
//						if (CTXT_Status != CTXT_STATUS_PENDING)
//						{
//							sata_isr();
//						}

						if (SCTXT_Status != CTXT_STATUS_PENDING)
						{
							{	// send csw
								//3609
								usb_tx_ctxt_send_status();
								//*usb_Msc0DIStatus = SCTXT_Status;
								//*usb_Msc0DICtrl = MSC_CSW_RUN;
							}
						}
					}
			}
		}
	}

#if 0
	else if (*usb_Msc0IntStatus_1 & MSC_TXSENSE_DONE_INT) 
	{	// 	UAS Sense IU Transfer thru TX(DI) context  has completed.
		DBG(("usb_msc_isr: tx sense done\n"));
					
		*usb_Msc0IntStatus_1 = MSC_TXSENSE_DONE_INT;
		// check pending Ctxt queue for TX(DI) Context ????
		usb_exec_que_ctxt();
		return;
	}
#endif

msc_isr_check_rx:
	// check RX context of MSC0
	if (val & MSC_RX_DONE_INT)
	{	// USB Data Out done on active CDB Context of RX Context
		//DBG(("usb_msc_isr: USB rx done\n"));
#ifdef DBG_ODD_U2
		if (dbg_flag)
		{
			//DBG0(("vURxd\n"));
		}
#endif	
		
		EAL = 0;
		tmp8 = *usb_Msc0DOXferStatus;
		if (tmp8)
		{
			//DBG0(("DOs:%BX %BX\n", tmp8, CACHE_BYTE0));
			*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET;
		#ifdef DBG_FUNCTION
			tmp8 = *usb_Msc0DOXferStatus;
			//DBG0(("DOs:%BX %BX\n", tmp8, CACHE_BYTE0));
		#endif
		}
		EAL = 1;
		//DBG0(("URD\n"));

		// if HALT asserted wait HALT clear...
//		if (Msc0Int & MSC_DOUT_HALT_INT)
//		{
//			if ((*usb_Msc0DOutCtrl & MSC_DOUT_HALT))
//				return;
//		}


		if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
		{
			u8 do_crl;


#ifdef UAS_EN
			if (!bot_mode)
			{
				*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;
			}
#endif

//#ifdef UAS_EN
#if 0
//			if (!bot_mode)
			{	// UAS Mode
				// RX transfer length is not 0, means the transfer is not done
				if (((*usb_Msc0RxXferCount_0) | (*usb_Msc0RxXferCount_1) | (*usb_Msc0RxXferCount_2) | (*usb_Msc0RxXferCount_3)) != 0)
				{
						//DBG0(("W ZLP\n"));
						*usb_USB3StateSelect = 0x8A;
						//*usb_USB3StateSelect;
						*ctxt_XferLen_0 = *usb_Msc0RxXferCount_0;
						*ctxt_XferLen_1 = *usb_Msc0RxXferCount_1;
						*ctxt_XferLen_2 = *usb_Msc0RxXferCount_2;
						*ctxt_XferLen_3 = *usb_Msc0RxXferCount_3;
						// move the state to uas_DATAOUT_xfer state
						*usb_USB3StateCtrl = 0x00;
						*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT;
						*usb_Msc0DOutCtrl = MSC_DOUT_RESET;
						*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET;

						*usb_Msc0RxCtxt_0 = ctxt_site;
						//Delayus(10);
						*usb_USB3StateSelect = 0;
						// expect send the ERDY out and transfer will continue
						return;
				}
			}
			//else
#endif


			do_crl = *usb_Msc0DOutCtrl;
			//DBG(("do_crl: %BX\n", do_crl));
			//DBG(("MscIntStatus_0:%bX\n", val));

			if (SCTXT_Status == CTXT_STATUS_PENDING)
			{
#if 0			// comment out for 3607 rev 0x20
				if ((do_crl & (MSC_DOUT_FLOW_CTRL|MSC_RXFIFO_DDONE|MSC_DOUT_HALT)) == (MSC_DOUT_FLOW_CTRL|MSC_RXFIFO_DDONE))
				{
					if (*usb_Msc0RxXferCount)
					{	// set Halt bit & 3607 will send ERDY 
						*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
						SCTXT_Status = CTXT_STATUS_HALT;
						// wait for Halt bit clear by system
						return;
					}
				}
				else
#endif				
				if (do_crl & (MSC_DOUT_HALT))
				{
					SCTXT_Status= CTXT_STATUS_HALT;
					// wait for Halt bit clear by system
					return;
				}
//				if (do_crl & MSC_RXFIFO_EMPTY)
//				{
					goto rx_done_ok;
//				}
//				return;

			}
			else if (SCTXT_Status == CTXT_STATUS_HALT)
			{
				if (do_crl & MSC_DOUT_HALT)
				{
					if (*usb_Msc0DICtrl & MSC_DIN_FLOW_CTRL)
					{
						*usb_Msc0DOutCtrlClr = MSC_DOUT_HALT;
					}
					return;
				}



#if 0
				if (bot_mode)
				{
					//*usb_Msc0Residue = *usb_Msc0RxXferCount;
					*usb_Msc0Residue_0 = *ctxt_XferLen_0;			// for BOT only
					*usb_Msc0Residue_1 = *ctxt_XferLen_1;			// for BOT only
					*usb_Msc0Residue_2 = *ctxt_XferLen_2;			// for BOT only
					*usb_Msc0Residue_3 = *ctxt_XferLen_3;			// for BOT only
				}
#endif					

rx_done_ok:
				*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT|MSC_DOUT_HALT_INT;

				{
					if (SCTXT_Flag & SCTXT_FLAG_U2B)
					{
						//DBG(("DO U2B dn %BX\n", SCTXT_Status));
						if (SCTXT_Status >= CTXT_STATUS_HALT)
						{
							if (*ctxt_PhaseCase_1 & B_BIT_11)		// Case 11 = 0x08
							{
								SCTXT_Status = CTXT_STATUS_PHASE;
							}
							else
							{
								SCTXT_Status = CTXT_STATUS_XFER_DONE;
							}
							//DBG(("ctxt_No_Data:%bX\n", *ctxt_No_Data));
							usb_data_out_done();
						}
						else
						{
							usb_rx_ctxt_send_status();
						}
					}
					else
					{	// D-out from USB to SATA

						if (SCTXT_CCMIndex != SCM_NULL)
						{
							SCTXT_Status = CTXT_STATUS_XFER_DONE;
						}
						else
						{
							if (SCTXT_Status <= CTXT_STATUS_PHASE)
							{
								// if it is USB-2-SATA command,  send status Back thru RX context
								usb_rx_ctxt_send_status();
							}
							else
							{
								ERR(("uRx dn:%bx\n", SCTXT_Status));
							}
						}
					}
//					*usb_Msc0IntStatus_0 = MSC_DOUT_HALT_INT;
				}
			}
			else if (SCTXT_Status <= CTXT_STATUS_PHASE)
			{
				*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT|MSC_DOUT_HALT_INT;
				usb_rx_ctxt_send_status();
			}
		}
		//else
		//{	// something wrong ????
		//	ERR(("something wrong\n"));
		//	*usb_Msc0IntStatus_0 = MSC_RX_DONE_INT|MSC_DOUT_HALT_INT;
		//}
	}

	// no halt in UAS		
	else if (val & MSC_DOUT_HALT_INT)		
	{
#ifdef DBG_ODD_U2
		if (dbg_flag)
		{
			//DBG0(("vUHALTout\n"));
		}
#endif	

		//DBG(("usb_msc_isr: halt out done\n"));

		if ((*usb_Msc0DOutCtrl & MSC_DOUT_HALT))
			return;

		*usb_Msc0IntStatus_0 = MSC_DOUT_HALT_INT;		
		//if ((ctxt_site = usb_curCtxt) != CTXT_NULL)
		{
			//if ((*usb_Msc0DOutCtrl & MSC_DOUT_HALT) == 0)
			{
#if 1
					if (bot_mode)
					{
						//EAL = 0;
						*usb_Msc0Residue_0 = *usb_Msc0RxXferCount_0;								// for BOT only
						*usb_Msc0Residue_1 = *usb_Msc0RxXferCount_1;			// for BOT only
						*usb_Msc0Residue_2 = *usb_Msc0RxXferCount_2;			// for BOT only
						*usb_Msc0Residue_3 = *usb_Msc0RxXferCount_3;			// for BOT only
						//*usb_Msc0Residue_0 = *ctxt_XferLen_0;			// for BOT only
						//*usb_Msc0Residue_1 = *ctxt_XferLen_1;			// for BOT only
						//*usb_Msc0Residue_2 = *ctxt_XferLen_2;			// for BOT only
						//*usb_Msc0Residue_3 = *ctxt_XferLen_3;			// for BOT only
						//EAL = 1;
					}
#endif					
					if (SCTXT_Flag & SCTXT_FLAG_U2B)
					{
						//DBG(("DOH U2B dn %BX\n", SCTXT_Status));
						// is USB-2-CPU command(not USB-2-SATA command)
						// process data from USB host for SCSI CDB. ????
//						SCTXT_Status = CTXT_STATUS_XFER_DONE;
						if (SCTXT_Flag & (SCTXT_FLAG_NO_DATA))
						{
							if (*ctxt_PhaseCase_1 & (B_BIT_13 | B_BIT_10))
							{	//case 10 
								SCTXT_Status = CTXT_STATUS_PHASE;
							}
							else
							{
								SCTXT_Status = CTXT_STATUS_GOOD;
							}
							usb_rx_ctxt_send_status();
						}
						else
						{
							// it is USB-2-CPU command(not USB-2-SATA command)
							// process data from USB host for SCSI CDB. ????
							usb_data_out_done();
						}
					}
					else
					{

						if (SCTXT_CCMIndex != SCM_NULL)
						{
							SCTXT_Status = CTXT_STATUS_XFER_DONE;
						}
						else
						{
							if (SCTXT_Status <= CTXT_STATUS_PHASE)
							{
								// if it is USB-2-SATA command,  send status Back thru RX context
								{	// send csw
									//3609
									usb_rx_ctxt_send_status();
									//*usb_Msc0DIStatus = SCTXT_Status;
									//*usb_Msc0DICtrl = MSC_CSW_RUN;
								}
							}
							else
							{
								//DBG(("USB Rx done SCTXT_Status:%bx\n", SCTXT_Status));

							}
						}
					}
			}
		}
	}	//	if (Msc0Int & MSC_DOUT_HALT_INT)		//
	
#if 0
	else if (val & MSC_RXSENSE_DONE_INT)
	{
		DBG(("usb_msc_isr: rx sense done\n"));

		*usb_Msc0IntStatus = MSC_RXSENSE_DONE_INT;
		//ctxt_site = usb_curCtxt;
		//usb_curCtxt = CTXT_NULL;

		// check pending Ctxt queue for RX(DO) Context ????
		usb_exec_rx_que_ctxt();
		return;

	}

	else if (*usb_Msc0IntStatus_1 & (MSC_RESP_DONE_INT)) 
	{
		DBG(("usb_msc_isr: response done\n"));
			
		*usb_Msc0IntStatus = (MSC_RESP_DONE_INT);;
	}
	DBG(("msc isr rtrn\n"));
	return;
#endif
}

void usb_wrong_sata_device()
{
	//DBG0(("Dif Class\n"));
	// Disable USB Enumeration
	*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
	*usb_DevCtrlClr_0 = USB_ENUM;

	// Erase Vital Data for FAST USB Enumeration
	if (valid_sflash_vital_data)
	{
		valid_sflash_vital_data = 0;
		fast_enum_usb_flag = 0;
		usb_BusPwr_FastEnum = 0;
		sflash_erase_sector(0xE);
	}
	usb_active = 0;
}

