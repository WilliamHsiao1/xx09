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
 * 3610		2010/05/15	Abel 	Initial version
 * 3607		2010/10/06	Ted		VBUS OFF detect modify
 * 3607		2010/11/18	Odin	clock enable/disable function
 *
 *****************************************************************************/

#define USB_HDD_C

#include "general.h"

void switch_regs_setting(u8 reg_mode)
{
	if (usb3_test_mode_enable == 0)
	{
		if (reg_mode & COMPLIANCE_TEST_MODE)
		{
		#ifdef HW_CAL_SSC
			// set SSC PPM to 4500ppm
			spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x09);
			//spi_phy_wr_retry(PHY_SPI_U3PMA, 0x08, 0xFC); 			
			//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_09, 0x9E);
			//spi_phy_wr_retry(PHY_SPI_U3PCS, 0x29, 0x0E);
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x01); // disable HW SSC function
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x00); 
			start_HW_cal_SSC = 0;
		#else
			// set SSC PPM to 4500ppm & PLL open Loop
			spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x89);

			// SSC Enable & SSC_UP_SPREAD_MODE
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x02, 0x44);
		#endif
			usb3_test_mode_enable = 1;
		}

		if (reg_mode & LOOPBACK_TEST_MODE)
		{
			// set SSC PPM equal 0
			//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x00);
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x07, 0x88);
		#ifndef AMD_X75
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x08, 0x7C);
		#endif

			// cdr_lp_gn[5:0], CDR second order filter tracking ranger up to 15K PPM enable: bit_7
			spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_09, 0x9D);
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x29, 0x7E);
			
			usb3_test_mode_enable = 1;
		}
	}
	else
	{
		if (reg_mode == NORMAL_MODE)
		{
			// set SSC PPM equal 0
		#ifdef HW_CAL_SSC
			// set SSC PPM equal 0 & open loop PLL
			spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, 0x00);
		#else
			// set SSC PPM equal 0 & closed loop PLL
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x03, 0x80);
			// SSC Disable
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x02, 0x4A);
		#endif
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x07, 0x88);

		#ifndef AMD_X75
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x08, 0xFC); 
		#else
			spi_phy_wr_retry(PHY_SPI_U3PMA, 0x08, 0x7C); 			
		#endif
			// cdr_lp_gn[5:0], CDR second order filter tracking ranger up to 15K PPM enable: bit_7
			spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_09, 0x9E);

		#ifndef AMD_X75
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x29, 0x0E);
		#else
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x29, 0x4E);
		#endif

		#ifdef HW_CAL_SSC
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x00); // disable HW SSC function
			start_HW_cal_SSC = 0;
		#endif
			usb3_test_mode_enable = 0;
		}
	}
}

void check_usb_mode(void)
{
	tmp8 = *usb_DevStatus_0;

	if (tmp8 &  USB3_MODE )
	{
		#ifdef DBG_FUNCTION
			//val = spi_phy_rd(PHY_SPI_U3PCS, 0x1C);
		#endif
		usbMode = CONNECT_USB3;
		DBG(("\tUsb3 %BX\n", val));

	#ifdef FAST_USB20
		if (rx_detect_count == RX_DETECT_COUNT1)
		{
			*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_8;

			// 7(6+1) ms USB2_CONNECT_TIMEOUT
			*usb_USB2ConnectTout_0 = 9;
			*usb_FSConnectTout_0 = 0x1a;		// 

			rx_detect_count = RX_DETECT_COUNT8;
		}
	#endif
//#ifdef A71_PATCH
	#if 1
		if (revision_a71)
		{
			*usb_DeviceRequestEn_0 &= (~(USB_GET_STATUS_ENABLE|USB_SET_FEATURE_ENABLE));
			*usb_DeviceRequestEn_1 &= (~(USB_CLEAR_FEATURE_ENABLE));
		}
	#endif
	}
	else
	{	
	#ifdef FAST_USB20
		if (rx_detect_count == RX_DETECT_COUNT8)
		{
			*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_1;

			// 7(6+1) ms USB2_CONNECT_TIMEOUT
			*usb_USB2ConnectTout_0 = 6;
			*usb_FSConnectTout_0 = 6;		// 

			rx_detect_count = RX_DETECT_COUNT1;
		}
	#endif

		if (*usb_DevStatus_0 & USB2_HS_MODE)
		{
			//*usb_CoreCtrl_2 |= USB3_MODE_DISABLE;
			DBG(("\tUsb2 %BX\n", *cpu_Clock_2));
			//*usb_CoreCtrl_2 &= ~USB3_MODE_DISABLE;
			usbMode = CONNECT_USB2;
	#if 0
			dump_phy();
	#endif
		}
		else
		{
			DBG(("\tUsb1\n"));
			if (sobj_State > SATA_PWR_DWN)
				*sata_HoldLv = 0x60;
			usbMode = CONNECT_USB1;
		}
	}
#if 0
	if ((usb_power_on_flag) && (usbMode != CONNECT_USB3))
	{
		DBG0(("enum ag\n"));
		reg_w8(usb_Ep0CtrlClr, EP0_SETUP);	
		*usb_DevCtrlClr_0 = USB_ENUM;		//disable USB enumeration
		*usb_CoreCtrl_2 &= ~(USB3_MODE_DISABLE | USB2_MODE_DISABLE);	
		Delay(200);
		*usb_DevCtrl_0 = USB_ENUM;		//do USB enumeration
		usbMode = CONNECT_UNKNOWN;
		usb_power_on_flag = 0;
	}
#endif
	//DBG0((" LT %bx\n", *usb_USB3LTSSM_0));
	//DBG0((" CMG_ST %bx\n", *usb_CMgr_Status_shadow));
	//	hdd_on_led();
	DBG0(("\tUsbmode %BX\n", usbMode));
	*usb_USBLinkConfig1_3 = (*usb_USBLinkConfig1_3 & (~LFPS_TRESETDELAY)) | LFPS_TRESETDELAY_18MS;
}

