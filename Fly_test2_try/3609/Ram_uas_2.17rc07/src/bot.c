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
 * bot.c		2010/08/15	Winston 	Initial version
 *				2010/10/06	Ted			VBUS OFF detect modify
 * 				2010/11/18	Odin		Implement clock enable/disable function
 *				2010/11/20	Odin		AES function
 * 3609			2010/12/21	Ted			CBW command handle modify for 3609
 *										Remove AES function
 *
 *****************************************************************************/

#define BOT_C

#include "general.h"

#ifdef LINK_RECOVERY		
	bit data recoveryFlag;
#endif

/****************************************\
 bot_init
   Dn
\****************************************/
void bot_init()
{
	bot_mode = 1;
	dbuf_bot_mode();

	hdd_post_write = 0;
	//hdd_tst_unit_cnt = 0;

	usb_curCtxt = CTXT_NULL;
	usb_ctxt_que = CTXT_NULL;
	usb_post_dout_ctxt = CTXT_NULL;
	usb_reenum_flag = 0;

	bot_cbw_active = 0;

//	cmdPending = 0;
	if (sobj_init)
	{
		{
			// 	turn on ACT LED
			hdd_on_led();
		}
	}

#ifdef STANDBY_TIMER
	hdd_tick = 0;
	hdd_standby_tick = 0;
#endif

#ifdef TEMP_TIMER
	current_tempTimer = 0;
	ata_smart_cmd_pending = 0;
#endif

//	xp2tdisk	= 0;
	
#ifdef LINK_RECOVERY		
	recoveryFlag = 0;
#endif

#if 1
	hdd_que_ctxt_site = CTXT_NULL;
#endif

#ifdef BOT_TOUT
	timerCnt = 0;
	timerExpire = 0;
#endif

}


/****************************************\
   bot_device_no_data
    Dn
\****************************************/
void bot_device_no_data()
{

	SCTXT_Flag |= SCTXT_FLAG_U2B;
	//*ctxt_Next = CTXT_NULL;
	SCTXT_DbufIndex = DBUF_SEG_NULL;

	// Is Hn ? 
	if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
	{	// Set to USB Data In
		SCTXT_Flag |= SCTXT_FLAG_DIN;
	}


	if (SCTXT_Flag & SCTXT_FLAG_DIN)
	{	// Hi > Dn(case 4) or Hn = Dn(Case 1)

//		if ((*ctxt_XferLen_0 | *ctxt_XferLen_1  | *ctxt_XferLen_2 | *ctxt_XferLen_3) == 0)
		if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
		{	// case 1
			// set to case 1 (Hn == Dn)			????? case bit should bit symbolic constant
			*ctxt_PhaseCase_0 = 0x02;
		}
		else
		{	// case 4 (Hi > Dn)
			*ctxt_PhaseCase_0 = 0x10;
		}
		*ctxt_PhaseCase_1 = 0;
		//send no data
		

		
#if 1
		SCTXT_Flag |= (SCTXT_FLAG_NO_DATA);
		usb_exec_tx_ctxt();
#else
		*usb_Msc0TxCtxt_0 = MSC_TX_CTXT_NODATA|ctxt_site;

		if (*ctxt_No_Data & MSC_CTXT_NODATA)
		{	// send CSW
			*usb_Msc0DIStatus = SCTXT_Status;
			*usb_Msc0DICtrl = MSC_CSW_RUN;
		}
#endif
		return;

	}
	else
	{	// Ho > Dn (CASE 9)
		if ((*ctxt_PhaseCase_0 | *ctxt_PhaseCase_1) == 0)
		{
			*ctxt_PhaseCase_0 = 0x00;
			*ctxt_PhaseCase_1 = 0x02;
		}
		else if ((*ctxt_PhaseCase_1 & 0x1C) )	// case 11, 12, 13
		{
			*ctxt_PhaseCase_0 = 0x00;
			*ctxt_PhaseCase_1 = 0x02;
		}

#if 1
		//*ctxt_No_Data = (MSC_RX_CTXT_NODATA);
		//SCTXT_Flag |= SCTXT_FLAG_NO_DATA;
		usb_exec_rx_ctxt();
#else
		*usb_Msc0RxCtxt_0 = MSC_RX_CTXT_NODATA|ctxt_site;
#endif


		return;
	}

}

