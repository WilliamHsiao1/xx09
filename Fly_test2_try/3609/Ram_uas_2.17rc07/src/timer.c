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
 * 3609		2010/12/21	Ted		Initial version
 *
 *****************************************************************************/
#define TIMER_C

#include "general.h"

//-----------------------------------------------------
//	xtimer_setup
//	TP = 100ms = (TICK+1)*(PERIOD +1)*32*CPU_CLK_PERIOD
//-----------------------------------------------------
void xtimer_setup()
{
//	xtimerCnt = 0;

	*cpu_XtimerCtrl = 0;

	// Falling Edge Trigger External Interrupt 1 
	IT1= 1;

	*cpu_XtimerTick = 42;
	
	*cpu_XtimerPeriod_0 = (6565) & 0xFF;
	*cpu_XtimerPeriod_1 = (6565) >> 8;
	
	*cpu_XtimerCtrl = XTIME_ENABLE;

#ifdef BOT_TOUT
//	timerCnt = 0;
//	timerExpire = 0;
#endif
	EX1 = 1;

}

/*****************************************************************************************
 * timer0_setup: 
 *****************************************************************************************/
#if 0
void timer0_hook()
{
	timerCnt = 0;
	timerExpire = 0;
	
	TH0 = TH0_VALUE;
	TL0 = TL0_VALUE;

	TF0 = 0;	//claer timer flag

	TR0 = 1;	//enable timer 0
	ET0 = 1;	//enable timer0 interrupt
	
}

void timer0_unload()
{
	TR0 = 0;	//disable timer 0
	ET0 = 0;	//disable timer0 interrupt	
}
#endif
//----------------------------------------------------------------------
// interrupt 每 N ms 發一次
// T = (1/CLK) * 12 * Count = N ms
// Count = N*1000 * CLK(M) / 12
// ex: 50M, 10ms 一次interrupt
// Count = 10000 * 50 / 12 = 41666
// 65536 - 41666 = 	0x5D3E --> TH0 = 0x5D, TL0 = 0x3E
//----------------------------------------------------------------------
void timer0_setup()
{
	DBG(("Timer 0 setup: "));

	TR0 = 0;	//disable timer 0
		

	//set TMOD = 0x?1
	TMOD = (TMOD & 0xF0) | 0x01;
	
	//PT0 = 1;	//set timer0 priority

	TH0 = TH0_VALUE;
	TL0 = TL0_VALUE;
	DBG(("%BX, %BX\n", TH0, TL0));
	

	TF0 = 0;	//claer timer flag
	//timerExpire = 0;

	TR0 = 1;	//enable timer 0
	ET0 = 1;	//enable timer0 interrupt
}


