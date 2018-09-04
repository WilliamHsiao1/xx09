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
 * 3609		2010/12/21	Ted			Initial version
 *
 *****************************************************************************/
#include "general.h"

#include <stdarg.h>

#ifdef DBG_FUNCTION
void UART_CH(u8 c)
{
	bit data restoreEAL;
	
	restoreEAL = EAL;
	EAL = 0;
	if (c == '\n') {
		S0BUF = 0x0D;
		while(1)
		{
		#ifdef UART_RX
			if(( TI0 ==1) || (TI_PASS == 1))
			//if(TI_PASS)
			{
				if (TI0 == 1)
					TI0 = 0;

				TI_PASS = 0;
				break;
			}
		#else
			if(( TI0 ==1))
			{
				TI0 = 0;
				break;
			}
		#endif
		}
	}
	
	S0BUF = c;
	while(1)
	{
	#ifdef UART_RX
		if(( TI0 ==1) || (TI_PASS == 1))
		//if(TI_PASS)
		{
			if (TI0 == 1)
				TI0 = 0;

			TI_PASS = 0;
			break;
		}
	#else
		if (TI0 ==1)
		{
			TI0 = 0;
			break;
		}
	#endif
	}
	EAL = restoreEAL;
}
#endif

#ifdef DBG_FUNCTION
void UART_STR(u8 *s)
{
	while (*s != '\0')
	{
		UART_CH(*s);
		s++;
	}

}
#endif

/****************************************\
   HEX8CHAR
\****************************************/
#ifdef DBG_FUNCTION
void HEX8CHAR(u8 hex)
{
	u8	print_char	= (hex & 0xF0) >> 4;

	if (print_char > 0x9)
	{
		UART_CH(print_char - 0xA + 'A');
	} 
	else
	{
		UART_CH(print_char + '0');
	}	

	print_char	= hex & 0xF;
	if (print_char > 0x9)	
	{
		UART_CH(print_char - 0xA + 'A');
	}
	else
	{
		UART_CH(print_char + '0');
	}
}
#endif



/****************************************\
   new_pv8
\****************************************/
#ifdef DBG_FUNCTION
void pv8(u8 value)
{
	//UART_CH('0');
	//UART_CH('x');
	HEX8CHAR(value);
}
#endif

/****************************************\
   new_pv16
\****************************************/
#ifdef DBG_FUNCTION
void pv16(u16 value)
{
	u8	print8	= value >> 8;
	
	//UART_CH('0');
	//UART_CH('x');
	HEX8CHAR(print8);

	print8	= value;
	HEX8CHAR(value);

}
#endif

/****************************************\
   new_pv32
\****************************************/
//#ifdef DBG_FUNCTION
#ifdef PR32
void pv32(u32 value)
{
	u8	print8	= value >> 24;

	//UART_CH('0');
	//UART_CH('x');
	HEX8CHAR(print8);

	print8	= value >> 16;
	HEX8CHAR(print8);

	print8	= value >> 8;
	HEX8CHAR(print8);

	print8	= value;
	HEX8CHAR(value);

}
#endif

