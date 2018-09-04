/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2004-2013, All Rights Reserved
 *
 *   This file contains confidential and propietary information
 *   which is the property of Initio Corporation.
 *
 *
 *****************************************************************************
 *
 * boot.c
 *
 *****************************************************************************/
	
//#include "selftest.h"
#include "general.h"
#include <intrins.h>

#define FW_SIGNATURE_Byte0 0x25
#define FW_SIGNATURE_Byte1 0xC9



#ifdef ASIC_CONF
void Init_hardware_default_wave(void)
{
//	u8 temp8;

	DBG(("Init_hardware_default wave\n"));
	//FIXME: must remove in REAL_CHIP !!
	//*cup_MemCtrl_2 = CPU_SLOW_1_CLK;

	// turn off USB3.0 TX & RX
	//tmp8 = spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
	//spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, tmp8 | (PD_USB3_RX));

#if 0//def PWR_SAVING
	*fw_temp_3 |= DIS_AUTO_RST_SW;
	*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x01; // set the switch regulator's voltage to 1.02V
//	*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x07; // set the switch regulator's voltage to 1.14V

#endif

	*usb_LinkCtrl_2 |= (TX_DATA3_ABORT_DISABLE|DISABLE_SAVE_LATENCY);
	//set BIT_26, 25, 24 = 101
	*usb_LinkCtrl_3 = (*usb_LinkCtrl_3 & (~TS_DETECT_COUNT)) | (TS_DETECT_COUNT_32|ENHANCED_TS_DETECT);

	*usb_USBLinkConfig0_1 = LFPS_POLLING_TYP_1DOT34US;
#if 1 // for Lecroy TD 6.5 failure
	*usb_USBLinkConfig0_2 = LFPS_POLLING1_MAX_1DOT6US;
#endif

	// This defines the lower 4 bits of the Ping.LFPS tBurst typicaltimer.
	*usb_USBLinkConfig1_2 = (*usb_USBLinkConfig1_2 & (~LFPS_PING_TYP)) | LFPS_PING_TYP_160NS;
	*usb_USBLinkConfig1_3 = (*usb_USBLinkConfig1_3 & (~LFPS_TRESETDELAY)) | LFPS_TRESETDELAY_1MS;

#if 0 	//Disable EOB
	DBG0(("Disable EOB\n"));
	*usb_USB3Option_0 |= USB3_DISABLE_EOB;
		
	DBG0(("set EOB timeout 10 us\n"));
	//Disable EOB mode, timeout is Nx100us
	*usb_U3EobTimeout_0 = 0x0A;
#endif
	
	//fix some USB2 host cannot connect
	//*usb_USB2Option = 0x10;
		
	*usb_CoreCtrl_0 |= (USB_SMALL_EP|USB_CMD_PENDING_ENABLE);

	//Burst Enable
#ifdef	BURST_16K
	*usb_Threshold0 = 0x88;
	*usb_Threshold1=  0x88;
	*usb_Threshold2=  0x88;	

	*usb_CoreCtrl_3 = (*usb_CoreCtrl_3 & (~USB3_BURST_EN) | 0x10);
#else
//	*usb_Threshold0 = 0x52;		//0x42;
//	*usb_Threshold1=  0x42;
//	*usb_Threshold2=  0x42;	


	*usb_Threshold0 = 0x78;		// RX is 7, TX is 8
	*usb_Threshold1=  0x78;
	*usb_Threshold2=  0x78;	

//	*usb_CoreCtrl_3 = (*usb_CoreCtrl_3 & (~USB3_BURST_EN) | (USB3_BURST_MAX_RX|USB3_BURST_MAX_TX|0x8));
	*usb_CoreCtrl_3 = (*usb_CoreCtrl_3 & (~USB3_BURST_EN) | 0x8);
#endif

	*usb_DeviceRequestEn_0 &= (~(SET_ISOCH_DELAY_ENABLE));

	//enable for phy setting, this line has to 
//	temp8 = spi_phy_rd(PHY_SPI_U3PMA, 0x0C);	
//	DBG(("USB_REG_C:%BX\n", temp8));	
	//spi_phy_wr_retry(PHY_SPI_U3PMA, 0x0C, 0x80);	
//	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x0C, 0x8A);	//B11
	
	//DBG0(("set CKCON 01\n"));
	CKCON = 0x01;
		
	//PMA_0[6:5]: internal regulator voltage
	// 00 -> 0.95V, 10-> 1.1V, 01 -> 1.0V, 11 -> 1.15V
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x00, 0x00);
	//DBG(("PMA:00 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x00)));

	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x01, 0x00);
	//DBG(("PMA:01 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x01)));


	tmp8 = spi_phy_rd(PHY_SPI_U3PMA, 0x03);
#ifdef HW_CAL_SSC
	spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, (tmp8 & (~0x0F)));
#else
	//+Phase Interploar in PLL Loop
	spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_03, (tmp8 & (~0x0F)) | 0x80);
