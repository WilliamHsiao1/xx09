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
 * 3610		2010/04/09	Odin	Initial version
 * 3610		2010/04/27	Odin	USB2.0 BOT Debugging,
 *									we must replace the address pointer into struct
 * 3607		2010/10/06	Ted		Add Chip Revision, Chip VBUS
 *									VBUS detect use GPIO2/Chip VBUS
 * 3607		2010/11/15	Odin		Add USB2_PLL_FREERUN define for fast_hdd
 * 3607		2010/11/20	Odin		AES function
 *
 *****************************************************************************/

#ifndef INI3609_H
#define INI3609_H

sfr SCTXT_INDEX			= 0xF1; 
sfr SCM_INDEX			= 0xF2; 

sfr XTIMER_CTRL			= 0xF3; 
	#define xtimer_disable()		XTIMER_CTRL = 0

//F4h-F7h: These four bytes will hold the CPU Read Cache.
//Each time a byte is read from the external register space
// (not including Instructions) the four bytes are stored little endian in F4h-F7h
sfr CACHE_BYTE0			= 0xF4; 
sfr CACHE_BYTE1			= 0xF5; 
sfr CACHE_BYTE2			= 0xF6; 
sfr CACHE_BYTE3			= 0xF7; 

sfr SCTXT_Flag			= 0xE8;

	#define SCTXT_FLAG_NO_DATA		BIT_7			// 1 : no Data xfer
	#define SCTXT_FLAG_DIN			BIT_6			// 1 : Data-in
	#define SCTXT_FLAG_U2B			BIT_5			// 1 :  Data transfer between USB & Dbuf
	#define SCTXT_FLAG_B2S			BIT_4			// 1 :  Data transfer between Dbuf & SATA
	#define SCTXT_FLAG_RET_TFR		BIT_3		// Return copy of TFR in CTXT_FIS upon upon command completion.
	#define SCTXT_FLAG_NCQ			BIT_2		// 
	#define SCTXT_FLAG_STANDBY		BIT_1		// 
	#define SCTXT_ATA_PASS			BIT_0		// ATA Pass-thru command
	sbit SCTXT_Flag_Dir		= 0xEF; 
	sbit SCTXT_flag_NO_DATA	= 0xEE; 
	sbit SCTXT_Flag_U2B		= 0xED; 			// 1 :  Data transfer between USB & Dbuf
	sbit SCTXT_Flag_B2S		= 0xEC; 			// 1 :  Data transfer between Dbuf & SATA
	sbit SCTXT_Flag_RET_TFR	= 0xEB; 		// Return copy of TFR in CTXT_FIS upon upon command completion.
	sbit SCTXT_Flag_NCQ		= 0xEB; 		// SATA NCQ PROTO

sfr SCTXT_Next			= 0xE9;
sfr SCTXT_DbufIndex		= 0xEA;
sfr SCTXT_DbufINOUT		= 0xEB;
sfr SCTXT_CCMIndex		= 0xEC;
sfr SCTXT_Status		= 0xED;
sfr SCTXT_LunStatus		= 0xEE;
sfr SCTXT_Tag			= 0xEF;

sfr SCM_prot		= 0xF8;
sfr SCM_CdbIndex	= 0xF9;
sfr SCM_SegIndex	= 0xFA;
sfr SCM_SegINOUT	= 0xFB;
sfr SCM_Next		= 0xFD;
sfr SCM_Status		= 0xFC;
sfr SCM_Fis_Staus	= 0xFE;
sfr SCM_Fis_Error	= 0xFF;

#define SCM_Fis_LBAM	SCM_prot
#define SCM_Fis_LBAH	SCM_SegINOUT

#define INIC3609_BASE_ADDR	0xC000
#define PERIPH_BASE_ADDR	0xC400
#define TX_DBUF_ADDR		0xC800
#define HOST_CONTEXT_ADDR	0xCC00
#define USB_BASE_ADDR		0xD000
#define USB_MSC_BASE_ADDR	0xD100

#define SATA_BASE_ADDR		0xE000
//#define AES_BASE_ADDR		0x40056400
//#define HOST_CONTEXT_ADDR	0x4005C000

// Ports Configuration of Data Buffer 12K
#define TX_DBUF_CPU_W_PORT	0x01
#define TX_DBUF_CPU_R_PORT	0x02
#define TX_DBUF_USB_W_PORT	0x03
#define TX_DBUF_USB_R_PORT	0x04
#define TX_DBUF_SATA0_W_PORT	0x05
#define TX_DBUF_SATA0_R_PORT	0x06
#define TX_DBUF_NULL			0x0

//DBUF Segment mapping
#define DBUF_SEG_S2U	0
#define DBUF_SEG_U2S	1
#define DBUF_SEG_C2U	2
#define DBUF_SEG_U2C	2
//#define DBUF_SEG_C2S	2
#define DBUF_SEG_S2C	1
#define DBUF_SEG_NULL   0xFF



#define	TX_DBUF_ID	(0)

#define    DIR_WRITE    0
#define    DIR_READ    1
//#define	RX_DBUF_ID	(1)

#define DBUF_SEG0_SIZE	8
#define DBUF_SEG0_UAS_SIZE	4

#define DBUF_SEG1_SIZE	8
#define DBUF_SEG1_UAS_SIZE	4

#define DBUF_SEG2_SIZE	4

#define DBUF_SEG0_ADDR	0
#define DBUF_SEG1_ADDR	0
#define DBUF_SEG2_ADDR	1024*8


/****************************************************************************/
/********                                                            ********/
/********      CPU Registers:								         ********/
/********                                                            ********/
/****************************************************************************/
#define disable_timer0()

/****************************************************************************/
/********                                                            ********/
/********        USB3 PMA Analog PHY Low Level Registers	         ********/
/********                                                            ********/
/****************************************************************************/

#define PMA_PHY_ANALOG_00		0x00
#define PMA_PHY_ANALOG_01		0x01
#define PMA_PHY_ANALOG_02		0x02
	#define SSC_CENTER_SPREAD_MODE	0x00
	#define SSC_DOWN_SPREAD_MODE	0x02
	#define SSC_UP_SPREAD_MODE		0x04
#define PMA_PHY_ANALOG_03		0x03
	#define EN_SSC_LOOP				0x80
	#define SSC_0PPM				0x00
	#define SSC_500PPM				0x01
	#define SSC_1000PPM				0x02
	#define SSC_1500PPM				0x03
	#define SSC_2000PPM				0x04
	#define SSC_4500PPM				0x09

#define PMA_PHY_ANALOG_04		0x04
#define PMA_PHY_ANALOG_05		0x05
#define PMA_PHY_ANALOG_06		0x06
#define PMA_PHY_ANALOG_07		0x07

#define PMA_PHY_ANALOG_08		0x08
#define PMA_PHY_ANALOG_09		0x09
#define PMA_PHY_ANALOG_0A		0x0A
#define PMA_PHY_ANALOG_0B		0x0B

#define PMA_PHY_ANALOG_0C		0x0C

/****************************************************************************/
/********                                                            ********/
/********        USB3 PCS Digital PHY Low Level Registers	         ********/
/********                                                            ********/
/****************************************************************************/

#define PCS_PHY_DIGIAL_00		0x00
#define PCS_PHY_DIGIAL_01		0x01
#define PCS_PHY_DIGIAL_02		0x02
#define PCS_PHY_DIGIAL_03		0x03

#define PCS_PHY_DIGIAL_04		0x04
#define PCS_PHY_DIGIAL_05		0x05
#define PCS_PHY_DIGIAL_06		0x06
#define PCS_PHY_DIGIAL_07		0x07

#define PCS_PHY_DIGIAL_08		0x08
#define PCS_PHY_DIGIAL_09		0x09
#define PCS_PHY_DIGIAL_0A		0x0A
#define PCS_PHY_DIGIAL_0B		0x0B

#define PCS_PHY_DIGIAL_0C		0x0C
#define PCS_PHY_DIGIAL_0D		0x0D
#define PCS_PHY_DIGIAL_0E		0x0E
#define PCS_PHY_DIGIAL_0F		0x0F

#define PCS_PHY_DIGIAL_10		0x10
#define PCS_PHY_DIGIAL_11		0x11
#define PCS_PHY_DIGIAL_12		0x12
#define PCS_PHY_DIGIAL_13		0x13
		#define PD_USB3_RX_EN			0x80
		#define PD_USB3_TX_EN			0x40
		#define PD_USB3_Reg_EN			0x20
		#define PD_USB3_PLL_EN			0x10
		#define HW_RESTART_TXRX_DISABLE		0x80			//for A61 or later
		#define HW_RESTART_USB2_ENABLE		0x40			//for A61 or later
		#define HW_RESTART_VREG_DISABLE		0x20			//for A61 or later
		#define HW_RESTART_PLL_DISABLE		0x10			//for A61 or later
		#define PD_USB3_RX				0x08
		#define PD_USB3_TX				0x04
		#define PD_VREG					0x02
		#define PD_USB3_PLL				0x01

#define PCS_PHY_DIGIAL_14		0x14
#define PCS_PHY_DIGIAL_15		0x15
#define PCS_PHY_DIGIAL_16		0x16
#define PCS_PHY_DIGIAL_17		0x17

#define PCS_PHY_DIGIAL_18		0x18
#define PCS_PHY_DIGIAL_19		0x19
#define PCS_PHY_DIGIAL_1A		0x1A
#define PCS_PHY_DIGIAL_1B		0x1B

#define PCS_PHY_DIGIAL_1C		0x1C
#define PCS_PHY_DIGIAL_1D		0x1D
#define PCS_PHY_DIGIAL_1E		0x1E
#define PCS_PHY_DIGIAL_1F		0x1F

/****************************************************************************/
/********                                                            ********/
/********        SATA Anaog & Digital Phy Control Registers          ********/
/********                                                            ********/
/****************************************************************************/
#define SATA_PHY_ANALOG_0_0		0x00
#define SATA_PHY_ANALOG_0_1		0x01
#define SATA_PHY_ANALOG_0_2		0x02
#define SATA_PHY_ANALOG_0_3		0x03

#define SATA_PHY_ANALOG_1_0		0x04
#define RX_SQU_ADJ		(B_BIT_2|B_BIT_1|B_BIT_0)		//
#define SATA_PHY_ANALOG_1_1		0x05
	#define TX_DRV_AMP		B_BIT_2		//
#define SATA_PHY_ANALOG_1_2		0x06
#define SATA_PHY_ANALOG_1_3		0x07

#define SATA_PHY_ANALOG_2_0		0x08
#define SATA_PHY_ANALOG_2_1		0x09
#define SATA_PHY_ANALOG_2_2		0x0A
#define SATA_PHY_ANALOG_2_3		0x0B

#define SATA_PHY_DIGIAL_0_0		0x0C
#define SATA_PHY_DIGIAL_0_1		0x0D
	#define PD_SATA_PLL		B_BIT_11	//0x08
	#define PD_DRVR_T_MUX	B_BIT_10	//0x04
	#define PD_TX			B_BIT_9		//0x02
	#define PD_RX			B_BIT_8		//0x01

#define SATA_PHY_STATUS			0x20


/****************************************************************************/
/********                                                            ********/
/********      General Registers:(address area: )                    ********/
/********                                                            ********/
/****************************************************************************/

/////////////////////////////////////////////////////////////////////////
#define chip_IntStaus_0		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0))
	#define CHIP_VBUS_OFF_INT		(B_BIT_7)
	#define CHIP_VBUS_ON_INT		(B_BIT_6)
	#define USB_SUSPEND_INT			(B_BIT_5)
	#define	DBUF_TX_INT				(B_BIT_4)
	#define MBIST_DONE				(B_BIT_3)
	#define USB_INT					(B_BIT_2)
	#define PERIPH_INT				(B_BIT_1)
	#define SATA0_INT				(B_BIT_0)

#define chip_IntStaus_1		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 1))
	#define	PROGRAM_ERROR			(B_BIT_9)
	#define	TEST_MODE_STAT			(B_BIT_8)

#define chip_IntStaus_2		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 2))

#define chip_IntStaus_3		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 3))
	#define SATA0CLK_FLUSHED		(B_BIT_28)
	#define USBCLK_FLUSHED			(B_BIT_27)


/////////////////////////////////////////////////////////////////////////
#define chip_IntEn_0		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 4))
	#define CHIP_VBUS_OFF_INTEN		(B_BIT_7)
	#define CHIP_VBUS_ON_INTEN		(B_BIT_6)
	#define USB_SUSPEND_INTEN		(B_BIT_5)
	#define	DBUF_TX_INTEN			(B_BIT_4)
	#define MBIST_DONEEN			(B_BIT_3)
	#define USB_INTEN				(B_BIT_2)
	#define PERIPH_INTEN			(B_BIT_1)
	#define SATA0_INTEN				(B_BIT_0)

#define chip_IntEn_1		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 5))
	#define	PROGRAM_ERROREN			(B_BIT_9)
	#define	TEST_MODE_STATEN		(B_BIT_8)

#define chip_IntEn_3		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 7))
	#define SATA0CLK_FLUSHEDEN		(B_BIT_28)
	#define USBCLK_FLUSHEDEN		(B_BIT_27)


/////////////////////////////////////////////////////////////////////////
#define chip_Revision	((u8 volatile xdata *)(INIC3609_BASE_ADDR + 8))
	#define	REV_A51			0x03
	#define	REV_A61			0x07
	#define	REV_A71			0x0F
	#define	REV_A91			0x1F

