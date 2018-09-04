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

#ifndef PRINT_H
#define PRINT_H

#ifdef UART_RX
	//Ex volatile TI_PASS;
	Ex bit data  TI_PASS;

	Ex u8 volatile uart_rx_buf[12];
	Ex u8 volatile uartRxPos;
	void uart_rx_parse();
#endif

#ifdef DBG_FUNCTION
	#define DBG(_x_)		dbgprintf _x_
	#define DBG0(_x_)		dbgprintf _x_
	#define MSG(_x_)		dbgprintf _x_
	#define ERR(_x_)		dbgprintf _x_

	void UART_CH(u8);
	void UART_STR(u8 *s);
	void pv8(u8 value);
	void uart_init();
	void dbgprintf(const char* szFormat, ...);
	void dump_reg();
	void dump_phy();
	void mem_dump(u8 volatile xdata * buf, u8 size);
	void setup_dump();
#else
	#define DBG(_x_)		
	#define DBG0(_x_)		
	#define MSG(_x_)		
	#define ERR(_x_)		

	#define UART_CH(_x_)
	#define UART_STR(_x_)
	#define pv8(_X_)
	#define dump_reg()
	
	#define dump_phy()
	#define mem_dump()
	#define setup_dump()
#endif
#endif //PRINT_H