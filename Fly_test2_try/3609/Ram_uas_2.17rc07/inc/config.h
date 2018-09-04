/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2008-2013, All Rights Reserved
 *
 *   This file contains confidential and propietary information
 *   which is the property of Initio Corporation.
 *
 *
 *******************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

//-----------------------------
// ASIC Setting
//-----------------------------
#define ASIC_CONF

#define		BAUD_RATE	115200


#ifndef ASIC_CONF
	// FPGA
	#define	CPU_CLK		50000000
	#define A71_PATCH

	#define	POWER_MANAGMENT
	#ifdef	POWER_MANAGMENT

		//#define	FAST_SATA_ON
		//#define PLL_BYPASS_TEST
		//#define PWR_SAVING
		//#define PWR_SAVING_SUSPEND
		//#ifdef PWR_SAVING_SUSPEND
		//#endif

	//	#define	DISABLE_ALL_IP
	//	#define	GPIO_HDDPOWER_DBG_LED0	GPIO5_DATA
	
		//#define GPIO_HDD_POWER			GPIO4_DATA
		#ifdef W_PROTECT
			#define GPIO_HDD_POWER	
			#define power_up_hdd()			
			#define power_down_hdd()		
		#else			
			#define GPIO_HDD_POWER			GPIO4_DATA
			#define power_up_hdd()			(*gpio_CtrlDO_0 |= GPIO_HDD_POWER)//
			#define power_down_hdd()		(*gpio_CtrlDO_0 &= ~GPIO_HDD_POWER)//
		#endif
	#endif


#else
	#define	CPU_CLK		88235294
	#define	REF_CLK		25000000
	#define A71_PATCH

	#define USE_GPIO2_VBUS

	//#define USB_FAST_ENUM

	//#define HW_CAL_SSC 
	#define DEBUG_LTSSM
	#ifdef DEBUG_LTSSM
		#define USB2_L1_PATCH
	#endif
	#define USB3_TX_DBG

	#define AMD_X75
	//#define RTEM_2_AGND

	#define	POWER_MANAGMENT
#ifdef	POWER_MANAGMENT
	#define PWR_SAVING
	//#define GPIO_HDD_POWER			GPIO4_DATA
	#ifdef W_PROTECT
		#define GPIO_HDD_POWER
		#define power_up_hdd()			
		#define power_down_hdd()		
	#else
		#define GPIO_HDD_POWER			GPIO4_DATA
		#define power_up_hdd()			(*gpio_CtrlDO_0 |= GPIO_HDD_POWER)//
		#define power_down_hdd()		(*gpio_CtrlDO_0 &= ~GPIO_HDD_POWER)//
	#endif
#endif
//#define HW_CAL_SSC 
#endif

//#define UAS_EN
#ifdef UAS_EN
	#define WIN8_UAS_PATCH
	//#define INTEL_SEQNUM_PATCH
	#ifdef INTEL_SEQNUM_PATCH
		#define AMD_CLR_FEATURE_PATCH
	#endif
	//#define SUPPORT_WWN
	#define	UAS_OVERLAPPED_TAG
	//#ifdef UAS_DI_TOUT
#else
	//#define OS_DESC
#endif

//#define DBG_FUNCTION
#ifdef DBG_FUNCTION
	//#define DBG_ODD_U2
	#define DEBUG
	//#define DEBUG0
	//#define DEBUG_2

	//#define PR32
	//#define UART_RX
	//#define DEBUG_DP
	#define DBG_UNDRUN

	//#define BOT_TOUT
	#ifdef BOT_TOUT
		//#define UAS_TOUT
	#endif

	//#define DEBUG_LTSSM

#endif

//#define REJECT_U1U2
//#define FRESCO_RECOVERY
#define FAST_USB20
#define USING_HW_TIMEOUT
#define SCSI_DOWNLOAD_FW
#define WRITE_BUFFER_TEST

#define SCSI_HID	0
#define PHYSICAL_MORE_512  // fast patch, just support 4K now, need update later
//#define W_PROTECT
//#define SUPPORT_3T_4K

#define POW_MGMT
//#define LINK_RECOVERY
//#define ADJUST_AMPLITUDE 
//#define U3_LOOPBACK
//#define SATA_AUTO_FIFO_RST

#define ATA_PASS_THROUGTH
#define USB2_DI_PATCH


//#define EQ_SETTING_TEST
#define ATAPI

#define STANDBY_TIMER
//#define TEMP_TIMER
//	#define	HDD_TEMP_TIMER	(4 * 60 *10)		// 4 min * 60 * 10

#ifndef DBG_FUNCTION
	#define ACT_LED_ON()		(*gpio_CtrlDO_0 &= (~GPIO0_DATA))
	#define ACT_LED_OFF()		(*gpio_CtrlDO_0 |=  (GPIO0_DATA))
#else
	#define ACT_LED_ON()		(*gpio_CtrlDO_0 &= (~GPIO3_DATA) )
	#define ACT_LED_OFF()		(*gpio_CtrlDO_0 |=  (GPIO3_DATA) )
#endif


#define SPI_FW_PRIMARY			0x0000
#define SPI_FW_SECONDARY		0x8000

#endif //CONFIG_H
