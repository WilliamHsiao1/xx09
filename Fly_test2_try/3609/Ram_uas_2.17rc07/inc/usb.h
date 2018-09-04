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
 *
 *****************************************************************************/

#ifndef USB_H
#define USB_H

//------------------------
#undef  Ex
#ifdef USB_C
	#define Ex
#else
	#define Ex extern
#endif

// USB Standard Device Request 
#define   BMREQTYPE     0x00
#define     REQTYPE_DIR(setup) ((((setup)[BMREQTYPE]) & 0x80))
#define       REQTYPE_H2D			(0x00 << 7)
#define       REQTYPE_D2H			(0x01 << 7)
#define     REQTYPE_TYPE(setup) ((((setup)[BMREQTYPE]) & 0x60))
#define       REQTYPE_STD			(0x00 << 5)
#define       REQTYPE_CLASS			(0x01 << 5)
#define       REQTYPE_VDR			(0x02 << 5)
#define     REQTYPE_RECIP(setup) (((setup)[BMREQTYPE]) & 0x1F)
#define       REQTYPE_DEV			0x00
#define       REQTYPE_INF			0x01
#define       REQTYPE_EP			0x02
#define   BREQ          0x01
#define     BREQ_GET_STAT			0x00
#define     BREQ_CLR_FEATURE		0x01
#define     BREQ_SET_FEATURE		0x03
#define     BREQ_SET_ADDR			0x05
#define     BREQ_GET_DESC			0x06
#define     BREQ_SET_DESC			0x07
#define     BREQ_GET_CFG			0x08
#define     BREQ_SET_CFG			0x09
#define     BREQ_GET_INTERFACE		0x0A
#define     BREQ_SET_INTERFACE		0x0B
#define     BREQ_SYN_FRAME			0x0C
// class-Specific Requests
#define     BREQ_SET_REPORT			0x09
#define     BREQ_SET_IDLE			0x0A
#define     BREQ_SET_ISOCHRONOUS_DELAY    0x31
#define     BREQ_GET_MAX_LUN		0xFE
#define   WVALUE        0x02
#define   WINDEX        0x04
#define   WLENGTH       0x06

// Vendor Request
//#define     GET_MS_DESCRIPTOR			0x01
#define			MS_OS_STRING_INDEX				0xEE
#define     GET_MS_VS_DESCRIPTOR		0x04
#define			DESCTYPE_MS_EXT_COMPAT_ID		0x04
#define			DESCTYPE_MS_EXT_PROPERTIES		0x05
//--------------------------
// USB Descriptors Type 
//--------------------------
#define   DESCTYPE_DEVICE			0x01
#define   DESCTYPE_CFG				0x02
#define   DESCTYPE_STR				0x03
#define   DESCTYPE_INF				0x04
#define   DESCTYPE_ENDPOINT			0x05
#define   DESCTYPE_DEV_QUAL			0x06			// USB 2.0 only
#define   DESCTYPE_OTHER_SPD_CFG	0x07			// USB 2.0 only
#define   DESCTYPE_INTF_PWR			0x08
#define   DESCTYPE_BOSS				0x0F			// USB 3.0 only
#define   DESCTYPE_DEV_CAP			0x10			// USB 3.0 only
#define   DESCTYPE_ENDPOINT_COMP	0x30			// USB 3.0 only

#define   FEAT_SEL_EP_HALT      0x00
#define   FEAT_SEL_DEV_RM_WAKE  0x01
#define   FEAT_SEL_TEST_MODE    0x02

#define		ENDPOINT_HALT		00
#define		FUNCTION_SUSPEND	00
#define		FEAT_U1_ENABLE		48
#define		FEAT_U2_ENABLE		49
#define		LTM_ENABLE			50
#define		B3_NTF_HOST_REL2	51

#define   TEST_J            0x01
#define   TEST_K            0x02
#define   TEST_SE0_NAK      0x03
#define   TEST_PACKET       0x04
#define   TEST_FORCE_ENABLE 0x05

//------------------------
Ex u8 bot_sense_data[4];
//Ex u32 USB_Sense_Length;

//Ex bit data usb_dir;
//Ex u8 cswStatus;
//Ex u8 usbState;
Ex bit data usb_active;
Ex bit data usb_reenum_flag;

Ex bit data usb_self_power;

Ex bit data usb_BusPwr_FastEnum;

Ex u8 module_vendor_text[16];
Ex u8 module_product_text[32];

//------------------------
#if 0
typedef struct
{
	//u8 CfgUSB3[9];
	//u8 BOTInfUSB3[9];
	
	//u8 BotEndPointUSB3[2][7];
	//u8 BotSSEndPointComp[6];
		
	//u8 UasEndPointUSB3[4][7];
	//u8 UasSSEndPointComp[4][6];
	//u8 Pipe[4][4];



	//u8 BOTInfUSB[9];
	//u8 BOTEndPointUSB3[2][7];
	//u8 BOTSSEndPointComp[2][6];



} DESCBUF;
#endif

//Ex DESCBUF desc;

extern u8 xdata DeviceUSB2[18];

extern u8 xdata  CfgUSB2[9+9+7+7];
	//u8 InfUSB2[9];
	//u8 EndPointUSB2[2][7];

extern	u8 xdata DeviceUSB3[18];

extern	u8 xdata CfgUSB3[9+9+7+6+7+6];
	//u8 BOTInfUSB3[9];
	
	//u8 BotEndPointUSB3[2][7];
	//u8 BotSSEndPointComp[6];

	// 	USB Binary Object Stor Descriptor