/////////////////////////////////////////////////////////////////////////
#define CHIP_ID2		(INIC3609_BASE_ADDR + 0x0C)
#define chip_ID2_0		((u8 volatile xdata *)(CHIP_ID2))
#define chip_ID2_1		((u8 volatile xdata *)(CHIP_ID2 + 1))
#define chip_ID2_2		((u8 volatile xdata *)(CHIP_ID2 + 2))
#define chip_ID2_3		((u8 volatile xdata *)(CHIP_ID2 + 3))
	
/////////////////////////////////////////////////////////////////////////
#define cup_MemCtrl_0		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x10))
	#define BOOT_MODE			0x00	// default mode
	#define OVERLAY_MODE		0x02
	#define SWAP_MODE			0x03
	#define CPU_RESET			(B_BIT_7)

	#define BOOT_CPU_SWAP()		{*cup_MemCtrl_0 = SWAP_MODE;}
	#define BOOT_CPU_RESET()	{*cup_MemCtrl_0 = (CPU_RESET|BOOT_MODE);}

#define cup_MemCtrl_1		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x11))
#define cup_MemCtrl_2		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x12))
	#define CPU_SLOW_1_CLK	0x01
	#define CPU_SLOW_2_CLK	0x02
	#define CPU_SLOW_3_CLK	0x03


/////////////////////////////////////////////////////////////////////////
// Chip ID Register Register
#define CHIP_ID			(INIC3609_BASE_ADDR + 0x14)
#define chip_ID_0		((u8 volatile xdata *)(CHIP_ID))
#define chip_ID_1		((u8 volatile xdata *)(CHIP_ID+1))
#define chip_ID_2		((u8 volatile xdata *)(CHIP_ID+2))
#define chip_ID_3		((u8 volatile xdata *)(CHIP_ID+3))

/////////////////////////////////////////////////////////////////////////
// CPU Clock Register
#define cpu_Clock_0		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x18))
	#define CPU_CLK_DIV			0x0F				// rw
	#define CPU_CLK_DIV_DEFAULT 0x0F

#define cpu_Clock_1		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x19))

#define cpu_Clock_2		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x1A))
	#define ASIC_RESET			(B_BIT_23)			// W
	#define USB3_PLL_RDY		(B_BIT_21)			// r
	#define USB2_PLL_RDY		(B_BIT_20)			// r
	#define SATA0_PLL_RDY		(B_BIT_16)			// r

#define cpu_Clock_3		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x1B))
	#define ASIC_PLL_BYPASS		(B_BIT_31)			// rw
	#define USBCLK_DSBL			(B_BIT_30)			// rw
	#define	USBCLK_SELECT		(B_BIT_29)			// rw
	#define ASIC_ALT_CLK		(B_BIT_28)			// rw
	#define	STOP_REFCLK			(B_BIT_27)			// rw
	#define	XTAL_DISABLE		(B_BIT_26)			// rw
	#define	FORCE_USB3CLK		(B_BIT_25)			// rw


/////////////////////////////////////////////////////////////////////////
// CPU_WAKEUP_CTRL Register
#define CPU_WAKEUP_CTRL			(INIC3609_BASE_ADDR + 0x1C)
#define cpu_wakeup_ctrl_0		((u8 volatile xdata *)(CPU_WAKEUP_CTRL))
	#define CPU_USB2_L1_SLEEP		B_BIT_3
	#define CPU_USB_VBUS			B_BIT_2
	#define CPU_USB_UNSUSPENED		B_BIT_1
	#define CPU_USB_SUSPENDED		B_BIT_0

#define cpu_wakeup_ctrl_1		((u8 volatile xdata *)(CPU_WAKEUP_CTRL+1))
	#define VBUS_SENSE				B_BIT_10
	#define VBUS_GPIO2				B_BIT_9
	#define USB_SELECT				B_BIT_8
#define cpu_wakeup_ctrl_2		((u8 volatile xdata *)(CPU_WAKEUP_CTRL+2))
	#define USB_WAKEUP_REQ			B_BIT_16

#ifdef USE_GPIO2_VBUS	// use GPIO2 as VBUS detection
	#define	USB_VBUS_ON()			(*cpu_wakeup_ctrl_1 & VBUS_GPIO2)
	#define USB_VBUS_OFF()			((*cpu_wakeup_ctrl_1 & VBUS_GPIO2) == 0)
	//extern bit USB_VBUS_OFF();

#else
	#define	USB_VBUS_ON()			(*cpu_wakeup_ctrl_0 & CPU_USB_VBUS)
	#define USB_VBUS_OFF()			((*cpu_wakeup_ctrl_0 & CPU_USB_VBUS) == 0)
#endif



// ++++++++++++++++++++++++++++++++++++++++++++++ //
// ++++++++++++++++++ MBIST Def +++++++++++++++++ //
// ++++++++++++++++++++++++++++++++++++++++++++++ //
#define mbist_Ctrl0_0		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x20))
	#define MBIST_DBUF_FAIL			(B_BIT_6)
	#define MBIST_DBUF_DONE			(B_BIT_5)
	#define MBIST_DBUF_EN			(B_BIT_4)
	#define MBIST_CTXTMEM_FAIL			(B_BIT_2)
	#define MBIST_CTXTMEM_DONE			(B_BIT_1)
	#define MBIST_CTXTMEM_EN			(B_BIT_0)
	

#define mbist_Ctrl0_1		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x21))
	#define	MBIST_MSCEDBUF_FAIL		(B_BIT_14)
	#define	MBIST_MSCEDBUF_DONE		(B_BIT_13)
	#define	MBIST_MSCEDBUF_EN		(B_BIT_12)
	#define MBIST_CTRLBUF_FAIL		(B_BIT_10)
	#define MBIST_CTRLBUF_DONE		(B_BIT_9)
	#define MBIST_CTRLBUF_EN		(B_BIT_8)


#define mbist_Ctrl0_2		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x22))
	#define MBIST_SATA0RX_FAIL		(B_BIT_22)
	#define MBIST_SATA0RX_DONE		(B_BIT_21)
	#define MBIST_SATA0RX_EN		(B_BIT_20)
	#define	MBIST_UTXRXFIFO_FAIL	(B_BIT_18)
	#define	MBIST_UTXRXFIFO_DONE	(B_BIT_17)
	#define	MBIST_UTXRXFIFO_EN		(B_BIT_16)

#define mbist_Ctrl0_3		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x23))
	#define		MBIST_SATACCM_FAIL		(B_BIT_30)
	#define		MBIST_SATACCM_DONE		(B_BIT_29)
	#define		MBIST_SATACCM_EN		(B_BIT_28)
	#define  	MBIST_IMEM_FAIL			(B_BIT_26)
	#define  	MBIST_IMEM_DONE			(B_BIT_25)
	#define  	MBIST_IMEM_EN			(B_BIT_24)

#define mbist_Ctrl1_0		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x24))
	#define		MBIST_MSCRXSBUF_FAIL		(B_BIT_6)
	#define		MBIST_MSCRXSBUF_DONE		(B_BIT_5)
	#define		MBIST_MSCRXSBUF_EN			(B_BIT_4)
	#define  	MBIST_MSCTXSBUF_FAIL		(B_BIT_2)
	#define  	MBIST_MSCTXSBUF_DONE		(B_BIT_1)
	#define  	MBIST_MSCTXSBUF_EN			(B_BIT_0)

//#define mbist_Ctrl1		((u32 volatile xdata *)(INIC3609_BASE_ADDR + 0x24))
//#define  	MBIST_SATA0TX_FAIL		(B_BIT_6)
//#define  	MBIST_SATA0TX_DONE		(B_BIT_5)
//#define  	MBIST_SATA0TX_EN		(B_BIT_4)


//#define mbist_Ctrl2		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x28))
//#define		MBIST_MSC0_TXSBUF_FAIL	(B_BIT_22)
//#define		MBIST_MSC0_TXSBUF_DONE	(B_BIT_21)
//#define		MBIST_MSC0_TXSBUF_EN	(B_BIT_20)
//#define		MBIST_MSC0_RXSBUF_FAIL	(B_BIT_18)
//#define		MBIST_MSC0_RXSBUF_DONE	(B_BIT_17)
//#define		MBIST_MSC0_RXSBUF_EN	(B_BIT_16)
//#define  	MBIST_SATA0CM_FAIL		(B_BIT_2)
//#define  	MBIST_SATA0CM_DONE		(B_BIT_1)
//#define  	MBIST_SATA0CM_EN		(B_BIT_0)


// ++++++++++++++++++++++++++++++++++++++++++++++ //
// ++++++++++++++++ PHY_SPI_CTRL ++++++++++++++++ //
// ++++++++++++++++++++++++++++++++++++++++++++++ //
#define PHY_SPI_CTRL	(INIC3609_BASE_ADDR + 0x40)
#define phy_spi_ctrl_0	((u8 volatile xdata *)(PHY_SPI_CTRL))
//#define		PHY_SPI_DATA(x)	(x & 0xFF)

#define phy_spi_ctrl_1	((u8 volatile xdata *)(PHY_SPI_CTRL+1))
//#define		PHY_ADDR_OFFSET	(8)
//#define		PHY_SPI_ADDR(x)	((x) << 8)

#define phy_spi_ctrl_2	((u8 volatile xdata *)(PHY_SPI_CTRL+2))
//#define		PHY_SEL_OFFSET	(16)
#define		PHY_SPI_U2		(0x0)		// USB2 PHY
#define		PHY_SPI_U3PMA	(0x1)		// USB3 PMA PHY
#define		PHY_SPI_U3PCS	(0x2)		// USB3 PCS PHY
#define		PHY_SPI_SATA	(0x3)		// SATA0 Phy

#define		PHY_CMD_READ	((0x4L) << 4)
#define		PHY_CMD_WRITE	((0x6L) << 4)
#define		PHY_CMD_RST		((0xFL) << 4)

#define phy_spi_ctrl_3	((u8 volatile xdata *)(PHY_SPI_CTRL+3))
	#define	PHY_DONE		B_BIT_31

// ++++++++++++++++++++++++++++++++++++++++++++++ //
// +++++++++ Switch Regulartor Control ++++++++++ //
// ++++++++++++++++++++++++++++++++++++++++++++++ //
#define SWREG_CTRL0	(INIC3609_BASE_ADDR + 0x44)
#define swreg_ctrl_0		((u8 volatile xdata *)(SWREG_CTRL0))

#define SWREG_CTRL1	(INIC3609_BASE_ADDR + 0x45)
#define swreg_ctrl_1		((u8 volatile xdata *)(SWREG_CTRL1))
#define		SW_MINI_DRIVER	B_BIT_3


// ++++++++++++++++++++++++++++++++++++++++++++++ //
#define CHIP_TEST_RESULT	(INIC3609_BASE_ADDR + 0x48)
#define chip_test_result	((u8 volatile xdata *)(CHIP_TEST_RESULT))
	#define		MBIST_TEST_FAIL		B_BIT_0
	#define		LOOP_TEST_FAIL		B_BIT_1
	//#define	LOOP_TEST1_FAIL		B_BIT_2
	//#define	ECB_TEST_FAIL		B_BIT_3
	//#define	XTS_TEST_FAIL		B_BIT_4

#define CHIP_STATUS			(INIC3609_BASE_ADDR + 0x49)	//	--> romcode_check_stat
#define chip_status			((u8 volatile xdata *)(CHIP_STATUS))
	#define	USB_FS				B_BIT_1
	#define	USB_HS				B_BIT_2
	#define	USB_SS				(B_BIT_1|B_BIT_2)
	#define	FW_ON_SPI			B_BIT_3
	#define	NV_ON_SPI			B_BIT_4			//	don't need pass to util.
	#define	HDD_VALID			B_BIT_5
	#define	NV_ON_I2C			B_BIT_6
	#define	NV_ON_HDD			B_BIT_7
	
#define FW_STATUS			(INIC3609_BASE_ADDR + 0x4A)
#define fw_status			((u8 volatile xdata *)(FW_STATUS))
	#define	BOOT_FROM_SECONDARY	B_BIT_1
	#define	BOOT_FROM_PRIMARY	B_BIT_0

#define FW_TEMP_3			(INIC3609_BASE_ADDR + 0x4B)
#define fw_temp_3			((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x4B))
	#define USB_ENUM_QUAL		B_BIT_7
	#define DIS_AUTO_RST_SW		B_BIT_4

#define HOST_CTXTMEM_PTR	(INIC3609_BASE_ADDR + 0x4C)
#define host_ctxtmem_ptr	((u8 volatile xdata *)(HOST_CTXTMEM_PTR))

/////////////////////////////////////////////////////////////////////////
#define	SHADOW_USB_REG0   (INIC3609_BASE_ADDR + 0x50)			//	SHADOW_REGISTER,	map with USB register[0]
//   [31:0]    USBINT
#define usb_IntStatus_shadow_0		((u8 volatile xdata *)(SHADOW_USB_REG0 + 0x000))

#define usb_IntStatus_shadow_1		((u8 volatile xdata *)(SHADOW_USB_REG0 + 0x001))

#define usb_IntStatus_shadow_2		((u8 volatile xdata *)(SHADOW_USB_REG0 + 0x002))

#define usb_CMgr_Status_shadow		((u8 volatile xdata *)(SHADOW_USB_REG0 + 0x003))