#endif
	DBG(("PMA:03 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x03)));

#ifdef HW_CAL_SSC

	// SSC Enable & SSC_DOWN_SPREAD_MODE
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x02, 0x42);
#else
	// SSC Disable
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x02, 0x4A);
#endif
	//DBG0(("PMA:02 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x02)));

	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x04, 0x64);	
	//DBG0(("PMA:04 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x04)));

	// Power dOWN "CDR limit detector" 
	spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05, 0x3E);	
	//DBG0(("PMA:05 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_05)));

	// Bit7,6: enable LFPS/EIDLE qualification
	// Bit 5: enable EIDLE detection
	// BIT0: enable interpolator giltch remove, 
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x06, 0x11);
	//DBG0(("PMA:06 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x06)));

	// LFPS detection squelch threshold -> 225/180mv
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x07, 0x88);
	//DBG0(("PMA:07 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x07)));

#ifndef AMD_X75
	//#define PCAP_IN		0xC0 // pma_8[7:6]: AFE equalization: low pole, less peaking
	//#define RTUNE_SQ2	0x20 // AFE equalization: high gain, narrow bandwidth
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x08, 0xFC);
#else
	//#define PCAP_IN		0x40 // pma_8[7:6]: AFE equalization: low pole, less peaking
	//#define RTUNE_SQ2	0x20 // AFE equalization: high gain, narrow bandwidth
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x08, 0x7C);
#endif
	//DBG0(("PMA:08 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x08)));

	//  cdr_lp_gn[5:0]
	// Bit7: enable CDR second order filter tracking ranger up to 15K PPM 
	//spi_phy_wr_retry(PHY_SPI_U3PMA, 0x09, 0x1D);				// default
	spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_09, 0x9E);
	//DBG0(("PMA:09 %bx\n", spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_09)));

	// set voltage swing amplitude 
	// PMA_A[3:2]
	//	00 -> 1V, 01 -> 850MV, 10 -> 775MV, 11 -> 625MV
	//spi_phy_wr_retry(PHY_SPI_U3PMA, 0x0A, 0x10);				// default
#if 1
	// bit4: Ensable pre-emphasis
	spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A, 0x10);	//1V	
#else
	spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A, 0x0C);	//625MV	
#endif
	//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A, 0x04);	//850MV	
	//DBG0(("PMA:0A %bx\n", spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0A)));

	
	// disable synchronization to TX
	//tmp8 = spi_phy_rd(PHY_SPI_U3PMA, 0x0B) | BIT_0;
	//spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0B, tmp8);
	spi_phy_wr_retry(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0B, 0x6D);
	//DBG0(("PMA:0B %bx\n", spi_phy_rd(PHY_SPI_U3PMA, PMA_PHY_ANALOG_0B)));

	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x0C, 0x80);	
	//DBG0(("PMA:0C %bx\n", spi_phy_rd(PHY_SPI_U3PMA, 0x0C)));

	//enable elastic buffer not drop all SKP
#if 1
	tmp8 = spi_phy_rd(PHY_SPI_U3PCS, 0x12);
	spi_phy_wr_retry(PHY_SPI_U3PCS, 0x12, tmp8|BIT_7);
#endif
	// fw enable zcap_tun value
	tmp8 = spi_phy_rd(PHY_SPI_U3PCS, 0x28);
#ifdef HW_ADAPTIVE
	tmp8 = (temp8 & ~(BIT_3|BIT_6)) | BIT_5; // BIT_3: bypass is disable, BIT_5: firmware control, BIT_6 firmware set loop start	
#else
	tmp8 |= BIT_3;		// BYPASS_ADEQ
#endif
	spi_phy_wr_retry(PHY_SPI_U3PCS, 0x28, tmp8);

	//DBG0(("PCS:28 %bx\n", spi_phy_rd(PHY_SPI_U3PCS, 0x28)));

#ifndef AMD_X75
	// set zcap value to 0x00
	tmp8 = (spi_phy_rd(PHY_SPI_U3PCS, 0x29)) & (~0xF0);
#else
	// set zcap value to 0x04
	tmp8 = (spi_phy_rd(PHY_SPI_U3PCS, 0x29) & (~0xF0)) | 0x40;
#endif
	spi_phy_wr_retry(PHY_SPI_U3PCS, 0x29, tmp8);
		
	//DBG0(("PCS:29 %bx\n", spi_phy_rd(PHY_SPI_U3PCS, 0x29)));
	
	// change voltage from 1.0 to 1.10(1.045) 
//	tmp8 =	(0x80) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_ANALOG_1_0);
//	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_ANALOG_1_0, tmp8);

	// sets Squelch threshold from (175mv-87.5mv) to (200mv-100mv).
	tmp8 =	0x05 | ((~RX_SQU_ADJ) & spi_phy_rd(PHY_SPI_SATA, SATA_PHY_ANALOG_1_0));
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_ANALOG_1_0, tmp8);
	// sets the TX amplitude (differential signal) from 0.5 V to to 0.58 V.
	tmp8 =	(TX_DRV_AMP) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_ANALOG_1_1);
	spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_ANALOG_1_1, tmp8);


	//////////////////////////////	
	// Disable USB2.0 termination impedance calibration 
	spi_phy_wr_retry(PHY_SPI_U2, 0x00, 0x01 | spi_phy_rd(PHY_SPI_U2, 0x00));

	//set USB2 phy eye
	spi_phy_wr_retry(PHY_SPI_U2, 0x05, 0x38);