/****************************************\
   bot_device_data_in
\****************************************/
void bot_device_data_in()
{

	//DBG(("bot d_in\n"));
	DBG0(("~byteCnt = %x \n",byteCnt));
	if (byteCnt==0)
	{	// Dn
		bot_device_no_data();
		return;
	}

	SCTXT_Flag |= SCTXT_FLAG_U2B;
	//*ctxt_Next = CTXT_NULL;

	//*ctxt_Status = CTXT_STATUS_GOOD;
	SCTXT_Status = CTXT_STATUS_GOOD;


	// input port of TX buffer Seg 2  is CPU Write
	// Output port of TX buffer Seg 2  is USB Read



		// Is Hn ? 
		if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
		{	// Set to USB Data In / 3609 TX Dbuff
			SCTXT_Flag |= SCTXT_FLAG_DIN;
		}
		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{	// Hi or Hn
			if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
			{	// case 2 (Hn < Di)
				bot_device_no_data();
				return;
			}
	
			*dbuf_MuxSel = DBUF_SEG_C2U;
#if 1
			*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_CPU_W_PORT;
#else
			*dbuf_MuxInOut = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_CPU_W_PORT;
#endif
            DBG0(("~byteCnt = %x\n",byteCnt));
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

			pU8 = (u8 *)mc_buffer;
			for (; sz16 != 0; sz16--)
			{
				*dbuf_DataPort = *pU8++;
			}
			
			//DBG("Segment DONE!\t");
			*dbuf_MuxCtrl = SEG_DONE;
			//*dbuf_MuxCtrl;

			*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;			
							
			{	// Hi
				if (cbwbyteCnt == byteCnt)
				{	// case 6 (Hi = Di)
					*ctxt_PhaseCase_0 = 0x40;
				}
				else if (cbwbyteCnt < byteCnt)
				{	// case 7 (Hi < Di)
					*ctxt_PhaseCase_0 = 0x80;
					SCTXT_Status = CTXT_STATUS_PHASE;	// phase Error
				}
				else //if (CTXT_XFRLENGTH < byteCnt)
				{	// case 5 (Hi > Di)
					*ctxt_PhaseCase_0 = 0x20;
				}
			}


			//dbuf_dbg(DIR_READ, DBUF_SEG_C2U);
			
			// input port of TX buffer Seg 2  is CPU Write
			// Output port of TX buffer Seg 2  is USB Read	

#if 1
			SCTXT_DbufIndex = DBUF_SEG_C2U;
			//SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_CPU_W_PORT;
			SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_NULL;

			usb_exec_tx_ctxt();
#else
			*usb_Msc0TxCtxt_0 = ctxt_site;
#endif


            //DBG("ok\n");
			return;
		}
		else
		{	// case 10 (Ho <> Di)
			*ctxt_PhaseCase_0 = 0x00;
			*ctxt_PhaseCase_1 = 0x04;

			// phase Error
			//*ctxt_Status = CTXT_STATUS_PHASE;
			SCTXT_Status = CTXT_STATUS_PHASE;


#if 1
			SCTXT_DbufIndex = DBUF_SEG_NULL;

			SCTXT_Flag |= SCTXT_FLAG_NO_DATA;
			usb_exec_rx_ctxt();
#else
//			usb_append_ctxt_que();
			*usb_Msc0RxCtxt_0 = MSC_RX_CTXT_NODATA|ctxt_site;
#endif			
			return;
		}
}


