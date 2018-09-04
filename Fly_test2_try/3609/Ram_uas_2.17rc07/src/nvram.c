/******************************************************************************
 *
 *   Copyright (C) Initio Corporation 2004-2013, All Rights Reserved
 *
 *   This file contains confidential and propietary information
 *   which is the property of Initio Corporation.
 *
 *
 *******************************************************************************/
#define NVRAM_C
#include <general.h>

#define GLOBAL_NVRAM_ADDR	0xF000

xdata NVRAM globalNvram _at_ 0x7C00;

#if (defined USB_FAST_ENUM || defined SCSI_DOWNLOAD_FW)

u8 NvVitalCheckSum(u8 xdata *ptr, u8 length)
{
	pU8 = ptr;
	tmp16 = 0;
	for (i8 = length; i8 != 0; i8--)
	{
		tmp16 += *pU8++;
    	if((tmp16 & 0xff00) != 0)
    	{
			// total is > than 8 bit so carry 1
			tmp16 = tmp16 & 0xff;
			tmp16++;
    	}
	}
	//DBG(("Cks %bx\n", (u8)tmp16));
	return ((u8)tmp16);
}

void sflash_wait_cmd_done()
{
	while (1)
	{
		if (*spi_IntStatus & SPI_IntStatus_Done)
			break;
	}

}

void sflash_rd_jedec_id()
{
	*spi_Ctrl_0 = 0x9F;
	*spi_Ctrl_1 = SPI_READ | SPI_DATAV | 4;		// 4 bytes
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();

	sflash_vid = *spi_Data_0;
	sflash_pid0 = *spi_Data_1;
	sflash_pid1 = *spi_Data_2;

	reg_w8(spi_IntStatus, SPI_IntStatus_Done);
	//DBG(("sf_vid:%bx %bx\n", sflash_vid, sflash_pid0));
}

void sflash_rd_id()
{
	
	*spi_Addr_0 = 0;
	*spi_Addr_1 = 0;
	*spi_Addr_2 = 0;
	*spi_Ctrl_0 = 0xAB;
	*spi_Ctrl_1 = SPI_READ | SPI_DATAV |SPI_ADDRV | 4;		// 4 bytes
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();
	sflash_vid = *spi_Data_0;
	sflash_pid0 = *spi_Data_1;

	*spi_IntStatus = SPI_IntStatus_Done;
	//DBG(("sf_id:%bx %bx\n", sflash_vid, sflash_pid0));

}

void sflash_get_type()
{
	*spi_IntStatus = SPI_IntStatus_Done;

	sflash_type = UNKNOWN_FLASH;
	sflash_rd_jedec_id();
	if (sflash_vid == PMC_VID)	// PMC
	{
		sflash_type = PMC; 
	}
	else if (sflash_vid == MXIC_VID)	// MXIC
	{
		sflash_type = MXIC; 
	}
	else if (sflash_vid == WINBOND_VID)	// WINDOND
	{
		sflash_type = WINBOND;
	}
	else  if (sflash_vid == GIGADATA_VID)	// GIGADATA
	{
		sflash_type = GIGADATA;
	}
	else if (sflash_vid == ESMT_VID)
	{
		sflash_type = ESMT;
	}
	else
	{
		sflash_rd_id();
		if (sflash_vid == SST_VID)
		{
			sflash_type = SST;
		}
	}

	if (sflash_type == UNKNOWN_FLASH)
	{
		fast_enum_usb_flag = 0;
		valid_sflash_vital_data = 0;
		//DBG0(("unknow flash\n" ));
	}
}

void sflash_Write_enable()
{
	*spi_IntStatus = SPI_IntStatus_Done;
wen_lp:
	// WREN
	*spi_Ctrl_0 = 0x06;				// WREN
	*spi_Ctrl_1 = SPI_WRITE;		// 0 bytes
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();

	*spi_IntStatus = SPI_IntStatus_Done;

	if ((sflash_type == MXIC)|| (sflash_type == ESMT))
	{

		*spi_Ctrl_0 = 0x05;							// MXIC RDSR
		*spi_Ctrl_1 = SPI_READ | SPI_DATAV | 1;		// 1 bytes
		*spi_Ctrl_2 = SPI_START;
		sflash_wait_cmd_done();
		tmp8 = *spi_Data_0;
		//DBG(("MXIC SR %BX\n", tmp8));
		*spi_IntStatus = SPI_IntStatus_Done;
		if ((tmp8 & 0x2) == 0)		// WEL bit
			goto wen_lp;
	}
}