/*------------------------------------------------------------------------------
/   Function: dbgprintf()
/-----------------------------------------------------------------------------*/
#ifdef DBG_FUNCTION
void dbgprintf(const char *format, ...)
{
	char *tempPtr;
	//char *tempPtr2;
	//u8 len;
	u8 getSize;
	va_list argp;

	tempPtr = format;
	va_start(argp, format);
	while (*tempPtr != '\0')
	{
		//len = 0;
		switch (*tempPtr)
		{
			case '%':
				// set default to get a 16-bits value
				getSize = 2;
				tempPtr++;
REPARSER_TYPE:				
				switch (*tempPtr)
				{
					case '%':
						UART_CH('%');
						break;
					case 'x':
					case 'X':
//PROCESS_UPPER_X:
			#ifdef PR32
						if (getSize == 4)
							pv32(va_arg(argp, unsigned long));
			#endif
						if (getSize == 2)
							pv16(va_arg(argp, unsigned short));
						if (getSize == 1)
							pv8(va_arg(argp, unsigned char));
						break;
			#if 0
					case 'c':
					case 'C':
						UART_CH(va_arg(argp, unsigned char));
						break;
			#endif
					case 's':
					case 'S':
#if 1
						UART_STR(va_arg(argp, char *));
#else
						tempPtr2 = va_arg(argp, char *);
						while (*tempPtr2 != '\0')
						{
							UART_CH(*tempPtr2);
							tempPtr2++;
						}
#endif
						break;
					case 'b':
					case 'B':
						getSize = 1;
						tempPtr++;
						goto REPARSER_TYPE;
			#ifdef PR32
					case 'l':
					case 'L':
						getSize = 4;
						tempPtr++;
						goto REPARSER_TYPE;
			#endif

			#if 0
					case '0':
						tempPtr++;	// read next
						len = *tempPtr - 48;	// output length
						tempPtr++;
						goto REPARSER_TYPE;
			#endif

					default:
//UNKNOWN:
						UART_CH('?');
						UART_CH('?');
				}
				break;
			default:
				UART_CH(*tempPtr);
		}
		tempPtr++; // next
	}
	va_end(argp);
}
#endif

//#ifdef DBG_FUNCTION
#if 0
void dump_phy()
{
	MSG(("U Phy"));

	for (i8=0; i8 <= 0x0D; i8++)
	{
		if ((i8 & 0xF) == 0x0)
			MSG(("\n"));

		MSG(("%bx ", spi_phy_rd(PHY_SPI_U3PMA, i8)));
	}
	MSG(("\n"));

	for (i8=0; i8 <= 0x29; i8++)
	{
		if ((i8 & 0xF) == 0x0)
			MSG(("\n"));

		MSG(("%bx ", spi_phy_rd(PHY_SPI_U3PCS, i8)));
	}
	MSG(("\n"));

}

#endif


#ifdef DBG_FUNCTION
void mem_dump(u8 volatile xdata * buf, u8 size)
{
	pU8 = buf;
	for (i8=0; i8 < size; i8++)
	{
		if ((i8 & 0xF) == 0x0)
			MSG(("\n%BX: ", i8));

		MSG(("%BX ", *pU8++));
	}
	MSG(("\n"));
}

#endif

#ifdef DBG_FUNCTION
void setup_dump()
{
	for (i8=0; i8 < 8; i8++)
	{
		MSG(("%BX ", inPktSetup[i8]));
	}
	MSG(("\n"));
}

#endif

#ifdef DBG_FUNCTION
void uart_init()
{
	bit	restoreEAL = EAL;		// save EAL

	EAL = 0;

	//-----------------------------
	// set UART timer
	//-----------------------------
	ET1 = 0;	// Disable timer 1 interrupt
	TF1 = 0;	// Clear timer 1 overflow
	TR1 = 0;	// Stop timer 1

    TI0 = 0;     // clear transmitting flag
	RI0 = 0;     // clear receiving flag
    
    // set serial port 0 at mode 1.
#ifdef UART_RX
	TI_PASS = 0;

	uartRxPos = 0;
    SM0 = 0;
    SM1 = 1;
    SM20 = 0;
	REN0 = 1;    // enable UART0 receiver

	S0CON  = 0x50;		/* SCON: mode 1, 8-bit UART, enable rcvr */
#else
	S0CON  = 0x40;		/* SCON: mode 1, 8-bit UART*/
#endif

	PCON |= 0x80;		// SMOD


	BD = 1;

#ifdef ASIC_CONF
	if (cpu_ref_clk)
	{
		// S0RELH & S0RELL overflow buad rate generator
		#define  S0_REF_REL (1024 - ((REF_CLK / (32L * BAUD_RATE))+1))
		S0RELH = S0_REF_REL >> 8;
		S0RELL = (S0_REF_REL & 0xFF);
	}
	else
#endif
	{
		// S0RELH & S0RELL overflow buad rate generator
		#define  S0REL (1024 - (CPU_CLK / (32L * BAUD_RATE)))
		S0RELH = S0REL >> 8;
		S0RELL = (S0REL & 0xFF);
	}

	//S0BUF = 0x0D;

#ifdef UART_RX
//    PS = 0;     // set to low priority
    ES0 = 1;     // Enable serial port 0 interrupt
 
    //restore EAL

	EAL = 1;
#else

    ES0 = 0;     // Enable serial port 0 interrupt
	EAL = restoreEAL;
#endif

}
#endif

