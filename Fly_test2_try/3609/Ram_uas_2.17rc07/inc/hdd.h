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
 * 3609		2011/06/09	WH		Initial version
 *
 *****************************************************************************/

#ifndef HDD_H
#define HDD_H

#undef  Ex
#ifdef HDD_C
	#define Ex
#else
	#define Ex extern
#endif

#ifndef SUPPORT_3T_4K
Ex u8 xdata rCapLimit;
#endif
#ifdef STANDBY_TIMER
	Ex bit  data hdd_post_write;
	Ex bit  data hdd_lun_ctrl;
	Ex bit  data hdd_tick;
	Ex bit  data hdd_standby_enabled;
	Ex u32	data hdd_standby_tick;
	Ex u32	xdata hdd_Standby_Timer;
#endif

//Ex u8	xdata	hdd_tst_unit_cnt;

//Ex u8	data	hdd_standby_counter;

// For Activity LED
//Ex u8 	data cmdPending;
Ex bit  data led_on;
Ex u8 	data led_state;

	#define LED_ON			0
	#define LED_ACTIVITY	1
	#define LED_OFF			2

Ex u8 	data led_interval;
	#define ACTIVITY_ON_TIME_USB3		1
	#define ACTIVITY_ON_TIME	 		4
	#define ACTIVITY_OFF_TIME_USB3		2
	#define ACTIVITY_OFF_TIME			6
//	#define FAULT_ON_TIME			1
//	#define FAULT_OFF_TIME			2

Ex u8	data led_activity_repeat;	
	#define ACTIVITY_REPEAT_USB3		7
	#define ACTIVITY_REPEAT				2

//#ifdef USB_FAST_ENUM
	Ex u8 idata hdd_que_ctxt_site;
	Ex u8 idata hdd_que_ctxt_tout;
		#define HDD_UAS_QUE_TOUT	180	//18 sec
		#define HDD_QUE_TOUT		85	//8.5 sec
		#define HDD_QUE_SHORT_TOUT	45	//4.5 sec
		//#define HDD_QUE_LONG_TOUT	200	//20 sec
//#endif

#ifdef UAS_EN
	Ex u8 idata hdd_que_dout_ctxt;
	Ex u8 idata hdd_que_dout_tout;
#endif

#ifdef SCSI_DOWNLOAD_FW
Ex u16 data flash_addr_low;
Ex u8 xdata FW_Download;
#define FW_DOWNLOAD_INITIALIZING	0
#define FW_DOWNLOAD_PROECESSING		1
#define FW_DOWNLOAD_FAILURE			2
#define FW_DOWNLOAD_SUCCESS			3

Ex u8	xdata	swap_mode;
		#define SMODE_NORMAL		1
		#define SMODE_ENTER_CFA	2
		#define SMODE_CFA			3
		#define SMODE_LEAVE_CFA	4

extern bit hdd_odd_scsi_downlod_fw(void);
#endif

extern void hdd_return_descriptor_sense(u8 *buffer);

extern void hdd_ata_err_2_sense_data();
extern void hdd_ata_return_tfr();
extern void	hdd_err_2_sense_data(u8 error);


extern void	hdd_inquiry_cmd();
extern void hdd_rd_buffer_cmd();
extern void hdd_request_sense_cmd();
extern void hdd_report_lun_cmd();

//extern void Ata_SetupErrorBlock(u8 error);
extern void hdd_StartAtaNoMediaCmd();

#if 1
	extern bit hdd_chk_drv_init();
#endif

extern bit hdd_chk_drv_state();

extern void hdd_StartAtaCmd();

void hdd_post_data_out();


extern void hdd_on_led();

extern void hdd_start_act_led();

extern void hdd_start_fault_led();

extern void hdd_standby_led();

extern void hdd_led_going_off();

void hdd_exec_bot_RW_ctxt();


#ifdef STANDBY_TIMER
	extern void hdd_calc_Idle_timer(U8 time);
	extern void hdd_tick_isr();
#endif

#endif     // HDD_H
