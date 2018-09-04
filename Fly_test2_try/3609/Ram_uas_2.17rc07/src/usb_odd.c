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
 * 3610		2010/05/15	Abel 	Initial version
 * 3607		2010/10/06	Ted		VBUS OFF detect modify
 * 3607		2010/11/20	Odin	AES function
 *
 *****************************************************************************/

#define USB_ODD_C

#include "general.h"


#ifdef ATAPI

#define odd_tick			hdd_tick
#define odd_que_ctxt_site	hdd_que_ctxt_site
#define odd_que_ctxt_tout	hdd_que_ctxt_tout

void odd_tick_isr()
{
#if 1
	if (sobj_State == SATA_RESETING)
	{				
		sobj_State_tout--;
		if (sobj_State_tout == 0)
		{
			if (sobj_init)
			{
				DBG0(("ODD A S_init RST TO\n"));
				sobj_init = 0;
				sobj_init_tout = 255;	// 25.5sec
				usb_active = 0;
				return;
			}
			DBG0(("ODD S RST TO\n"));
			// SATA Phy Ready Time-Out, Reset SATA Again
			//sata_Reset(SATA_HARD_RST);
			sata_HardReset();
		}
		return;
	}
#endif

#if 1
	else if (sobj_State == SATA_PHY_RDY)
	{
		sobj_State_tout--;
		if (sobj_State_tout == 0)
		{
			DBG0(("Odd S PRDY TO\n"));
			// SATA Drive Ready Time-Out
			//sata_Reset(SATA_HARD_RST);
			sata_HardReset();
		}
		return;
	}
#endif

#if 0
	if	((sobj_State == SATA_DRV_RDY) && (sobj_State_tout > SATA_DRV_INIT_TOUT))
	{
		// SATA Drive INIT Time-Out
		usb_active = 0;
		return;
	}
#endif

}
void bot_odd_exec_ctxt()
{
	if (sobj_init == 0)
	{
		odd_que_ctxt_site = ctxt_site;
		odd_que_ctxt_site = HDD_QUE_TOUT;					// 
	}
	else
		sata_exec_ctxt();

}

void initio_RdInqCmd()
{
	DBG0(("RdInq\n"));


	*(mc_buffer + 0) = globalNvram.ModelId[0];
	*(mc_buffer + 1) = globalNvram.ModelId[1];

	*(mc_buffer + 2) = NV_VERSION(0);
	*(mc_buffer + 3) = NV_VERSION(1);

	xmemcpy(module_vendor_text, &mc_buffer[4], 8);		//4-11
	xmemcpy(module_product_text, &mc_buffer[12], 16);		//12-27
		
	*(mc_buffer + 28) = (NV_VERSION(0) & 0x0F) + '0';
	*(mc_buffer + 29) = '.';
	*(mc_buffer + 30) = (NV_VERSION(1) >> 4) + '0';
	*(mc_buffer + 31) = (NV_VERSION(1) & 0x0F) + '0';
		
	//INITIO_TEXT_24 = "(C) 2002-04 Initio Corp."
	WRITE_BLOCK(INITIO_TEXT_24, ((u8 *)(mc_buffer + 32)), 24);
		
	xmemset(mc_buffer + 56, 0x00, 8);

	// boot status
	*(mc_buffer + 57) = (*fw_status & 0x03);

	// USB Speed
	*(mc_buffer + 58) = (usbMode << 1);

//				if (valid_hdd)
//					*(mc_buffer + 58) |= 0x40;
		
		
		
	*(mc_buffer + 59) = 0x80 | usb_AdapterID;
	*(mc_buffer + 60) = usb_PortID0;
	*(mc_buffer + 61) = usb_PortID1;
	*(mc_buffer + 62) = usb_PortID2;
	*(mc_buffer + 63) = *chip_Revision;
	byteCnt = 64;
	usb_device_data_in();
	return;
}