/****************************************\
   C2B
\****************************************/
#ifdef UART_RX
u8 C2B(u8 ch)
{
//	u8 v1, v2;
#if 0
	if ((ch >= '0') && (ch <= '9'))
		return (ch - '0');
		
	if ((ch >= 'A') && (ch <= 'F'))
		return (ch - 'A' + 10);
	
	return 0;
#else
	ch -= '0';
	if (ch > 9)
	{
		ch -= 7;
		if (ch > 15)
			ch = 0;
	}
	return ch;
#endif
}
#endif

/****************************************\
   StrToU32
\****************************************/
//u32 StrToU32(u8 ch[])
//{
//	return ( ((u32)C2B(ch[0]) << 28) | ((u32)C2B(ch[1]) << 24) | ((u32)C2B(ch[2]) << 20) | ((u32)C2B(ch[3]) << 16) |
//	((u32)C2B(ch[4]) << 12) | ((u32)C2B(ch[5]) << 8) | ((u32)C2B(ch[6]) << 4) | (u32)C2B(ch[7]));
//}


/****************************************\
   StrToU16
\****************************************/
#ifdef UART_RX
u16 StrToU16(u8 ch[])
{
	return (((u16)C2B(ch[0]) << 12) | ((u16)C2B(ch[1]) << 8) | ((u16)C2B(ch[2]) << 4) | (u16)C2B(ch[3]));
}
#endif


/****************************************\
   StrToU8
\****************************************/
#ifdef UART_RX
u8 StrToU8(u8 ch[])
{
	return ((C2B(ch[0]) << 4) | C2B(ch[1]));
}
#endif

