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

#ifndef UAS_H
#define UAS_H

#undef  Ex
#ifdef UAS_C
	#define Ex
#else
	#define Ex extern
#endif

#ifdef UAS_EN

//IU identify
#define	Command_IU			0x01
#define	Sense_IU			0x03
#define Response_IU      	0x04
#define TaskManagement_IU	0x05
#define ReadReady_IU		0x06
#define WriteReady_IU		0x07

//Task attribute code
#define   Task_Simple		0x00
#define   Task_HeadOfQueue	0x01
#define   Task_Ordered		0x02
#define   Task_ACA			0x04

#define MAX_UAS_TAG_ID	0x1F

#if 0
typedef struct
{
	u8 CMD_Type;
	u8 Reserve0;
	u16 Initiator_Tag ;
	u16 Length;
	
	u8 Task_Info;
	
	u8 Reserve1;

	u32 Logical_Unit_0;
	u32 Logical_Unit_1;

	u8 CDB[32];

} UAS_CMD_IU;
#endif

#define TASK_ATTRIBUTE    0x07
#define TASK_PRIORITY       0x78

#if 0
typedef struct
{
	u8 CMD_Type;
	u8 Reserve0;
	u16 Initiator_Tag ;
	u16 Length;
	
	u8 Status;
	
	u8 Reserve1;

	u8 Sense_Data[20];

} UAS_SENSE_IU;
#endif

//Task management response code
#define   TaskManageComplete    		0x00
#define   InvalidInformationUnit 		0x02
#define   TaskManageNotSupported     	0x04
#define   TaskManageFunctionFailed		0x05
#define   TaskManageFunctionSucceed		0x08
#define	  IncorrectLUN					0x09
#define	  OverlappedTagAttempted		0x0A

typedef struct
{
	u8 CMD_Type;
	u8 Reserve0;
	u16 Initiator_Tag ;
	u8 Function;
	u8 Reserve1;
	u16 Task_Tag;
	u16 Logical_Unit;
} UAS_TASK_Manager_IU;

#define  ABORT_TASK   		0x01
#define  ABORT_TASK_SET    	0x02
#define  CLEAR_TASK_SET        0x04
#define  LOGICAL_UNIT_RESET  0x08
#define  I_T_NEXUS_RESET       0x10
#define  CLEAR_ACA                  0x40
#define  QUERY_TASK                0x80
#define  QUERY_TASK_SET        0x81
#define  QUERY_ASYC_EVENT    0x82

typedef struct
{
	u8 CMD_Type;
	u8 Reserve0;
	u16 Initiator_Tag ;
	u16 Additional_Info;
	u8 Response_Code;

} UAS_RESPONSE_IU;

typedef struct
{
	u8 CMD_Type;
	u8 Reserve0;
	u16 Initiator_Tag ;

} UAS_Read_Ready_IU;

typedef struct
{
	u8 CMD_Type;
	u8 Reserve0;
	u16 Initiator_Tag ;

} UAS_Write_Ready_IU;

#if 0
	Ex u8	xdata intel_SeqNum_monitor;
		#define INTEL_SEQNUM_START_MONITOR		B_BIT_0
		#define INTEL_SEQNUM_CHECK_CONDITION	B_BIT_1
		#define INTEL_SEQNUM_CLEAR_FEATURE_HALT	B_BIT_2
	Ex u8	xdata intel_SeqNum_Monitor_Count;
	Ex bit	data intel_host_flag;
#endif

Ex bit data uas_ci_paused;
Ex u8 data uas_ci_paused_ctxt;
//Ex u8 data uas_task_tag;

Ex u8 data uas_tur_tsf_cnt;

Ex	u8  tiu_task_tag0;
Ex	u8	tiu_initiator_Tag;
Ex	u8	tiu_func;
Ex	u8	tiu_flag;
#ifdef UAS_DI_TOUT
	Ex u8 idata uas_xfer_to_cnt;
#endif

#ifdef DEBUG_DP
	//Ex u8 idata uas_timeout;
#endif

Ex u8	data	uas_st_tout_cnt;

#ifdef UAS_OVERLAPPED_TAG
	Ex U32_DATA	data	uas_cmd_tag;
	//Ex U32_DATA	idata	uas_func_tag;
#endif
	Ex u8		idata	uas_riu_Tag;

Ex u8 xdata uas_senseBuffer[24];

#ifdef DEBUG_DP
	//Ex u8 data uas_dead_time_out;
#endif

void uas_init();

void uas_device_no_data();
void uas_device_data_in();
void uas_device_data_out();

void uas_bus_reset();

extern void uas_push_ctxt_site_to_free_fifo();

extern bit uas_abort_ctxt(u8 ctxt);

extern bit uas_abort_ctxt_w_status(u8 ctxt);

extern void uas_abort_all();

extern void UAS_Task_Manage_IU();

#if 1
	extern void uas_send_sense_iu();
#else
	extern void uas_din_send_sense_iu();
	extern void uas_dout_send_sense_iu();
#endif

extern bit uas_chk_TSFull();

extern void usb_uas();

#endif	// #ifdef UAS_EN

#endif	// #ifndef UAS_H

