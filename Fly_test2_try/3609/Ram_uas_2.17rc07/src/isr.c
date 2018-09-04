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
 * 3609		2010/12/21	Ted			Initial version
 *
 *****************************************************************************/
#include "general.h"
	
#if 1
void asic_isr() interrupt 0
{
	{
#if 0
		//  Disable ASIC interrupt
		*chip_IntEn_0 = 0;
		*chip_IntEn_1 = 0;
		*chip_IntEn_3 = 0;
#endif
		//DBG(("AI "));
	}
}
#endif

void timer0_isr() interrupt 1
{
	TH0 = TH0_VALUE;
	TL0 = TL0_VALUE;
	if (usb3_U2_inactivity_timer)
	{
		usb3_U2_inactivity_timer--;
	}

#ifdef USB2_L1_PATCH
	if (usb2_L1_reject_timer)
	{
		//if ((sobj_State == SATA_READY) && (usb_curCtxt == CTXT_NULL))
		if (usb2_L1_reject_timer)
		{
			if ((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED) == 0)
			{
				if (*usb_Msc0CtxtUsed_0 == 0)
				{
					if ((--usb2_L1_reject_timer) == 0)
					{	// Enable USB2.0 L1
						*usb_DevCtrlClr_2 = USB2_L1_DISABLE;
					}
				}
			}
		}

	}

#endif
	if ((usbMode == CONNECT_USB2) || (usbMode == CONNECT_USB1))
	{
		return;
	}

#ifdef DEBUG_LTSSM

	#if 0
		if ((temp_USB3StateCtl == 0x57) ||
			(temp_USB3StateCtl == 0x47))
		{
			UART_CH('.');
		}
	#endif

	if (usb_power_on_flag)
	{
		// start to count the LTSSM TS1 duration time
		if (temp_USB3StateCtl == 0x37)
		{
			usb3_ts1_timer--;
		}
		else
			usb3_ts1_timer = 90;

		if (usb3_ts1_timer == 0)
		{
			usb3_enter_loopback_timer = 20;
_SET_LOOPBACK_PHYSETTING:
			// switch to analog loopback test mode
			switch_regs_setting(LOOPBACK_TEST_MODE);
			UART_CH('L');
			UART_CH('\n');
		}

		if (usb3_enter_loopback_timer)
		{
			if ((temp_USB3StateCtl & 0x0F) == 0x0B)
			{
				// has entered loopback mode
				usb3_enter_loopback_timer = 0;
			}
			else
			{
				if (--usb3_enter_loopback_timer == 0)
				{
					// switch to normal mode phy setting
					switch_regs_setting(NORMAL_MODE);
					UART_CH('N');
					UART_CH('\n');
				}
			}
		}
	}	//if (usb_power_on_flag)

	*usb_USB3StateSelect = 0;
	cur_USB3StateCtl = *usb_USB3StateCtrl;
	if (cur_USB3StateCtl != temp_USB3StateCtl)	
	{
	#if 0
		if ((temp_USB3StateCtl == 0x57) || (temp_USB3StateCtl == 0x47))
		{
			MSG(("\nLS %BX\n", temp_USB3StateCtl));
		#ifdef DEBUG_ENUM
			if (cur_USB3StateCtl == 0x04)
				goto _VBUS_CHECK;
		#endif
		}
	#endif

	//#ifdef USB3_TX_DBG
	#if 0
		//if (cur_USB3StateCtl == 0x25)
		if ((cur_USB3StateCtl == 0x25) || (cur_USB3StateCtl == 0x35))
		{	// current state is RX detect
			if ((temp_USB3StateCtl & 0x0F) == 0x07)
			{	//previous state is polling
				//polling_failed_count = 1;
				//if (usb_rst_tx_flag)
				{
					usb_rst_usb3_tx_FIFO();
				}
			}
		}
		else
	#endif	// #ifdef USB3_TX_DBG
	#ifdef USB3_TX_DBG
		if (cur_USB3StateCtl == 0x18)
		{	// current state is Recovery active
			if ((temp_USB3StateCtl == 0x27) || (temp_USB3StateCtl == 0x37))
			{	//previous state is polling active
				{
					usb_rst_usb3_tx_FIFO();
				}
			}
		}
		else if ((cur_USB3StateCtl & 0x0F) == 0x08)
		{	// current state is  Recovery
			if (temp_USB3StateCtl == 0x40)
			{	//previous state is U0 Normal
				if (count_recvry_en)
				{
					recovery_count++;
					//if (usb_rst_tx_flag)
					if (recovery_count > 1)
					{
						usb_rst_usb3_tx_FIFO();
					}
				}
				else
				{
					recovery_count = 0;
				}
			}
		}
	#endif	// #ifdef USB3_TX_DBG
#ifdef EQ_SETTING_TEST
 		 if ((*usb_USB3StateCtrl & 0x0F) == 0x40)
		{	// current state is  Recovery
			if (temp_USB3StateCtl == 0x40)
			{	//previous state is U0 Normal
				if ((count_recvry_en) && (recovery_count <0xFF))
				{
					recovery_count++;
				}
			//	else
			//	{
			//		recovery_count = 0;
			//	}
			}
		}
#endif

       	temp_USB3StateCtl = cur_USB3StateCtl;
	#ifdef USB3_TX_DBG
		if ((temp_USB3StateCtl == 0x47) || (temp_USB3StateCtl == 0x57))
 		{
			//UART_CH('.');
			usb_rst_usb3_tx_FIFO();
		}
		else
	#endif

		{
			if (temp_USB3StateCtl == 0x16)
			{
				usb_inactive_count++;
			}
			else if (temp_USB3StateCtl != 0x26)
			{
				usb_inactive_count = 0;
			}
			if (usb_inactive_count > 0x04)
			{
				*usb_DevCtrl_0 = EXIT_SS_INACTIVE;
				usb_inactive_count = 0;
				UART_CH('#');
	#ifdef DEBUG_ENUM
				if ((usb_inactive_count & 0x1F) == 0x1F)
				{
					UART_CH('.');
				}
				if (usb_inactive_count == 0x80)
				{
_VBUS_CHECK:
					*usb_DevCtrlClr_0= USB_ENUM;
					while (1)
					{
						//if (USB_VBUS_OFF())
						//	return;
					}
				}
			#endif
			}
			else
			{
				//MSG(("LS %bx\n", temp_USB3StateCtl)); 	
				UART_STR("L "); pv8(temp_USB3StateCtl); UART_CH('\n');
			}

			if (temp_USB3StateCtl == 0x0A)
			{
				switch_regs_setting(COMPLIANCE_TEST_MODE);
				//MSG(("C\n"));
				UART_CH('C');
				UART_CH('\n');
			}
			else if ((temp_USB3StateCtl & 0x0F) == 0x0B) // loop back mode
			{
				// in the case that the loopback test state has smoothly come, F/W changes the phy setting
				if (usb3_test_mode_enable == 0)
					goto _SET_LOOPBACK_PHYSETTING;
			}
		}	
	}	//if (cur_USB3StateCtl != temp_USB3StateCtl)
#else //ifndef DEBUG_LTSSM

	if (usb_power_on_flag)
	{
		// start to count the LTSSM TS1 duration time
		if (temp_USB3StateCtl == 0x37)
		{
			if (--usb3_ts1_timer == 0)
			{
				// switch to analog loopback test mode
				usb3_enter_loopback_timer = 2;
_SET_LOOPBACK_PHYSETTING:
				switch_regs_setting(LOOPBACK_TEST_MODE);
				UART_CH('L');
				UART_CH('\n');
			}
		}
		else
			usb3_ts1_timer = 8;

		if (usb3_enter_loopback_timer)
		{
			if ((temp_USB3StateCtl & 0x0F) == 0x0B)
			{
				// has entered loopback mode
				usb3_enter_loopback_timer = 0;
			}
			else
			{
				if ((--usb3_enter_loopback_timer) == 0)
				{
					// switch to normal mode phy setting
					switch_regs_setting(NORMAL_MODE);
					UART_CH('N');
					UART_CH('\n');
				}
			}
		}
	}

	*usb_USB3StateSelect = 0;
	//*usb_USB3StateSelect ;
	cur_USB3StateCtl = *usb_USB3StateCtrl;
	if (cur_USB3StateCtl != temp_USB3StateCtl)	
	{
	//#ifdef USB3_TX_DBG
	#if 0
		//if (*usb_USB3StateCtrl == 0x25)
		if ((cur_USB3StateCtl == 0x25) || (cur_USB3StateCtl == 0x35))
		{	// current state is RX detect
			if ((temp_USB3StateCtl & 0x0F) == 0x07)
			{	//previous state is polling
				//polling_failed_count = 1;
				//if (usb_rst_tx_flag)
				{
					usb_rst_usb3_tx_FIFO();
				}
			}
		}
		else
	#endif	// #ifdef USB3_TX_DBG
	#ifdef USB3_TX_DBG
		if ((cur_USB3StateCtl & 0x0F) == 0x08)
		{	// current state is  Recovery
			if (temp_USB3StateCtl == 0x40)
			{	//previous state is U0 Normal
				if (count_recvry_en)
				{
					recovery_count++;
					//if (usb_rst_tx_flag)
					if (recovery_count > 1)
					{
						usb_rst_usb3_tx_FIFO();
						recovery_count = 0;
					}
				}
				else
				{
					recovery_count = 0;
				}
			}
		}
	#endif	//	#ifdef USB3_TX_DBG
#ifdef EQ_SETTING_TEST
 		 if ((*usb_USB3StateCtrl & 0x0F) == 0x40)
		{	// current state is  Recovery
			if (temp_USB3StateCtl == 0x40)
			{	//previous state is U0 Normal
				if ((count_recvry_en) && (recovery_count <0xFF))
				{
					recovery_count++;
				}
			//	else
			//	{
			//		recovery_count = 0;
			//	}
			}
		}
#endif
		temp_USB3StateCtl = cur_USB3StateCtl;
	//#if 0
	#ifdef USB3_TX_DBG
		if ((temp_USB3StateCtl == 0x47) || (temp_USB3StateCtl == 0x57))
		{
			usb_rst_usb3_tx_FIFO();
		}
		else
	#endif

		if (temp_USB3StateCtl == 0x16)
		{	// Inactive Quite
			usb_inactive_count++;
		}
		else if (temp_USB3StateCtl != 0x26)
		{
			usb_inactive_count = 0;
		}
		if (usb_inactive_count > 0x01)
		{
			*usb_DevCtrl_0 = EXIT_SS_INACTIVE;
			usb_inactive_count = 0;
			UART_CH('#');
		}
		else
		{
			//MSG(("LT %bx\n", temp_USB3StateCtl)); 
			UART_STR("L "); pv8(temp_USB3StateCtl); UART_CH('\n');
		}
		
		if (temp_USB3StateCtl == 0x0A)
		{
			switch_regs_setting(COMPLIANCE_TEST_MODE);
			UART_CH('c');	UART_CH('\n');
		}
		else if ((temp_USB3StateCtl & 0x0F) == 0x0B) // loop back mode
		{
			// in the case that the loopback test state has smoothly come, F/W changes the phy setting
			if (usb3_test_mode_enable == 0)
				goto _SET_LOOPBACK_PHYSETTING;
		}
	}
#endif	// #ifdef DEBUG_LTSSM
}