#if 1
	//set USB2 Termination Impedence
	// Adjust REG_RTERM<3:0>
	spi_phy_wr_retry(PHY_SPI_U2, 0x01, (spi_phy_rd(PHY_SPI_U2, 0x01) & 0x0F) | 0x80);
#endif
}

#else
// For FPGA
void Init_hardware_default_wave(void)
{
	//u8 temp8;

	DBG0(("Init_hardware_default wave\n"));
	//FIXME: must remove in REAL_CHIP !!
	//*cup_MemCtrl_2 = CPU_SLOW_1_CLK;

	//set BIT_26, 25, 24 = 101
	*usb_LinkCtrl_3 = (*usb_LinkCtrl_3 & ~0x07) | 0x05;
	
#if 0 	//Disable EOB
	DBG0(("Disable EOB\n"));
	//Disable EOB
	*usb_USB3Option_0 |= USB3_DISABLE_EOB;
		
	DBG0(("set EOB timeout 10 us\n"));
	//Disable EOB mode, timeout is Nx100us
	*usb_U3EobTimeout_0 = 0x0A;
#endif
	
	//fix some USB2 host cannot connect
	//*usb_USB2Option = 0x10;
		
	*usb_CoreCtrl_0 |= (USB_SMALL_EP|USB_CMD_PENDING_ENABLE);

	//Burst Enable
#ifdef	BURST_16K
	*usb_Threshold0 = 0x88;
	*usb_Threshold1=  0x88;
	*usb_Threshold2=  0x88;	

	*usb_CoreCtrl_3 = (*usb_CoreCtrl_3 & (~USB3_BURST_EN) | 0x10);
#else
	*usb_Threshold0 = 0x42;
	*usb_Threshold1=  0x42;
	*usb_Threshold2=  0x42;	
	*usb_CoreCtrl_3 = (*usb_CoreCtrl_3 & (~USB3_BURST_EN) | 0x8);
#endif

	*usb_DeviceRequestEn_0 &= (~(SET_ISOCH_DELAY_ENABLE));

	//*usb_USB3Option_1 |= USB3_DP_SKID_DISABLE; 				
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x00, 0xBE);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x01, 0xE4);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x02, 0x91);	//key point
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x03, 0x45);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x04, 0x2C);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x05, 0x97);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x06, 0x98);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x07, 0x43);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x08, 0x02);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x09, 0x88);
	spi_phy_wr_retry(PHY_SPI_U3PMA, 0x0A, 0x43);	//key point
#if 0
	*usb_MSC0_Proto_Retry_LA_option_0 = 0x01; // set protocol retry LA to mode 1
	*usb_MSC0_Proto_Retry_LA_option_1 = 0x00;
	*usb_MSC0_Proto_Retry_LA_option_2 = 0x00;
	*usb_MSC0_Proto_Retry_LA_option_3 = 0x00;
#endif
	
}

#endif
void update_global_variables()
{
	hdd_serial = 0;
	if (globalNvram.fwCtrl & 0x10)
	{
		hdd_serial = 1;
	}

#ifdef STANDBY_TIMER
	hdd_calc_Idle_timer(globalNvram.sataHddIdleTime);
#endif

	// Based on Strap option, restore USB Power Settings 
	if (usb_self_power)
	{	// USB Self Power
		globalNvram.USB3_PWR = 1;					// Power comsumption of the USB3.0 device from the bus (8mA)
		globalNvram.USB_ATTR = 0xC0;				// Self Power
		globalNvram.USB2_PWR = 1;					// Power comsumption of the USB2.0 device from the bus (2mA)
	}
	else
	{	// USB Bus Power
		globalNvram.USB3_PWR = 112;					// Power comsumption of the USB3.0 device from the bus (896mA)
		globalNvram.USB_ATTR = 0x80;				// bus Power
		globalNvram.USB2_PWR = 250;					// Power comsumption of the USB2.0 device from the bus (500mA)
	}

}