extern	u8 xdata BOSS[5+7+10];
	//u8 USB2ExtendCapabilties[7];
	//u8 USB3Capabilties[10];

extern u8 xdata Str[4];

Ex u8 xdata Mfc[2 + 16 * 2];
Ex u8 xdata Product[2 + 32 * 2];
Ex u8 xdata Serial[2 + 16 * 4];

#define ICONFIG   0x00
#define IINF      0x00

#define IMFC      0x01
#define IPRODUCT  0x02
#define ISERIAL   0x03

#define DESCTYPE_HID    0x21
#define DESCTYPE_REPORT 0x22


/*
/////////////////////////////////////////////////////
typedef struct
{
	u8	tx_curCtxt;
	u8	tx_que;
} USB_TX_Context;

Ex USB_TX_Context usb_tx_ctxt;
*/


/////////////////////////////////////////////////////
/*
typedef struct
{
	u8	rx_curCtxt;
	u8	rx_que;
	u8			post_dout;
} USB_RX_Context;

Ex USB_RX_Context usb_rx_ctxt;
*/
Ex	u8 data usb_int_Stat_1;
Ex	u8 data msc_int_status_1;

Ex	u8 data usb_curCtxt;
Ex	u8 data usb_ctxt_que;
Ex	u8 data usb_post_dout_ctxt;

Ex bit data bot_mode;

Ex u8 data usbMode;
#define CONNECT_UNKNOWN		0     
#define CONNECT_USB1		1
#define CONNECT_USB3     3
#define CONNECT_USB2     2

Ex bit usb3_function_suspend;
Ex u16 idata usb3_U2_inactivity_timer;
Ex u8 data usb3_device_U2_state;
Ex bit usb3_u2_enable;
Ex u8 xdata usb_ltssm_state_0;

#ifdef U3_LOOPBACK
	Ex bit data u3_loopback_mode;
#endif

Ex bit data usb_power_on_flag;
#ifdef USB3_TX_DBG
		//Ex	bit	data	polling_failed_count;
		//Ex	bit data	usb_rst_tx_flag;
	Ex	bit data	count_recvry_en;
	Ex	u8	data	recovery_count;
#endif
#ifdef HW_CAL_SSC
Ex bit data start_HW_cal_SSC;
#endif
//Ex bit data pending_HW_cal_SSC;

#ifdef REJECT_U1U2
	Ex	bit	data u1U2_reject_state;
		#define	U1U2_REJECT	1 // when the external device is busy, reject the U1U2
		#define	U1U2_ACCEPT	0
	Ex	u8	idata U1U2_ctrl_timer;
		#define U1U2_REJECT_TIME 10	// 10 * 100ms = 1s
		#define U1U2_REJECT_TIME_12S	120	// 120 * 100ms = 12s
#endif
#ifdef USB2_L1_PATCH

Ex u8 idata usb2_L1_reject_timer;
#endif
Ex bit data rx_detect_count;
	#define RX_DETECT_COUNT8	0
	#define RX_DETECT_COUNT1	1

Ex u8 data  temp_USB3StateCtl;
Ex u8 data  cur_USB3StateCtl;
Ex u8 data  usb_inactive_count;
#if 1

Ex bit data	usb3_test_mode_enable;

Ex u8 data	usb3_ts1_timer;
Ex u8 data	usb3_enter_loopback_timer;


Ex u8 data	usb_mode_ctrl;
	#define USB3_MODE_ONLY			0x01
	#define USB23_MODE_ENABLE		0x00
	#define USB_MODE_CHANGED		0x02
#endif


//------------------------
extern void Text16(u8 xdata *src, u8 xdata *dest, u8 len);
extern void Hex2Text16(u8 xdata *src, u8 xdata *dest, u8 len);
extern void InitDesc(void);
extern void Ep0_2SendStatus(void);
extern void Ep0_2Send(u8 * ptr, u16 len);
extern void Ep0_2Send_direct(u16 len);
extern void usb_control();
extern void UsbExec_Init();

void usb_reenum();
extern void usb_rx_fifo_rst();
#ifdef UAS_EN
extern void usb_rst_do();
#endif
extern void usb_tx_ctxt_send_status();
extern void usb_exec_tx_ctxt();
//extern void usb_exec_tx_que_ctxt(u8 ctxt);



extern void usb_rx_ctxt_send_status();
extern void usb_exec_rx_ctxt();
#ifdef UAS_EN
extern void usb_exec_que_ctxt();
#endif
//extern void usb_append_rx_ctxt_que();
extern void usb_append_ctxt_que();


extern void usb_data_out_done();


extern void usb_device_no_data();
extern void usb_device_data_in();
extern void usb_device_data_out();

#ifdef USB3_TX_DBG
	void usb_rst_usb3_tx_FIFO(void);
#endif
#ifdef PWR_SAVING
	extern void usb_clock_disable(void);
	extern void	usb_clock_enable(void);
	extern void	usb20_clock_enable(void);
#endif

extern bit usb_suspend(bit fast_flag);

void usb_msc_isr();


void UsbExec_Init(void);

void usb_wrong_sata_device();

extern void Init_hardware_default(void);

#ifdef USB2_L1_PATCH
extern void usb_rej_L1_in_io_transfer(void);
#endif

extern  void usb_start();

#endif