#ifdef DEBUG_DP
void dump_reg()
{
	//DBG(("-----------------------------\n"));
	
#if 0
	*usb_USB3StateSelect = 0x00;
	MSG(("LT:%BX\n", *usb_USB3StateCtrl));

	//USB2 PORT STATE
	*usb_USB3StateSelect = 0x01;
	MSG(("usb2 state:%BX\n", *usb_USB3StateCtrl));

	//for LPM Debug
	*usb_USB3StateSelect = 0x09;
	MSG(("uUSB3StateCtrl:%BX\n", *usb_USB3StateCtrl));
	*usb_USB3StateSelect = 0x00;

	MSG(("uUSB3LTSSM:%BX%BX\n", *usb_USB3LTSSM_1, *usb_USB3LTSSM_0));
	MSG(("uDevState:%BX\n", *usb_DevState));
	MSG(("uDevSt:%BX%BX\n", *usb_DevStatus_1, *usb_DevStatus_0));
	MSG(("uDevAddress:%BX\n", *usb_DevAddress));
	MSG(("uCMgr_Status:%BX\n", *usb_CMgr_Status_shadow));
#endif	

	*dbuf_MuxSel = 0;
	MSG(("S0:%BX\n", *dbuf_MuxInOut));

	*dbuf_MuxSel = 1;
	MSG(("S1:%BX\n", *dbuf_MuxInOut));
	
	//*dbuf_MuxSel = 2;
	//MSG(("S2:%BX\n", *dbuf_MuxInOut));
#if 1
	MSG(("dUW %BX %BX%BX ",
		dbuf_Port[TX_DBUF_USB_W_PORT].dbuf_Port_Count_3,
		dbuf_Port[TX_DBUF_USB_W_PORT].dbuf_Port_Count_1,
		dbuf_Port[TX_DBUF_USB_W_PORT].dbuf_Port_Count_0
	));

	MSG(("%BX%BX\n",
		dbuf_Port[TX_DBUF_USB_W_PORT].dbuf_Port_Addr_1,
		dbuf_Port[TX_DBUF_USB_W_PORT].dbuf_Port_Addr_0
	));
#endif

#if 0
	MSG(("dSR %BX %BX%BX ",
		dbuf_Port[TX_DBUF_SATA0_R_PORT].dbuf_Port_Count_3,
		dbuf_Port[TX_DBUF_SATA0_R_PORT].dbuf_Port_Count_1,
		dbuf_Port[TX_DBUF_SATA0_R_PORT].dbuf_Port_Count_0
	));

	MSG(("%BX%BX\n",
		dbuf_Port[TX_DBUF_SATA0_R_PORT].dbuf_Port_Addr_1,
		dbuf_Port[TX_DBUF_SATA0_R_PORT].dbuf_Port_Addr_0
	));
#endif

#if 1
	MSG(("dSW %BX %BX%BX ",
		dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_3,
		dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_1,
		dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Count_0
	));

	MSG(("%BX%BX\n",
		dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_1,
		dbuf_Port[TX_DBUF_SATA0_W_PORT].dbuf_Port_Addr_0
	));
#endif

#if 1
	MSG(("dUR %BX %BX%BX ",
		dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_3,
		dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_1,
		dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Count_0
	));

	MSG(("%BX%BX\n",
		dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_1,
		dbuf_Port[TX_DBUF_USB_R_PORT].dbuf_Port_Addr_0
	));
#endif

#if 0
	MSG(("dCR %BX %BX%BX ",
		dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_3,
		dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_1,
		dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Count_0
	));
			
	MSG(("%BX%BX\n",
		dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Addr_1,
		dbuf_Port[TX_DBUF_CPU_R_PORT].dbuf_Port_Addr_0
	));
#endif

#if 0
	MSG(("dCW %BX %BX%BX ",
		dbuf_Port[TX_DBUF_CPU_W_PORT].dbuf_Port_Count_3,
		dbuf_Port[TX_DBUF_CPU_W_PORT].dbuf_Port_Count_1,
		dbuf_Port[TX_DBUF_CPU_W_PORT].dbuf_Port_Count_0
	));
	
	MSG(("%BX%BX\n",
		dbuf_Port[TX_DBUF_CPU_W_PORT].dbuf_Port_Addr_1,
		dbuf_Port[TX_DBUF_CPU_W_PORT].dbuf_Port_Addr_0
	));	
#endif

#if 1
	MSG(("uI:%BX%BX\n",  *usb_IntStatus_shadow_1, *usb_IntStatus_shadow_0));	

	MSG(("mI:%BX%BX\n", *usb_Msc0IntStatus_1, *usb_Msc0IntStatus_0));
	MSG(("mTCt:%BX%BX\n", *usb_Msc0TxCtxt_1, *usb_Msc0TxCtxt_0));
	MSG(("mTXC:%BX%BX%BX\n", *usb_Msc0TxXferCount_2, *usb_Msc0TxXferCount_1, *usb_Msc0TxXferCount_0));
	MSG(("mTXBCNT:%BX%BX\n", *usb_Msc0_TXBCNT_1, *usb_Msc0_TXBCNT_0));
	//MSG(("mDICtr:%BX\n", *usb_Msc0DICtrlClr));
	//MSG(("mDIXSt:%BX\n", *usb_Msc0DIXferStatus));
	//MSG(("mDISt:%BX\n", *usb_Msc0DIStatus));
#endif

#if 1
	MSG(("mRCt:%BX%BX\n", *usb_Msc0RxCtxt_1, *usb_Msc0RxCtxt_0));
	MSG(("mRXC:%BX%BX%BX\n", *usb_Msc0RxXferCount_2, *usb_Msc0RxXferCount_1, *usb_Msc0RxXferCount_0));
	//MSG(("mRXBCNT:%BX%BX\n", *usb_Msc0_RXBCNT_1, *usb_Msc0_RXBCNT_0));
	//MSG(("sRxStat:%BX%BX\n", *sata_DataRxStat_1, *sata_DataRxStat_0));
	//MSG(("mDOCtr:%BX\n", *usb_Msc0DOutCtrlClr));
	//MSG(("mDOXSt:%BX\n", *usb_Msc0DOXferStatus));
	//MSG(("mDOSt:%BX\n", *usb_Msc0DOStatus));
	//MSG(("mFISDmaCtrl_0:%BX\n", *usb_Msc_FISDmaCtrl_0));
#endif	
	
#if 0
	MSG(("sI:%BX%BX\n", *sata_IntStatus_1, *sata_IntStatus_0));//'
	MSG(("SRSt:%BX%BX\n", *sata_DataRxStat_1, *sata_DataRxStat_0));
	MSG(("sFI:%BX%BX\n", *sata_FrameInt_1, *sata_FrameInt_0));
	MSG(("sTc:%BX%BX%BX\n", *sata_TCnt_2, *sata_TCnt_1, *sata_TCnt_0));
	//MSG(("sPcnt:%BX%BX%BX\n", *sata_PCnt_2, *sata_PCnt_1, *sata_PCnt_0));
#endif	
	
	//MSG(("ctxtmem_ptr:%BX\n", *host_ctxtmem_ptr));
	
	
	MSG(("cScm:%BX %BX\n", sobj_curScm, sobj_State));
	if (sobj_curScm != SCM_NULL)
	{
		SCM_INDEX = sobj_curScm;
		MSG(("%BX %BX ", SCM_prot, SCM_CdbIndex));
		MSG(("%BX %BX\n", SCM_SegIndex, SCM_SegINOUT));
		
#if 0	
		*sata_CCMSITEINDEX = sobj_curScm;
		*sata_BlkCtrl_0 |= CCMREAD;
		//MSG(("CCM:\n"));
		//mem_dump(sata_Ccm_prot, 32);
		*sata_BlkCtrl_0 &= ~CCMREAD;

		if (SCM_CdbIndex != CTXT_NULL)
		{
			ctxt_site = SCM_CdbIndex;
	
			MSG(("S_CIdx:%BX\n", ctxt_site));
			*host_ctxtmem_ptr = ctxt_site;
			SCTXT_INDEX = ctxt_site;
			
			//MSG(("->CTXT:\n"));
			MSG(("C_DIdx:%BX\n", SCTXT_DbufIndex));
			MSG(("C_DIO:%BX\n", SCTXT_DbufINOUT));
			//MSG(("CDB:\n"));
			//mem_dump(ctxt_CDB, 16);	
		}
#endif
	}

#if 1
	MSG(("\nu_curC %bx\n", usb_curCtxt));
	if (usb_curCtxt != CTXT_NULL)
	{
		*host_ctxtmem_ptr = usb_curCtxt;
		SCTXT_INDEX = usb_curCtxt;

		MSG(("cSIdx:%BX %BX\n", SCTXT_CCMIndex, SCTXT_Tag));
		MSG(("cFg:%BX cSt:%bx\n", SCTXT_Flag, SCTXT_Status));
		MSG(("cXL:%bx%bx\n", *ctxt_XferLen_1, *ctxt_XferLen_0));
		MSG(("%bx\n", ctxt_CDB[0]));
		//mem_dump(ctxt_CDB, 16);	
	}

	
	
	
	//if (bot_mode)
	//	MSG(("ctxt_PhaseCase:%BX%BX\n", *ctxt_PhaseCase_1, *ctxt_PhaseCase_0));
	
	//MSG(("-----------------------------\n"));
#endif	
}
#endif