void main(void)
{
	*usb_DevCtrlClr_0 = USB_ENUM;		//disable USB enumeration
	
	// change to the lowest speed for  CPU  
	*cpu_Clock_0 = CPU_CLK_DIV_DEFAULT;//CPU_CLK_DIV_DEFAULT;

#ifdef ASIC_CONF
	cpu_ref_clk = 0;
#endif
	//FIXME: should check DD

	// Enable UART 
#ifdef DBG_FUNCTION
	// Activity LED off & HDD Power OFF
	//*gpio_CtrlDO_0 = (*gpio_CtrlDO_0 & (~GPIO_HDD_POWER)) | GPIO3_DATA;
	*gpio_CtrlDO_0 = (*gpio_CtrlDO_0) | (GPIO3_DATA|GPIO_HDD_POWER);
	*gpio_DOEn_0 |= (GPIO_HDD_POWER | GPIO3_EN);

#ifdef UART_RX
	*gpio_CtrlFsel_0 = UR_RXSEL|UR_TXSEL;
#else
	*gpio_CtrlFsel_0 = UR_TXSEL;
#endif
	*gpio_CtrlFsel_1 &= (~GPIO4_FSE_MASK);
	uart_init();

#else
	//*gpio_CtrlDO_0 = (*gpio_CtrlDO_0 & (~GPIO_HDD_POWER)) | GPIO0_DATA;
	*gpio_CtrlDO_0 = (*gpio_CtrlDO_0) | (GPIO_HDD_POWER|GPIO0_DATA);
	*gpio_DOEn_0 |= (GPIO_HDD_POWER |GPIO0_EN);

	// GPIO only for 3, 2, 1 & 0
	*gpio_CtrlFsel_0 = 0;

	// GPIO only for 4
	*gpio_CtrlFsel_1 &= (~GPIO4_FSE_MASK);
#endif
	
	*gpio_CtrlFsel_1 |= SPI_SDI_SEL|SPI_CS_SEL;
	*gpio_CtrlFsel_2 |= SPI_SCK_SEL|SPI_SDO_SEL;

	//MSG(("====================\n"));
	//MSG(("%s %s\n", __DATE__, __TIME__));
	//MSG(("V%BX%BX\n\n", *((u8 code *) (0x7C29)), *((u8 code *) (0x7C2A))));
	//Delay(1000);
	//MSG(("=========================\n"));

	tmp8 = *chip_Revision;
	DBG0(("Rev %BX ", tmp8));

	*fw_temp_3 |= DIS_AUTO_RST_SW;
	*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x00; // set the switch regulator's voltage to 1.08V
//	*swreg_ctrl_0 = (*swreg_ctrl_0 & ~0x07) | 0x07; // set the switch regulator's voltage to 1.14V

#ifdef ASIC_CONF
	revision_a61 = 0;
	#ifdef A71_PATCH
		revision_a71 = 0;
	#endif
	revision_a91 = 0;

	if (tmp8 >= REV_A61)
	{
		revision_a61 = 1;
		//tmp8 = spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
		//DBG0(("\n\t%BX %BX\n", tmp8, *cpu_Clock_2));
		//DBG0(("\t\t%BX\n", *cpu_Clock_2));

		while (1)
		{
			DBG(("\t%BX\n", *cpu_Clock_2));
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_PLL_DISABLE|PD_USB3_RX|PD_USB3_TX|PD_USB3_PLL);
			Delay10us(1);
			//turn on PLL
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|PD_USB3_RX|PD_USB3_TX);
			Delay(2);

			if ((*cpu_Clock_2 & USB3_PLL_RDY))
			{
				break;
			}
		}
		//turn on TX & RX
		spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);

	#ifdef A71_PATCH
		if (tmp8 >= REV_A71)
		{
			revision_a71 = 1;
			if (tmp8 >= REV_A91)
				revision_a91 = 1;
		}
	#endif
	}

#else
	// FPGA
	revision_a61 = 1;
	#ifdef A71_PATCH
		revision_a71 = 1;
	#endif

#endif
	
	*usb_CoreCtrl_1 &= ~VBUS_RESET_ENUM_EN;
	*usb_CoreCtrl_2 |= (VBUS_ENUM_DISABLE);

	// Disable USB2.0 PLL Free Running
	//*usb_DevCtrlClr_2 = USB2_PLL_FREERUN;

	// 10(9+1) ms USB2_CONNECT_TIMEOUT
	*usb_USB2ConnectTick_0 = (25000 & 0xFF);
	*usb_USB2ConnectTick_1 = (25000 >> 8);

	//*usb_USB2ConnectTout_0 = 9;		//move down
	//*usb_USB2ConnectTout_1 = 00;
	//*usb_USB2ConnectTout_1 = 01;

	// disable USB3 Idle Timeout
	*usb_USB3Idle_Tout = 0;

	//fix some USB2 host cannot connect
	*usb_USB2Option_0 = USB2_FS_MODE_QUALIFIER|USB2_HS_MODE_QUALIFIER|USB2_MODE_PRIORITY;
	*usb_USB2Option_1 = USB2_WARM_RESET_PSM|USB2_SS_INACTIVE_PSM|USB2_WARM_RESET_SEL_CONN|USB2_SS_INACTIVE_CONN|USB2_WARM_RESET_CONN;

	//*usb_USB3Option_0 = (*usb_USB3Option_0 & (~(USB3_MAX_POST))) | USB3_MAX_POST2;
	//DBG(("DISABLE DP_SKID\n"));