//
// INITIO Read NVRAM		BUFFER OFFSET == 0x00FF00
//
void initio_RdNvramCmd()
{	
#if 1
	//U8 i;
	DBG0(("RdNvram\n"));

	xmemset(mc_buffer, 0x00, 0x200);

	xmemcpy(((u8 xdata *)&globalNvram), mc_buffer, NVR_DATASIZE);
	//xmemcpy(((u8 xdata *)&serialStr), mc_buffer+0x5C, 20);
	
	byteCnt = NVR_DATASIZE;
	
	byteCnt = Min(byteCnt, *((u16 *)(&CmdBlk[7])));
	usb_device_data_in();
#endif
	return;
}

#ifdef SCSI_DOWNLOAD_FW
void odd_post_data_out()
{
	//u16	byteCnt;
	DBG0(("O pdo\n"));
	dbuf_get_data(DBUF_SEG_U2C);	
	usb_rx_fifo_rst();

	SCTXT_Status = CTXT_STATUS_GOOD;
	
	switch (CmdBlk(0))
	{
	/****************************************\
		SCSI_WRITE_BUFFER10
	\****************************************/
	case SCSI_WRITE_BUFFER10:   //
		if ((CmdBlk(1) == 0x5) && 
			(CmdBlk(2) == 0x00) && 
			(CmdBlk(3) == 0x03) && 
			(CmdBlk(5) == 0x00) &&
			(CmdBlk(7) == 0x02) &&
			(CmdBlk(10) == 0x25) &&
			(CmdBlk(11) == 0xc9))
		{
			DBG0(("w r download 2\n"));
			hdd_odd_scsi_downlod_fw();
			break;
		}
	default:
		hdd_err_2_sense_data(ERROR_ILL_PARAM);
		break;
	}	

	usb_rx_ctxt_send_status();

}
#endif