#if 1
//External interrupt 2
void xtimer_isr() interrupt 2
{
#ifdef BOT_TOUT
	if (bot_mode)
	{
		if (timerCnt)
		{
			timerCnt--;
			if (timerCnt == 0)
			{
				//timerCnt = 0;
				//MSG(("BOT Timeout!\n"));

				//timer0_unload();
				timerExpire = 1;
			}
		}
	}
	#ifdef UAS_TOUT
	else
	{
		if (timerCnt)
		{
			timerCnt--;
			if (timerCnt == 0)
			{
				//timerCnt = 0;
				//MSG(("BOT Timeout!\n"));

				//timer0_unload();
				timerExpire = 1;
			}
		}
	}
	#endif //#ifdef UAS_TOUT
#endif

#ifdef STANDBY_TIMER
	hdd_tick = 1;
#endif

#if SCSI_HID
	if(scsi_hid)
		softGpioData = HID_GPIO_READ(); //so that the code below sync with same button status
	scsiGpioData |= softGpioData;		//
	//	hidGpio = softGpioData;
#endif

	//xtimerCnt = 0;
	if (led_interval)
	{
		led_interval--;
	}
	
	if (led_state == LED_ACTIVITY)
	{
		if (led_interval == 0)
		{
			if (led_on)
			{
				ACT_LED_OFF();
				led_on = 0;
				if (usbMode == CONNECT_USB3)
				{
					led_interval = ACTIVITY_OFF_TIME_USB3;
				}
				else
				{
					led_interval = ACTIVITY_OFF_TIME;
				}
			}
			else
			{
				ACT_LED_ON();
				led_on = 1;
				if (usbMode == CONNECT_USB3)
				{
					led_interval = ACTIVITY_ON_TIME_USB3;
				}
				else
				{
					led_interval = ACTIVITY_ON_TIME;
				}

				if(led_activity_repeat != 0)
				{
					led_activity_repeat--;
				}
				
				if(led_activity_repeat == 0)
				{
					if ((sobj_State == SATA_READY) ||
						(sobj_State == SATA_STANDBY) ||
						(sobj_State == SATA_PWR_DWN))
					{
						led_state = LED_ON;
					}
				}
			}
		}
	}
}
#endif


#ifdef UART_RX
void serial_port_isr() interrupt 4
{
	char getChar;
	u8 getEnd;

	if (RI0)
	{
		getEnd = 0;
		RI0 = 0;
		
		getChar = S0BUF;
			
		if (getChar == '\r')
		{
			getEnd = 1;
			UART_CH('\n');
		}
		else if (getChar == 0x08)
		{	// backspace
			if (uartRxPos != 0)
			{
				uartRxPos--;
			}
			uart_rx_buf[uartRxPos] = 0x00;
			UART_CH(getChar);
			UART_CH(' ');
			UART_CH(getChar);
			return;
		}			

		UART_CH(getChar);
		uart_rx_buf[uartRxPos++] = getChar;
		
		//avoid buffer overfloww
		if (uartRxPos > 11)
			uartRxPos = 11;

		if (getEnd == 1)
		{
			uart_rx_parse();
			uartRxPos = 0;
			getEnd = 0;
		}
	}
	
	if (TI0)
	{
		TI0 = 0;
		TI_PASS = 1;
	}
}
#else
void serial_port_isr() interrupt 4
{
	ES0 = 0;
}
#endif //UART_RX