void sflash_Write_disable()
{
	*spi_IntStatus = SPI_IntStatus_Done;
	// WREN
	*spi_Ctrl_0 = 0x04;				// WRDI
	*spi_Ctrl_1 = SPI_WRITE;		// 0 bytes
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();
	*spi_IntStatus = SPI_IntStatus_Done;
}

void sflash_wait_Write_done()
{
	*spi_IntStatus = SPI_IntStatus_Done;
	*spi_Ctrl_0 = 0x05;							// RDSR
	*spi_Ctrl_1 = SPI_READ | SPI_DATAV | 1;		// 1 bytes
	do
	{
		*spi_Ctrl_2 = SPI_START;
		sflash_wait_cmd_done();
		*spi_IntStatus = SPI_IntStatus_Done;
	} while (*spi_Data_0 & 0x1);
}

void sflash_erase_chip()
{
	if (sflash_type == UNKNOWN_FLASH)
	{
		sflash_get_type();
		if (sflash_type == UNKNOWN_FLASH)
		{
			//DBG(("unknow flash\n"));
			return;
		}
	}

	*spi_IntStatus = SPI_IntStatus_Done;

	if ((sflash_type == SST) || (sflash_type == WINBOND))
	{
		*spi_Data_0 = 0x00;

		*spi_Ctrl_0 = 0x50;								// EWSR
		*spi_Ctrl_1 = SPI_WRITE | SPI_DATAV | 1;		// 1 bytes
		*spi_Ctrl_2 = SPI_START;

		sflash_wait_cmd_done();
		*spi_IntStatus = SPI_IntStatus_Done;
	}
	else if ((sflash_type == MXIC)|| (sflash_type == ESMT))
	{
		sflash_Write_enable();
	}
    if (sflash_type != ESMT) //<<<<<<
    {
		*spi_Data_0 = 0x00;
		*spi_Ctrl_0 = 0x01;								// WRSR
		*spi_Ctrl_1 = SPI_WRITE | SPI_DATAV | 1;		// 1 bytes
		*spi_Ctrl_2 = SPI_START;

		sflash_wait_cmd_done();
	}

	*spi_IntStatus = SPI_IntStatus_Done;

	if ((sflash_type ==  MXIC))
	{
		// wait for WIP is gone
		sflash_wait_Write_done();
	}

	sflash_Write_enable();

	// chip Erase
	*spi_Ctrl_0 = 0xC7;	
	*spi_Ctrl_1 = SPI_WRITE;		// 0 bytes
	*spi_Ctrl_2 = SPI_START;
	
	*spi_IntStatus = SPI_IntStatus_Done;

	sflash_wait_cmd_done();

	*spi_IntStatus = SPI_IntStatus_Done;

	sflash_wait_Write_done();
}