/****************************************\
   usb_odd
\****************************************/
void usb_odd()
{

//u8 flush_flag=0;
//u32 clock;
//u8 ODD_First_Cmd;

	//ODD_First_Cmd = 1;
	//unset it to fix Ch9 unknow get desc type, now set again
	*usb_DeviceRequestEn_1 |= USB_GET_DESCR_STD_UTILITY;

	// HOST_BLK_SIZE=512 SATA_BLK_SIZE=512 ATA6 command
	//*usb_Msc0Lun_SAT_0 = 0x07;

	bot_init();

	// Enable "Setup Device Request" interrupt of EP 0
	*usb_Ep0IntEn = EP0_SETUP;

	// Enable MSCn_INTEN register
	*usb_Msc0IntEn_0 = (BOT_RST_INT|MSC_TX_DONE_INT|MSC_RX_DONE_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT);
	*usb_Msc0IntEn_1 = 0;
	*usb_Msc0IntEn_2 = (MSC_CSW_DONE_INT);

	// Enable MSCn_INTEN register
	*usb_IntEn_0 = HOT_RESET|WARM_RESET|USB_BUS_RST;
	*usb_IntEn_1 = CDB_AVAIL_INT|MSC0_INT|CTRL_INT;

	*sata_IntEn_0 = RXDATARLSDONEEN|DATATXDONEEN|FRAMEINTPENDEN|PHYINTPENDEN;

	*usb_DeadTimeout_0 = 0;
	*usb_DeadTimeout_1 = 0;
	
	//*usb_DevCtrl |=	EXIT_SS_INACTIVE;
	DBG(("\nODD %BX\n", sobj_State));
	//*usb_CoreCtrl_3 |= USB3_BURST_MAX_RX;
	*usb_Threshold0 = 0x88;		// RX is 8, TX is 8
	//*usb_Threshold0 = 0x44;
	//*usb_Threshold1=  0x44;
	//*usb_Threshold2=  0x44;

	//we are interested in u0 enter and exit
	//*usb_USB3LTSSMEnter |= LTSSM_U0;
	//*usb_USB3LTSSMExit |= LTSSM_U0;

	//*usb_DevCtrl |= USB3_U1U2_EXIT;


	// suppor US BOT only
	while(1)
	{
begin:
		if (USB_VBUS_OFF())
		{
			MSG(("O vbus off\n"));
#ifdef PWR_SAVING
			// Enable USB2.0 PLL
			usb20_clock_enable();
#endif
			usb_active = 0;
			//bot_usb_bus_reset();
			return;
		}

				

		if((*cpu_wakeup_ctrl_0 & CPU_USB_SUSPENDED))
		{
			DBG0(("BO Susp!\n"));
#if 1
			if (*cpu_wakeup_ctrl_0 & CPU_USB2_L1_SLEEP)
			{
				DBG0(("B USB2_L1_SLEEP!\n"));
				*cpu_wakeup_ctrl_0 = CPU_USB2_L1_SLEEP;
			}
#endif
			if (*usb_CMgr_Status_shadow == 0)
			{

			#ifdef HW_CAL_SSC
				start_HW_cal_SSC = 1;
			#endif

				usb3_test_mode_enable = 0;

				return;

			}


			if (usb_suspend(0))
				return;		
		}
		

		if (*chip_IntStaus_0 & USB_INT)
		{
			//u8 data usb_int_Stat_1;

			if (*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
			{
#ifdef ASIC_CONF
			//	if (revision_a61)
				{
					spi_phy_wr_retry(PHY_SPI_U3PCS, PCS_PHY_DIGIAL_13, HW_RESTART_USB2_ENABLE);
				}
#endif
				// Enable USB2.0 PLL
				//*usb_DevCtrl_2 = USB2_PLL_FREERUN;
				*usb_DevCtrlClr_1 = USB2_FORCE_SUSP;

				if (*usb_IntStatus_shadow_0 & USB_BUS_RST)
					*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_1;
				else
					*usb_LinDBG_2 = USB_RX_DETECT_CNT_EQ_8;
				
				//dump_reg();
				MSG(("O Rst\n"));
#ifdef EQ_SETTING_TEST
				if(usbMode == CONNECT_USB3)
				{
					if(usb_30_bus_rst < 0xFF)
						usb_30_bus_rst++;
				}
				else
				{
					if(usb_20_bus_rst < 0xFF)
						usb_20_bus_rst++;
				}
#endif

				bot_usb_bus_reset();
				return;
			}


			if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
			{
_bot_odd_bulk_reset:
				if (bot_cbw_active)
				{
					*usb_CtxtFreeFIFO = ctxt_site;
				}

				*usb_Msc0IntStatus_0 = BOT_RST_INT;
				bot_usb_bus_reset();
				MSG(("OBulk RST\n"));
				continue;
			}


			usb_int_Stat_1 = *usb_IntStatus_shadow_1;
			if (usb_int_Stat_1 & CTRL_INT)
			{	
				// EP0_SETUP----------------
				if (*usb_Ep0Ctrl & EP0_SETUP)
				{	
					DBG(("stp\n"));
					usb_control();
				}
				goto begin;
			}

				
			/****************************************\
				USB Command IU 
			\****************************************/

			if ((bot_cbw_active == 0) &&
				(usb_int_Stat_1 & CDB_AVAIL_INT))
			{
				bot_cbw_active = 1;
				ctxt_site = *usb_CtxtAvailFIFO;
				*host_ctxtmem_ptr = ctxt_site;
				SCTXT_INDEX = ctxt_site;
				SCTXT_Flag = 0;
				
				// check cbw 
				if (*ctxt_Flag & (CTXT_FLAG_LENGTH_ERR|CTXT_FLAG_SIZE_ERR|CTXT_FLAG_SIG_ERR|CTXT_FLAG_LUN_ERR)) //
				{
					*usb_Msc0DICtrl = MSC_DI_HALT;
					*usb_Msc0DOutCtrl = MSC_DOUT_HALT;	
					if(*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
					{
#ifdef EQ_SETTING_TEST
						if(usbMode == CONNECT_USB3)
						{
							if(usb_30_bus_rst < 0xFF)
								usb_30_bus_rst++;
						}
						else
						{
							if(usb_20_bus_rst < 0xFF)
								usb_20_bus_rst++;
						}
#endif

						DBG(("goto reset!\n"));
						return;
					}
					
					if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
					{
						DBG(("goto bulk reset\n"));
						goto _bot_odd_bulk_reset;
					}
				
					if (*usb_Ep0Ctrl & EP0_SETUP)
					{
						DBG(("SETUP \n"));

						usb_control();
					}
					
					//if (*usb_IntStatus & VBUS_OFF)
					if (USB_VBUS_OFF())
					{
						usb_active = 0;
						//bot_usb_bus_reset();
					
						return;
					}
				}

				if (*ctxt_Control & CTXT_CTRL_DIR)
				{
					SCTXT_Flag = SCTXT_FLAG_DIN;
				}

				if ((*ctxt_XferLen_0|*ctxt_XferLen_1|*ctxt_XferLen_2|*ctxt_XferLen_3) == 0)
				{
					*ctxt_No_Data = MSC_CTXT_NODATA;
					SCTXT_Flag |= SCTXT_FLAG_NO_DATA;
				}

				//if (*ctxt_No_Data & MSC_CTXT_NODATA)
				//{
				//	SCTXT_Flag |= SCTXT_FLAG_NO_DATA;
				//}

				COPYU32_REV_ENDIAN_X2D(ctxt_XferLen_0, &cbwbyteCnt);

				SCTXT_CCMIndex = CCM_NULL;
				SCTXT_DbufIndex = DBUF_SEG_NULL;
				
				//*ctxt_Status = (CTXT_STATUS_PENDING);
				SCTXT_Status = CTXT_STATUS_PENDING;

				//usb_dir = (*ctxt_Control &0x80)?1:0;

				CmdBlk(0) = ctxt_CDB[0];
			//	MSG(("Cmd:%BX\n", CmdBlk(0)));
				CmdBlk(1) = ctxt_CDB[1];
				CmdBlk(2) = ctxt_CDB[2];
				CmdBlk(3) = ctxt_CDB[3];
				CmdBlk(4) = ctxt_CDB[4];
				CmdBlk(5) = ctxt_CDB[5];
				CmdBlk(6) = ctxt_CDB[6];
				CmdBlk(7) = ctxt_CDB[7];
				CmdBlk(8) = ctxt_CDB[8];
				CmdBlk(9) = ctxt_CDB[9];
				CmdBlk(10) = ctxt_CDB[10];
				CmdBlk(11) = ctxt_CDB[11];
		#ifdef USB_FAST_ENUM
				if (sobj_init == 0)
				{
					if(CmdBlk(0) == SCSI_REQUEST_SENSE)
					{
DBG0(("R Sens\n"));
						hdd_request_sense_cmd();
						goto begin;
					}
				}
		#endif
odd_start_scsi_cmd:				
				if(CmdBlk(0) == SCSI_READ10)
				{
					if((CmdBlk(7)==0)&&(CmdBlk(8)==0))
					{   // case 1 (Hn = Dn), case 4 (Hi > Dn), or case 9(Ho > Dn)
						SCTXT_Status = CTXT_STATUS_GOOD;
						usb_device_no_data();
						goto begin;
					}
					goto odd_read;
			}
			else if(CmdBlk(0) == SCSI_READ12)
			{
					if ((CmdBlk(6)==0) &&
						(CmdBlk(7)==0) &&
						(CmdBlk(8)==0) &&
						(CmdBlk(9)==0))
					{   // case 1 (Hn = Dn), case 4 (Hi > Dn), or case 9(Ho > Dn)
						SCTXT_Status = CTXT_STATUS_GOOD;
						usb_device_no_data();
						goto begin;
					}
odd_read:
					// check case 2 (Hn < Di), case 7 (Hi < Di), or case 10 (Ho <> Di)
					//if ((*ctxt_PhaseCase_0 & 0x84) || (*ctxt_PhaseCase_1 & 0x04))
					//{
					//	SCTXT_Status = CTXT_STATUS_PHASE;			// phase Error
					//	bot_device_no_data();
					//	break;
					//}

					// case 5 (Hi > Di) or case  6(Hi = Di)
//							*ctxt_SataProto		= PROT_DMAIN;
					SCTXT_DbufIndex	= DBUF_SEG_S2U;
					SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) |  TX_DBUF_SATA0_W_PORT;;

					*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 
					//SCTXT_Status = CTXT_STATUS_PENDING;

					hdd_start_act_led();
				}
				else if(CmdBlk(0) == SCSI_WRITE10)
				{
					if((CmdBlk(7)==0)&&(CmdBlk(8)==0))
					{
						SCTXT_Status = CTXT_STATUS_GOOD;
						usb_device_no_data();
						goto begin;
					}
					goto odd_write;
				}
				else if (CmdBlk(0) == SCSI_WRITE12)
				{
					if ((CmdBlk(6)==0) &&
						(CmdBlk(7)==0) &&
						(CmdBlk(8)==0) &&
						(CmdBlk(9)==0))
					{
						SCTXT_Status = CTXT_STATUS_GOOD;
						usb_device_no_data();
						goto begin;
					}
odd_write:
					if(SCTXT_Flag & SCTXT_FLAG_DIN)						//case 8 Hi<>Do
					{
						DBG(("\nCase 8"));
						SCTXT_Status = CTXT_STATUS_PHASE;
						usb_device_no_data(); 	
						goto begin;
					}

					// check check case 3(Hn < Do), 8(Hi <> DO), or case 13 (Ho < Do)
					//if ((*ctxt_PhaseCase_0 & 0x08) || (*ctxt_PhaseCase_1 & 0x21))
					//{
					//	SCTXT_Status = CTXT_STATUS_PHASE;			// CSW status
					//	bot_device_no_data();
					//	break;							
					//}
//					CTXT_PROTO = PROT_DMAOUT;
					SCTXT_DbufIndex = DBUF_SEG_U2S;
					SCTXT_DbufINOUT = (TX_DBUF_SATA0_R_PORT << 4) |  TX_DBUF_USB_W_PORT;

					*ctxt_CCMcmdinten = 0;				//CTXT_ccm_cmdinten = 0; 
					SCTXT_Status = CTXT_STATUS_PENDING;
					
					hdd_start_act_led();
//							autoFis = 1;
					//sata_exec_ctxt();

					//break;
				}
				else if(CmdBlk(0) == SCSI_READ_BUFFER10)
				{
//#ifdef DBG_FUNCTION
#if 0
	for(i8 = 0; i8 < 12; i8++)
	{
		DBG0(("%BX ", CmdBlk(i8)));
	}
	DBG0(("\n"));
#endif

					if ((CmdBlk(1) == 0x01) &&				// MODE == Vendor specific
						(CmdBlk(2) == 0x00)	&&				// BUFFER ID == 0
						(CmdBlk(6) == 0x00))				// ALLOCATION LENGTH[0] == 0
					{
						// read model & Firmware ver
						if ((CmdBlk(3) == 0x00) &&		// BUFFER OFFSET[0] == 0x00
							(CmdBlk(5) == 0x00))		// BUFFER OFFSET[2] == 0x00
						{
							if (CmdBlk(4)== 0x00)		// BUFFER OFFSET[1] == 0x00
							{
								//InitioReadInquiry - start		BUFFER OFFSET == 0x000000
								//SCTXT_Status = CTXT_STATUS_GOOD;
								initio_RdIn3qCmd();
								goto begin;
							}
							else if (CmdBlk(4) == 0xFF)		// BUFFER OFFSET[1] == 0xFF
							{
								//
								// INITIO Read NVRAM		BUFFER OFFSET == 0x00FF00
								//
								//SCTXT_Status = CTXT_STATUS_GOOD;
								initio_RdNvramCmd();
								goto begin;
							}
			#if 0
							else if (CmdBlk(4) == 0xFC)		// BUFFER OFFSET[1] == 0xFF
							{
								mc_buffer[0] = 0x11;
								mc_buffer[1] = 0x22	 ;
								mc_buffer[2] = 0x33;
								mc_buffer[3] = 0x44;
								mc_buffer[0x49] = 0x08;
								flash_addr_low = CmdBlk(9);
								sflash_write_data((u8  xdata *)&mc_buffer[0],flash_addr_low,0x100);
								flash_addr_low += 0x01;
								mc_buffer[0x100] = 0x55;
								mc_buffer[0x101] = 0x66;
								mc_buffer[0x102] = 0x77;
								mc_buffer[0x103] = 0x88;
								sflash_write_data((u8  xdata *)&mc_buffer[0x100],flash_addr_low,0x100);
								byteCnt = 0x200;
								usb_device_data_in();
								goto begin;
							}
							else if (CmdBlk(4) == 0xFD)		// BUFFER OFFSET[1] == 0xFF
							{
								//
								// INITIO Read NVRAM		BUFFER OFFSET == 0x00FF00
								//
								//SCTXT_Status = CTXT_STATUS_GOOD;
								xmemset(mc_buffer, 0x00, 0x100);
								sflash_read_data((CmdBlk(9)),0x100);
								byteCnt = 0x100;
								usb_device_data_in();
								goto begin;
							}
							else if (CmdBlk(4) == 0xFE)		// BUFFER OFFSET[1] == 0xFF
							{
								//
								// INITIO Read NVRAM		BUFFER OFFSET == 0x00FF00
								//
								SCTXT_Status = CTXT_STATUS_GOOD;
								sflash_erase_sector(CmdBlk(9));
								byteCnt = 0x200;
								xmemset(mc_buffer, 0x00, 0x200);
								usb_device_data_in();
								goto begin;
							}
							else if (CmdBlk(4) == 0xFB)		// BUFFER OFFSET[1] == 0xFF
							{
								//
								// INITIO Read NVRAM		BUFFER OFFSET == 0x00FF00
								//
								SCTXT_Status = CTXT_STATUS_GOOD;
								sflash_erase_chip();
								byteCnt = 0x200;
								xmemset(mc_buffer, 0x00, 0x200);
								usb_device_data_in();
								goto begin;
							}
			#endif
						}
					}
				}
#ifdef SCSI_DOWNLOAD_FW
				else if(CmdBlk(0) == SCSI_WRITE_BUFFER10)
				{
				    DBG(("~SCSI_DOWNLOAD_FW SCSI_WRITE_BUFFER10\n"));
					byteCnt = *((u16 *)(&CmdBlk(7)));
					if ((CmdBlk(1) == 0x5) && 
						(CmdBlk(2) == 0x00) && 
						(CmdBlk(3) == 0x03) && 
						//(CmdBlk(4) == 0x00) && 
						(CmdBlk(5) == 0x00) &&
						(CmdBlk(7) == 0x02) &&
						(CmdBlk(10) == 0x25) &&
						(CmdBlk(11) == 0xc9))
					{
						DBG0(("w r data_out 3\n"));
						usb_device_data_out();
						goto begin;
					}
				}				
#endif
				// ATAPI pass-through
				if (cbwbyteCnt == 0)
				{	// No Data
					SCTXT_Flag |= (SCTXT_FLAG_U2B|SCTXT_FLAG_B2S);
					*ctxt_SataProto = PROT_PACKET_ND << 4;
					//*ctxt_SataProto = PROT_PACKET << 4;

					//do SAT, no need to fill sata_Ccm_xfercnt
					//Set Case 1, Hn=Dn
					*ctxt_PhaseCase_0 =	0x02;

					SCTXT_DbufIndex = DBUF_SEG_NULL;
					*ctxt_CCMcmdinten = D2HFISIEN;
					
		//#ifdef USB_FAST_ENUM
		#if 1
					bot_odd_exec_ctxt();
		#else
					sata_exec_ctxt();
		#endif
				}
				else if (SCTXT_Flag & SCTXT_FLAG_DIN)
				{	//  data-in

					//SCTXT_Flag = 0;
					//*ctxt_SataProto = PROT_PACKET << 4;
					*ctxt_SataProto = PROT_PACKET_DI << 4;
				
					*ctxt_PhaseCase_0 = 0x40;				// case 6

					SCTXT_DbufIndex = DBUF_SEG_S2U;
					SCTXT_DbufINOUT = (TX_DBUF_USB_R_PORT << 4) | TX_DBUF_SATA0_W_PORT;
					
					*ctxt_CCMcmdinten = 0;

#ifdef USB2_DI_PATCH	// USB2.0 patch
					if (usbMode == CONNECT_USB2)
					{
						//if ((CmdBlk(0) == SCSI_INQUIRY) && (CmdBlk(4) == 12))
						//{
						//	SCTXT_Flag |= (SCTXT_FLAG_U2B|SCTXT_FLAG_B2S);
						//}
						if ((16 > cbwbyteCnt) && (cbwbyteCnt > 8) )
						{
							SCTXT_Flag |= (SCTXT_FLAG_U2B|SCTXT_FLAG_B2S);
						}
					#if 1	
					#ifdef DBG_ODD_U2
						DBG0(("vCmd %bx %bx\n", (CmdBlk(0)), SCTXT_Flag));
						// hw: 40h, u2b/b2s: 70h
						dbg_flag = 1;
					#endif	
					#else
						if (CmdBlk(0) == SCSI_REQUEST_SENSE)
						{
							SCTXT_Flag |= (SCTXT_FLAG_U2B|SCTXT_FLAG_B2S);
						}
					#endif	
					}
#endif

				//#ifdef USB_FAST_ENUM
				#if 1
					bot_odd_exec_ctxt();
				#else
					//sata_exec_ctxt();
					bot_odd_exec_ctxt();
				#endif

				}
				else
				{	// data-Out
					//*ctxt_SataProto = PROT_PACKET << 4;
					*ctxt_SataProto = PROT_PACKET_DO << 4;

					*ctxt_PhaseCase_1 = 0x10;				// case 12

					//SCTXT_Flag = 0;
					SCTXT_DbufIndex = DBUF_SEG_U2S;
					SCTXT_DbufINOUT = (TX_DBUF_SATA0_R_PORT << 4) | TX_DBUF_USB_W_PORT;

					*ctxt_CCMcmdinten = 0;

				#if 0
					if((ctxt_CDB[0] == SCSI_WRITE10) && (blockLen <= 2))
					{	// PIO-Out
						*((u32 *)&(ctxt_FIS[FIS_TYPE])) = (PACKET << 16)|(0x80 << 8)|(HOST2DEVICE_FIS) ;
					}
					else
					{	// DMA-Out
						*((u32 *)&(ctxt_FIS[FIS_TYPE])) = (FEATURES_DMA << 24)|(PACKET << 16)|(0x80 << 8)|(HOST2DEVICE_FIS) ;
					}
					*((u32 *)&(CTXT_FIS[FIS_LBA_LOW])) = (MASTER << 24)|(0xFFFF<< 8);
					*((u32 *)&(CTXT_FIS[FIS_LBA_LOW_EXP])) = 0;
					*((u32 *)&(CTXT_FIS[FIS_SEC_CNT])) = 0;
				#endif

				//#ifdef USB_FAST_ENUM
				#if 1
					bot_odd_exec_ctxt();
				#else
					sata_exec_ctxt();
				#endif
				}

			}	// 		if (*usb_IntStatus & CDB_AVAIL_INT)

			if (usb_int_Stat_1 & MSC0_INT)
			{
				if (*usb_Msc0IntStatus_0 & (MSC_TX_DONE_INT|MSC_RX_DONE_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT))
					usb_msc_isr();

				if (*usb_Msc0IntStatus_2 & (MSC_CSW_DONE_INT))
				{
					//DBG(("MSC_CSW_DONE_INT\n"));
					*usb_Msc0IntStatus_0 = 0xff;	//MSC_TX_DONE_INT|MSC_RX_DONE_INT|MSC_DIN_HALT_INT|MSC_DOUT_HALT_INT;
					*usb_Msc0IntStatus_2 = 0xff;

					//tx_curCtxt = NULL;
					//rx_curCtxt = NULL;
					usb_curCtxt = CTXT_NULL;
					bot_cbw_active = 0;
					
#ifdef DBG_ODD_U2
					DBG0(("vCsWD %bx\n", (CmdBlk(0))));
					DBG0(("-------\n"));
					dbg_flag = 0;
#endif	
					goto begin;
				}

			}	// if ((usb_int_Status & MSC0_INT)



		}	//	if (*chip_IntStaus & USB_INT)

#if 1
		if ((sobj_State > SATA_PWR_DWN)  && (*chip_IntStaus_0 & SATA0_INT))
		{
#if 0
			if (sata_IntStatus_0 & PHYINTPEND)
			{
				if (sata_PhyInt & PHYDWNI)
				{
					sobj_State = SATA_RESETING;
					usb_active = 0;
					return;
				}
			}
#endif
			sata_isr();

	#ifdef USB_FAST_ENUM
			if (sobj_init == 0)
			{
				if (sobj_State == SATA_DRV_RDY)
				{
					if (sobj_class == org_sobj_class)
					{
						if (atapi_fast_enum_init())
							return;
					}
					else// if (sobj_class == DEVICECLASS_ATA)
					{
						// Not ATAPI Device
						usb_wrong_sata_device();
						return;
					}
				}
			}
	#endif

			if (odd_que_ctxt_site != CTXT_NULL)
			{
				if (sobj_init)
				{
					hdd_que_ctxt_site = CTXT_NULL;
					goto odd_start_scsi_cmd;
				}
			}


		}
#endif
#ifdef SCSI_DOWNLOAD_FW
		if (usb_post_dout_ctxt != CTXT_NULL)
		{
			ctxt_site = usb_post_dout_ctxt;
			usb_post_dout_ctxt = CTXT_NULL;
			
			odd_post_data_out();
		}
#endif

		if (sobj_State >= SATA_DRV_RDY)
		{
			if ((*sata_PhyStat & PHYRDY) == 0)
			{
				if (sobj_State > SATA_READY)
				{
					DBG(("ODD S UPLUG\n"));
					sobj_State = SATA_RESETING;
					usb_active = 0;
					return;
				}
				else
				{
					DBG(("ODD SPhy NR\n"));
					sata_Reset(SATA_HARD_RST);
				}
			}

		}
#if 1
		if (odd_tick)
		{
			odd_tick_isr();
			if (usb_active == 0)
				return;
			if (odd_que_ctxt_site != CTXT_NULL)
			{
				if (odd_que_ctxt_tout)
				{
					odd_que_ctxt_tout--;
					if (odd_que_ctxt_tout == 0)
					{
						odd_que_ctxt_site = CTXT_NULL;

						// Time out
						hdd_err_2_sense_data(ERROR_UA_BECOMING_READY);
						usb_device_no_data();
					}
				}
			}
			if (sobj_init == 0)
			{
				if (sobj_init_tout)
				{
					sobj_init_tout--;
				}
				else
				{
					sobj_State = SATA_NO_DEV;
					//bot_odd_led_off();
					DBG0(("sobj_init_tout\n"));
					usb_active = 0;
					return;
				}
			}
			odd_tick = 0;
		}
	}
}
#endif

#endif