#ifdef USB2_L1_PATCH
void usb_rej_L1_in_io_transfer(void)
{
	// because hardware does not support the L1 resume automatically, firmware does the patch to reject L1 in IO transfer
	if (usbMode == CONNECT_USB2)
	{
		// Disable USB2.0 L1
		*usb_DevCtrl_2 = USB2_L1_DISABLE;
		*cpu_wakeup_ctrl_2 = *cpu_wakeup_ctrl_2  | USB_WAKEUP_REQ;
		usb2_L1_reject_timer = 8;	
	}
}
#endif

/****************************************\
   usb_hdd
\****************************************/
void usb_start()
{
#ifdef DEBUG_LTSSM
	//u8 data old_ltssm_state;
#endif
	//u16 usb_det_tout;
//	u8 usb_st_mc;
	//bit data disabled_flag;

	*usb_DeadTimeout_0 = 0;
	*usb_DeadTimeout_1 = 0;



//	Response_IU_Pending = 0;

//	DBUF_DbufInit();

#ifdef SCSI_DOWNLOAD_FW
	FW_Download = FW_DOWNLOAD_INITIALIZING;
#endif

	usb_mode_ctrl = USB23_MODE_ENABLE;

	//old_cmgr_status = tmp8 = *usb_CMgr_Status_shadow;
	//DBG0(("CMG_ST %bx\n", tmp8));

	//disabled_flag = 0;
	if ((sobj_State > SATA_PWR_DWN) && (sobj_init == 0))
	//if ((SATA_PHY_RDY >= sobj_State) && (sobj_State > SATA_PWR_DWN))
	{
		*sata_IntEn_0 = FRAMEINTPENDEN|PHYINTPENDEN;
	}

	usb_ltssm_state_0 = *usb_USB3LTSSM_0_shadow;

#if 0// def DEBUG_LTSSM
	*usb_USB3StateSelect = USB3_LTSSM;
	tmp8 = *usb_USB3StateCtrl;
	MSG(("LT %BX\n", tmp8));
	//old_ltssm_state = tmp8;
#endif

	bot_cbw_active = 0;
	#ifdef STANDBY_TIMER
		hdd_tick = 0;
		hdd_standby_tick = 0;
	#endif
#ifdef EQ_SETTING_TEST
	count_recvry_en = 1;
	recovery_count = 0;
	sobj_crc_count = 0;
#endif
	while(1)
	{
begin:
		{
			if (*usb_USB3LTSSM_0_shadow == LTSSM_POLLING)
			{
			#ifdef HW_CAL_SSC
				EAL = 0;
				if (start_HW_cal_SSC)
				{
					//if (spi_phy_rd(PHY_SPI_U3PCS, 0x43) & BIT_3) // USB RX ready
					{
						spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x03); // enable HW calculate SSC
						MSG(("E Ad\n"));
						start_HW_cal_SSC = 0;
					}			
				}
				EAL = 1;
			#endif
				if ((usb_mode_ctrl & USB3_MODE_ONLY) == 0)
					usb_mode_ctrl |= USB_MODE_CHANGED;
			}
			else
			{
				if (usb_mode_ctrl & USB3_MODE_ONLY)
					usb_mode_ctrl |= USB_MODE_CHANGED;
			}
			
			if (usb_mode_ctrl & USB_MODE_CHANGED)
			{
				if ((usb_mode_ctrl & USB3_MODE_ONLY) == 0)
				{
					*usb_CoreCtrl_2 = (*usb_CoreCtrl_2 & (~USB3_MODE_DISABLE)) | USB2_MODE_DISABLE;
					usb_mode_ctrl |= USB3_MODE_ONLY;
					MSG(("3\n"));
				}
				else
				{
					*usb_CoreCtrl_2 &= ~(USB3_MODE_DISABLE | USB2_MODE_DISABLE);
					usb_mode_ctrl &= ~USB3_MODE_ONLY;
					MSG(("A %BX\n", *usb_USB3LTSSM_0_shadow));
				}
				usb_mode_ctrl &= ~USB_MODE_CHANGED;
			}
		}

		// check USB VBUS
		if (USB_VBUS_OFF())
		{
			DBG0(("V off\n"));
//			usb_active = 0;
			//bot_usb_bus_reset();
			return;
		}		


		if((tmp8 = *cpu_wakeup_ctrl_0) & CPU_USB_SUSPENDED)
		{
			DBG0(("susSig %bx\n", tmp8));
		#ifdef USB2_L1_PATCH		
			if (tmp8 & CPU_USB2_L1_SLEEP)
			{
				DBG0(("U2L1\n"));
				while (1)
				{
					if (USB_VBUS_OFF())
					{
						//DBG0(("\tH VB OF1\n"));
						//bot_usb_bus_reset();
						return; //	power down funciton implement in usb_main()
					}
					if (*cpu_wakeup_ctrl_0 & CPU_USB_UNSUSPENED)
					{
						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;
						*usb_IntStatus_0 = USB_SUSPEND;
						
						DBG0(("Res\n"));

						goto begin;
					}			
					if ((*usb_IntStatus_shadow_0 & (USB_BUS_RST |WARM_RESET)))
					{
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
							DBG0(("abWake2\n"));
							goto begin;
						}
					}
				}	// while
			}
			else
		#endif
			{
				DBG0(("RU2EN\n"));
				EAL = 0;
				spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
				EAL = 1;
			}

			tmp8 = *usb_CMgr_Status_shadow;
			//DBG0(("H Sus %BX %BX\n", *usb_CMgr_Status_shadow, *usb_DevState_shadow));
			DBG0(("uss %BX %BX\n", tmp8, *usb_DevState_shadow));
			if ((cfa_active == 0) &&
				(usb_BusPwr_FastEnum) &&
				((tmp8 == 0x22) ||	//USB3.0	CMGR_USB3_SUSPEN
				 ((tmp8 == 0x15) && ((*usb_DevState_shadow & L1_SLEEP) == 0)))
			   )			//USB2.0 CMGR_USB2_SUSPEND
			{
				if (usb_suspend(1))
					return;		
			}
			else
			{
				//DBG0(("HSu %bx\n", *cpu_wakeup_ctrl_0));

		        // there is a condition that happen in usb2 host that cause the unsuspend
		        // pending even when there is no unsuspend
		        //clear it first, Can we missing the real unsuspend?, possibilty is real low.
				*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;

				usb3_function_suspend = 0;

				DBG0(("fSus %bx\n", *cpu_wakeup_ctrl_0));

				// wait for 60s
				for (tmp16=0; tmp16 < 30000; tmp16++)
				{
				//DBG0((" %bx ", *cpu_wakeup_ctrl_0));
			
					if (USB_VBUS_OFF())
					{
						DBG(("\tH VB OF1\n"));
						//bot_usb_bus_reset();
						return; //	power down funciton implement in usb_main()
					}
					if (*cpu_wakeup_ctrl_0 & CPU_USB_UNSUSPENED)
					{
						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;
						*usb_IntStatus_0 = USB_SUSPEND;
						
						DBG0(("\tHW %bx\n", *usb_CMgr_Status_shadow));

						goto begin;
					}			
					if (((*usb_IntStatus_shadow_0 & (USB_BUS_RST |HOT_RESET)) ||
						(*usb_IntStatus_shadow_1 & (CTRL_INT)) ||
						(*usb_USB3LTSSM_0_shadow & LTSSM_U0)) && (usb_power_on_flag == 1)
						)
					{
						// Enable USB2.0 PLL
						//*usb_DevCtrl_2 = USB2_PLL_FREERUN;

						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED;
						*usb_IntStatus_0 = USB_SUSPEND;
						Delay(1);
						if ((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED) == 0)
						{
							DBG0(("abWake1\n"));
							goto begin;
						}
					}

					if (sobj_State >= SATA_DRV_RDY)
					{
						if ((*sata_PhyStat & PHYRDY) == 0)
						{
							DBG0(("SPhy NRdy"));
							sata_Reset(SATA_HARD_RST);
							return;
						}
					}

					if ((sobj_State > SATA_PWR_DWN)  && (*chip_IntStaus_0 & SATA0_INT))
					{
						sata_isr();
					#ifdef USB_FAST_ENUM
						if (sobj_init == 0)
						{
							if (sobj_State == SATA_DRV_RDY)
							{
								if (sobj_class != org_sobj_class)
								{
									// Wrong Device
								#if 1
									usb_wrong_sata_device();
								#else
									DBG0(("Dif Class\n"));
									// Disable USB Enumeration
									#ifdef PWR_SAVING
										*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
									#endif
									*usb_DevCtrlClr_0 = USB_ENUM;

									// Erase Vital Data for FAST USB Enumeration
									if (valid_nv_vital_data)
									{
										valid_nv_vital_data = 0;
										NvEraseVitalData();
										fast_enum_usb_flag = 0;
										usb_BusPwr_FastEnum = 0;
									}
									usb_active = 0;
								#endif
									return;
								}
							}
						}
					#endif
					}
					Delay(2);
				}
				DBG0(("\tH tout\n"));
		#if 1
				//hdd_led_going_off();

			#ifdef ATAPI
				if (sobj_class == DEVICECLASS_ATA)
			#endif
				{
					//if (sobj_State > SATA_READY)
					//{	// reset SATA HDD
					//	scan_sata_device();
					//}

				#ifdef POW_MGMT
					if (PwrMgmt && ((sobj_State == SATA_DRV_RDY) || (sobj_State == SATA_READY)))
				#else
					if (((sobj_State == SATA_DRV_RDY) || (sobj_State == SATA_READY)))
				#endif
					{
					#ifdef USB_FAST_ENUM
						if (sobj_State == SATA_DRV_RDY)
						{
							#ifdef USB2_L1_PATCH
								if (usbMode == CONNECT_USB2)
								{
									usb2_L1_reject_timer = 8;
								}
							#endif

							if (ata_fast_enum_init())
								return;
						}
					#endif
						ata_ExecNoDataCmd(ATA6_STANDBY_IMMEDIATE, 0);
						sobj_State = SATA_STANDBY;
					#ifdef PWR_SAVING
						sata_pwr_down();
					#endif
					}
					hdd_led_going_off();

				}
				xtimer_disable();
		#endif
		#ifndef USB2_L1_PATCH
				ET0 = 0;			//Disable Timer 0 interrupt
		#endif
				while (1)
				{
				
					if (*cpu_wakeup_ctrl_0 & CPU_USB_UNSUSPENED)
					{
						DBG0(("H Wa %BX\n", *usb_CMgr_Status_shadow));
						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED;
						//DBG0(("#%BX %BX\n", *usb_CMgr_Status_shadow, *usb_DevState_shadow));
						break;
					}
	
					if (USB_VBUS_OFF())
					{
						MSG(("H vbus off\n"));
						usb_active = 0;
						//bot_usb_bus_reset();
						return;
					}
//					if ((*usb_IntStatus_0 & USB_BUS_RST) && (usb_power_on_flag == 1))
					if ((*usb_IntStatus_shadow_0 & USB_BUS_RST))
					{
						// Enable USB2.0 PLL
						//*usb_DevCtrl_2 = USB2_PLL_FREERUN;

						*cpu_wakeup_ctrl_0 = CPU_USB_SUSPENDED;
						*usb_IntStatus_0 = USB_SUSPEND;
						DBG(("clr %bx, %lbx\n", *cpu_wakeup_ctrl_0, *usb_IntStatus_shadow_0));
						Delay(1);
						DBG(("%bx, %bx\n", *cpu_wakeup_ctrl_0, *usb_IntStatus_shadow_0));	
						if ((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED) == 0)
							break;
					}
				}
				
				hdd_on_led();
				xtimer_setup();
				timer0_setup();
			#ifdef STANDBY_TIMER
				hdd_tick = 0;
				hdd_standby_tick = 0;
			#endif
			}
		}		

		if (usb3_function_suspend)
		{
			if ((usb3_device_U2_state == 0) && (usb3_u2_enable))
			{
				if (usb3_U2_inactivity_timer == 0)
				{
					DBG(("u2 inattm %bx, ", *usb_U2InactivtyTimeout));
					if (*usb_U2InactivtyTimeout == 0xFF)
					{
						MSG(("devS %bx\n", *usb_DevStatus_0));
						usb3_device_U2_state = 1;
						*usb_DevCtrl_1 = USB3_U2_ENTER;
						MSG(("dev U2\n"));
					}
				}
			}
			else if (usb3_device_U2_state)
			{
				if (usb_ltssm_state_0 != *usb_USB3LTSSM_0_shadow)
				{
					if (usb_ltssm_state_0 == LTSSM_U2)
					{
						*usb_DevCtrlClr_1 = USB3_U2_ENTER;
						MSG(("dev U2 exit"));
					}	
					usb_ltssm_state_0 = *usb_USB3LTSSM_0_shadow;
				}
			}
		}

		if (sobj_State == SATA_PWR_DWN)
		{
			//if ((globalNvram.USB_ATTR & 0x40) == 0)
			if (usb_self_power == 0)
			{	// Bus Power device
				if (*usb_DevState_shadow & (USB_CONFIGURED))
				{
			#if 0
					if (usbMode == CONNECT_USB3)
					{
						//*usb_DevCtrl_1 = USB2_FORCE_SUSP;
						*usb_DevCtrlClr_2 =	USB2_PLL_FREERUN;	// disable U2 PLL Freerun
					}
			#endif

			#ifdef PWR_SAVING
					sata_pwr_up();
					sata_Reset(SATA_HARD_RST);
			#endif
DBG0(("Cfg\n"));
				}
			}
		}

		// EP0_SETUP----------------
		if (*usb_Ep0Ctrl & EP0_SETUP)
		{	
#ifdef DEBUG_LTSSM
			usb3_U2_inactivity_timer = 100;
#else
			usb3_U2_inactivity_timer = 10;
#endif

		#ifdef HW_CAL_SSC
			// disable HW calculate center Freq from Host
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x00);

			// SSC Disable
			//spi_phy_wr_retry(PHY_SPI_U3PMA, 0x02, 0x4A);
		#endif
			//if (sobj_State > SATA_PWR_DWN)
			//	*sata_HoldLv = 0x48; // set to default value

			if (usbMode == CONNECT_UNKNOWN)
			{
				// disable HW calculate SSC
				//spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x00);

				check_usb_mode();
			}

			switch_regs_setting(NORMAL_MODE);
			usb_power_on_flag = 0;
			//DBG(("Setup\n"));
			usb_control();
			// in NEC host, it will not issue the bus reset, then change the protocol
			continue;
		}

		if (*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
		{
			EAL = 0;
	#ifdef ASIC_CONF
			//if (revision_a61)
			{
				spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
			}
	#endif
			usb_inactive_count = 0;
		#ifdef USB3_TX_DBG	
			//polling_failed_count = 0;
			recovery_count = 0;
			count_recvry_en = 1;
			//usb_rst_tx_flag = 0;
		#endif
			EAL = 1;
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


			// Enable USB2.0 PLL
//			*usb_DevCtrl_2 = USB2_PLL_FREERUN;
//			*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;

			// Enable USB2.0 L1
			*usb_DevCtrlClr_2 = USB2_L1_DISABLE;

			if (*usb_IntStatus_shadow_0 & USB_BUS_RST)
			{
			#if 1
				if (rx_detect_count == RX_DETECT_COUNT8)
				{
					*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_1;

					if (*usb_USB3LTSSM_0_shadow == LTSSM_RXDETECT) // patch the issue that the RX-detection CNT can't be changed in the state of RX-detect
						*usb_DevCtrl_1 = EXIT_SS_DISABLED;

					// 7(6+1) ms USB2_CONNECT_TIMEOUT
					*usb_USB2ConnectTout_0 = 6;
					*usb_FSConnectTout_0 = 6;		// 

					rx_detect_count = RX_DETECT_COUNT1;
				}
			#endif
			}
			else
			{
				if (usb3_test_mode_enable)
				{
					if (*usb_IntStatus_shadow_0 & WARM_RESET)
					{
						switch_regs_setting(NORMAL_MODE);
					}
				}

				//*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_8;
				if (rx_detect_count == RX_DETECT_COUNT1)
				{
					*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_8;

					// 7(6+1) ms USB2_CONNECT_TIMEOUT
					*usb_USB2ConnectTout_0 = 9;
					*usb_FSConnectTout_0 = 0x1a;		// 

					rx_detect_count = RX_DETECT_COUNT8;
				}
				//spi_phy_wr_retry(PHY_SPI_U3PCS, 0x12, spi_phy_rd(PHY_SPI_U3PCS, 0x12) | BIT_7);
			}

#if 1		// for old NEC Driver
			*usb_DevCtrlClr_1 = USB3_U1_REJECT;	
			*usb_DevCtrlClr_2 = USB3_U2_REJECT;	

#endif

			MSG(("HRs:%bx ", *usb_IntStatus_shadow_0));

			//usb_Msc0IntStatus must clear before usb_IntStatus
			//*usb_Msc0IntStatus_0 = BOT_RST_INT;
			*usb_IntStatus_0 = HOT_RESET|WARM_RESET|USB_BUS_RST;
			//*usb_IntStatus_1 = MSC0_INT;
			
			//bot_usb_bus_reset();

		#if 0
			// turn off power if it is USB BUS Power device
			//if ((globalNvram.USB_ATTR & 0x40) == 0)
			if ((usb_self_power == 0) &&
				(fast_enum_usb_flag))
			{	// bus power
				if ((sobj_State > SATA_PWR_DWN))
				{
					sata_pwr_down();
				}
			}
		#endif

			usbMode = CONNECT_UNKNOWN;
			usb3_u2_enable = 0;
			usb3_function_suspend = 0;
			usb3_device_U2_state = 0;
			//usb_det_tout = 0;	
			//disabled_flag = 0;
			continue;	// to check the RST & SUSP again				
		}


		if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
		{

			MSG(("H Bulk Rst:"));

#if 1
			bot_usb_bus_reset();
#else
			*dbuf_MuxSel = 0;
			*dbuf_MuxCtrl = SEG_RESET;
			*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;

			*dbuf_MuxSel = 1;
			*dbuf_MuxCtrl = SEG_RESET;
			*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;

			*dbuf_MuxSel = 2;
			*dbuf_MuxCtrl = SEG_RESET;
			*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
				
			*usb_Ep0CtrlClr = EP0_HALT;
			*usb_RecoveryError = 0x00;                       //for usb hot rest
			*usb_Msc0DICtrlClr = MSC_DIN_DONE | MSC_CSW_RUN | MSC_DI_HALT ;
			*usb_Msc0DOutCtrlClr = MSC_DOUT_RESET | MSC_DOUT_HALT ;

//			DBUF_DbufInit();		
#endif
			*usb_Msc0IntStatus_0 = BOT_RST_INT;		
		}

#ifdef USB_FAST_ENUM
		//if (*usb_DevState & USB_CONFIGURED)
		//if ((sobj_State > SATA_PWR_DWN) && (sobj_init == 0))
		if ((sobj_State > SATA_PWR_DWN))
		{
			if (sobj_State < SATA_DRV_RDY)
			{
				if (*chip_IntStaus_0 & SATA0_INT)
				{
					sata_isr();
	#if 1
					if (sobj_State >= SATA_DRV_RDY)
					{
						DBG(("On Led\n"));
						hdd_on_led();
					}

					if (sobj_init == 0)
					{
						if (sobj_State == SATA_DRV_RDY)
						{
							if (sobj_class != org_sobj_class)
							{
								// Wrong Device
							#if 1
								usb_wrong_sata_device();
							#else
								DBG(("Dif Class\n"));
								// Disable USB Enumeration
								#ifdef PWR_SAVING
									*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
								#endif
								*usb_DevCtrlClr_0 = USB_ENUM;

								// Erase Vital Data for FAST USB Enumeration
								if (valid_nv_vital_data)
								{
									valid_nv_vital_data = 0;
									NvEraseVitalData();
									fast_enum_usb_flag = 0;
									usb_BusPwr_FastEnum = 0;
								}
								usb_active = 0;
							#endif
								return;
							}
						}
					}
	#endif
				}
			}
			//if (sobj_State == SATA_DRV_RDY)
			//{
			//	if (ata_fast_enum_init())
			//		return;
			//}
		}
#endif

#ifdef UAS_EN
		if ((*usb_IntStatus_shadow_1 & CDB_AVAIL_INT) ||  (*usb_Msc0IntStatus_1 & MSC_TASK_AVAIL_INT))
#else
		if ((*usb_IntStatus_shadow_1 & CDB_AVAIL_INT))
#endif
		{
#ifdef USB3_TX_DBG
			count_recvry_en = 0;
#endif
			// for old NEC Driver
			*usb_DevCtrl_1 = USB3_U1_REJECT;	
			*usb_DevCtrl_2 = USB3_U2_REJECT;

		#ifdef REJECT_U1U2
			u1U2_reject_state = U1U2_REJECT;
			if (usbMode == CONNECT_USB3)
			{
				// reject until end of CBW & CIU
				U1U2_ctrl_timer = U1U2_REJECT_TIME;
			}
			else
			{
				// no U1 & U2 for USB2.0 & USB1.1
				// It should stay in "U1U2_REJECT" forever
				U1U2_ctrl_timer = 0;
			}
		#endif

		#ifdef HW_CAL_SSC
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x00); 
		#endif

		#if 1
			//disable elastic buffer not drop all SKP
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x12, (spi_phy_rd(PHY_SPI_U3PCS, 0x12) & ~BIT_7));
		#endif
			if (usbMode == CONNECT_UNKNOWN)
			{
				check_usb_mode();
			}

		#ifdef PWR_SAVING
			if (sobj_State == SATA_PWR_DWN)
			{
				DBG(("SATA pwr down %bx\n", *usb_DevState_shadow));
				//if (*usb_DevState_shadow & (USB_CONFIGURED))
				{
					sata_pwr_up();
					sata_Reset(SATA_HARD_RST);
				}
			}
		#endif

			*usb_DeviceRequestEn_1 |= USB_GET_DESCR_STD_UTILITY;

		#ifdef UAS_EN
			if(*usb_Msc0Ctrl_1 & MSC_UAS_MODE)
			{
				bot_mode = 0;
			}
		#endif

	#ifdef USB2_L1_PATCH
			if (usbMode == CONNECT_USB2)
			{
				usb_rej_L1_in_io_transfer();
			}
			else
			{
				ET0 = 0;
			}
	#else
			if (usbMode == CONNECT_USB2)
			{	// Disable USB2.0 L1
				*usb_DevCtrl_2 = USB2_L1_DISABLE;
				ET0 = 0;			//Disable Timer 0 interrupt
			}
		#ifndef USB3_TX_DBG
			else
			{
				// disable timer 0
				//ET0 = 0;			//Disable Timer 0 interrupt
			}
		#endif
	#endif

			DBG(("\t %BX %BX %BX %BX\n", sobj_State, sobj_class, org_sobj_class, (u8)sobj_init));