#ifdef UART_RX
/****************************************\
   uart_rx_parse
\****************************************/
void uart_rx_parse()
{
	u16 addr16;
	u8 addr8;
	u8 type8;
	u8 value8;
	u8 *buf = uart_rx_buf;

#if 0
	if ((buf[0] == 'H') && (buf[1] == 'E') && (buf[2] == 'L') && (buf[3] == 'P'))
	{
		MSG(("--------------------------------\n"));
		MSG(("| Memory Read:                 |\n"));
		MSG(("| MR $ADDR                     |\n"));
		MSG(("| ex: MR C016       		     |\n"));
		MSG(("|  --------------------------  |\n"));
		MSG(("| Memory Write:                |\n"));
		MSG(("| MW $ADDR $VALUE              |\n"));
		MSG(("| ex: MW C016 34    	         |\n"));
		MSG(("|  --------------------------  |\n"));
		MSG(("| PHY Read                     |\n"));
		MSG(("| PR $TAG &ADDR                |\n"));
		MSG(("| ex: PR 01 08                 |\n"));
		MSG(("|  --------------------------  |\n"));
		MSG(("| PHY Write                    |\n"));
		MSG(("| PW $TAG $ADDR $VALUE         |\n"));
		MSG(("| ex: PW 01 08 02              |\n"));
		MSG(("|  --------------------------  |\n"));
		return;
	}
#endif

#if 0
	if ((buf[0] == 'L') && (buf[1] == 'E') && (buf[2] == 'D'))
	{
		value8 = StrToU8(&buf[4]);
		LED(value8);
		MSG(("--\n"));
		return;
	}
#endif

#if 0
	if ((buf[0] == 'D') && (buf[1] == 'P'))
	{
	#ifdef DEBUG_DP
		dump_reg();
		MSG(("--\n"));
	#else
		dump_phy();
	#endif

		return;
	}
#endif

#if 1
	if ((buf[0] == 'M') && (buf[1] == 'R'))
	{
		addr16 = StrToU16(&buf[3]);
		MSG(("MR %X\n", (u16)addr16));
		MSG((":%BX\n", reg_r8(addr16)));
		MSG(("--\n"));		
	}
	else if ((buf[0] == 'M') && (buf[1] == 'W'))
	{
		addr16 = StrToU16(&buf[3]);
		value8 = StrToU8(&buf[8]);
		MSG(("MW %X %BX\n", addr16, value8));
		reg_w8(addr16, value8);
		MSG(("--\n"));		
	}
#if 0
	else if ((buf[0] == 'C') && (buf[1] == 'R'))
	{
		addr16 = StrToU16(&buf[3]);
		MSG(("CR %X\n", (u16)addr16));
		MSG((":%BX\n", *((u8 code *) addr16) ));
		MSG(("--\n"));		
	}
#endif
	else if ((buf[0] == 'P') && (buf[1] == 'R'))
	{
		type8 = StrToU8(&buf[3]);
		addr8 = StrToU8(&buf[6]);
		MSG(("PR %BX %BX\n", type8, addr8));
		MSG((":%BX\n", spi_phy_rd(type8, addr8)));
		MSG(("--\n"));		
	}
	else if ((buf[0] == 'P') && (buf[1] == 'W'))
	{
		type8 = StrToU8(&buf[3]);
		addr8 = StrToU8(&buf[6]);
		value8 = StrToU8(&buf[9]);
		MSG(("PW %BX %BX %BX\n", type8, addr8, value8));
		spi_phy_wr(type8, addr8, value8);
		MSG(("--\n"));		
	}
	else
#endif
	{
		MSG(("-xx-\n"));
	}
	return;
}
#endif //UART_RX