#ifdef UAS_EN
	*usb_USB3Option_1 = (*usb_USB3Option_1 & (~(USB3_DP_SKID_DISABLE_1|USB3_DP_SKID_DISABLE_0))) | (USB3_POLLING_PSM|USB3_DP_SKID_DISABLE_1|USB3_DISABLE_FLOW_DEAD|USB3_ENABLE_FLOW_PRIME);
#else
	*usb_USB3Option_1 = (*usb_USB3Option_1 & (~(USB3_DP_SKID_DISABLE_1|USB3_DP_SKID_DISABLE_0))) | (USB3_POLLING_PSM|USB3_DP_SKID_DISABLE_1);
#endif
	//DBG0(("usb_USB3Option_1 %bx\n", *usb_USB3Option_1));

	// 120 ms USB2_CONNECT_TIMEOUT
	*usb_FSConnectTick_0 = (25000 & 0xFF);
	*usb_FSConnectTick_1 = (25000 >> 8);

	//*usb_FSConnectTout_0 = 0x1a;		// move down
	//*usb_FSConnectTout_1 = 00;

	*usb_LinDBG_0 |= USB3_TXLINK_MODE;
	*usb_LinDBG_1 |= (UX_EXIT_LFPS_LOW|USB3_Ux_EXIT_DISABLE_FW|LFPS_MODE_1);

	// USB3 RXDETECT retries count
	//*usb_LinDBG_2 = 7;
	*usb_LinDBG_3 |= USB_DISCARD_HEADPACKET_WITH_K_BYTE;

	// MaxLun is zero
	*usb_Msc0LunCtrl = 0;

	DBG(("\tCK3 %BX\n", *cpu_Clock_3));

	//Init_hardware_default_wave();
	//Init_hardware_default_old();

	DBG(("\tCK2 %BX\n", *cpu_Clock_2));

	//romcode_check_stat	|= FW_ON_SPI;	
	//DBG(("ok\n"));
	//goto	JUMP2RAM;

	cfa_active = 0;
	fast_enum_usb_flag = 0;
	usb_BusPwr_FastEnum = 0; 
	load_nvram_from_HDD_flag = 0;
#ifdef EQ_SETTING_TEST
     usb_20_bus_rst = 0x00;
     usb_30_bus_rst = 0x00;
#endif

#if SCSI_HID
	scsi_hid = 1;
#endif

#ifdef W_PROTECT
	writeprotect = 0;
    *gpio_DOEn_0 &= ~GPIO4_EN;
    if(IS_WRITE_PROTECT() == 0)
		writeprotect = 1;
#endif

	//NvReadDefaultlNvram();
	fw_version[0] = globalNvram.feVersionNvramOnly[0];
	fw_version[1] = globalNvram.feVersionNvramOnly[1];
	//xmemcpy(globalNvram.feVersionNvramOnly, fw_version, 2);

	vendor = INITIO;
	usb_self_power = 1;

	NvReadGlobalNvram();
	if ((*gpio_DD_0 & GPIO5_DATA))
	{	// Bus Power.
		MSG(("Bus pwr\n"));
		usb_self_power = 0;
		// disable Pull-Up resistor of GPIO[5] & GPIO[2] to save power
		*gpio_PU_0 &= ~(GPIO5_DATA|GPIO2_DATA);
	}
#ifdef USE_GPIO2_VBUS	// use GPIO2 as VBUS detection
	else
	{	// self Power.
		MSG(("self pwr\n"));
		// Disable PU resistor of GPIO2 
		*gpio_PU_0 &= ~GPIO2_DATA;
		Delay(10);
		// Select GPIO2 as source of VBUS
		*cpu_wakeup_ctrl_1 |= USB_SELECT;
	}
#endif
	update_global_variables();
#ifdef USB_FAST_ENUM
	fast_enum_usb_flag = 1; // c
		// disable USB serial extract from HDD
	//globalNvram.fwCtrl &= ~0x10;				
#endif
#ifdef POW_MGMT
	DrvPwrMgmt = 0;
	if ((globalNvram.powerManagement & 0x01))
	{	// SATA Drive Power Management
		DrvPwrMgmt = 1;
	}

	PwrMgmt = 0;
	if ((globalNvram.powerManagement & 0x02))
	{	// HDD Power Management
		PwrMgmt = 1;
	}
#endif		
	
#ifdef POW_MGMT
	if (DrvPwrMgmt)
	{
		//*gpio_CtrlDO_0 &=  (~GPIO_HDD_POWER);
		*gpio_CtrlDO_0 |=  (GPIO_HDD_POWER);
		*gpio_DOEn_0 |= GPIO_HDD_POWER;
	}
#endif
	sobj_init = 0;
	sobj_init_tout = SATA_INIT_TOUT;	// 25sec

#ifdef USB_FAST_ENUM
	sflash_type = UNKNOWN_FLASH;
	valid_sflash_vital_data = 0;
#endif	//USB_FAST_ENUM
	