#ifdef USB_FAST_ENUM
			if ((cfa_active == 0) && (sobj_init == 0))
			{
				if (sobj_State == SATA_DRV_RDY)
				{
					if (sobj_class == org_sobj_class)
					{
						if (sobj_class == DEVICECLASS_ATA)
						{
							if (ata_fast_enum_init())
								return;
						}
					#ifdef ATAPI
						else if (sobj_class == DEVICECLASS_ATAPI)
						{
							if (atapi_fast_enum_init())
								return;
						}
					#endif
						else
						{
DBG0(("U Wrong class %BX\n", sobj_class));
							sobj_State = SATA_NO_DEV;
							usb_active = 0;
							return;
						}
					}
				}
			}
#endif
		#ifndef DBG_FUNCTION
			// disable timer 0
			ET0 = 0;			//Disable Timer 0 interrupt
		#endif

		#ifdef UAS_EN
			if(*usb_Msc0Ctrl_1 & MSC_UAS_MODE)
			{
				*usb_LinDBG_0 |= DISCARD_ZERO_LENGTH_QUALIFIER;
			//#ifdef INTEL_SEQNUM_PATCH
			#if 0
				intel_host_flag = 0;
				intel_SeqNum_Monitor_Count = 3;
				intel_SeqNum_monitor = INTEL_SEQNUM_START_MONITOR;
			#endif
				usb_uas();

			//#ifdef INTEL_SEQNUM_PATCH
			#if 0
				intel_SeqNum_monitor = 0;
				intel_host_flag = 0;
			#endif
			}
			else
		#endif
			{
#ifdef ATAPI
			DBG0(("sobj_class %bx\n", sobj_class));
			tmp8 = sobj_class;
	#ifdef USB_FAST_ENUM
			//in fast enumeration mode,when code come into here,sboj_class maybe is unkown or none,but flash has valid data,
			//at this time,we need use valid data for fast enumeration.
			if (((sobj_class == DEVICECLASS_UNKNOWN) || (sobj_class == DEVICECLASS_NONE)) && (valid_vital_from_sflash ==1))
			{
			  	tmp8 = org_sobj_class;
				DBG0(("org_sobj_class %bx\n", org_sobj_class));
			}
	#endif
			if (tmp8 == DEVICECLASS_ATAPI)
			{
				usb_odd();
			}
			else
#endif
				usb_bot();
			}


			DBG((" usb_active %BX\n", (u8)usb_active));

			if (usb_active == 0)
				return;

			// check VBus
			//if (*usb_IntStatus & VBUS_OFF)
			if (USB_VBUS_OFF())
			{
				//bot_usb_bus_reset();
				return;
			}

			// enable elastic buffer not drop all SKP
			spi_phy_wr_retry(PHY_SPI_U3PCS, 0x12, spi_phy_rd(PHY_SPI_U3PCS, 0x12) | BIT_7);

			timer0_setup();

			//usb_det_tout = 0;	
			//disabled_flag = 0;
			usbMode = CONNECT_UNKNOWN;
			usb3_u2_enable = 0;
			usb3_function_suspend = 0;
			usb3_device_U2_state = 0;

			#ifdef UAS_EN
				// back to NCQ Mode
				//*usb_Msc0Lun_SAT_0 = (*usb_Msc0Lun_SAT_0 & ~SAT_CMD) | SATA_NCQ_CMD;
				*usb_LinDBG_0 &= ~DISCARD_ZERO_LENGTH_QUALIFIER;
			#endif