/////////////////////////////////////////////////////////////////////////
//#define	usb_DevState_shadow	   	(INIC3609_BASE_ADDR + 0x54)	//	SHADOW_REGISTER	//	map with USB register[0xC]
#define usb_DevState_shadow		((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x54))
#define	SHADOW_USB_REG1_USB3_STATE_CTRL		(INIC3609_BASE_ADDR + 0x55)	//	SHADOW_REGISTER	//	map with USB reigster[0xC9]
#define	SHADOW_USB_REG1_USB3_LTSSM_STATE	(INIC3609_BASE_ADDR + 0x56)	//	SHADOW_REGISTER	//	map with USB reigster[0xC2]
//	[7:0]     USB_DEVICE_STATE
//	[15:8]    USB3_STATE_CTRL
//	[31:16]   USB3_LTSSM_STAT

#define usb3_Phy_Ctrl_0			((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x5C))

#define cpu_XtimerCtrl				((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x060))
	#define XTIME_ONCE		B_BIT_2
	#define XTIME_RESTART	B_BIT_1
	#define XTIME_ENABLE	B_BIT_0
#define cpu_XtimerTick				((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x061))
#define cpu_XtimerPeriod_0			((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x062))
#define cpu_XtimerPeriod_1			((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x063))

/*****************************************************************************/
/********                                                             ********/
/******** programmable Peripheral Controller                          ********/
/********          (address area:0xC400 - 0xC5xx)             ********/
/*****************************************************************************/


//*****************************************************************************
// GPIO Control Registers  (000h-030h)
//*****************************************************************************
// defines the associated GPIO pin is an output
//#define GPIO_DOEn			(PERIPH_BASE_ADDR + 0x000)
#define gpio_DOEn_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x000))
	#define   GPIO0_EN			B_BIT_0
	#define   GPIO1_EN			B_BIT_1
	#define   GPIO2_EN			B_BIT_2
	#define   GPIO3_EN			B_BIT_3
	#define   GPIO4_EN			B_BIT_4
	#define   GPIO5_EN			B_BIT_5
	#define   GPIO6_EN			B_BIT_6
	#define   GPIO7_EN			B_BIT_7
#define gpio_DOEn_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x001))
	#define   GPIO8_EN			B_BIT_8
	#define   GPIO9_EN			B_BIT_9
	#define   GPIO10_EN			B_BIT_10

#define GPIO_CTRL_DO		(PERIPH_BASE_ADDR + 0x004)
#define gpio_CtrlDO_0		((u8 volatile xdata *)(GPIO_CTRL_DO))
#define gpio_CtrlDO_1		((u8 volatile xdata *)(GPIO_CTRL_DO + 0x001))
#define gpio_CtrlDO_2		((u8 volatile xdata *)(GPIO_CTRL_DO + 0x002))
#define gpio_CtrlDO_3		((u8 volatile xdata *)(GPIO_CTRL_DO + 0x003))

/////////////////////////////////////////////////////////////////////////
#define GPIO_CTRL_DI		(PERIPH_BASE_ADDR + 0x008)
#define gpio_DI_0			((u8 volatile xdata *)(GPIO_CTRL_DI))
#define gpio_DI_1			((u8 volatile xdata *)(GPIO_CTRL_DI + 1))
#define gpio_DI_2			((u8 volatile xdata *)(GPIO_CTRL_DI + 2))
#define gpio_DI_3			((u8 volatile xdata *)(GPIO_CTRL_DI + 3))

/////////////////////////////////////////////////////////////////////////
#define GPIO_CTRL_DD		(PERIPH_BASE_ADDR + 0x00C)

#define gpio_DD_0			((u8 volatile xdata *)(GPIO_CTRL_DD))
	#define   GPIO0_DATA		B_BIT_0
	#define   GPIO1_DATA		B_BIT_1
	#define   GPIO2_DATA		B_BIT_2
	#define   GPIO3_DATA		B_BIT_3
	#define   GPIO4_DATA		B_BIT_4
	#define   GPIO5_DATA		B_BIT_5
	#define   GPIO6_DATA		B_BIT_6
	#define   GPIO7_DATA		B_BIT_7

#define gpio_DD_1			((u8 volatile xdata *)(GPIO_CTRL_DD + 1))
	#define   GPIO8_DATA		B_BIT_8
	#define   GPIO9_DATA		B_BIT_9
	#define   GPIO10_DATA		B_BIT_10

#define gpio_DD_2			((u8 volatile xdata *)(GPIO_CTRL_DD + 2))

#define gpio_DD_3			((u8 volatile xdata *)(GPIO_CTRL_DD + 3))

#define gpio_PU_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x010))
#define gpio_PU_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x011))
#define gpio_PU_2			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x012))
#define gpio_PU_3			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x013))

#define gpio_PD_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x014))
#define gpio_PD_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x015))
#define gpio_PD_2			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x016))
#define gpio_PD_3			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x017))


#define gpio_I0_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x020))
#define gpio_I0_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x021))
#define gpio_I0_2			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x022))
#define gpio_I0_3			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x023))

#define gpio_I0_EN_0		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x024))
#define gpio_I0_EN_1		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x025))
#define gpio_I0_EN_2		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x026))
#define gpio_I0_EN_3		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x027))

#define gpio_I1_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x028))
#define gpio_I1_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x029))
#define gpio_I1_2			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x02A))
#define gpio_I1_3			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x02B))

#define gpio_I1_EN_0		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x02C))
#define gpio_I1_EN_1		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x02D))
#define gpio_I1_EN_2		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x02E))
#define gpio_I1_EN_3		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x02F))

/////////////////////////////////////////////////////////////////////////
#define GPIO_CTRL_FSEL		((PERIPH_BASE_ADDR + 0x030))
#define gpio_CtrlFsel_0		(U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x030)
	#define LED0_0_SEL			0x01			// GPIO 0
	#define UR_TXSEL			0x02			// GPIO 0
	#define LED1_0_SEL			0x03			// GPIO 0
	#define GPIO0_FSE_MASK		0x03			// GPIO 0

	#define LED0_1_SEL			0x04			// GPIO 1
	#define UR_RXSEL			0x08			// GPIO 1
	#define LED1_1_SEL			0x0C			// GPIO 1
	#define GPIO1_FSE_MASK		0x0C			// GPIO 1

	#define	LED0_2_SEL			0x10			// GPIO 2
	#define	LED1_2_SEL			0x30			// GPIO 2
	#define GPIO2_FSE_MASK		0x30			// GPIO 2

	#define	LED0_3_SEL			0x40			// GPIO 3
	//#define I2C_SDA				0x80			// GPIO 3
	#define	LED1_3_SEL			0xC0			// GPIO 3
	#define	GPIO3_FSE_MASK		0xC0			// GPIO 3

#define gpio_CtrlFsel_1		((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x031))
	#define	LED0_4_SEL			0x01			// GPIO 4
	#define I2C_SCL				0x02			// GPIO 4
	#define	LED1_4_SEL			0x03			// GPIO 4
	#define GPIO4_FSE_MASK		0x03			// GPIO 4

	#define LED0_5_SEL			0x04			// GPIO 5
	#define LED1_5_SEL			0x08			// GPIO 5
	#define GPIO5_FSE_MASK		0x0C			// GPIO 5

	#define	SPI_CS_SEL			0x10			// GPIO 6
	#define GPIO6_FSE_MASK		0x30			// GPIO 6

	#define	SPI_SDI_SEL			0x40			// GPIO 7
	//#define	GSPI_SDI_SEL	0x80			// GPIO 7
	#define	GPIO7_FSE_MASK		0xC0			// GPIO 7

#define gpio_CtrlFsel_2		((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x032))
	#define	SPI_SDO_SEL			0x01			// GPIO 8
	//#define	GSPI_SDO_SEL	0x02			// GPIO 8
	#define GPIO8_FSE_MASK		0x03			// GPIO 8

	#define	SPI_SCK_SEL			0x04			// GPIO 9
	//#define	GSPI_SCK_SEL	0x08			// GPIO 9	
	#define GPIO9_FSE_MASK		0x0C			// GPIO 9

	#define GPIO10_FSE_MASK		0x30			// GPIO 10

#define gpio_CtrlFsel_3		((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x033))


////////////////////////////////////////////////////////////////////////////////////
//		Serial Flash Controller Registers (160h-17Ch)
////////////////////////////////////////////////////////////////////////////////////
#define spi_Ctrl_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x160))
	#define	  SPI_CMD_READ 	3			// SST SST25VF010A and PMC Pm25LV010 sflash device

#define spi_Ctrl_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x161))
	#define   SPI_CNT_MASK			(B_BIT_10|B_BIT_9|B_BIT_8)
	//#define   SPI_CNT_SHIFT		8
	#define   SPI_ADDRV				B_BIT_11
	#define   SPI_DATAV				B_BIT_12
	#define   SPI_WRITE				B_BIT_13
	#define   SPI_READ				0x00
	#define   SFLASH_PAGE_CLEAR		B_BIT_14
	#define	  SFLASH_RDBURST_EN		B_BIT_15
#define spi_Ctrl_2			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x162))
	#define   SPI_START				B_BIT_16

#define spi_Ctrl_3			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x163))
	#define   SFLASH_PAGE_RESET		B_BIT_31

	//#define   SPI_WR_DONE			0x02
	//#define   SPI_DATA_READY		0x01

/////////////////////////////////////////////////////////////////////////
#define spi_Data_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x164))
#define spi_Data_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x165))
#define spi_Data_2			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x166))
#define spi_Data_3			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x167))


/////////////////////////////////////////////////////////////////////////
#define spi_Addr_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x168))
#define spi_Addr_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x169))
#define spi_Addr_2			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x16A))
#define spi_Addr_3			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x16B))

/////////////////////////////////////////////////////////////////////////
#define spi_Clock_0			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x16C))
#define spi_Clock_1			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x16D))

/////////////////////////////////////////////////////////////////////////
#define spi_IntStatus		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x170))
	#define SPI_IntStatus_Done	0x1

/////////////////////////////////////////////////////////////////////////
#define spi_IntEn			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x171))

#define spi_RdB_count		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x172))






////////////////////////////////////////////////////////////////////////////////////
//		LED0 Controller Registers (180h-1FCh)
////////////////////////////////////////////////////////////////////////////////////
// --------------------------------------------
// LED ctrl0:
// bit[31:28]	reserved
// bit[23:16]	PWM_Period:  T = Tled*N
// bit[7:0]	PWD_repeat:  defines how many LED PWM period are counted before int

// ---------------------------------------------
// LED ctrl1:
// bit[31:25]	reserved
// bit[24]	Done_en: when 1, will generate an interrupt to the cpu
// bit[23]	Enable
// bit[22]	PWM_mode;
// bit[21]	Forever : run forever no interrupt
// bit[20]	Auto_dir: 1: from bright to dark, 0: from dark to bright;
// bit[19:18]	Auto_mode:
// bit[17]	Polarity
// bit[16]	Done
// bit[15:8]	Up_limit: this is the up value of bright
// bit[7:0]      Low_limit: this is the lowest value of bright


#define led0_repeat			((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x180))
#define led0_bright			((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x181))
#define led0_period			((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x182))
#define led0_timer_unit		((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x183))

#define led0_lowLimit		((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x184))
#define led0_HighLimit		((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x185))
#define led0_ctrl			((U8 volatile xdata *)(PERIPH_BASE_ADDR + 0x186))
	#define LED_ENABLE			B_BIT_23
	#define LED_PWM_MODE		B_BIT_22
 	#define LED_FOREVER			B_BIT_21  //
	#define	LED_DIR_DIM			B_BIT_20
	//#define LED_STEP			B_BIT_19|B_BIT_18
	#define LED_SING_DIR		B_BIT_19
	#define LED_BI_DIR			B_BIT_18
	#define LED_POLARITY		B_BIT_17

#define led0_IntStatus		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x188))
	#define LED_DONE			B_BIT_0

#define led0_IntEn			((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x189))
	#define	led0_IntrDisable()	(*led0_IntEn = 0)
	#define	led0_IntrEnable()	(*led0_IntEn = LED_DONE)

/*
// ==============================================
// blinking mode setting:
#define		Blinking_ctrl0	0x00ff0053
#define		Blinking_ctrl1	0x0080ff00

// pwm
#define		PWM_ctrl0	0x0f100010
#define		PWM_ctrl1	0x00f2ff00
*/

////////////////////////////////////////////////////////////////////////////////////
//		Peripheral Interrupt Registers (200h)
////////////////////////////////////////////////////////////////////////////////////
#define periph_IntStatus_0	((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x200))
	//#define 	GSPI0_INT		B_BIT_4
	//#define 	UART0_INT		B_BIT_0

#define periph_IntStatus_1	((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x201))
	#define 	SFLASH_INT		B_BIT_15
	#define 	GPIO_I1_INT		B_BIT_13
	#define 	GPIO_I0_INT		B_BIT_12
	#define 	I2C0_INT		B_BIT_8

#define periph_IntStatus_2	((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x202))
	#define 	LED0_INT		B_BIT_16


#define periph_IntEn_0		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x204))
#define periph_IntEn_1		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x205))
#define periph_IntEn_2		((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x206))

#define Periph_Db_Timing_0	((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x208))
#define Periph_Db_Timing_1	((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x209))
#define Periph_Db_Timing_2	((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x20A))
#define Periph_Db_Timing_3	((u8 volatile xdata *)(PERIPH_BASE_ADDR + 0x20B))

// ++++++++++++++++++++++++++++++++++++++++++++++ //
// +++++++++ Peripheral CRC-16 Registers ++++++++ //
// ++++++++++++++++++++++++++++++++++++++++++++++ //
#define CRC_CTRL			(PERIPH_BASE_ADDR + 0x210)
#define crc_ctrl_0			((u8 volatile xdata *)(CRC_CTRL))
	#define	CRC_EN				BIT_0