/****************************************\
   bot_device_data_out
\****************************************/
void bot_device_data_out()
{

	if (byteCnt == 0)
	{
		SCTXT_Status = CTXT_STATUS_GOOD;
		bot_device_no_data();
		return;
	}

	SCTXT_Flag |= SCTXT_FLAG_U2B;
	//SCTXT_Next = CTXT_NULL;

//		if ((*ctxt_XferLen_0 | *ctxt_XferLen_1  | *ctxt_XferLen_2 | *ctxt_XferLen_3) == 0)
		if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
		{	// Set to USB TX Context
			SCTXT_Flag |= SCTXT_FLAG_DIN;
		}

		if (SCTXT_Flag & SCTXT_FLAG_DIN)
		{	// Hi or Hn
			
//			if ((*ctxt_XferLen_0 | *ctxt_XferLen_1  | *ctxt_XferLen_2 | *ctxt_XferLen_3) == 0)
			if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
			{	// case 3 (Hn < Do)
				*ctxt_PhaseCase_0 = 0x08;
			}
			else
			{	// case 8 (Hi <> D0)
				*ctxt_PhaseCase_0 = 0x01;
			}
			SCTXT_Status = CTXT_STATUS_PHASE;	// phase Error

#if 1
			SCTXT_DbufIndex = DBUF_SEG_NULL;

			SCTXT_Flag |= SCTXT_FLAG_NO_DATA;
			usb_exec_tx_ctxt();
#else
			*usb_Msc0TxCtxt_0 = MSC_TX_CTXT_NODATA|ctxt_site;
#endif

			if (SCTXT_Flag & SCTXT_FLAG_NO_DATA)
			{
				*usb_Msc0DIStatus = SCTXT_Status;
				*usb_Msc0DICtrl = MSC_CSW_RUN;
			}
			return;


		}
		else
		{	// Ho
			// USB Data Out
			//DBG(("bot_device_data_out\n"));

			SCTXT_Status = CTXT_STATUS_PENDING;
			if (cbwbyteCnt == byteCnt)
			{	// case 12 (Ho = Do)
				*ctxt_PhaseCase_1 = 0x10;
			}
			else if (cbwbyteCnt < byteCnt)
			{	// case 13 (Ho < Do)
				*ctxt_PhaseCase_1 = 0x20;
				SCTXT_Status = CTXT_STATUS_PHASE;	// phase Error
				bot_device_no_data();
				return;
			}
			else //if (*usb_Msc0CBWLength > byteCnt)
			{	// case 11 (Ho > Do)
				*ctxt_PhaseCase_1 = 0x08;
			}

			// input port of RX buffer Seg 2  is CPU Read
			// Output port of RX buffer Seg 2  is USB Write
	
#if 1
			SCTXT_DbufIndex = DBUF_SEG_U2C;
			//SCTXT_DbufINOUT = (TX_DBUF_CPU_R_PORT << 4) | TX_DBUF_USB_W_PORT; 
			SCTXT_DbufINOUT = (TX_DBUF_NULL << 4) | TX_DBUF_USB_W_PORT; 

			//*ctxt_No_Data = 0;
			usb_exec_rx_ctxt();
#else
			*dbuf_MuxSel = DBUF_SEG_U2C;
			*dbuf_MuxInOut = (TX_DBUF_CPU_R_PORT << 4) | TX_DBUF_USB_W_PORT;

			*usb_Msc0RxCtxt_0 = ctxt_site;
#endif
			return;
		}

}



/****************************************\
   bot_usb_bus_reset
\****************************************/
void bot_usb_bus_reset()
{

	//DBG(("bot_usb_bus_reset\n"));
	usbMode = CONNECT_UNKNOWN;				

	*usb_Msc0IntStatus_0 = 0xFF;	//MSC_TX_DONE_INT|MSC_RX_DONE_INT|BOT_RST_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT;
	*usb_Msc0IntStatus_1 = 0xFF;		//MSC_CONFIG_INT;
	*usb_Msc0IntStatus_2 = 0xFF;

	*usb_IntStatus_0 = HOT_RESET|WARM_RESET|USB_BUS_RST;
	//*usb_IntStatus_1 = CDB_AVAIL_INT|CTRL_INT|MSC0_INT;

	if (sobj_State > SATA_PWR_DWN)
		sata_Reset(USB_SATA_RST);

	dbuf_Reset();

//#ifndef SATA_AUTO_FIFO_RST
	if (sobj_State > SATA_PWR_DWN)
	{
		*sata_BlkCtrl_1 = (RXSYNCFIFORST | TXSYNCFIFORST);	//reset sata TX  fifo
		*sata_BlkCtrl_1;

		//3609 reset sata_SiteCmdIntEn to 0
		*sata_SiteCmdIntEn_0 = 0;
	}
//#endif

//	DBUF_DbufInit();
	dbuf_init();

	*usb_Ep0CtrlClr = EP0_HALT;
	*usb_Msc0DICtrl = MSC_DATAIN_RESET;
	*usb_Msc0DICtrlClr = MSC_DIN_DONE | MSC_CSW_RUN | MSC_TXFIFO_RST | MSC_DI_HALT;
	//*usb_Msc0DICtrlClr;
	*usb_Msc0DOutCtrl = MSC_DOUT_RESET;
	*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET | MSC_DOUT_HALT;
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

	bot_init();

	//MSG(("bot_bus_rst\n"));

}