void sflash_erase_sector(u8 sector)
{
	//DBG(("er sec\n"));

	if (sflash_type == UNKNOWN_FLASH)
	{
		sflash_get_type();
		if (sflash_type == UNKNOWN_FLASH)
		{
			//DBG(("unknow flash\n"));
			return;
		}
	}

	*spi_IntStatus = SPI_IntStatus_Done;

	if ((sflash_type == SST) || (sflash_type == WINBOND))
	{
		*spi_Data_0 = 0x00;

		*spi_Ctrl_0 = 0x50;								// EWSR
		*spi_Ctrl_1 = SPI_WRITE | SPI_DATAV | 1;		// 1 bytes
		*spi_Ctrl_2 = SPI_START;

		sflash_wait_cmd_done();
		*spi_IntStatus = SPI_IntStatus_Done;
	}
	else if ((sflash_type == MXIC)|| (sflash_type == ESMT))
	{
		sflash_Write_enable();
	}

    if (sflash_type != ESMT) //<<<<<<
    {
		*spi_Data_0 = 0x00;
		*spi_Ctrl_0 = 0x01;								// WRSR
		*spi_Ctrl_1 = SPI_WRITE | SPI_DATAV | 1;		// 1 bytes
		*spi_Ctrl_2 = SPI_START;
		sflash_wait_cmd_done();
	}

	*spi_IntStatus = SPI_IntStatus_Done;

	if ((sflash_type ==  MXIC))
	{
		// wait for WIP is gone
		sflash_wait_Write_done();
	}

	sflash_Write_enable();

	// Sector Erase
	*spi_Addr_0 = 0;
	*spi_Addr_1 = sector << 4;
	*spi_Addr_2 = 0;

	if (sflash_type == PMC)
		*spi_Ctrl_0 = 0xD7;						// SECTOR_ER_PMC
	else
		*spi_Ctrl_0 = 0x20;						// SECTOR_ER_SST

	*spi_IntStatus = SPI_IntStatus_Done;

	*spi_Ctrl_1 = SPI_WRITE | SPI_ADDRV;		// 0 bytes
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();	
	*spi_IntStatus = SPI_IntStatus_Done;
	sflash_wait_Write_done();
}
#endif

#ifdef USB_FAST_ENUM
void sflash_rd_vital_data()
{
	//DBG0(("rd vital\n"));

	*spi_IntStatus = SPI_IntStatus_Done;

	*spi_RdB_count = sizeof(VITAL_DATA);		// Byte Count
	*spi_Addr_0 = 0;
	*spi_Addr_1 = 0xE0;
	*spi_Addr_2 = 0;
	//DBG0(("*spi_Clock %bx\n", *spi_Clock_0));
	*spi_Clock_0 = 0x03;

	*spi_Ctrl_0 = SPI_CMD_READ;
	*spi_Ctrl_1 = SFLASH_RDBURST_EN|SPI_READ|SPI_ADDRV | SPI_DATAV;		// 4 bytes
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();
	pU8 = mc_buffer;
	for (i8= sizeof(VITAL_DATA); i8 != 0; i8--)
	{
		*pU8++ = *spi_Data_0;
	}

	*spi_IntStatus = SPI_IntStatus_Done;
#ifdef DBG_FUNCTION
		mem_dump((u8  xdata *)&mc_buffer[0], sizeof(VITAL_DATA));
#endif

	if ((mc_buffer[0] == VITAL_SIG_0) && (mc_buffer[1] == VITAL_SIG_1) &&
		(mc_buffer[0x7F] == NvVitalCheckSum((u8  xdata *)&mc_buffer[0], 0x7F)))
	{
		xmemcpy(mc_buffer, (u8 xdata *)(&sflash_vital_data), sizeof(VITAL_DATA));
#if 0
		if (sflash_vital_data.sobj_class == DEVICECLASS_ATAPI)
			return 1;
#endif
		//DBG0(("valid vital\n"));
		valid_sflash_vital_data = 1;
	}
	else
		valid_sflash_vital_data = 0;
}

void sflash_wr_vital_data()
{

	//DBG0(("wr vital\n"));

	if (sflash_type == UNKNOWN_FLASH)
	{
		sflash_get_type();
		if (sflash_type == UNKNOWN_FLASH)
			return;
	}
	sflash_erase_sector(0xE);


	pU8 = (u8 xdata *)&sflash_vital_data;
	i8 = 0;
loop:
	sflash_Write_enable();

	// Reset
	*spi_Ctrl_3 = SFLASH_PAGE_RESET;
	*spi_Ctrl_3 = 0;

	if ((sflash_type == MXIC) && (sflash_pid0 == 0x22))		// 5121E & 1021E
	{
		//DBG(("i8 %bx\n", i8));
		for (tmp8 = 32; tmp8 != 0; tmp8--)
		{
			 *spi_Data_0 = *pU8++;
		}
		*spi_Addr_0 = i8;
		i8 += 32;
	}
	else
	{
		for (tmp8= sizeof(VITAL_DATA); tmp8 != 0; tmp8--)
		{
			 *spi_Data_0 = *pU8++;
		}
		*spi_Addr_0 = 0;
		i8= sizeof(VITAL_DATA);
	}

	*spi_Addr_1 = 0xE0;
	*spi_Addr_2 = 0;

	tmp8 = 0x02;			// Page Program
	if (sflash_type == SST)
		tmp8 = 0xAF;		// SST AAI(Auto Address Increment Program)
	*spi_Ctrl_0 = tmp8;
	*spi_Ctrl_1 = SFLASH_PAGE_CLEAR|SPI_WRITE|SPI_ADDRV | SPI_DATAV;		//
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();
	*spi_IntStatus = SPI_IntStatus_Done;

	sflash_wait_Write_done();
	if (i8 < sizeof(VITAL_DATA))
		goto loop;
	if (sflash_type == SST)
		sflash_Write_disable();
}