#define CRC_DATA			(PERIPH_BASE_ADDR + 0x214)
#define crc_data_0			((u8 volatile xdata *)(CRC_DATA))
#define crc_data_1			((u8 volatile xdata *)(CRC_DATA + 1))
#define crc_data_2			((u8 volatile xdata *)(CRC_DATA + 2))
#define crc_data_3			((u8 volatile xdata *)(CRC_DATA + 3))

/****************************************************************************/
/********                                                            ********/
/********    Tx Data Buffer Registers:							     ********/
/********                                                            ********/
/****************************************************************************/
typedef struct _DBUF_SEG {
	u8	dbuf_Seg_ADDR_0;			// 0x0
	u8	dbuf_Seg_ADDR_1;			// 0x1
	u8	dbuf_Seg_ADDR_2;			// 0x2
	u8	dbuf_Seg_ADDR_3;			// 0x3
	u8	dbuf_Seg_InOut;				// 0x4
	u8	dbuf_Seg_OUT;				// 0x5
	u8	dbuf_Seg_Size;				// 0x6
	u8	dbuf_Seg_Control;			// 0x7
} DBUF_SEG, *PDBUF_SEG;

//dbuf_Seg_Control
	#define 	SEG_VALID				B_BIT_7
	#define 	SEG_EMPTY				B_BIT_6
	#define 	SEG_OUT1_SDONE			B_BIT_5
	#define 	SEG_OUT0_SDONE			B_BIT_4
	#define 	SEG_IN_SDONE			B_BIT_3
	#define 	SEG_SEL_OUT_CNT			B_BIT_2
	#define 	SEG_DONE				B_BIT_1
	#define 	SEG_RESET				B_BIT_0

typedef struct _DBUF_PORT {

	u8 dbuf_Port_Addr_0;		// 0x0
	u8 dbuf_Port_Addr_1;		// 0x1
	u8 dbuf_Port_Addr_2;		// 0x2
	u8 dbuf_Port_Addr_3;		// 0x3
	u8 dbuf_Port_Count_0;		// 0x4
	u8 dbuf_Port_Count_1;		// 0x5
	u8 dbuf_Port_Count_2;		// 0x6
	u8 dbuf_Port_Count_3;		// 0x7

} DBUF_PORT, *PDBUF_PORT;


//#define dbuf_Seg				((DBUF_SEG volatile xdata *)(TX_DBUF_ADDR + 0x000))
//extern DBUF_SEG volatile xdata dbuf_Seg[];

#define dbuf_BufferSize			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x080))


#define	dbuf_BufferStatus		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x081)) 
#define	dbuf_SegmentStatus		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x082)) 
#define	dbuf_PortStatus			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x083)) 

#define	dbuf_DataPort			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x084)) 

#define dbuf_Thresh				((u8 volatile xdata *)(TX_DBUF_ADDR + 0x088)) 
#define	dbuf_IntStatus			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x08A)) 
#define	dbuf_IntEn				((u8 volatile xdata *)(TX_DBUF_ADDR + 0x08B)) 

#define	dbuf_PerrAddr_0			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x08C)) 
#define	dbuf_PerrAddr_1			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x08D)) 
#define	dbuf_PerrAddr_2			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x08E)) 
#define	dbuf_PerrAddr_3			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x08F)) 
	
#define	dbuf_MemoryPort_0		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x090)) 
#define	dbuf_MemoryPort_1		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x091)) 
#define	dbuf_MemoryPort_2		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x092)) 
#define	dbuf_MemoryPort_3		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x093)) 


#define	dbuf_MemoryAddr_0		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x094)) 
#define	dbuf_MemoryAddr_1		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x095)) 
#define	dbuf_MemoryAddr_2		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x096)) 
#define	dbuf_MemoryAddr_3		((u8 volatile xdata *)(TX_DBUF_ADDR + 0x097)) 

//#define	dbuf_PortnStatus[14]	((u8 volatile xdata *)(TX_DBUF_ADDR + 0x0A0))
extern u8 volatile xdata dbuf_PortnStatus[];

#define	dbuf_Port				((DBUF_PORT volatile xdata *)(TX_DBUF_ADDR + 0x100))
//DBUF_PORT volatile xdata dbuf_Port[];

#define	dbuf_MuxSel				((u8 volatile xdata *)(TX_DBUF_ADDR + 0x180)) 

#define dbuf_MuxAddr_0			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x184))
#define dbuf_MuxAddr_1			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x185))
#define dbuf_MuxAddr_2			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x186))
#define dbuf_MuxStatus			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x187))
	#define SEG_IDLE   				0
	#define SEG_BUSY   				1

#define dbuf_MuxInOut			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x188))
#define dbuf_MuxOut1			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x189))
#define dbuf_MuxSize			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x18A))
#define dbuf_MuxCtrl			((u8 volatile xdata *)(TX_DBUF_ADDR + 0x18B))

/****************************************************************************/
/********                                                            ********/
/********    Host CDB Context Registers:					                         ********/
/********                                                            ********/
/****************************************************************************/
#define CTXT_NULL	0xFF


#define ctxt_Flag				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x000))
	#define CTXT_FLAG_INVALID_IU	B_BIT_7			//received IU is not a Command or Task Management Funct
	#define CTXT_FLAG_PHASE_ERR		B_BIT_6
	#define CTXT_FLAG_SAT_FAIL		B_BIT_5
	#define CTXT_FLAG_LENGTH_ERR	B_BIT_4			// CDB length of CBW was not 1-16 bytes or any reserved bits in byte 14 were non-zero
	#define CTXT_FLAG_SIZE_ERR		B_BIT_3			// CBW was not exactly 31 bytes.
	#define CTXT_FLAG_SIG_ERR		B_BIT_2			// CBW Signature was incorrect
	#define CTXT_FLAG_LUN_ERR		B_BIT_1			// any reserved bits in byte 15 of CBW were non-zero.
	#define CTXT_FLAG_IFNUM			B_BIT_0

	//#define CTXT_FLAG_CBW_LEN_ZERO	BIT_7			// 1 :  
	//#define CTXT_FLAG_U2B			BIT_6			// 1 :  Data transfer between USB & Dbuf
	//#define CTXT_FLAG_B2S			BIT_5			// 1 :  Data transfer between Dbuf & SATA
	//#define CTXT_FLAG_RET_TFR		BIT_4		// Return copy of TFR in CTXT_FIS upon upon command completion.
	//#define CTXT_FLAG_SYNC			BIT_3			// 1 :  

#define ctxt_Control			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x001))
	#define CTXT_CTRL_DIR			(BIT_7)				// 0 = Data-Out from host to the device,

#define ctxt_SatFailReason		((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x002))
	#define SAT_UNKNOWN_CDB			(BIT_5)
	#define SAT_ALIGN_ERR_END		(BIT_4)	
	#define SAT_ALIGN_ERR_START		(BIT_3)	
	#define SAT_LBA_OVRN			(BIT_2)	
	#define SAT_LBA_ERR				(BIT_1)	
	#define SAT_SECCNT_ERR			(BIT_0)	

//#define ctxt_DBUF_IN_OUT		ctxt_SatFailReason
//#define ctxt_Next				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x002))

#define ctxt_SataProto			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x003))

#define ctxt_ITag_0				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x004))
#define ctxt_ITag_1				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x005))
#define ctxt_ITag_2				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x006))
#define ctxt_ITag_3				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x007))

#define ctxt_XferLen_0			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x008))
#define ctxt_XferLen_1			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x009))
#define ctxt_XferLen_2			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x00A))
#define ctxt_XferLen_3			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x00B))


#define ctxt_Lun				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x00C))

#define ctxt_CDBLength			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x00D))

#define ctxt_PhaseCase_0		((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x00E))
#define ctxt_PhaseCase_1		((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x00F))


#define ctxt_CDB				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x010))
//	#define		CmdBlk(i)			(ctxt_CDB[i])



//#define ctxt_DBufIndex			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x024))
//#define ctxt_DBuf_IN_OUT		((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x025))

//#define ctxt_Index				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x026))


//#define ctxt_Status				((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x027)) 
	#define CTXT_STATUS_GOOD			0x00
	#define CTXT_STATUS_ERROR			0x01
	#define CTXT_STATUS_PHASE			0x02
	#define CTXT_STATUS_XFER_DONE		0xFD
	#define CTXT_STATUS_HALT			0xFE
	#define CTXT_STATUS_PENDING			0xFF

//#define	ctxt_LunStatus			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x028))	//	STATUS  code  of  UAS SENSE IU 
	#define LUN_STATUS_GOOD				0x00 		// command complete w/o Error
	#define LUN_STATUS_CHKCOND			0x02 		// Check status (command complete w sense data).
	#define LUN_STATUS_BUSY				0x08 		// LUN is busy.
	#define LUN_STATUS_TASK_SET_FULL	0x28		// lack of task set resources to accept additional UAS Command IU
	#define LUN_STATUS_TASK_ABORTED		0x40		// lack of task set resources

//#define ctxt_CCMIndex			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x029))
#define ctxt_CCMcmdinten		((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x02A))

#define ctxt_No_Data			((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x02F))
	//#define	MSC_CTXT_VALID				B_BIT_15
	#define	MSC_CTXT_NODATA				B_BIT_14

#define ctxt_FIS				(((u8 volatile xdata *)(HOST_CONTEXT_ADDR + 0x030)))


/****************************************************************************/
/********                                                            ********/
/********    USB Registers:					                         ********/
/********                                                            ********/
/****************************************************************************/
#define usb_IntStatus_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x000))
	#define	DEV_CONFIG				B_BIT_7
	#define	HOT_RESET				B_BIT_6
	#define	WARM_RESET				B_BIT_5
	#define	USB_BUS_RST				B_BIT_4
	#define USB_SUSPEND		 		B_BIT_3
	#define USB_WAKEUP		 		B_BIT_2
	#define	VBUS_ON					B_BIT_1
	#define	VBUS_OFF				B_BIT_0

#define usb_IntStatus_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x001))
	#define USB3_ERROR	 			B_BIT_15
	#define USB3_STATE	 			B_BIT_14
	#define MSCE_SERVICE 			B_BIT_13
	#define CDB_AVAIL_INT 			B_BIT_12
	#define	MSC1_INT				B_BIT_11
	#define	MSC0_INT				B_BIT_10
	#define	INTIN_INT				B_BIT_9
	#define	CTRL_INT				B_BIT_8

#define usb_IntStatus_2			((u8 volatile xdata *)(USB_BASE_ADDR + 0x002))
	//#define UART_PKT_RCVD			B_BIT_17
	#define USB3_COUNT_MAX 			B_BIT_16

#define usb_IntEn_0				((u8 volatile xdata *)(USB_BASE_ADDR + 0x004))
#define usb_IntEn_1				((u8 volatile xdata *)(USB_BASE_ADDR + 0x005))
#define usb_IntEn_2				((u8 volatile xdata *)(USB_BASE_ADDR + 0x006))

#define	usb_DevState			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C))
	#define VBUS					B_BIT_7
	#define L1_SLEEP				B_BIT_6
	#define USB_ATTACHED			B_BIT_5
	#define USB_POWERED				B_BIT_4
	#define USB_SUSPENDED 			B_BIT_3
	#define USB_DEFAULT				B_BIT_2
	#define USB_ADDRESSED			B_BIT_1
	#define USB_CONFIGURED			B_BIT_0
	
#define	usb_DevAddress			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D))

#define	usb_DevStatus_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0E))
	#define		USB3_MODE			B_BIT_7
	#define		USB2_MODE			B_BIT_6
	#define		USB2_HS_MODE		B_BIT_3
	#define		U1_ENABLE			B_BIT_1
	#define		U2_ENABLE			B_BIT_0

#define	usb_DevStatus_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0F))
	#define		USB3_PROTOCOL_IDLE	B_BIT_15


#define usb_DevCtrl_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x010))
#define usb_DevCtrlClr_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x014))
	#define		EXIT_SS_INACTIVE		B_BIT_6
	#define		USB_FW_RDY				B_BIT_4
	#define		FS_ONLY					B_BIT_2
	#define		USB_ENUM				B_BIT_1
	#define		SELF_POWER				B_BIT_0
#define usb_DevCtrl_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x011))
#define usb_DevCtrlClr_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x015))
	#define		USB3_U1_REJECT			B_BIT_15
	#define		USB3_U2_ENTER			B_BIT_14
	#define		USB3_U1_ENTER			B_BIT_13
	#define		USB3_U1U2_EXIT			B_BIT_12
	#define		USB2_FORCE_SUSP			B_BIT_10
	#define		USB_CORE_RESET			B_BIT_9
	#define		EXIT_SS_DISABLED		B_BIT_8
#define usb_DevCtrl_2			((u8 volatile xdata *)(USB_BASE_ADDR + 0x012))
#define usb_DevCtrlClr_2		((u8 volatile xdata *)(USB_BASE_ADDR + 0x016))
	#define		USB2_L1_DISABLE			B_BIT_18
	#define		USB2_PLL_FREERUN		B_BIT_17
	#define		USB3_U2_REJECT			B_BIT_16
#define usb_DevCtrl_3			((u8 volatile xdata *)(USB_BASE_ADDR + 0x013))
#define usb_DevCtrlClr_3		((u8 volatile xdata *)(USB_BASE_ADDR + 0x017))



#define USB_CoreCtrl			(USB_BASE_ADDR + 0x018)
#define usb_CoreCtrl_0			((u8 volatile xdata *)(USB_CoreCtrl))
#ifdef ASIC_CONF
	#define	USB_SMALL_EP			B_BIT_3
#else
	#define	USB_SMALL_EP			B_BIT_2