/****************************************\
   usb_bot
\****************************************/
void usb_bot()
{
	*usb_Msc0Lun_SAT_0 = (*usb_Msc0Lun_SAT_0 & ~SAT_CMD) | SATA_DMAE_CMD;
	*usb_CoreCtrl_3 |=  USB3_BURST_MAX_RX;//*usb_CoreCtrl_3 &=  ~USB3_BURST_MAX_RX;
#if 0
	//unset it to fix Ch9 unknow get desc type, now set again
	if (revision_a91)
		*usb_CoreCtrl_3 &=  ~USB3_BURST_MAX_RX;

#endif
	bot_init();

	// Enable "Setup Device Request" interrupt of EP 0
	*usb_Ep0IntEn = EP0_SETUP;

	// Enable MSCn_INTEN register
	*usb_Msc0IntEn_0 = (BOT_RST_INT|MSC_TX_DONE_INT|MSC_RX_DONE_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT);
	*usb_Msc0IntEn_1 = 0;
	*usb_Msc0IntEn_2 = (MSC_CSW_DONE_INT);

	// Enable MSCn_INTEN register
	*usb_IntEn_0 = HOT_RESET|WARM_RESET|USB_BUS_RST;
	*usb_IntEn_1 = CDB_AVAIL_INT|MSC0_INT|CTRL_INT;

	if (cfa_active == 0)
		*sata_IntEn_0 = RXDATARLSDONEEN|CMDTXERREN|DATATXDONEEN|FRAMEINTPENDEN|PHYINTPENDEN;

//	*chip_IntEn_0 = USB_INTEN|SATA0_INTEN;

//	*chip_IntEn_0 = 0;

	*usb_DeadTimeout_0 = 0;
	*usb_DeadTimeout_1 = 0;

	//DBG0(("BOT %bx\n", sobj_State));
	if(usbMode == CONNECT_USB3)
	{
		*usb_DevCtrl_1 = USB2_FORCE_SUSP;
		//*usb_DevCtrlClr_2 =	USB2_PLL_FREERUN;	// disable U2 PLL Freerun
	}
#ifdef PWR_SAVING
	else
	{
		if (revision_a61)
		{	// power down USB3.0 Phy
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|HW_RESTART_VREG_DISABLE|HW_RESTART_PLL_DISABLE|PD_USB3_TX|PD_USB3_RX|PD_VREG|PD_USB3_PLL);
			//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|PD_USB3_TX|PD_USB3_RX);
		}
	}
#endif

#ifdef BOT_TOUT
	timerExpire = 0;
