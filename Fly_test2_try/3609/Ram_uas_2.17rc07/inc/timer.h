/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2009-2013, All Rights Reserved
 *
 *   This file contains confidential and propietary information
 *   which is the property of Initio Corporation.
 *
 *						Header of All BOT Project
 *
 *****************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#undef  Ex
#ifdef TIMER_C
	#define Ex
#else
	#define Ex extern
#endif

//----------------------------------------------------------------------
// interrupt 每 N us 發一次
// T = (1/CLK) * 12 * Count = N us
// Count = N * CLK(M) / 12
//--------------------------------------------------------------------
#ifdef DEBUG_LTSSM
	//T=100us, CLK=88.235294M
	// Count = 100 * 88.235294 / 12 = 735
	// 65536 - 735 = 	0xFD21 --> TH0 = 0xFD, TL0 = 0x21
	#define	TH0_VALUE  0xFD
	#define	TL0_VALUE  0x21
#else
	//T=1ms, CLK=88.235294M
	// Count = 1000 * 88.235294 / 12 = 7352
	// 65536 - 7352 = 	0xe348 --> TH0 = 0xE3, TL0 = 0x48
	#define	TH0_VALUE  0xE3
	#define	TL0_VALUE  0x48
#endif

#ifdef BOT_TOUT
	Ex bit data timerExpire;
	Ex u8 data timerCnt;
#endif

//Ex u16 volatile xtimerCnt;

void timer0_setup();
//void timer0_hook();
//void timer0_unload();

void xtimer_setup();


#endif //TIMER_H