#endif
	#define USB_CMD_PENDING_ENABLE	B_BIT_1

#define usb_CoreCtrl_1			((u8 volatile xdata *)(USB_CoreCtrl + 1))
	#define		USB3_TXDEEMPH			(B_BIT_15|B_BIT_14)		// bit 15:14
	#define		UAS_ZERO_DATA			B_BIT_13				// bit 13
	#define		VBUS_RESET_ENUM_EN		B_BIT_12				// bit 12
	//#define		SUSP_SM_HP_FIX		B_BIT_11
	//#define		SE0_HP_FIX			B_BIT_10
	//#define		ACK_MISS_EN			B_BIT_9
	//#define		USB3_ERR_DSBL		B_BIT_8
	#define		AUTO_BOT4				B_BIT_11
	#define		AUTO_BOT3				B_BIT_10
	#define		AUTO_BOT2				B_BIT_9
	#define		AUTO_BOT1				B_BIT_8

#define usb_CoreCtrl_2			((u8 volatile xdata *)(USB_CoreCtrl + 2))
	#define		VBUS_ENUM_DISABLE		B_BIT_22		// bit 22
	#define		USB3_MODE_DISABLE		B_BIT_20		// bit 20
	#define		USB2_MODE_DISABLE		B_BIT_19		// bit 19
	#define		USB3_TXMARGIN			(B_BIT_18|B_BIT_17|B_BIT_16)		// bit 18:16

#define usb_CoreCtrl_3			((u8 volatile xdata *)(USB_CoreCtrl + 3))
	#define USB3_BURST_MAX_RX		B_BIT_31
	#define USB3_BURST_MAX_TX		B_BIT_30
	#define	USB3_BURST_EN			(B_BIT_28|B_BIT_27|B_BIT_26|B_BIT_25|B_BIT_24)		// bit 28:24



#define usb_Threshold0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x01C))
#define usb_Threshold1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x01D))
#define usb_Threshold2		((u8 volatile xdata *)(USB_BASE_ADDR + 0x01E))

#define usb_CMgr_Status		((u8 volatile xdata *)(USB_BASE_ADDR + 0x01F))
	#define CMGR_USB3_MODE 			B_BIT_5
	#define CMGR_USB2_MODE 			B_BIT_4
	#define CMGR_IDLE				0
	#define CMGR_USB3_CONNECT		1
	#define CMGR_USB3_SUSPEND		2
	#define CMGR_USB2_DETECT		3
	#define CMGR_USB2_CONNECT		4
	#define CMGR_USB2_SUSPEND		5
	#define CMGR_USB_DISCONNECTED	6

#define usb_Ep0Ctrl			((u8 volatile xdata *)(USB_BASE_ADDR + 0x20))
#define usb_Ep0CtrlClr		((u8 volatile xdata *)(USB_BASE_ADDR + 0x21))
	#define EP0_FLOW_CTRL		B_BIT_7
	#define	CTRL_DATA			B_BIT_5
	#define	CTRL_STAT			B_BIT_4
	#define EP0_SETUP			B_BIT_3
	#define EP0_SRUN			B_BIT_2
	#define EP0_RUN				B_BIT_1
	#define EP0_HALT			B_BIT_0

#define usb_Ep0IntEn		((u8 volatile xdata *)(USB_BASE_ADDR + 0x22))
	#define CTRL_HALT_STAT		B_BIT_1

#define usb_RestartCode		((u8 volatile xdata *)(USB_BASE_ADDR + 0x23))
	#define	RestartCode_BootFromICCM	(0xA5)

#define usb_Ep0TxLengh_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x24))  //[9-0] bits
#define usb_Ep0TxLengh_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x25))  //[9-0] bits

#define usb_Ep0BufLengh_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x26))  //[9-0] bits
#define usb_Ep0BufLengh_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x27))  //[9-0] bits

// Setup Packet (Hdr0 - Hdr7[7:0])
#define inPktSetup      ((u8 volatile xdata *)(USB_BASE_ADDR + 0x28))



#define usb_IntInCtrl			((u8 volatile xdata *)(USB_BASE_ADDR + 0x34))
#define usb_IntInCtrlClr		((u8 volatile xdata *)(USB_BASE_ADDR + 0x35))

#define usb_IntInTxLength		((u8 volatile xdata *)(USB_BASE_ADDR + 0x36))

#define usb_CtxtSize_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x38))
#define usb_CtxtSize_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x39))
	#define RST_CTXT_MEM			B_BIT_15

#define usb_CtxtAvailFIFO		((u8 volatile xdata *)(USB_BASE_ADDR + 0x3A))

#define usb_CtxtFreeFIFO		((u8 volatile xdata *)(USB_BASE_ADDR + 0x3B))

//USB2_CONNECT_TICK[15:0] - Define tick rate for timer:  Ttick = (USB2_CONNECT_TICK+1)*Trefclk;
#define usb_USB2ConnectTick_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x3C))
#define usb_USB2ConnectTick_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x3D))


//  USB2_CONNECT_TIMEOUT[15:0] - Defines the timeout Period: Ttimeout = (USB2_CONNECT_TIMEOUT+1)*Ttick
#define usb_USB2ConnectTout_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x3E))
#define usb_USB2ConnectTout_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x3F))


// USB3_IDLE_TIMEOUT 
#define usb_USB3Idle_Tout		((u8 volatile xdata *)(USB_BASE_ADDR + 0x40))


#define usb_U2InactivtyTimeout	((u8 volatile xdata *)(USB_BASE_ADDR + 0x41))

#define usb_DeadTimeout_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x42))
#define usb_DeadTimeout_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x43))

#define	USB_DeviceRequestEn		(USB_BASE_ADDR + 0x44)
#define usb_DeviceRequestEn_0		((u8 volatile xdata *)(USB_DeviceRequestEn))
	#define USB_GET_STATUS_ENABLE	B_BIT_7
	#define USB_SET_FEATURE_ENABLE	B_BIT_4
	#define SET_INTERFACE_ENABLE	B_BIT_3
	#define	SET_ISOCH_DELAY_ENABLE	B_BIT_2

#define usb_DeviceRequestEn_1		((u8 volatile xdata *)(USB_DeviceRequestEn + 1))
	#define 	USB_GET_DESCR_STD_UTILITY	B_BIT_15
	#define 	USB_GET_DESCR_VND_UTILITY	B_BIT_14
	#define 	USB_CLEAR_FEATURE_ENABLE	B_BIT_11



#define usb_ConfigCtrl			((u8 volatile xdata *)(USB_BASE_ADDR + 0x46))

#define usb_USB2TestMode		((u8 volatile xdata *)(USB_BASE_ADDR + 0x47))
	#define   TM_TEST_PACKET		B_BIT_3
	#define   TM_TEST_SE0_NAK		B_BIT_2
	#define   TM_TEST_K				B_BIT_1
	#define   TM_TEST_J				B_BIT_0

#define usb_WakeUpCtrl_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x48))
#define usb_WakeUpCtrl_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x49))

#define usb_USB2Option_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x4C))
	#define USB2_FS_MODE_QUALIFIER		B_BIT_7
	#define USB2_SUSPEND_RESET_SEL		B_BIT_6
	#define USB2_SUSPEND_RESET_CTRL		(B_BIT_5|B_BIT_4)
	#define USB2_HS_MODE_QUALIFIER		B_BIT_2
	#define USB2_MODE_PRIORITY			B_BIT_1

#define usb_USB2Option_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x4D))
	#define USB2_BUS_CONNECTED_WAIT_ENABLE	B_BIT_15
	#define USB2_VBUS_TIMER_ENABLE			B_BIT_14
	#define USB2_WARM_RESET_SEL_PSM			B_BIT_13
	#define USB2_WARM_RESET_SEL_CONN		B_BIT_12
	#define USB2_SS_INACTIVE_PSM			B_BIT_11
	#define USB2_WARM_RESET_PSM				B_BIT_10
	#define USB2_SS_INACTIVE_CONN			B_BIT_9
	#define USB2_WARM_RESET_CONN			B_BIT_8

#define usb_USB3Option_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x4E))
	#define USB3_MAX_POST			(B_BIT_6|B_BIT_5|B_BIT_4)
	#define USB3_MAX_POST2			B_BIT_5
	#define USB3_MAX_POST1			B_BIT_4
	#define	USB3_DISABLE_EOB		B_BIT_3
#define usb_USB3Option_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x4F))
	#define USB3_POLLING_PSM		B_BIT_14			// when usb3 LTSSM is polling, disable USB2 port state machine
	#define USB3_DP_SKID_DISABLE_1	B_BIT_13
	#define USB3_DP_SKID_DISABLE_0	B_BIT_12
	#define USB3_TXOFS_MAX_ENABLE	B_BIT_11
	#define USB3_DISABLE_FLOW_DEAD	B_BIT_9
	#define USB3_ENABLE_FLOW_PRIME	B_BIT_8

#define usb_MsceDBufferPort_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x050))
#define usb_MsceDBufferPort_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x051))
#define usb_MsceDBufferPort_2	((u8 volatile xdata *)(USB_BASE_ADDR + 0x052))
#define usb_MsceDBufferPort_3	((u8 volatile xdata *)(USB_BASE_ADDR + 0x053))

#define usb_MsceDataCount_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x054))
#define usb_MsceDataCount_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x055))

#define usb_MsceSense			((u8 volatile xdata *)(USB_BASE_ADDR + 0x056))

#define usb_MsceStatus			((u8 volatile xdata *)(USB_BASE_ADDR + 0x057))
#define usb_MsceCtrl			((u8 volatile xdata *)(USB_BASE_ADDR + 0x058))
	#define UART_MODE			B_BIT_4

#define usb_VbusDeglitch		((u8 volatile xdata *)(USB_BASE_ADDR + 0x059))
#define usb_U3EobTimeout_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x05A))
#define usb_U3EobTimeout_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x05B))

//USB2_CONNECT_TICK[15:0] - Define tick rate for timer:  Ttick = (FS_CONNECT_TICK+1)*Trefclk;
#define usb_FSConnectTick_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x5C))
#define usb_FSConnectTick_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x5D))


//  FS_CONNECT_TIMEOUT[15:0] - Defines the timeout Period: Ttimeout = (FS_CONNECT_TIMEOUT+1)*Ttick
#define usb_FSConnectTout_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x5E))
#define usb_FSConnectTout_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x5F))


#define usb_MsceCDBBuffer		((u8 volatile xdata *)(USB_BASE_ADDR + 0x060))		// (060h-07Ch)


#define usb_U1Sel				((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A0))
#define usb_U1Pel				((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A1))

#define usb_U2Sel_0				((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A4))
#define usb_U2Sel_1				((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A5))

#define usb_U2Pel_0				((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A6))
#define usb_U2Pel_1				((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A7))

#define usb_LinkCtrl_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A8))
	#define DESCRSE_SKP_NUM			B_BIT_7
	#define SEND_8LFPS_RCV_1		B_BIT_4
#define usb_LinkCtrl_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0A9))
#define usb_LinkCtrl_2			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0AA))
	#define DISABLE_U1U2_AUTO		B_BIT_23
	#define ENABLE_TX_TRUNCATE		B_BIT_22
	#define DISABLE_SAVE_LATENCY	B_BIT_21	// when this one is not set, link will try to save 2 clocks
	#define TX_DATA3_ABORT_DISABLE	B_BIT_20
#define usb_LinkCtrl_3			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0AB))
	#define HW_U1U2_ENABLE			B_BIT_31
	#define DECLARE_PROTOCOL_IDLE	B_BIT_30
	#define WBE_ON					B_BIT_29
	#define ENABLE_TS_CLEAR			B_BIT_27
	#define TS_DETECT_COUNT			(B_BIT_26 | B_BIT_25)
		#define TS_DETECT_COUNT_16		B_BIT_25
		#define TS_DETECT_COUNT_32		B_BIT_26
		#define TS_DETECT_COUNT_128		(B_BIT_26 | B_BIT_25)
	#define ENHANCED_TS_DETECT		B_BIT_24

// Link Debug Parameters
#define usb_LinDBG_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0AC))
	#define USB3_TXLINK_MODE				B_BIT_4
	#define DISCARD_ZERO_LENGTH_QUALIFIER	B_BIT_0
#define usb_LinDBG_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0AD))
	#define LFPS_MODE_1						B_BIT_8
	#define USB3_Ux_EXIT_DISABLE_FW			B_BIT_9
	#define UX_EXIT_LFPS_LOW				B_BIT_10
	#define DISABLE_TIMEOUT					B_BIT_11
#define usb_LinDBG_2			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0AE))
	#define USB_RX_DETECT_CNT				(B_BIT_19|B_BIT_18|B_BIT_17|B_BIT_16)
	#define USB_RX_DETECT_CNT_EQ_1									B_BIT_16
	#define USB_RX_DETECT_CNT_EQ_2							B_BIT_17
	#define USB_RX_DETECT_CNT_EQ_3							(B_BIT_17|B_BIT_16)
	#define USB_RX_DETECT_CNT_EQ_4					(B_BIT_18)
	#define USB_RX_DETECT_CNT_EQ_7					(B_BIT_18|B_BIT_17|B_BIT_16)
	#define USB_RX_DETECT_CNT_EQ_8			(B_BIT_19)
	#define USB_RX_DETECT_CNT_EQ_10			(B_BIT_19|		B_BIT_17)
	#define USB_RX_DETECT_CNT_EQ_11			(B_BIT_19|		B_BIT_17|B_BIT_16)
	#define USB_RX_DETECT_CNT_EQ_15			(B_BIT_19|B_BIT_18|B_BIT_17|B_BIT_16)