void sflash_init_vital_data()
{
	//DBG0(("in vital\n"));
	if (sflash_type == UNKNOWN_FLASH)
	{
		sflash_get_type();
		if (sflash_type == UNKNOWN_FLASH)
			return;
	}
	valid_sflash_vital_data = 1;

	xmemset((u8 *)&sflash_vital_data, 0xFF, sizeof(VITAL_DATA));

	sflash_vital_data.signature[0] = VITAL_SIG_0;
	sflash_vital_data.signature[1] = VITAL_SIG_1;
	sflash_vital_data.sobj_class = sobj_class;

	sflash_vital_data.vendor = vendor;
	if (sobj_class == DEVICECLASS_ATA)
	{
		sflash_vital_data.sectorLBA_l = sobj_sectorLBA_l;
		sflash_vital_data.sectorLBA_h = sobj_sectorLBA_h;
		sflash_vital_data.qdepth = sobj_qdepth;
		xmemcpy((u8 xdata *)&sobj_serialStr[0],  (u8 xdata *)&(sflash_vital_data.iStr[0]), 20);
#ifdef PHYSICAL_MORE_512
		sflash_vital_data.physical_sector_size = sobj_physical_sector_size;
		MSG(("sw ss %x", sobj_physical_sector_size));
#endif
	}
	xmemcpy((u8 *)&(globalNvram.modelStrLength),  (u8 xdata *)&(sflash_vital_data.modelStrLength), 0x7E-0x2F);
	sflash_vital_data.check_sum = NvVitalCheckSum((u8  xdata *)&sflash_vital_data, 0x7F);   //nv_vital_data.CheckSum;
#ifdef DBG_FUNCTION
		mem_dump((u8  xdata *)&sflash_vital_data, sizeof(VITAL_DATA));
#endif

	sflash_wr_vital_data();
}
#endif

#if 0
void NvReadExtNvram(void)
{

}
#endif

#if 0
void NvReadDefaultlNvram(void)
{
	for (sz16 = 0; sz16< sizeof(NVRAM); sz16++)//only read the 0x00-0x7f INITIO NVRAM part
	{
		*((U8 *)&globalNvram + sz16) = ((U8 code *)0x0080)[sz16];
	}
	xmemset((U8 *)&globalNvram + 0x80, 0x00, 0xF0);
	xmemset((U8 *)&globalNvram + 0x170, 0x00, 0x90);
}
#endif

#ifdef SCSI_DOWNLOAD_FW
#if 0
void sflash_read_data(u16 flash_addr,u16 read_size)
{
	*spi_IntStatus = SPI_IntStatus_Done;

	if(read_size == 0x100)
		*spi_RdB_count = 0x00;			// Byte Count
	else
		*spi_RdB_count = read_size;		// Byte Count

	*spi_Addr_0 = 0;
	*spi_Addr_1 = flash_addr;
	*spi_Addr_2 = 0;
	//DBG0(("*spi_Clock %bx\n", *spi_Clock_0));
	*spi_Clock_0 = 0x03;

	*spi_Ctrl_0 = SPI_CMD_READ;
	*spi_Ctrl_1 = SFLASH_RDBURST_EN|SPI_READ|SPI_ADDRV | SPI_DATAV;		// 4 bytes
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();
	pU8 = mc_buffer;
	for (tmp16 = read_size; tmp16 != 0; tmp16--)
	{
		*pU8++ = *spi_Data_0;
	}

	*spi_IntStatus = SPI_IntStatus_Done;
#ifdef DBG_FUNCTION0
	//mem_dump((u8  xdata *)&mc_buffer[0], read_size);
#endif
}
#endif

