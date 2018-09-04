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
 * 3607		2010/11/20	Odin		AES function
 * 3609		2011/03/29	Ted			Overlap SEG0/SEG1
 *
 *****************************************************************************/
#include "general.h"

/********************************************************************\
	dbuf_get_data()
		Get up to 512 byte to from DBUF to mc_buffer thru CPU_R Port

\********************************************************************/
void dbuf_get_data(u8 segment_num)
{
	
	*dbuf_MuxSel = segment_num;
	*dbuf_MuxInOut = (*dbuf_MuxInOut & 0x0F) | (TX_DBUF_CPU_R_PORT << 4) ;

	*dbuf_MuxCtrl = SEG_DONE;
	*dbuf_MuxCtrl;

	sz16 = (dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_0) + (((u16)dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_1) << 8);
	DBG(("dbuf_get_data %x\n", sz16));
	if (sz16 > MC_BUFFER_SIZE)
		sz16 = MC_BUFFER_SIZE;


	pU8 = mc_buffer;
 	for (; sz16 != 0; sz16--)
	{
		*pU8 = *dbuf_DataPort;	
		pU8++;
 	}

	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxCtrl;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;

}
#if 0
/********************************************************************\
	dbuf_write_data()
		Write 512 byte to from mc_buffer to Seg 2 of DBUF thru CPU W Port

\********************************************************************/
void dbuf_write_data()
{

	*dbuf_MuxSel = DBUF_SEG_S2C;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_CPU_W_PORT;

	pU8 = mc_buffer;
 	for (sz16 = 512; sz16 != 0; sz16--)
	{
		*dbuf_DataPort = *pU8;	
		pU8++;
 	}


}
#endif
#ifdef UAS_EN
u8 dbuf_GetIdleSeg()
{

	// check seg 0
	*dbuf_MuxSel = 0;
	if (*dbuf_MuxStatus == SEG_IDLE)
	{
		*dbuf_MuxStatus = SEG_BUSY;
		return 0;
	}

	// check seg 1
	*dbuf_MuxSel = 1;
	if (*dbuf_MuxStatus == SEG_IDLE)
	{
		*dbuf_MuxStatus = SEG_BUSY;
		return 1;
	}
	return DBUF_SEG_NULL;
}
#endif
void DBUF_SegFree(u8 segIndex)
{
	*dbuf_MuxSel = segIndex;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxStatus = SEG_IDLE;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxInOut;

}
//#endif

void dbuf_Reset()
{
	// seg 0
	*dbuf_MuxSel = 0;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxStatus = SEG_IDLE;
	//*dbuf_MuxStatus;

	// seg 1
	*dbuf_MuxSel = 1;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxStatus = SEG_IDLE;
	//*dbuf_MuxStatus;

	// seg 2
	*dbuf_MuxSel = 2;
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxCtrl = SEG_RESET;
	*dbuf_MuxStatus = SEG_IDLE;
	//*dbuf_MuxStatus;

}
#ifdef UAS_EN

// SEG0/SEG1 4K each
void dbuf_uas_mode()
{
	//Set Seg 0
	*dbuf_MuxSel = 0;

	*dbuf_MuxAddr_0 = 0;
	*dbuf_MuxAddr_1 = 0;
	*dbuf_MuxAddr_2 = 0;
	*dbuf_MuxStatus = SEG_IDLE;

	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxOut1 = 0;
	*dbuf_MuxSize = DBUF_SEG0_UAS_SIZE;
	*dbuf_MuxCtrl = SEG_RESET;

	//Set Seg 1
	*dbuf_MuxSel = 1;
	
	*dbuf_MuxAddr_0 = 0;
	*dbuf_MuxAddr_1 = (DBUF_SEG0_UAS_SIZE*1024) >> 8;
	*dbuf_MuxAddr_2 = (DBUF_SEG0_UAS_SIZE*1024) >> 16;
	*dbuf_MuxStatus = SEG_IDLE;

	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxOut1 = 0;
	*dbuf_MuxSize = DBUF_SEG1_UAS_SIZE;
	*dbuf_MuxCtrl = SEG_RESET;

}
#endif



void dbuf_bot_mode()
{
	//Set Seg 0
	*dbuf_MuxSel = 0;
	
	*dbuf_MuxAddr_0 = 0;
	*dbuf_MuxAddr_1 = 0;
	*dbuf_MuxAddr_2 = 0;
	*dbuf_MuxStatus = SEG_IDLE;
	
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxOut1 = 0;
	*dbuf_MuxSize = DBUF_SEG0_SIZE;
	*dbuf_MuxCtrl = SEG_RESET;

	//Set Seg 1
	*dbuf_MuxSel = 1;
	
	*dbuf_MuxAddr_0 = 0;
	*dbuf_MuxAddr_1 = 0;
	*dbuf_MuxAddr_2 = 0;
	*dbuf_MuxStatus = SEG_IDLE;
	
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxOut1 = 0;
	*dbuf_MuxSize = DBUF_SEG1_SIZE;
	*dbuf_MuxCtrl = SEG_RESET;

}
// SEG0/SEG1 overlap 8K
void dbuf_init()
{
	//Set Seg 0
#if 1
	dbuf_bot_mode();
#else
	*dbuf_MuxSel = 0;
	
	*dbuf_MuxAddr_0 = 0;
	*dbuf_MuxAddr_1 = 0;
	*dbuf_MuxAddr_2 = 0;
	*dbuf_MuxStatus = SEG_IDLE;
	
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxOut1 = 0;
	*dbuf_MuxSize = DBUF_SEG0_SIZE;
	*dbuf_MuxCtrl = SEG_RESET;

	//Set Seg 1
	*dbuf_MuxSel = 1;
	
	*dbuf_MuxAddr_0 = 0;
	*dbuf_MuxAddr_1 = 0;
	*dbuf_MuxAddr_2 = 0;
	*dbuf_MuxStatus = SEG_IDLE;
	
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxOut1 = 0;
	*dbuf_MuxSize = DBUF_SEG1_SIZE;
	*dbuf_MuxCtrl = SEG_RESET;
#endif
	//Set Seg 2
	*dbuf_MuxSel = 2;
	
	*dbuf_MuxAddr_0 = 0;
	*dbuf_MuxAddr_1 = (DBUF_SEG0_SIZE*1024) >> 8;
	*dbuf_MuxAddr_2 = (DBUF_SEG0_SIZE*1024) >> 16;
	*dbuf_MuxStatus = SEG_IDLE;
	
	*dbuf_MuxInOut = (TX_DBUF_NULL << 4) | TX_DBUF_NULL;
	*dbuf_MuxOut1 = 0;
	*dbuf_MuxSize = DBUF_SEG2_SIZE;
	*dbuf_MuxCtrl = SEG_RESET;
}