#define usb_LinDBG_3			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0AF))
	#define USB_DISCARD_HEADPACKET_WITH_K_BYTE	B_BIT_31

#define usb_RecoveryError		((u8 volatile xdata *)(USB_BASE_ADDR + 0x0B0))


#define usb_LinkErrorCount_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0B4))
#define usb_LinkErrorCount_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0B5))
#define usb_LinkErrorCount_2	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0B6))
#define usb_LinkErrorCount_3	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0B7))


#define usb_DecodeErrorCount_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0B8))
#define usb_DecodeErrorCount_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0B9))
#define usb_DecodeErrorCount_2	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0BA))
#define usb_DecodeErrorCount_3	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0BB))

#define usb_DisparityErrorCount_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0BC))
#define usb_DisparityErrorCount_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0BD))
#define usb_DisparityErrorCount_2	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0BE))
#define usb_DisparityErrorCount_3	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0BF))

#define usb_USB3Status_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C0))
#define usb_USB3Status_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C1))

#define usb_USB3LTSSM_0			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C2))
#define usb_USB3LTSSM_0_shadow	((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x56))
	#define LTSSM_U2			B_BIT_7
	#define LTSSM_U1			B_BIT_6
	#define LTSSM_U0			B_BIT_5
	#define LTSSM_COMPLIANCE	B_BIT_4
	#define LTSSM_POLLING		B_BIT_3
	#define LTSSM_RXDETECT		B_BIT_2
	#define LTSSM_INACTIVE		B_BIT_1
	#define LTSSM_DISABLED		B_BIT_0

#define usb_USB3LTSSM_1			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C3))
#define usb_USB3LTSSM_1_shadow	((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x57))
	#define LTSSM_HOTRESET		B_BIT_11
	#define LTSSM_LOOPBACK		B_BIT_10
	#define LTSSM_RECOVERY		B_BIT_9
	#define LTSSM_U3			B_BIT_8

#define usb_USB3LTSSMEnter_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C4))
#define usb_USB3LTSSMEnter_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C5))

#define usb_USB3LTSSMExit_0		((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C6))
#define usb_USB3LTSSMExit_1		((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C7))

#define usb_USB3StateSelect		((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C8))
	#define USB3_LTSSM					0x00
	#define USB20_PORT_ST				0x01

#define usb_USB3StateCtrl			((u8 volatile xdata *)(USB_BASE_ADDR + 0x0C9))
#define usb_USB3StateCtrl_shadow	((u8 volatile xdata *)(INIC3609_BASE_ADDR + 0x55))

#define usb_USB3Capabilities_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0CC))
#define usb_USB3Capabilities_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0CD))
#define usb_USB3Capabilities_2	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0CE))
#define usb_USB3Capabilities_3	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0CF))

#define usb_USBLinkConfig0_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D0))
#define usb_USBLinkConfig0_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D1))
	#define LFPS_POLLING_TYP			0xFF
	#define LFPS_POLLING_TYP_1DOT34US	0x9C
	#define LFPS_POLLING_TYP_1DOT2US	0x8C
#define usb_USBLinkConfig0_2	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D2))
	#define LFPS_POLLING1_MAX_1DOT6US	0xC7
#define usb_USBLinkConfig0_3	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D3))

#define usb_USBLinkConfig1_0	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D4))
#define usb_USBLinkConfig1_1	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D5))
#define usb_USBLinkConfig1_2	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D6))
	#define LFPS_PING_TYP				0xF0
	#define LFPS_PING_TYP_160NS			0x50
#define usb_USBLinkConfig1_3	((u8 volatile xdata *)(USB_BASE_ADDR + 0x0D7))
	#define LFPS_TRESETDELAY_1MS	0x01
	#define LFPS_TRESETDELAY_18MS	0x12
	#define LFPS_TRESETDELAY		0x1F



//////////////////////////////////////////////////////////////////
// MSC Interface Registers (100-17Fh/180h-1FFh)
#define usb_Msc0IntStatus_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x00))
	#define MSC_RXSENSE_DONE_INT	B_BIT_7		// UAS Sense IU Transfer thru RX(DO) context  has completed.
	#define MSC_TX_DONE_INT			B_BIT_6		// USB Data In done on active CDB Context of TX Context
	#define MSC_RX_DONE_INT			B_BIT_5		// USB Data Out done on active CDB Context of RX Context
	#define BOT_RST_INT 			B_BIT_4
	#define MSC_DIN_HALT_INT		B_BIT_3		// This status is set whenever MSCn_DATAIN_HALT changes in the MSCn Data In  ctrl register(usb_Msc0DICtrl)
	#define MSC_DOUT_HALT_INT		B_BIT_2		// This status is set whenever MSCn_DATAIN_HALT changes in the MSCn Data Out  ctrl register(usb_Msc0DOCtrl)
	#define MSC_STAT_HALT_INT		B_BIT_1
	#define MSC_CMD_HALT_INT		B_BIT_0

#define usb_Msc0IntStatus_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x01))
	#define MSC_ST_TIMEOUT_INT		B_BIT_15
	#define MSC_DI_TIMEOUT_INT		B_BIT_14
	#define MSC_DO_TIMEOUT_INT		B_BIT_13
	#define MSC_CONFIG_INT			B_BIT_12
	#define MSC_TASK_AVAIL_INT		B_BIT_11
	#define MSC_TSFULL_DONE_INT		B_BIT_10
	#define MSC_RESP_DONE_INT		B_BIT_9
	#define MSC_TXSENSE_DONE_INT	B_BIT_8		// UAS Sense IU Transfer thru TX(DI) context  has completed.


#define usb_Msc0IntStatus_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x02))
	#define MSC_DATAOUT_HUNG		B_BIT_22
	#define MSC_DATAIN_HUNG			B_BIT_21
	//#define MSC_CBW_AUTO			B_BIT_20
	//#define MSC_CBW_AVAIL			B_BIT_19
	#define MSC_LOOP_INT			B_BIT_18
	#define MSC_CSW_DONE_INT		B_BIT_17
	#define MSC_CMD_TIMEOUT_INT		B_BIT_16

#define usb_Msc0IntEn_0			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x04))
#define usb_Msc0IntEn_1			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x05))
#define usb_Msc0IntEn_2			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x06))

#define usb_Msc0Ctrl_0			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0C))
	#define MSC_UAS_ENABLE			B_BIT_2
	#define MSC_BOT_ENABLE			B_BIT_1
	#define MSC_WAKEUP_CAP			B_BIT_0

#define usb_Msc0Ctrl_1			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0D))
	#define MSC_SET_INTFC_RESET		B_BIT_12
	#define MSC_ALTERNATE			B_BIT_11
	#define MSC_UAS_MODE			B_BIT_10
	#define MSC_FUNC_SUSPEND		B_BIT_9
	#define MSC_WAKE_ENABLE			B_BIT_8

#define usb_Msc0CmdCtrl			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0E))
#define usb_Msc0CmdCtrlClr		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0F))
	#define MSC_CMD_RESET			B_BIT_1
	#define MSC_CMD_HALT			B_BIT_0

#define usb_Msc0StatCtrl		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x10))
	#define MSC_STAT_RESET			(B_BIT_6|BIT_5|BIT_4)
#define usb_Msc0StatCtrlClr		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x11))
	#define MSC_STAT_FLOW_CTRL		B_BIT_7				// set when the STAT Pipehas sent an NRDY/NAK and the Host is waiting for an ERDY/ACK
    #define MSC_STAT_HOST_WAIT		(B_BIT_6|BIT_5|BIT_4)
		#define STATUS_ACKR			B_BIT_6
		#define STATUS_ACKE			B_BIT_4
	#define MSC_STAT_RESP_RUN		B_BIT_3
	#define MSC_STAT_TXSENSE_RUN	B_BIT_2
	#define MSC_STAT_RXSENSE_RUN	B_BIT_1
	#define MSC_STAT_HALT			B_BIT_0

#define usb_Msc0StatInXferStat_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x12))
#define usb_Msc0StatInXferStat_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x13))

#define usb_Msc0DICtrl			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x14))
#define usb_Msc0DICtrlClr		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x15))
	#define MSC_DIN_FLOW_CTRL		B_BIT_7		// (rsc)
	#define MSC_TXFIFO_SDONE		B_BIT_5		// (r)
	#define	MSC_DIN_DONE			B_BIT_3		// (rsc)
	#define MSC_CSW_RUN				B_BIT_2		// (rsc) for BOT only
	#define MSC_TXFIFO_EMPTY 		B_BIT_1		// (R) for usb_Msc0DICtrlClr only
	#define MSC_DATAIN_RESET		B_BIT_1		// (W) for usb_Msc0DICtrl only
	#define MSC_TXFIFO_RST			B_BIT_1		// (W) for usb_Msc0DICtrlClr only
	#define MSC_DI_HALT				B_BIT_0		// (rsc)

#define usb_Msc0DIXferStatus	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x16))
	#define	MSC_DATAIN_HOST_WAIT	0x70
		#define ACKE					0x10
		#define ACK1					0x30
		#define ACKR					0x40
	#define	MSCn_TXDONE_REASON		0x0F
		#define	XFRGOOD					0x00
		#define	XFRDORN					0x02
		#define	XFRDURN					0x04
		#define	XFRHALT					0x05
		#define	XFRSUSP					0x06
		#define	XFRRERR					0x07
		#define	XFRSRDY					0x08

// BOT CSW Status or UAS TX Sense IU Status
#define usb_Msc0DIStatus		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x17))

#define usb_Msc0DOutCtrl		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x18))
#define usb_Msc0DOutCtrlClr		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x19))
	#define	MSC_DOUT_FLOW_CTRL			B_BIT_7
	#define MSC_RXFIFO_DDONE			B_BIT_5		// (r)
	#define	MSC_DOUT_ACCUM				B_BIT_4
	#define	MSC_DOUT_BBKT				B_BIT_3
	#define	MSC_DOUT_DONE				B_BIT_2
	#define	MSC_DOUT_RESET				B_BIT_1		// (W) for usb_Msc0DOutCtrl only
	#define	MSC_RXFIFO_RESET			B_BIT_1		// (W) for usb_Msc0DOutCtrlClr only
	#define MSC_RXFIFO_RST				B_BIT_1		// (r)
	#define MSC_RXFIFO_EMPTY			B_BIT_1		// (r)
	#define MSC_DOUT_HALT				B_BIT_0

#define usb_Msc0DOXferStatus	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1A))
	#define	MSC_DATAOUT_HOST_WAIT	0x70
		#define DPE						0x10
		#define DP0						0x20
		#define DPR						0x40
	#define	MSCn_RXDONE_REASON		0x0F
		#define XFRHURN 				0x03
		#define XFRHORN 				0x01
		#define XFRDURN 				0x04
		#define XFRHALT 				0x05

// UAS RX Sense IU Status
#define usb_Msc0DOStatus		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1B))

#define usb_Msc0Residue_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1C))			// For BOT Mode
#define usb_Msc0Residue_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1D))			// For BOT Mode
#define usb_Msc0Residue_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1E))			// For BOT Mode
#define usb_Msc0Residue_3		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1F))			// For BOT Mode


#define usb_Msc0Tag_0			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1C))			// For UAS Mode
#define usb_Msc0Tag_1			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x1D))			// For UAS Mode

#define usb_Msc0Response_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x20))
#define usb_Msc0Response_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x21))
#define usb_Msc0Response_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x22))
#define usb_Msc0Response_3		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x23))

#define usb_Msc0SIXferLength_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x24))
#define usb_Msc0SIXferLength_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x25))

#define usb_Msc0XferStatus		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x26))

#define usb_Msc0CtxtLimit_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x28))
#define usb_Msc0CtxtLimit_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x29))

#define usb_Msc0CtxtUsed_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x2A))
#define usb_Msc0CtxtUsed_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x2B))

#define usb_Msc0RxXferCount_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x2C))
#define usb_Msc0RxXferCount_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x2D))
#define usb_Msc0RxXferCount_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x2E))
#define usb_Msc0RxXferCount_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x2F))

#define usb_Msc0RxCtxt_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x30))
	#define	MSC_RX_CTXT_NODATA		B_BIT_7
	//#define	MSC_RX_AUTO_FIS			B_BIT_6

#define usb_Msc0RxCtxt_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x31))
	//#define	MSC_RX_CTXT_VALID		B_BIT_15

#define usb_Msc0TxXferCount_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x34))
#define usb_Msc0TxXferCount_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x35))
#define usb_Msc0TxXferCount_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x36))
#define usb_Msc0TxXferCount_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x37))

#define usb_Msc0TxCtxt_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x38))
	#define		MSC_TX_CTXT_NODATA		B_BIT_7
	//#define		MSC_TX_AUTO_FIS			B_BIT_6

#define usb_Msc0TxCtxt_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x39))
	//#define		MSC_TX_CTXT_VALID		B_BIT_15

#define usb_MscClrFea_Halt		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x3A))
	#define		MSC_CMD_CLRFEAT_HALT		B_BIT_3
	#define		MSC_STAT_CLRFEAT_HALT		B_BIT_2
	#define		MSC_DATAOUT_CLRFEAT_HALT	B_BIT_1
	#define		MSC_DATAIN_CLRFEAT_HALT		B_BIT_0

#define usb_Msc0TaskITag_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x3C))
#define usb_Msc0TaskITag_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x3D))

#define usb_Msc0TaskTTag_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x3E))
#define usb_Msc0TaskTTag_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x3F))