START_UP:	
	usb_AdapterID = 0xff;
	usb_PortID0 = 0xff;
	usb_PortID1 = 0xff;
	usb_PortID2 = 0xff;
	hdd_lun_ctrl = 1;

	usb_active = 0;

	//MSG(("Ck v "));
	if (USB_VBUS_OFF())
	{
		MSG((">OFF\n"));
		if (cfa_active == 0)
		{
			// turn on HDD power
			power_down_hdd();		// turn off HDD power
			// Disable Dbuf clock
			*cpu_Clock_0 = 0x0f;
		#ifdef ASIC_CONF

			// Enable ASIC Reference Clock as CPU Clock
				*cpu_Clock_3 |= ASIC_PLL_BYPASS;
				cpu_ref_clk = 1;

				// Power-down SATA PLL, SATA TX, & SATA RX
				tmp8 =	(PD_SATA_PLL | PD_TX|PD_RX) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
				spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);	// SATA PLL power

				Delay(1);	

				#ifdef DBG_FUNCTION
					uart_init();
					DBG(("ref clk\n"));
				#endif
				sobj_State = SATA_PWR_DWN;
		#else
				// Power-down SATA TX & SATA RX
				tmp8 =	(PD_TX|PD_RX) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
				spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);	// SATA PLL power
		#endif

#ifdef PWR_SAVING
				// switch USB clock to Reference Clock
				*cpu_Clock_3 |= USBCLK_SELECT;

				// disable USB2 PLL
				*usb_DevCtrl_1 = USB2_FORCE_SUSP;

				//tmp8 = spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
				//DBG0(("\n\t%BX%BX\n", tmp8, *cpu_Clock_2));
				// 	Disable USB3.0 TX & RX 
				//spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|PD_USB3_RX|PD_USB3_TX);
				
				// Disable USB3.0 TX, RX, vreg, & PLL 
				spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_VREG_DISABLE|HW_RESTART_PLL_DISABLE|PD_USB3_RX|PD_USB3_TX|PD_VREG|PD_USB3_PLL);
#endif
		}
		while(1)
		{
			if (USB_VBUS_ON()) 	//USB cable plug-in
				break;
		}
		DBG(("On\n"));
		if (cfa_active == 0)
		{
#ifdef PWR_SAVING
			// Enable USB3.0 VREG
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|HW_RESTART_PLL_DISABLE|PD_USB3_RX|PD_USB3_TX|PD_USB3_PLL);

			// Enable USB3.0 PLL
			spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_TXRX_DISABLE|PD_USB3_RX|PD_USB3_TX);
			while (1)
			{
				if ((*cpu_Clock_2 & (USB3_PLL_RDY)))
				{
					break;
				}
			}	

			// Enable USB3.0 TX & RX
			spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);

			// Enable USB2.0 PLL
			*usb_DevCtrl_2 =	USB2_PLL_FREERUN;
			*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
			Delay(1);

			*cpu_Clock_3 &= ~USBCLK_SELECT;
#endif
		}
	}
		
	Init_hardware_default_wave();

	DBG(("cpu_Clock_32 %BX%BX\n", *cpu_Clock_3,*cpu_Clock_2));
	DBG(("usb_DevCtrl_21 %BX%BX %bx\n", *usb_DevCtrl_2,*usb_DevCtrl_1, spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13)));

	*usb_DevCtrl_1 = USB_CORE_RESET;

	//*usb_CoreCtrl_2 &= ~(USB3_MODE_DISABLE | USB2_MODE_DISABLE);
	//*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;

	*usb_IntStatus_0 = 0xFF;
	*usb_IntStatus_1 = 0xFF;
	*usb_IntStatus_2 = 0xFF;


	rx_detect_count = RX_DETECT_COUNT8;
	// USB3 RXDETECT retries count
	*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_8;

	*usb_USB2ConnectTout_0 = 9;

	*usb_FSConnectTout_0 = 0x1a;

	// Enable Dbuf clock to 88MH
	*cpu_Clock_0 = 0xFF;//0xFF;

	
	// Default in BOT Mode
	bot_mode = 1;

	dbuf_init();