#ifdef A71_PATCH
			if (revision_a71)
			{
				*usb_DeviceRequestEn_0 |= ((USB_GET_STATUS_ENABLE|USB_SET_FEATURE_ENABLE));
				*usb_DeviceRequestEn_1 |= ((USB_CLEAR_FEATURE_ENABLE));;
			}
#endif

			//fix ch9 unknow get desc type
			*usb_DeviceRequestEn_1 &= (~USB_GET_DESCR_STD_UTILITY);

#if 1		// for old NEC Driver
			*usb_DevCtrlClr_1 = USB3_U1_REJECT;	
			*usb_DevCtrlClr_2 = USB3_U2_REJECT;	
		#ifdef REJECT_U1U2
			u1U2_reject_state = U1U2_ACCEPT;
		#endif
#endif
		}

		if (sobj_State >= SATA_STANDBY)
		{
			if ((*sata_PhyStat & PHYRDY) == 0)
			{
				sobj_State = SATA_RESETING;
//				usb_active = 0;
				return;
			}
		}

	#ifdef STANDBY_TIMER
		if (hdd_tick)
		{
			if (hdd_standby_enabled)
			{
				if ((sobj_State == SATA_DRV_RDY) || (sobj_State == SATA_READY))
				{
					hdd_standby_tick++;
					if (hdd_standby_tick > hdd_Standby_Timer)
					{
					#ifdef USB_FAST_ENUM
						if (sobj_init == 0)
						{
							if (sobj_State == SATA_DRV_RDY)
							{
								if (ata_fast_enum_init())
									return;
							#ifdef USB2_L1_PATCH
								if (usbMode == CONNECT_USB2)
								{
									usb2_L1_reject_timer = 8;
								}
							#endif
							}
						}
					#endif
						ata_ExecNoDataCmd(ATA6_FLUSH_CACHE_EXT, 0);
						hdd_post_write = 0;

						if (ata_ExecNoDataCmd(ATA6_STANDBY_IMMEDIATE, 0) != CTXT_STATUS_ERROR)
						{
							DBG(("HDD0 STB\n"));
							hdd_standby_tick = 0;
							sobj_State = SATA_STANDBY;
							//hdd_standby_counter = 0;
							//hdd_post_write = 0;
						}
						//sata_pwr_down();
						hdd_standby_led();
					#ifdef PWR_SAVING
						{
							//sata_pwr_down();
						}
					#endif
					}
				}
			}
			hdd_tick = 0;
		}
	#endif	
	} // End of while loop
}