#define usb_Msc0LunSATX		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x40))
	#define	FUA_FPDMA_DISABLE		B_BIT_1
	#define	FUA_EXT_DISABLE			B_BIT_0

#define usb_Msc0TaskFunc		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x41))

#define usb_Msc0TaskFlag		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x42))
	//#define	TASK_SIZE_ERR		(B_BIT_1)			//
	#define	TASK_LUN_ERR		(B_BIT_0)			// 

#define usb_Msc0LunCtrl			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x43))

#define usb_Msc0LunCtxtUsed_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x44))
#define usb_Msc0LunCtxtUsed_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x45))

#define usb_Msc0Lun_SAT_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x46))
	#define	HOST_BLK_SIZE		(B_BIT_7|B_BIT_6|B_BIT_5)			// bit 7:5
		#define	HOST_BLK_4K			(BIT_6|BIT_5)
		#define	HOST_BLK_2K			B_BIT_6
		#define	HOST_BLK_1K			B_BIT_5
		#define	HOST_BLK_512			0
	#define	SATA_BLK_SIZE		(B_BIT_4|B_BIT_3)					// bit 4:3
		#define	SATA_BLK_4K			(B_BIT_4|B_BIT_3)
		#define	SATA_BLK_512		0
	#define	SAT_CMD				(B_BIT_2|B_BIT_1|B_BIT_0)			// bit 2:0
		#define	SATA_NCQ_CMD		B_BIT_2
		#define	SATA_DMAE_CMD		B_BIT_0
		#define	SATA_DMA_CMD		0
		#define SATA_PACKET_CMD		7

#define usb_Msc0Lun_SAT_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x47))
	#define SAT_DISABLE			B_BIT_11

#define usb_Msc0LunExtentLo_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x48))
#define usb_Msc0LunExtentLo_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x49))
#define usb_Msc0LunExtentLo_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x4A))
#define usb_Msc0LunExtentLo_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x4B))

#define usb_Msc0LunExtentHi_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x4C))
#define usb_Msc0LunExtentHi_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x4D))
#define usb_Msc0LunExtentHi_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x4E))
#define usb_Msc0LunExtentHi_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x4F))


#define usb_Msc0LunOffset_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x50))
#define usb_Msc0LunOffset_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x51))
#define usb_Msc0LunOffset_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x52))
#define usb_Msc0LunOffset_3		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x53))
#define usb_Msc0LunOffset_4		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x54))
#define usb_Msc0LunOffset_5		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x55))
#define usb_Msc0LunOffset_6		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x56))
#define usb_Msc0LunOffset_7		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x57))


#define usb_Msc0TxSensePort_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x58))
#define usb_Msc0TxSensePort_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x59))
#define usb_Msc0TxSensePort_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x5A))
#define usb_Msc0TxSensePort_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x5B))

#define usb_Msc0RxSensePort_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x5C))
#define usb_Msc0RxSensePort_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x5D))
#define usb_Msc0RxSensePort_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x5E))
#define usb_Msc0RxSensePort_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x5F))

#define usb_Msc0TxSenseAddress	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x60))

#define usb_Msc0RxSenseAddress	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x61))

#define usb_Msc0TxBurst			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x62))

#define usb_Msc0RxBurst			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x63))


#define usb_Msc0LoopCtrl_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x64))
#define		MSC_LOOP_FAIL			B_BIT_7	// r
#define		MSC_LOOP_TXPAUSED		B_BIT_6	// r
#define		MSC_LOOP_RXPAUSED		B_BIT_5	// r
#define		MSC_LOOP_RUN			B_BIT_4	// r
#define		MSC_LOOP_BLK4K			0x0C	// LOOP_BLKSIZE 4096 Bytes
#define		MSC_LOOP_BLK2K			0x08	// LOOP_BLKSIZE 2048 Bytes
#define		MSC_LOOP_BLK512			0x04	// LOOP_BLKSIZE 512 Bytes
#define		MSC_LOOP_NONBLK			0x00	// LOOP_BLKSIZE none
#define		MSC_LOOP_FWEN			B_BIT_1
#define		MSC_LOOP_HWEN			B_BIT_0

#define usb_Msc0LoopCtrl_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x65))
#define		MSC_LOOP_DONE			B_BIT_8	// rc

#define usb_Msc0LoopCtrl_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x66))
#define		MSC_LOOP_RESET			B_BIT_21
#define		MSC_LOOP_TXCONT			B_BIT_20
#define		MSC_LOOP_RXCONT			B_BIT_19
#define		MSC_LOOP_TXPAUSE		B_BIT_18
#define		MSC_LOOP_RXPAUSE		B_BIT_17
#define		MSC_LOOP_START			B_BIT_16


#define usb_Msc0LoopData_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x68))
#define usb_Msc0LoopData_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x69))
#define usb_Msc0LoopData_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x6A))
#define usb_Msc0LoopData_3		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x6B))


// MSCn Loop Expected Data Register (_6Ch)
#define usb_Msc0LoopEData_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x6C))
#define usb_Msc0LoopEData_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x6D))
#define usb_Msc0LoopEData_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x6E))
#define usb_Msc0LoopEData_3		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x6F))


// MSCn Loop Count Register (_70h)
#define usb_Msc0_LoopCount_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x70))
#define usb_Msc0_LoopCount_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x71))
#define usb_Msc0_LoopCount_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x72))
#define usb_Msc0_LoopCount_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x73))


// MSCn FIFO Count Register (_74h)
#define usb_Msc0_TXBCNT_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x74))
#define usb_Msc0_TXBCNT_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x75))

#define usb_Msc0_RXBCNT_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x76))
#define usb_Msc0_RXBCNT_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x77))


// MSCn RX Status Qualifier Register (_78h)
#define usb_Msc0_RXSTATQ_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x78))	// Status Qualifier in the Sense IU for a RX Sense IU tx.
#define usb_Msc0_RXSTATQ_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x79))	// Status Qualifier in the Sense IU for a RX Sense IU tx.


// MSCn TX Status Qualifier Register (_7Ah)
#define usb_Msc0_TXSTATQ_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x7A))	// Status Qualifier in the Sense IU for a TX Sense IU tx.
#define usb_Msc0_TXSTATQ_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x7B))	// Status Qualifier in the Sense IU for a TX Sense IU tx.


// MSCn TX Status Qualifier Register (_7Ch)
#define usb_Msc0_TSFSTATQ_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x7C))	// Status Qualifier in the Sense IU
#define usb_Msc0_TSFSTATQ_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x7D))	// for the automatic Hardware Task Set Full Sense IU tx.

// MSCn TX Status Qualifier Register (_7Eh)
#define usb_Msc0_BSYSTATQ_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x7E))	// Status Qualifier in the Sense IU 
#define usb_Msc0_BSYSTATQ_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x7F))	// for automatic SCSI Busy Sense IU tx.


#define usb_Msc0TaskLun_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x80))	// LSB of the LUN from the received Task Management IU.
#define usb_Msc0TaskLun_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x81))
#define usb_Msc0TaskLun_2		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x82))
#define usb_Msc0TaskLun_3		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x83))
#define usb_Msc0TaskLun_4		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x84))
#define usb_Msc0TaskLun_5		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x85))
#define usb_Msc0TaskLun_6		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x86))
#define usb_Msc0TaskLun_7		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x87))

#define usb_Msc0_TxAvail_Stat_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x88))
//#define usb_Msc0_TxAvail_Stat_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x89))
//#define usb_Msc0_TxAvail_Stat_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x8A))
//#define usb_Msc0_TxAvail_Stat_3	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x8B))

#define usb_Msc0RxAvail_status	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x89))

//#define usb_Msc0BOTFlag		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x90))
//	#define MSC_PHASE_ERR			B_BIT_6
//	#define MSC_SAT_ERR				B_BIT_5
//	#define MSC_LENGTH_ERR			B_BIT_4
//	#define MSC_SIZE_ERR			B_BIT_3
//	#define MSC_SIG_ERR				B_BIT_2
//	#define MSC_LUN_ERR				B_BIT_1
	
//#define usb_Msc0SatFail			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x91))
//	#define MSC_UNKNOWN_CDB			B_BIT_5
//	#define MSC_ALIGN_ERR_END		B_BIT_4
//	#define MSC_ALIGN_ERR_START		B_BIT_3
//	#define MSC_LBA_OVRN			B_BIT_2
//	#define MSC_LBA_ERR				B_BIT_1
//	#define MSC_SECCNT_ERR			B_BIT_0

//#define usb_Msc0BOTSATACtrl		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x92))
			
//#define usb_Msc0BOTDMACtrl		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x93))
//	#define MSC_AUTO_FIS_EN			B_BIT_2
//	#define MSC_CBW_RESET_EN		B_BIT_1
//	#define MSC_BOT_START_EN		B_BIT_0

//#define usb_Msc0PhaseCase_0		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x94))
//#define usb_Msc0PhaseCase_1		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x95))

//#define usb_Msc0BOTCBW[31]	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0A0))
//#define usb_Msc0CBW			((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0xA0))
//#define usb_Msc0CBWLength		((u32 volatile xdata *)(USB_MSC_BASE_ADDR + 0xA8))
//#define usb_Msc0CBWFlag		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0xAC))
//#define usb_Msc0CBWCDB		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0xAF))

//#define usb_Msc0BOTFIS[16]		((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0xC0))
//extern u8 volatile xdata usb_Msc0BOTFIS[];


#define usb_Msc_FISDmaCtrl_0	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0D0))
#define usb_Msc_FISDmaCtrl_1	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0D1))
	#define MSC_FIS_BUSY			B_BIT_15
#define usb_Msc_FISDmaCtrl_2	((u8 volatile xdata *)(USB_MSC_BASE_ADDR + 0x0D2))
	#define MSC_FIS_ENABLE			B_BIT_16

#ifndef ASIC_CONF
	#define usb_MSC0_Proto_Retry_LA_option_0	((u8 volatile *)(USB_MSC_BASE_ADDR + 0xEC))	
	#define usb_MSC0_Proto_Retry_LA_option_1	((u8 volatile *)(USB_MSC_BASE_ADDR + 0xED))	
	#define usb_MSC0_Proto_Retry_LA_option_2	((u8 volatile *)(USB_MSC_BASE_ADDR + 0xEE))	
	#define usb_MSC0_Proto_Retry_LA_option_3	((u8 volatile *)(USB_MSC_BASE_ADDR + 0xEF))	
#endif

// USB Interrupt In Buffer
#define usb_IntIn				((u8 volatile xdata *)(USB_BASE_ADDR + 0x300))

#define usb_CtrlBuffer			((u8 volatile xdata *)(USB_BASE_ADDR + 0x400))




/****************************************************************************/
/********                                                            ********/
/********        SATA Registers:			                         ********/
/********                                                            ********/
/****************************************************************************/

#define	sata_IntStatus_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x000)) 
	#define RXDATARLSDONE		B_BIT_7		// data reception protocol has completed and all data of this context has been released 
												// and flushed to the ASIC Data Buffer or discarded 
												// and no data of this context is pending in the Receive Data FIFO (RXDATAFIFO).
	#define RXRLSDONE			B_BIT_6
	#define SITEINV				B_BIT_5
	#define CMDTXERR			B_BIT_4
	#define DATARXDONE			B_BIT_3		// Data reception is done
	#define DATATXDONE			B_BIT_2		// Data transmission is done
	#define FRAMEINTPEND		B_BIT_1
	#define PHYINTPEND			B_BIT_0

#define	sata_IntStatus_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x001)) 
	#define SATACCMLOADEDEN		B_BIT_09
	#define SATAERRCNTEN		B_BIT_08


#define sata_IntEn_0		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x002))
	#define RXDATARLSDONEEN		B_BIT_7
	#define RXRLSDONEEN			B_BIT_6
	#define SITEINVEN			B_BIT_5
	#define CMDTXERREN			B_BIT_4
	#define DATARXDONEEN		B_BIT_3
	#define DATATXDONEEN		B_BIT_2
	#define FRAMEINTPENDEN		B_BIT_1
	#define PHYINTPENDEN		B_BIT_0

#define sata_IntEn_1		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x003))
	#define SATACCMLOADED		B_BIT_09
	#define SATAERRCNT			B_BIT_08


#define	sata_IpVersion		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x004)) 

#define	sata_RxFISCtrl		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x005))
	#define RXFISFIFORST		0x80
	#define RXFISFIFOBB			0x01
 
#define	sata_DataFIFOCtrl	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x006))
	#define RXDATAFIFORST		0x80
	#define TXDATAFIFORST		0x40
	#define RXDATAFIFOBB		0x01

#define	sata_HoldLv			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x007)) 

#define sata_PhyInt			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x008))
	#define PHYDWNI				B_BIT_7
	#define OOBERRI				B_BIT_6
	#define DEVDETI				B_BIT_5
	#define COMINITI			B_BIT_4
	#define PHYRDYI				B_BIT_3
	#define PMPRTLI				B_BIT_2
	#define PMSLMBRI			B_BIT_1
	#define COMRESETI			B_BIT_0

#define sata_PhyIntEn		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x00C)) 
	#define PHYDWNIEN			B_BIT_7
	#define OOBERRIEN			B_BIT_6
	#define DEVDETIEN			B_BIT_5
	#define COMINITIEN			B_BIT_4
	#define PHYRDYIEN			B_BIT_3
	#define PMPRTLIEN			B_BIT_2
	#define PMSLMBRIEN			B_BIT_1
	#define COMRESETIEN			B_BIT_0