#ifdef USB_FAST_ENUM
	if (fast_enum_usb_flag)
	{
		sflash_rd_vital_data();
		if (valid_sflash_vital_data == 0)
		{
			sflash_get_type();
			goto detect_mode;
		}

		val = sflash_vital_data.sobj_class;
		if ((val != DEVICECLASS_ATA) && (val != DEVICECLASS_ATAPI) )
		{
			goto detect_mode;
		}
		sobj_class = val;
		org_sobj_class = val;
		valid_vital_from_sflash = 1;
	
		if (usb_self_power)
		{
			MSG(("bPowUP0 %bx\n", *gpio_DI_0));
			power_up_hdd();		// turn on HDD power
			Delay(50);	
			// Power-up SATA PLL, SATA TX, & SATA RX
			sata_pll_pwr_up();
			sata_RegInit();
			sata_Reset(SATA_HARD_RST);
			sobj_init = 0;
			sobj_init_tout = 255;	// 25sec
		}
		else
		{	//USB bus power
#ifdef ASIC_CONF
			// Enable ASIC Reference Clock
			*cpu_Clock_3 |= ASIC_PLL_BYPASS; // use the external crystall clock, it's 25M in current PCBA	
			cpu_ref_clk = 1;
			
			// Power-down SATA PLL, SATA TX, & SATA RX
			//tmp8 =	(PD_SATA_PLL | PD_TX|PD_RX) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
			// Power-down SATA TX & SATA RX
			tmp8 =	(PD_TX|PD_RX) | spi_phy_rd(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1);
			spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_DIGIAL_0_1, tmp8);	// SATA PLL power
#endif
			usb_BusPwr_FastEnum = 1;
			sobj_State = SATA_PWR_DWN;
		}
		
		DBG0(("F E %bx\n", sobj_State));
		if (sobj_class == DEVICECLASS_ATA)
		{
			sobj_sectorLBA_l = sflash_vital_data.sectorLBA_l;
			sobj_sectorLBA_h = sflash_vital_data.sectorLBA_h;
			sobj_qdepth = sflash_vital_data.qdepth;

			sobj_ncq_mode = 0;
			if (sobj_qdepth > 1)
			{
				sobj_ncq_mode = 1;
			}
			sobj_WrCache_supported = 0;
			sobj_WrCache_enabled = 0;
			xmemcpy(sflash_vital_data.iStr, sobj_serialStr, 20);

			if (hdd_serial)
				xmemcpy(sobj_serialStr, serialStr, 20);
			else
				xmemcpy(globalNvram.iSerial, serialStr, 20);

			ata_get_serial_num();
#ifdef PHYSICAL_MORE_512
			sobj_physical_sector_size = sflash_vital_data.physical_sector_size;
			MSG(("sl ss%x", sobj_physical_sector_size));
#endif	
			sobj_sector_4K = 0;
#ifdef SUPPORT_3T_4K			
			if (sobj_sectorLBA_h)
				sobj_sector_4K = 1;
#endif			

		#if	0	//def UAS_EN
			bot_mode = 0;
			ata_set_usb_msc();
			bot_mode = 1;
		#else
			ata_set_usb_msc();
		#endif

		}
		else
		{
			hdd_serial = 0;
			xmemcpy(globalNvram.iSerial, serialStr, 20);
		}
		update_global_variables();
		goto start_usb_emulation;
	}
#endif

#ifdef USB_FAST_ENUM
detect_mode:	
#endif

	MSG(("bPowUP1 %bx\n", *gpio_DI_0));
	power_up_hdd();		// turn on HDD power
	Delay(100);	
	// Power-up SATA PLL, SATA TX, & SATA RX
	sata_pll_pwr_up();
	
	sata_RegInit();

	scan_sata_device();
#if 0
	//ceck sata 1 or 2
	if (*sata_PhyStat & SNGEN2)
	{     //only adjust the sata eye for gen2
		spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_ANALOG_1_1, 0x4E);
		spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_ANALOG_2_1, 0x1A);
		spi_phy_wr_retry(PHY_SPI_SATA, SATA_PHY_ANALOG_2_3, 0x40);
	}
#endif	
	cfa_active = 0;
	
	if (!DevicesFound)
	{
no_drive:
		sobj_class = DEVICECLASS_CFA;
		sobj_init = 1;
		cfa_active = 1;
		hdd_serial = 0;
		fast_enum_usb_flag = 0;
		usb_BusPwr_FastEnum = 0;
		sobj_State = SATA_NO_DEV;
		sobj_mediaType  = MEDIATYPE_REMOVABLE;
		xmemcpy(globalNvram.iSerial, serialStr, 20);
	}
	else
	{
		switch(sobj_class)
		{
			case DEVICECLASS_ATA:
				if (ata_init())
					goto no_drive;
				#ifdef POW_MGMT
				if (DrvPwrMgmt)
					PwrMgmt = 1;
				#endif
				sobj_init = 1;
				*chip_status	|= HDD_VALID;		// Valid HDD
				hdd_on_led();


#ifdef USB_FAST_ENUM
				if (fast_enum_usb_flag)
				{
					sflash_init_vital_data();
				}
#endif
				break;

			case DEVICECLASS_ATAPI:
#ifdef ATAPI
				if (atapi_init() == 0)
				{
					sobj_init = 1;
					hdd_serial = 0;
#ifdef POW_MGMT
					PwrMgmt = 0;
#endif
					xmemcpy(globalNvram.iSerial, serialStr, 20);
#ifdef USB_FAST_ENUM
					if (fast_enum_usb_flag)
					{
						sflash_init_vital_data();
					}
#endif

					break;
				}
				goto no_drive;
#endif

			default:
				goto no_drive;
		}


	}


	//sobj_mediaType  = MEDIATYPE_FIXED;
#ifdef USB_FAST_ENUM
start_usb_emulation:
#endif
#if SCSI_HID
	if (scsi_hid)
	{
		*gpio_DOEn_0 &= ~GPIO1_EN;
		//*gpio_DOEn_0 &= ~GPIO4_EN;
		scsiGpioData = HID_GPIO_READ();
		//DBG0(("scsi_enabled: %Bx\n", hidGpio));
	}