#endif

	while(1)
	{

begin:
		
	#ifdef BOT_TOUT
		if (timerExpire == 1 )
		{
			//DBG0(("BOT_TOUT\n"));
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
			dump_reg();
			*usb_DevCtrlClr_0 = USB_ENUM;

			EX1 = 0;
			ET0 = 0;
			while (1)
			{
			}
		}
	#endif

		if (USB_VBUS_OFF())
		{
			EAL = 0;
			MSG(("Bv of\n"));
#ifdef PWR_SAVING
			// Enable USB2.0 PLL
			//usb20_clock_enable();
			// Enable USB3.0 PLL
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
#endif
			usb_active = 0;
			//bot_usb_bus_reset();
			return;
		}

		//if (usb_active == 0)
		//	return;


		if((tmp8 = *cpu_wakeup_ctrl_0) & (CPU_USB_SUSPENDED))
		{
		#ifdef USB2_L1_PATCH		
			if (tmp8 & CPU_USB2_L1_SLEEP)
			{
				MSG((" L1\n"));
				//*cpu_wakeup_ctrl_0 = CPU_USB2_L1_SLEEP;
				while (1)
				{
					if (USB_VBUS_OFF())
					{
						EAL = 0;
						// Enable USB3.0 PLL
						//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
						spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_USB2_ENABLE|PD_USB3_TX|PD_USB3_RX);
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
						
						//DBG(("\tRes\n"));

						goto begin;
					}			
					if ((*usb_IntStatus_shadow_0 & (USB_BUS_RST |WARM_RESET)))
					{
						EAL = 0;
						spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
						EAL = 1;
						// Enable USB2.0 PLL
						//*usb_DevCtrl_2 = USB2_PLL_FREERUN;
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

						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED;
						*usb_IntStatus_0 = USB_SUSPEND;
						//Delay(1);
						if ((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED) == 0)
						{
							//DBG0(("abnormal wake\n"));
							goto begin;
						}
					}
				}
			}
			else
		#endif
			{
				if (*usb_CMgr_Status_shadow == 0)
				{
#ifdef ASIC_CONF
					EAL = 0;
					//if (revision_a61)
					{
						spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
					}
					EAL = 1;
#endif
					// Enable USB2.0 PLL
					//*usb_DevCtrl_2 = USB2_PLL_FREERUN;
					*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
					usbMode = CONNECT_UNKNOWN;
					usb3_u2_enable = 0;
					usb3_function_suspend = 0;
					usb3_device_U2_state = 0;
					//DBG0(("CMgr_St 0\n"));
				#ifdef HW_CAL_SSC
					//start_HW_cal_SSC = 1;
				#endif

					usb3_test_mode_enable = 0;

					return;
				}

				if (usb_suspend(0))
					return;	
			}
		}
		
		if (*chip_IntStaus_0 & USB_INT)
		{
			//u8 data usb_int_Stat_1;
			if (*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
			{
#ifdef ASIC_CONF
				EAL = 0;
				//if (revision_a61)
				{
					spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
				}
				EAL = 1;

				// Enable USB2.0 PLL
				//*usb_DevCtrl_2 = USB2_PLL_FREERUN;
				*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
#endif
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

				//dump_reg();
				MSG(("BRs:\n"));

				bot_usb_bus_reset();
				return;
			}

			usb_int_Stat_1 = *usb_IntStatus_shadow_1;

			/****************************************\
				USB Command IU 
			\****************************************/

			if (bot_cbw_active == 0) 
			{	
				if (usb_int_Stat_1 & CDB_AVAIL_INT)
				{			
				#ifdef REJECT_U1U2
					if (usbMode == CONNECT_USB3)
					{
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
					}
				#endif

				#if 0
					if (*usb_Msc0DICtrl & MSC_CSW_RUN)
					{
						//DBG(("MSC_CSW_RUN:%X\n", *usb_Msc0DICtrl));
						*usb_Msc0DICtrlClr = MSC_CSW_RUN;
					}
				#endif
				#ifdef UAS_EN
					if((*usb_Msc0Ctrl_1 & MSC_UAS_MODE))
					{
						return;
					}
				#endif			
		#ifdef USB2_L1_PATCH
					usb_rej_L1_in_io_transfer();
		#endif
					bot_cbw_active = 1;
					ctxt_site = *usb_CtxtAvailFIFO;
					*host_ctxtmem_ptr = ctxt_site;
					SCTXT_INDEX = ctxt_site;
					SCTXT_Flag = 0;
					
					// check cbw 
					if (*ctxt_Flag & (CTXT_FLAG_LENGTH_ERR|CTXT_FLAG_SIZE_ERR|CTXT_FLAG_SIG_ERR|CTXT_FLAG_LUN_ERR)) //
					{
						
						while(1)
						{

							*usb_Msc0DICtrl = MSC_DI_HALT;
							*usb_Msc0DOutCtrl = MSC_DOUT_HALT;

							if(*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
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

								//DBG(("CBW Err Rec rst\n"));
								return;
							}
							
							if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
							{
								//DBG(("goto bulk reset\n"));
								goto _bot_bulk_reset;
							}
						
							
							if (*usb_Ep0Ctrl & EP0_SETUP)
							{
					#ifdef DEBUG
//					printf("SETUP \n");
					#endif

								usb_control();
							}

							//if (*usb_IntStatus & VBUS_OFF)
							if (USB_VBUS_OFF())
							{
								//DBG(("V off\n"));

								usb_active = 0;
								//bot_usb_bus_reset();
							
								return;
							}
						}
					}

					if (*ctxt_Control & CTXT_CTRL_DIR)
					{
						SCTXT_Flag = SCTXT_FLAG_DIN;
					}

					if (*ctxt_No_Data & MSC_CTXT_NODATA)
					{
						SCTXT_Flag |= SCTXT_FLAG_NO_DATA;
					}

					COPYU32_REV_ENDIAN_X2D(ctxt_XferLen_0, &cbwbyteCnt);


					//*ctxt_CCMIndex = CCM_NULL;
					SCTXT_CCMIndex = CCM_NULL;
					//SCTXT_DbufIndex = DBUF_SEG_NULL;
					
					SCTXT_Status = CTXT_STATUS_GOOD;
					
	#if 0	//DBUG USE
					*dbuf_MuxSel = 0;

					if (*dbuf_MuxInOut != 0)
					{
						ERR(("Inout0:%BX\n", *dbuf_MuxInOut));
					}

					*dbuf_MuxSel = 1;
					if (*dbuf_MuxInOut != 0)
					{
						ERR(("Inout1:%BX\n", *dbuf_MuxInOut));
					}

					*dbuf_MuxSel = 2;				
					if (*dbuf_MuxInOut != 0)
					{
						ERR(("Inout2:%BX\n", *dbuf_MuxInOut));
					}				

	#endif


						CmdBlk(0) = ctxt_CDB[0];
						//timer0_hook();
						//MSG(("Cmd:%BX %BX\n", CmdBlk(0), ctxt_site));
						//CmdBlk(1) = ctxt_CDB[1];
						//CmdBlk(2) = ctxt_CDB[2];
						//CmdBlk(3) = ctxt_CDB[3];
						//CmdBlk(4) = ctxt_CDB[4];
						//CmdBlk(5) = ctxt_CDB[5];
						//CmdBlk(6) = ctxt_CDB[6];
						//CmdBlk(7) = ctxt_CDB[7];
						//CmdBlk(8) = ctxt_CDB[8];
						//CmdBlk(9) = ctxt_CDB[9];
						//CmdBlk(10) = ctxt_CDB[10];
						//CmdBlk(11) = ctxt_CDB[11];
						//CmdBlk(12) = ctxt_CDB[12];
						//CmdBlk(13) = ctxt_CDB[13];
						//CmdBlk(14) = ctxt_CDB[14];
						//CmdBlk(15) = ctxt_CDB[15];
						


start_scsi_cmd:
					//*usb_IntStatus	= CDB_AVAIL_INT;
					if (cfa_active)
					{
						//DBG(("???\n"));

						//*ctxt_Next = CTXT_NULL;
						hdd_StartAtaNoMediaCmd();
					}
					else
					{
						//MSG(("Cmd: %BX\n", CmdBlk(0)));
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
									//ERR(("Bad Rd cmd %bx\n", *ctxt_SatFailReason));
#if 0	// debug
		mem_dump(ctxt_CDB, 16);
#endif
									//*ctxt_Next = CTXT_NULL;
									hdd_err_2_sense_data(ERROR_ILL_CDB);
									usb_device_no_data();
									break;
								}

								tmp8 = *ctxt_PhaseCase_0;
								val = *ctxt_PhaseCase_1;
								SCTXT_Next = CTXT_NULL;

								// case 1 (Hn = Dn), case 4 (Hi > Dn), or case 9 (Ho > Dn)
								if ((tmp8 & 0x12) || (val & 0x02))
								{
								    //DBG(("CASE1 CASE4 CASE9\n"));
									//CTXT_Status = CTXT_STATUS_GOOD;
									usb_device_no_data();
									break;
								}
								// check case 2 (Hn < Di), case 7 (Hi < Di), or case 10 (Ho <> Di)
								if ((tmp8 & 0x84) || (val & 0x04))
								{
								    //DBG(("CASE2 CASE7 CASE10\n"));
									//*ctxt_Status = CTXT_STATUS_PHASE;			// phase Error
									SCTXT_Status = CTXT_STATUS_PHASE;			// phase Error
									bot_device_no_data();
									break;
								}


								// case 5 (Hi > Di) or case  6(Hi = Di)
								//DBG(("----Read CASE5 CASE6----\n"));
#ifdef DBG_FUNCTION
								if (sobj_State < SATA_READY)
								{
									//DBG0(("RD %bx %bx\n", sobj_State, sobj_State_tout));
								}
#endif

#ifdef PWR_SAVING
								if (sobj_State == SATA_PWR_DWN)
								{
									sata_pwr_up();
									// turn-on LED
									//hdd_on_led();
								}
#endif
	//							*ctxt_SataProto		= PROT_DMAIN;
	//							//*ctxt_DBufIndex	= DBUF_SEG_S2U;
								SCTXT_DbufIndex	= DBUF_SEG_S2U;

								//*ctxt_DBuf_IN_OUT = (TX_DBUF_USB_R_PORT << 4) |  TX_DBUF_SATA0_W_PORT;
								SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) |  TX_DBUF_SATA0_W_PORT;

								//*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 
								//CTXT_Status = CTXT_STATUS_PENDING;

//#ifdef USB_FAST_ENUM
#if 1
								hdd_exec_bot_RW_ctxt();
#else
								sata_exec_dmae_RW_ctxt();
	#ifdef BOT_TOUT
								timerCnt = 80;			// 8 sec
	#endif
#endif

								break;
							}
						
						case SCSI_WRITE6:
						case SCSI_WRITE10:
						case SCSI_WRITE12:
						case SCSI_WRITE16:
							{	
						#ifdef W_PROTECT
								if(writeprotect == 1)
								{
									hdd_err_2_sense_data(ERROR_WRITE_PROTECT);
									usb_device_no_data();
									break;
								}
						#endif
								/****************************************\
								 SCSI_WRITE Commands
								\****************************************/
								if (*ctxt_SatFailReason & (SAT_UNKNOWN_CDB|SAT_LBA_OVRN|SAT_LBA_ERR|SAT_SECCNT_ERR))
								{
									//DBG(("Bad Wr cmd %bx\n", *ctxt_SatFailReason));
									//*ctxt_Next = CTXT_NULL;
									hdd_err_2_sense_data(ERROR_ILL_CDB);
									usb_device_no_data();
									break;
								}

								tmp8 = *ctxt_PhaseCase_0;
								val = *ctxt_PhaseCase_1;
								//*ctxt_Next = CTXT_NULL;

								// case 1 (Hn = Dn), case 4 (Hi < Dn), or case 9 (Ho > Dn)
								if ((tmp8 & 0x12) || (val & 0x02))
								{
								    //DBG(("Write CASE1 CASE4 CASE9\n"));
									//SCTXT_Status = CTXT_STATUS_GOOD;
									bot_device_no_data();
									break;
								}

								// check check case 3(Hn < Do), 8(Hi <> DO), or case 13 (Ho < Do)
								if ((*ctxt_PhaseCase_0 & 0x08) || (*ctxt_PhaseCase_1 & 0x21))
								{
								   // DBG(("Write CASE3 CASE8 CASE13\n"));
									//*ctxt_Status = CTXT_STATUS_PHASE;			// CSW status
									SCTXT_Status = CTXT_STATUS_PHASE;			// CSW status
									bot_device_no_data();
									break;							
								}
								// case 11(Ho > Do) & case 12(Ho = Do)
								//if (sobj_State < SATA_READY)
								//	DBG0(("WR %bx\n", sobj_State));
                                //DBG(("---Write CASE11 CASE12---\n"));
#ifdef PWR_SAVING
								if (sobj_State == SATA_PWR_DWN)
								{
									sata_pwr_up();
									// turn-on LED
									//hdd_on_led();
								}
#endif

	//							CTXT_PROTO = PROT_DMAOUT;

								//*ctxt_DBufIndex = DBUF_SEG_U2S;
								SCTXT_DbufIndex	= DBUF_SEG_U2S;

								//*ctxt_DBuf_IN_OUT = (TX_DBUF_SATA0_R_PORT << 4) |  TX_DBUF_USB_W_PORT;
								SCTXT_DbufINOUT = (TX_DBUF_SATA0_R_PORT << 4) |  TX_DBUF_USB_W_PORT;

								//*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 
								//SCTXT_Status = CTXT_STATUS_PENDING;
								
							#if 0
								ctxt_FIS[FIS_COMMAND] = ATA6_WRITE_DMA_EXT;			//COMMAND
							#endif
//#ifdef USB_FAST_ENUM
#if 1
								hdd_exec_bot_RW_ctxt();
#else
								sata_exec_dmae_RW_ctxt();
	#ifdef BOT_TOUT
								timerCnt = 80;			// 8 sec
	#endif
#endif
									hdd_post_write = 1;
								break;
							}
						
							/****************************************\
							other commands
							VERIFY, INQUIRY, READ_CAPACITY,
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
							CmdBlk(15) = ctxt_CDB[15];

							//*ctxt_Next = CTXT_NULL;

							hdd_StartAtaCmd();

							//*usb_IntStatus_1 = CDB_AVAIL_INT|MSC0_INT;
						}	// switch (CTXT_CDB[0])
					}
					goto begin;
				}	// 		if (*usb_IntStatus & CDB_AVAIL_INT)
			}	//	if (bot_cbw_active == 0) 
		#ifdef LINK_RECOVERY		
			else
			{
				if (recoveryFlag == 0)
				{
					if (*usb_USB3LTSSM_1 & LTSSM_RECOVERY)
					{
						recoveryFlag = 1;
					}
				}
				else
				{
					if ((*usb_USB3LTSSM_1 & LTSSM_RECOVERY) == 0)
					{
						recoveryFlag = 0;
						if (CmdBlk(0) == 0x28)
						{
							//DBG0(("RR "));
						}
						else if (CmdBlk(0) == 0x2A)
						{
							//DBG0(("WR "));
						}
						else
						{
							//DBG0(("R?%BX ", CmdBlk(0)));
						}
					}
				}
			}
		#endif		//LINK_RECOVERY



			if (usb_int_Stat_1 & CTRL_INT)
			{	
				// EP0_SETUP----------------
				if (*usb_Ep0Ctrl & EP0_SETUP)
				{	
					//DBG(("stp\n"));
					usb_control();
				}
				goto begin;
			}

			if (usb_int_Stat_1 & MSC0_INT)
			{
				if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
				{
	_bot_bulk_reset:
					if (bot_cbw_active)
					{
						*usb_CtxtFreeFIFO = ctxt_site;
						*usb_Msc0CtxtUsed_0 = 0xFF;
					}

					*usb_Msc0IntStatus_0 = BOT_RST_INT;
					bot_usb_bus_reset();
					//DBG(("Bulk RST:"));

#if 0					
	MSG(("usb_Msc0TxCtxt_0:%BX\n", *usb_Msc0TxCtxt_0));
	MSG(("usb_Msc0TxCtxt_1:%BX\n", *usb_Msc0TxCtxt_1));

	*usb_USB3StateSelect = 0x09;
	DBG0(("usb_USB3StateCtrl: %BX\n", *usb_USB3StateCtrl));	
#endif
					continue;
				}

				if (*usb_Msc0IntStatus_0 & (MSC_TX_DONE_INT|MSC_RX_DONE_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT))
					usb_msc_isr();

				if (*usb_Msc0IntStatus_2 & (MSC_CSW_DONE_INT))
				{
					//xtimer_unload();
					//DBG(("MSC_CSW_DONE_INT\n"));
					*usb_Msc0IntStatus_0 = 0xff;	//MSC_TX_DONE_INT|MSC_RX_DONE_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT;
					*usb_Msc0IntStatus_2 = 0xff;

					usb_curCtxt = CTXT_NULL;
					bot_cbw_active = 0;
#ifdef REJECT_U1U2
					//U1U2_ctrl_timer = U1U2_REJECT_TIME;
#endif
					if (usb_reenum_flag)
					{
						Delay(100);

						usb_reenum();
						bot_init();
					}
					//goto begin;
				}

			}	// if ((usb_int_Status & MSC0_INT)
		}	// 	if (*chip_IntStaus_0 & USB_INT)


#if 1
		if ((sobj_State > SATA_PWR_DWN)  && (*chip_IntStaus_0 & SATA0_INT))
		{
#if 0
			if (sata_IntStatus_0 & PHYINTPEND)
			{
				if (sata_PhyInt & PHYDWNI)
				{
					sobj_State = SATA_RESETING;
					usb_active = 0;
					return;
				}
			}
#endif
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
					else //if (sobj_class == DEVICECLASS_ATAPI)
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
				if (sobj_State >= SATA_STANDBY)
				{
					hdd_que_ctxt_site = CTXT_NULL;
					goto start_scsi_cmd;
				}
			}

		}
#endif
		
		if (sobj_State >= SATA_DRV_RDY)
		{
			if ((*sata_PhyStat & PHYRDY) == 0)
			{
				if (sobj_State > SATA_READY)
				{
					//DBG(("S UPLUG\n"));
					sobj_State = SATA_RESETING;
					usb_active = 0;
					return;
				}
				else
				{
					//DBG(("SPhy NR\n"));
					sata_Reset(SATA_HARD_RST);
				}
			}

		}
		if (cfa_active)
		{
			if (*sata_PhyStat & PHYRDY)
			{
				if (sobj_State == SATA_NO_DEV)
				{
					sobj_State = SATA_RESETING;
					cfa_active = 0;
					usb_active = 0;
					return;
				}
			}
		}
		
		if (usb_post_dout_ctxt != CTXT_NULL)
		{
			ctxt_site = usb_post_dout_ctxt;
			usb_post_dout_ctxt = CTXT_NULL;
			
			//{
				 hdd_post_data_out();
			//}
		}

//#ifdef STANDBY_TIMER
#if 1
		if (hdd_tick)
		{
			hdd_tick_isr();
			if (usb_active == 0)
				return;
			if (hdd_que_ctxt_site != CTXT_NULL)
			{
				if (hdd_que_ctxt_tout)
				{
					hdd_que_ctxt_tout--;
					if (hdd_que_ctxt_tout == 0)
					{
						hdd_que_ctxt_site = CTXT_NULL;

						// Time out
						hdd_err_2_sense_data(ERROR_UA_BECOMING_READY);
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
					hdd_led_going_off();
#if 1
					sobj_State = SATA_NO_DEV;

					//DBG0(("sobj_init_tout\n"));
					usb_active = 0;
					return;
#else
					sobj_init = 1;
					if (hdd_que_ctxt_site != CTXT_NULL)
					{
						hdd_que_ctxt_site = CTXT_NULL;

						hdd_err_2_sense_data(ERROR_UA_NO_MEDIA);
						usb_device_no_data();
						hdd_start_fault_led();
					}
#endif
				}
			}
			hdd_tick = 0;
		}
#endif


	} // End of while loop
}