#define sata_PhyCtrl_0		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x010))
	#define PHYGEN3				BIT_5		// 6.0 Gbps
	#define PHYGEN2				BIT_4		// 3.0 Gbps
	#define PHYGEN1				BIT_3		// 1.5 Gbps
	#define PHYPRTL				BIT_2		// causes the phy to transition to a partial state
	#define PHYSLMBR			BIT_1		// causes the phy to transition to a slumber state
	#define PHYRST				BIT_0		// start COMRESET sequence

#define sata_PhyCtrl_1		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x011))

#define sata_PhyCtrl_2		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x012))
	#define PHYPRTLDIS			B_BIT_18		// device partial PM transition is ignored
	#define PHYSLMBRDIS			B_BIT_17		// device slumber PM transition is ignored

#define sata_PhyCtrl_3		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x013))
	#define PHYPWRUP			B_BIT_31		// enables operation (power) to the phy



#define sata_PhyStat		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x014))
	#define SNGEN3				B_BIT_5
	#define SNGEN2				B_BIT_4
	#define SNGEN1				B_BIT_3
	#define PMPRTL				B_BIT_2
	#define PMSLMBR				B_BIT_1
	#define PHYRDY				B_BIT_0		// phy has completed OOB and speed negotiation and is ready to RX & TX data.


#define sata_OOBRty			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x016)) 

#define sata_OOBCnt			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x017)) 


// State Machine control registers
//	SMCTRL register (018h)
#define sata_SMCtrl_0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x018)) 				// State Machine control register
	#define SMSEL_MASK			0x7FFF

#define sata_SMCtrl_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x019)) 				// State Machine control register
	#define SMMOD				B_BIT_15

#define sata_SMSTAT_0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x01A)) 				// state machine status register
#define sata_SMSTAT_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x01B)) 				// state machine status register

#define	sata_PMPORTNUM			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x01C)) 
#define	sata_DEVCTRL			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x01D)) 

#define sata_DATAFISPORT		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x01E)) 

#define sata_CCMSITEINDEX		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x01F)) 


//////////////////////////////////////////////////////////////////////////////////////////
// Execution Queue register block
//	EXQCTRL register (020h)
#define	sata_EXQCtrl		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x020)) 				// execution queue control register
	#define EXQHSTRT			BIT_1			// starts operation of the high priority execution queue
	#define EXQNSTRT			BIT_0			// starts operation of the normal priority execution queue

//	EXQOPR register (021h)
#define	sata_EXQOPR			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x021))
	#define EXQHRST				BIT_7			// resets the high priority queue
	#define EXQNRST				BIT_6			// resets the normal priority queue
	#define CMDRTRY				BIT_0			// transmission of the command described by CMDXMTTAG register is retried

#define	sata_EXQNStat		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x022))

#define	sata_EXQHStat		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x023)) 
	#define EXQRUN				0x80
	#define EXQPEND_MASK		0x3F

#define	sata_EXQNINP		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x024)) 

#define	sata_EXQHINP		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x025)) 

#define	sata_CurrTag		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x028)) 

#define	sata_CmdXmtTag		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x029)) 

#define	sata_CTagXmtStat	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x02A))
	#define CMDSITEINV			0x40
	#define PHYEI				0x20
	#define LINKEI				0x10
	#define SYNCEI				0x04
	#define RERRI				0x02
 
#define	sata_RxTag			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x02B)) 

#define sata_CCMDIntEn_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x02C))     //Value of CMDINTEN of the current command being executed
#define sata_CCMDIntEn_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x02D))
#define sata_CCMDIntEn_2	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x02E))
#define sata_CCMDIntEn_3	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x02F))

#define sata_FrameInt_0		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x030))
	#define UNKNFISI			B_BIT_7
	#define DATAFISI			B_BIT_6
	#define SETDEVBITSI			B_BIT_5
	#define PIOSETUPI			B_BIT_4
	#define BISTACTI			B_BIT_3
	#define DMASETUPI			B_BIT_2
	#define DMAACTI				B_BIT_1
	#define D2HFISI				B_BIT_0

#define sata_FrameInt_1		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x031))
	#define UNEXPFISI			B_BIT_11
	#define VENDFISI			B_BIT_9
	#define NOTIFYI				B_BIT_8

//	FRAMEINTEN register (034h)
#define sata_FrameIntEn_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x034)) 
	#define UNKNFISIEN			B_BIT_7
	#define DATAFISIEN			B_BIT_6
	#define SETDEVBITSIEN		B_BIT_5
	#define PIOSETUPIEN			B_BIT_4
	#define BISTACTIEN			B_BIT_3
	#define DMASETUPIEN			B_BIT_2
	#define DMAACTIEN			B_BIT_1
	#define D2HFISIEN			B_BIT_0

#define sata_FrameIntEn_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x035)) 
	#define UNEXPFISIEN			B_BIT_11
	#define VENDFISIEN			B_BIT_9
	#define NOTIFYIEN			B_BIT_8

//	FPDMA_TAG register (038h)
#define	sata_FpdmaTag		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x038))

//	FPDMA_TAG_LIMIT register (039h)
#define	sata_FpdmaTagLimit	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x039))

//	SITE_CMDINTEN register (03Ch)
#define	sata_SiteCmdIntEn_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x03C))
#define	sata_SiteCmdIntEn_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x03D))
#define	sata_SiteCmdIntEn_2	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x03E))
#define	sata_SiteCmdIntEn_3	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x03F))

//	DATATXSTAT register (040h)
#define sata_DataTxStat_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x040))
	#define DTXUNDRN			B_BIT_7			//The device transmitted less data than we asked for
	#define DTXOVRN				B_BIT_6			//device requested more data than available
	#define DTXPHYE				B_BIT_5			// Data transmission completed with Phy error
	#define DTXLINKE			B_BIT_4			// Data transmission completed with link protocol error
	#define DTXSYNCETX			B_BIT_3			// Data transmission completed with transmission of SYNC Escape.
	#define DTXSYNCERX			B_BIT_2			// Data transmission completed with reception of SYNC Escape
	#define DTXRERR				B_BIT_1			// Data transmission completed with R_ERRp

#define sata_DataTxStat_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x041))
	#define DTXSDONE			B_BIT_9
	#define DTXPROTE			B_BIT_8			// Protocol error.

//	DATARXSTAT register (042h)
#define sata_DataRxStat_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x042)) 
	#define DRXUNDRN			B_BIT_7
	#define DRXOVRN				B_BIT_6
	#define DRXPHYE				B_BIT_5
	#define DRXLINKE			B_BIT_4
	#define DRXSYNCETX			B_BIT_3
	#define DRXSYNCERX			B_BIT_2
	#define DRXRERR				B_BIT_1

#define sata_DataRxStat_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x043)) 
	#define DRXDDONE			B_BIT_10
	#define DRXSDONE			B_BIT_9
	#define DRXPROTE			B_BIT_8

#define sata_PCnt_0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x044)) 
#define sata_PCnt_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x045)) 
#define sata_PCnt_2			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x046)) 
#define sata_PCnt_3			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x047)) 

#define sata_TCnt_0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x048)) 
#define sata_TCnt_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x049)) 
#define sata_TCnt_2			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x04A)) 
#define sata_TCnt_3			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x04B)) 

#define sata_XCnt_0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x04C)) 
#define sata_XCnt_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x04D)) 
#define sata_XCnt_2			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x04E)) 
#define sata_XCnt_3			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x04F)) 

//	SATA_BUILDOPTS register (05Ch)
#define sata_BuildOpts_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x05C)) 
#define sata_BuildOpts_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x05D)) 

// TXDATAFIFOSIZE register (060h)
#define sata_TxDataFifoSize_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x060))
#define sata_TxDataFifoSize_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x061))

// RXDATAFIFOSIZE register (064)
#define sata_RxDataFifoSize_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x064))
#define sata_RxDataFifoSize_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x065))

// Free Tag Queue register block
//	FREE_TAG_CNT register (068h)
#define sata_PacketLba_0		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x068))
#define sata_PacketLba_1		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x069))
#define sata_PacketLba_2		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x06A))
#define sata_PacketLba_3		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x06B))

#define sata_PacketFeature		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x06C))

#define sata_UNKNFISCnt			((u16 volatile xdata *)(SATA_BASE_ADDR + 0x07C)) 

#define sata_CRCErrCnt_0		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x080)) 
#define sata_CRCErrCnt_1		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x081)) 

#define sata_DECErrCnt			((u16 volatile xdata *)(SATA_BASE_ADDR + 0x084)) 

#define sata_DSPErrCnt			((u16 volatile xdata *)(SATA_BASE_ADDR + 0x088)) 
	
// Shadow ATA Register Block
#define	sata_Status				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x08C)) 
#define	sata_Error				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x08D)) 
#define	sata_EStatus			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x08E)) 
#define	sata_Port				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x08F)) 

#define	sata_LbaL				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x090)) 
#define	sata_LbaM				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x091)) 
#define	sata_LbaH				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x092)) 
#define	sata_Device				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x093)) 

#define	sata_LbaLH				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x094)) 
#define	sata_LbaMH				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x095)) 
#define	sata_LbaHH				((u8 volatile xdata *)(SATA_BASE_ADDR + 0x096)) 

#define	sata_SectCnt			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x098)) 
#define	sata_SectCntH			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x099)) 

//	BISTCTRL register (0AFh)
#define	sata_BistCtrl			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0AF)) 

// Status and Control Registers
#define sata_SActive_0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0BC)) 
#define sata_SActive_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0BD)) 
#define sata_SActive_2			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0BE)) 
#define sata_SActive_3			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0BF)) 

#define sata_SNotification_0	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0C0)) 
#define sata_SNotification_1	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0C1)) 
#define sata_SNotification_2	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0C2)) 
#define sata_SNotification_3	((u8 volatile xdata *)(SATA_BASE_ADDR + 0x0C3)) 


// FISRCV Received FIS register block
#define	sata_FISRCV0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x100)) 
#define	sata_FISRCV1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x120)) 
#define	sata_FISRCV2			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x140)) 

#define sata_RxFFCtrl			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x160)) 
	#define RXFISOUTP_MASK		0x0C
	#define RXFISINP_MASK		0x03

#define	sata_BlkCtrl_0			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x164)) 
	#define SIMPLE_FPDMA		B_BIT_7		// (RW) 
	#define TXSYNCFIFOAUTO		B_BIT_6		// (RW) 
	#define RXSYNCFIFOAUTO		B_BIT_5		// (RW) 
	#define CCMREAD				B_BIT_4		// (RW) 
	#define CONTDIS				B_BIT_3		// (RW) 
	#define GCRCDSBL			B_BIT_2		// (RW) 
	#define CRCDSBL				B_BIT_1		// (RW) 
	#define DATALOOP			B_BIT_0		// (RW) 

#define	sata_BlkCtrl_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x165)) 
	#define TXSYNCFIFORST		B_BIT_14		// (W) 
	#define RXSYNCFIFORST		B_BIT_13		// (W) 
	#define PROTBLKRST			B_BIT_12		// (W) resets the Protocol layer
	#define TSPTBLKRST			B_BIT_11		// (W) resets the Link and Transport layers
	#define PHYBLKRST			B_BIT_10		// (W) resets the Phy block
	#define SBLKRESET			B_BIT_9			// (W) master reset for SATA block
	#define SACTRESET			B_BIT_8			// (W) resets the SActive register

#define	sata_BlkCtrl_2			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x166)) 
	#define AUTO_COMMAND_EXEC	B_BIT_18		// (RW) 
	#define DTXDDONEDIS			B_BIT_17		// (RW) 
	#define DRXSDONEDIS			B_BIT_16		// (RW) 

#define	sata_Rsvd168			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x168))

//////////////////////////////////////////////////////////////////////////////////////////////////
#define sata_Ccm_prot			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x180)) 
#define sata_Ccm_prot_1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x181)) 
//#define sata_Ccm_site			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x182)) 
//#define sata_Ccm_prot_3			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x803)) 
//#define sata_Ccm_valid			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x183))
	#define CCM_V		0x80			// indicates that the contents of this command site is valid.

#define sata_Ccm_cmdinten_0		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x184)) 
#define sata_Ccm_cmdinten_1		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x185)) 
//#define sata_Ccm_SegIndex		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x186)) 
//#define sata_Ccm_SegINOUT		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x187)) 

#define sata_Ccm_xfercnt_0		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x188)) 
#define sata_Ccm_xfercnt_1		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x189)) 
#define sata_Ccm_xfercnt_2		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x18A)) 
#define sata_Ccm_xfercnt_3		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x18B)) 

//#define sata_Ccm_Reserved12		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x18C))
//#define sata_Ccm_Status			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x18C)) 
	#define	CCM_STATUS_GOOD			0
	#define	CCM_STATUS_ERROR		1
	#define	CCM_STATUS_PEND			0xff

#define sata_Ccm_Reserved13		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x18D)) 
#define sata_Ccm_Reserved14		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x18E)) 
#define sata_Ccm_Reserved15		((u8 volatile xdata *)(SATA_BASE_ADDR + 0x18F)) 

#define sata_Ccm_FIS			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x190)) 
#define sata_Ccm_FIS1			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x191)) 
#define sata_Ccm_FIS2			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x192)) 
#define sata_Ccm_FIS3			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x193)) 
#define sata_Ccm_FIS4			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x194)) 
#define sata_Ccm_FIS5			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x195)) 
#define sata_Ccm_FIS6			((u8 volatile xdata *)(SATA_BASE_ADDR + 0x197)) 

//#define sata_Ccm[32]			((SATA_CCM volatile xdata *)(SATA_BASE_ADDR + 0x800)) 
//extern SATA_CCM volatile xdata sata_Ccm[];

#endif    // INI3609_H