#endif


	//usb_clock_enable();

	// turn on USB3.0 TX & RX
	//tmp8 = spi_phy_rd(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13);
	//spi_phy_wr(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, tmp8 | (PD_USB3_TX|PD_USB3_RX));

	DBG(("M0\n"));


	
#ifdef UAS_EN
	if (cfa_active == 0)
	{	// at least support 8 tags
		*usb_Msc0Ctrl_0 = MSC_UAS_ENABLE|MSC_BOT_ENABLE;
		// intercept "Set Interface Request" 
		*usb_DeviceRequestEn_0 &= ~SET_INTERFACE_ENABLE;
	}
	else
#endif
	{	// enable BOT only
		*usb_Msc0Ctrl_0 = MSC_BOT_ENABLE;
	}


	// for old NEC Driver
 	*usb_DevCtrlClr_1 = USB3_U1_REJECT;	
	*usb_DevCtrlClr_2 = USB3_U2_REJECT;

	//UsbExec_Init();

#if 0 //debug
	*usb_USB3StateSelect = USB3_LTSSM;
	DBG0(("LT st:%bx\n",  *usb_USB3StateCtrl));
#endif
	//if ((globalNvram.USB_ATTR & 0x40))
	if (usb_self_power)
		*usb_DevCtrl_0 = USB_FW_RDY | USB_ENUM | SELF_POWER;
	else
		*usb_DevCtrl_0 = USB_FW_RDY | USB_ENUM;
	UsbExec_Init();
	//fix ch9 unknow get desc type
	*usb_DeviceRequestEn_1 &= (~USB_GET_DESCR_STD_UTILITY);

#ifdef USB_FAST_ENUM
	if ((sobj_init == 1) && (sobj_State == SATA_READY))
#endif
	{
		hdd_on_led();
		DBG(("M2\n"));
	}

	if (cfa_active == 0)
	{	// 	// SPI Disabled
		xtimer_setup();
		timer0_setup();
	}
	EAL = 1;
	DBG(("M3\n"));


	usb_active = 1;
	
//#ifdef ATAPI
//	if (sobj_class == DEVICECLASS_ATAPI)
//		usb_odd();
//	else
//#endif
	usb_start();
	DBG0(("E0 %BX\n", *cpu_Clock_2));
	
	*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;
	
#ifdef ASIC_CONF
//	if (revision_a61)
	{
		spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
		Delay(1);
	}
#endif
	*usb_DevCtrlClr_0 = USB_ENUM;		//disable USB enumeration

	// disable HW calculate SSC(default POR state)
	spi_phy_wr_retry(PHY_SPI_U3PCS, 0x1C, 0x00);

	xtimer_disable();

	ET0 = 0;			// 	//disable timer0 interrupt
	usb_active = 0;

	hdd_led_going_off();

	//MSG(("E1\n"));
	
	if (cfa_active == 0)
	{	// 	// SPI Disabled
		if (sobj_State == SATA_NO_DEV)
		{
			Delay(600);
			MSG(("no drive %BX\n", *cpu_Clock_2));
			goto no_drive;
		}

		if ((sobj_State == SATA_STANDBY) || (sobj_State == SATA_PWR_DWN))
		{
			if (sobj_State == SATA_PWR_DWN)
			{
				sata_pll_pwr_up();
			}
			MSG(("E2\n"));
			// soft reset chip
			*cpu_Clock_0 = 0x6F;
			Delay(1);
			*cpu_Clock_2 = ASIC_RESET;
			Delay(1);
		}

#ifdef ATAPI
		if (sobj_class == DEVICECLASS_ATA)
#endif	//#ifdef ATAPI
		{
			if (sobj_State > SATA_READY)
			{	// reset SATA HDD
				scan_sata_device();

				*sata_BlkCtrl_1 = (RXSYNCFIFORST | TXSYNCFIFORST);	//reset sata TX  fifo
				//*sata_BlkCtrl_1;

				//3609 reset sata_SiteCmdIntEn to 0
				*sata_SiteCmdIntEn_0 = 0;

				dbuf_init();
				*usb_Msc0DICtrlClr = MSC_TXFIFO_RST;
				*usb_Msc0DOutCtrlClr = MSC_RXFIFO_RESET ;
			}
		#ifdef POW_MGMT
			if ((PwrMgmt) &&  (sobj_State == SATA_READY))
		#else
			if (sobj_State == SATA_READY)
		#endif
			{
				ata_ExecNoDataCmd(ATA6_FLUSH_CACHE_EXT, 0);
				ata_ExecNoDataCmd(ATA6_STANDBY_IMMEDIATE, 0);
				sobj_State = SATA_STANDBY;
			}
		}
	}//if (cfa_active == 0)

	MSG(("E3\n"));

#ifdef PWR_SAVING
	//if (DrvPwrMgmt)
	{
		//sata_pwr_down();
		power_down_hdd();
	}
#endif
	DBG(("E4\n"));

	goto START_UP;
}