void sflash_write_data(u8 xdata *ptr,u16 flash_addr,u16 downlozd_size)
{
	//DBG0(("wr data\n"));
	//DBG0(("addr %x \n", flash_addr));

	if (sflash_type == UNKNOWN_FLASH)
	{
		sflash_get_type();
		if (sflash_type == UNKNOWN_FLASH)
			return;
	}

	pU8 = ptr;
	tmp16 = 0;
loop:
	sflash_Write_enable();

	// Reset
	*spi_Ctrl_3 = SFLASH_PAGE_RESET;
	*spi_Ctrl_3 = 0;

	if ((sflash_type == MXIC) && (sflash_pid0 == 0x22))			// 5121E & 1021E
	{
		//DBG0(("tmp16 %x\n", tmp16));
		for (tmp8 = 32; tmp8 != 0; tmp8--)
		{
			 *spi_Data_0 = *pU8++;
		}
		*spi_Addr_0 = (u8)tmp16;
		tmp16 += 32;
	}
	else
	{
		for (tmp16 = downlozd_size; tmp16 != 0; tmp16--)
		{
			 *spi_Data_0 = *pU8++;
		}
		*spi_Addr_0 = 0;
		tmp16 = downlozd_size;
	}

	*spi_Addr_1 = (u8)flash_addr;
	*spi_Addr_2 = 0;

	tmp8 = 0x02;			// Page Program
	if (sflash_type == SST)
		tmp8 = 0xAF;		// SST AAI(Auto Address Increment Program)
	*spi_Ctrl_0 = tmp8;
	*spi_Ctrl_1 = SFLASH_PAGE_CLEAR|SPI_WRITE|SPI_ADDRV | SPI_DATAV;		//
	*spi_Ctrl_2 = SPI_START;

	sflash_wait_cmd_done();
	*spi_IntStatus = SPI_IntStatus_Done;

	sflash_wait_Write_done();
	if (tmp16 < downlozd_size)
		goto loop;
	if (sflash_type == SST)
		sflash_Write_disable();
}
#endif

void NvReadGlobalNvram(void)
{
	*spi_Addr_2 = 0;

	pU8 = mc_buffer;
	for (sz16 = 0; sz16 < NVR_DATASIZE; sz16+= 4)
	{
		// SPI Address
		tmp16 = 0xF000 + sz16;
		*spi_Addr_0 = tmp16 & 0xFF;
		*spi_Addr_1 = tmp16 >> 8;

		*spi_Ctrl_0 = SPI_CMD_READ;
		*spi_Ctrl_1 = SPI_READ|SPI_ADDRV | SPI_DATAV | 4;		// 4 bytes
		*spi_Ctrl_2 = SPI_START;

		do {
			tmp8 = reg_r8(spi_IntStatus);
		} while (!(tmp8 & SPI_IntStatus_Done));

		reg_w8(spi_IntStatus, tmp8);

		*pU8++ = *spi_Data_0;
		*pU8++ = *spi_Data_1;
		*pU8++ = *spi_Data_2;
		*pU8++ = *spi_Data_3;
	}

	//DBG(("G NVRAM %bx %bx %bx %bx\n", mc_buffer[0], mc_buffer[1], mc_buffer[2], mc_buffer[3]));
	if ((mc_buffer[0] == 0x25) &&
		(mc_buffer[1] == 0xC9) &&
		(mc_buffer[2] == 0x36) &&
		(mc_buffer[3] == 0x09) )
	{
		//DBG(("iSeri %bx %bx %bx\n", mc_buffer[0x5C], mc_buffer[0x5D], mc_buffer[0x5E]));
		*chip_status |= NV_ON_SPI;
		xmemcpy(mc_buffer, (u8 xdata *)(&globalNvram), NVR_DATASIZE);
	}
}
