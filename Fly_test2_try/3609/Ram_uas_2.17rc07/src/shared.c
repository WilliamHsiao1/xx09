
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
 
#define SHARED_C

#include "general.h"
u8 xdata mc_buffer[512] _at_ 0x7E00;

/****************************************\
   Delay10us
\****************************************/
#if 1
void Delay10us(u8 time)
{
	//u8 data i;
	//for (i = 0; i < time; i++)
	for (; time != 0; time--)
	{
		//u8 data j;
#ifdef ASIC_CONF
		if (cpu_ref_clk)
		{
			for (val = 0; val < 15 ; val++)
			{
			}
		}
		else
#endif
		{
			for (val = 0; val < 54 ; val++)
			{
			}
		}
	}
}
#endif

/****************************************\
   Delay
\****************************************/
void Delay(u16 time)
{
	//1000: 1 sec
	//u16 data i;
	//for (i = 0; i < time; i++)
	for (; time != 0; time--)
	{
		//u16 data j;
#ifdef ASIC_CONF
		if (cpu_ref_clk)
		{
			for (sz16 = 0; sz16 < 1547 ; sz16++)
			{
			}
		}
		else
#endif
		{
			for (sz16 = 0; sz16 < 5460 ; sz16++)
			{
			}
		}
	}
}

/****************************************\
   memcpySwap16
\****************************************/
#if 1
void memcpySwap16(u8 *src, u8 *dest, u8 n16)
{
	//u32	i;
	u8 tmp8;

	for (i8 = 0; i8 < n16; i8++)
	{
		tmp8 = src[i8 * 2];
		dest[i8 * 2] = src[i8 * 2 + 1];
		dest[i8 * 2 + 1] = tmp8;
	}
}
#endif

/****************************************\
   xmemset
\****************************************/
void xmemset(u8 xdata *ptr, u8 chr, u8 count)
{
#if 0
	u8 i8;
	for (i8 = 0; i8 < count; i++)
		*ptr++ = chr;
#else
	while (count--) 
	{
		*ptr++ = (char)chr;
	}
#endif
}

/****************************************\
   xmemcpy
\****************************************/
void xmemcpy(u8 xdata *src, u8 xdata *dest, u16 size)
{
	u16 i;

	for (i = 0; i < size; i++)
	{
		*dest++ = *src++;
	}
	return;
}

bit xmemcmp(u8 xdata *src, u8 xdata *dest, u8 size)
{
	for (i8 = size; i8 != 0; i8--)
	{
		if (*src++ != *dest++)
			return 1;
	}
	return 0;
}

#if 0
void memcpy16(u8 xdata *src, u8 *dest, u16 size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		*dest = *src;
	}
	return;
}

u8 xstrlen(u8 *str)
{
	i = 0;
	while(1)
	{
		if (str[i] != 0x00)
		{
			i++;
		}
		else
		{
			break;
		}
	}
	return i;
}
#endif

/****************************************\
   CrSetModelText
\****************************************/
void CrSetModelText(u8 *buf)
{
	if (globalNvram.fwCtrl & PRODUCT_STR_FROM_NVRAM)
	{
		for(i8 = 0; i8 < 32; i8++)
		{
			module_product_text[i8] = ' ';
		}
		//DBG(("CrSetModelText\n"));
		xmemcpy(globalNvram.modelText, buf, 32);
		//mem_dump(globalNvram.modelText, 16);
		//mem_dump(buf, 16);
		return;
	}
}

/****************************************\
   CrSetVendorText
\****************************************/
void CrSetVendorText(u8 *buf)
{
	if (globalNvram.fwCtrl & VENDOR_STR_FROM_NVRAM)
	{
		for(i8 = 0; i8 < 16; i8++)
		{
			module_vendor_text[i8] = ' ';
		}
		xmemcpy(globalNvram.vendorText, buf, 16);
		return;
	}
}

/****************************************\
   spi_phy_wr
\****************************************/
#if 1
void spi_phy_wr(u8 phy_type, u8 phy_reg, u8 write_value)
{
	*phy_spi_ctrl_0 = write_value;
	*phy_spi_ctrl_1 = phy_reg;
	*phy_spi_ctrl_2 = PHY_CMD_WRITE | phy_type;
#if 1
	if (*phy_spi_ctrl_3 & PHY_DONE)
			return;
	Delay10us(1);
	if (*phy_spi_ctrl_3 & PHY_DONE)
			return;
	DBG(("\tSPI WR%BX\n", phy_reg));
#else
   //wait for 50us
	for(i8 = 10; i8 != 0; i8--)
	{
		if (*phy_spi_ctrl_3 & PHY_DONE)
		{
			return;
		}
	    //wait for 5us
		Delay10us(1);
	}	
	DBG(("\tSPI WR%BX\n", phy_reg));
#endif
}
#endif

/****************************************\
   spi_phy_rd
\****************************************/
#if 1
u8 spi_phy_rd(u8 phy_type, u8 phy_reg)
{
	//Reset DATA
	*phy_spi_ctrl_0 = 0xFF; 

	*phy_spi_ctrl_1 = phy_reg;
	*phy_spi_ctrl_2 = PHY_CMD_READ | phy_type;
#if 1
	if (*phy_spi_ctrl_3 & PHY_DONE)
	{
		return *phy_spi_ctrl_0;
	}
	Delay10us(1);
	if ((*phy_spi_ctrl_3 & PHY_DONE) == 0)
	{
		DBG(("\tSPI RD%BX\n", phy_reg));
	}
	return 	*phy_spi_ctrl_0;
#else
    //wait for 100us
	for(i8 = 10; i8 != 0; i8--)
	{
		if (*phy_spi_ctrl_3 & PHY_DONE)
		{
			return 	*phy_spi_ctrl_0;
		}
	   //wait for 5us
		Delay10us(1);
	}	
	DBG(("\tSPI RD%BX\n", phy_reg));
	return *phy_spi_ctrl_0;
#endif
}
#endif



