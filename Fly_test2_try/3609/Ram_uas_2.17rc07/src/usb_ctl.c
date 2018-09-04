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
 * 3607		2010/10/06	Ted			BOS description too large fixed
 *
 *****************************************************************************/

#include	"general.h"


#define	OUTBUF	usb_CtrlBuffer

//USB2.0 device descriptor
u8 xdata DeviceUSB2[18] = {
	18,					//DeviceUSB2[0] = sizeof(desc.DeviceUSB2);	// Descriptor length
	DESCTYPE_DEVICE,	//DeviceUSB2[1] 		// Descriptor type - Device
	0x10,				//DeviceUSB2[2] 		// Specification Version (BCD)
	0x02,				//DeviceUSB2[3] 		// inidcate USB 2.1 speed compilant
	0x00,				//DeviceUSB2[4] 		// Device class
	0x00,				//DeviceUSB2[5] 		// Device sub-class
	0x00,				//DeviceUSB2[6] 		// Device sub-sub-class
	0x40,				//DeviceUSB2[7] 		// Maximum packet size(64B)
	0xFD,				//DeviceUSB2[8] 		// 2 bytes Vendor ID
	0x13,				//DeviceUSB2[9] 		//
	0x10,				//DeviceUSB2[10]		// 2 bytes Product ID:
	0x39,				//DeviceUSB2[11] 		//
	0x00,				//DeviceUSB2[12			// Product version ID
	0x00,				//DeviceUSB2[13] 
	IMFC,				//DeviceUSB2[14] 		// iManufacture=01 (index of string describe manufacture)
	IPRODUCT,			//DeviceUSB2[15] 		// iProduct=02 (index of string describe product)
	ISERIAL,			//DeviceUSB2[16] 		// iSerial=03 (index of string describe dev serial #)
	0x01				//DeviceUSB2[17] 		// Number of configurations
};

   //config +interface Descriptor+Two endpoints descriptors
u8 xdata 	CfgUSB2[] = {
	9,				//CfgUSB2[0]				// config Descriptor length
	DESCTYPE_CFG,	//CfgUSB2[1] 				//DESCTYPE_CFG = 0x02
#ifdef UAS_EN
	9+9+7+7+9+28+16,//CfgUSB2[2] = 
#else
	9+9+7+7,		//CfgUSB2[2] = sizeof(desc.CfgUSB2) + sizeof(desc.InfUSB2) + sizeof(desc.EndPointUSB2); //total length of data return
#endif
	0x00,			//CfgUSB2[3] 
	0x01,			//CfgUSB2[4] 				// Number of interfaces
	0x01,			//CfgUSB2[5] 				// Configuration number
	ICONFIG,		//CfgUSB2[6] 				// iConfiguration=00 (index of string describe config)
	0xc0,			//CfgUSB2[7]				// Attributes = 1100 0000 (bit7 res=1 and bit6 =self power)
	0x01,			//CfgUSB2[8]				// Power comsumption of the USB device from the bus (2mA)
	9,				//BoTInfUSB2[0]				// interface Descriptor length
	DESCTYPE_INF,	//BoTInfUSB2[1]				// Descriptor type - Interface
	0x00,			//BoTInfUSB2[2] 				// Zero-based index of this interface
	0x00,			//BoTInfUSB2[3] 				// Alternate setting
	0x02,			//BoTInfUSB2[4] 				// Number of end points
	0x08,			//BoTInfUSB2[5] 				// MASS STORAGE Class.
	0x06,			//BoTInfUSB2[6] 				// 06: SCSI transplant Command Set 
	0x50,			//BoTInfUSB2[7] 				// BULK-ONLY TRANSPORT
	IINF,			//BoTInfUSB2[8]				//iInterface=00
	7,				//EndPointUSB2[0][0] = sizeof(desc.EndPointUSB2[0]);	// Descriptor length
	DESCTYPE_ENDPOINT,	//EndPointUSB2[0][1]	// Descriptor type - Endpoint
	0x8B,			//EndPointUSB2[0][2] 		// In Endpoint number 11
	0x02,			//EndPointUSB2[0][3] 		// Endpoint type (bit0-1) - Bulk
	0x00,			//EndPointUSB2[0][4] 		// 2 bytes Maximun packet size (bit0-10)
	0x02,			//EndPointUSB2[0][5] 
	0x00,			//EndPointUSB2[0][6] 		// Polling interval
	7,				//EndPointUSB2[1][0] = sizeof(desc.EndPointUSB2[0]);	// Descriptor length
	DESCTYPE_ENDPOINT,	//EndPointUSB2[1][1] 	// Descriptor type - Endpoint
	0x0A,			//EndPointUSB2[1][2] 		// Out Endpoint number 10
	0x02,			//EndPointUSB2[1][3]		// Endpoint type - Bulk
	0x00,			//EndPointUSB2[1][4]		// Maximun packet size
	0x02,			//EndPointUSB2[1][5] 
	0x00			//EndPointUSB2[1][6]		// Polling interval
#ifdef UAS_EN
	,9,				//UasInfUSB2[0] = sizeof(UasInfUSB2);	// uas interface Descriptor len			//32
	DESCTYPE_INF,	//UasInfUSB2[1] 			// Descriptor type - Interface
	0x00,			//UasInfUSB2[2] 			// Zero-based index of this interface
	0x01,			//UasInfUSB2[3] 			// Alternate setting
	0x04,			//UasInfUSB2[4] 			// Number of end points
	0x08,			//UasInfUSB2[5] 			// MASS STORAGE Class.
	0x06,			//UasInfUSB2[6] 			// 06: SCSI transplant Command Set 
	0x62,			//UasInfUSB2[7] 			// UAS
	IINF,			//UasInfUSB2[8] 			// iInterface=00
					//					xmemcpy(UasEndPointUSB3[0], (u8 *)OUTBUF+53, 7);
	7,				//UasEndPointUSB2[0][0]		//command out endpoint Desc len						// 41
	DESCTYPE_ENDPOINT,	//UasEndPointUSB2[0][1] 	// Descriptor type - Endpoint
	0x08,			//UasEndPointUSB2[0][2] 		// Command Out
	0x02,			//UasEndPointUSB2[0][3] 		// bulk
	0x00,			//UasEndPointUSB2[0][4] 		// 2 bytes Maximun packet size (bit0-10)
	0x02,			//UasEndPointUSB2[0][5] 
	0x00,			//UasEndPointUSB2[0][6] 		// Polling interval endpoint for data xfer
	0x04,			//Pipe[0][0] 					//Pipe length									//48
	0x24,			//Pipe[0][1] 					//type -- pipe
	0x01,			//Pipe[0][2] 					//ID -- command pipe
	0x00,			//Pipe[0][3] 					//reserved
					//
	7,				//UasEndPointUSB2[1][0] = 		// status pipe endpoint Descriptor len			//52
	DESCTYPE_ENDPOINT,	//UasEndPointUSB2[1][1]		// Descriptor type - Endpoint
	0x89,			//UasEndPointUSB2[1][2]			// Status In
	0x02,			//UasEndPointUSB2[1][3]			// Endpoint type - Bulk
	0x00,			//UasEndPointUSB2[1][4]			// Maximun packet size 512
	0x02,			//UasEndPointUSB2[1][5]
	0x00,			//UasEndPointUSB2[1][6]			// Polling interval
	0x04,			//Pipe[1][0] 					// pipe length									//59
	0x24,			//Pipe[1][1] = ;				// type -- pipe
	0x02,			//Pipe[1][2] = ;				// ID -- status pipe
	0x00,			//Pipe[1][3] = ;				// reserved				
					//
	7,				//UasEndPointUSB2[2][0]			// Data Out Endpoint Descriptor length			//63
	DESCTYPE_ENDPOINT,	//UasEndPointUSB2[2][1]		// Descriptor type - Endpoint
	0x0A,			//UasEndPointUSB2[2][2] 		// Out endpoint 10
	0x02,			//UasEndPointUSB2[2][3] 		// bulk
	0x00,			//UasEndPointUSB2[2][4] 		// Maximun packet size 512
	0x02,			//UasEndPointUSB2[2][5] 
	0x00,			//UasEndPointUSB2[2][6]			// Polling interval
	0x04,			//Pipe[2][0]					// pipe length									//70
	0x24,			//Pipe[2][1]					// type -- pipe
	0x04,			//Pipe[2][2]					// ID -- data out
	0x00,			//Pipe[2][3]					// reserve
					//
	7,				//UasEndPointUSB2[3][0]			// Data-In Endpoint Descriptor length			//74
	DESCTYPE_ENDPOINT,	//UasEndPointUSB3[3][1]		// Descriptor type - Endpoint
	0x8B,			//UasEndPointUSB2[3][2]
	0x02,			//UasEndPointUSB2[3][3]			// Endpoint type - Bulk
	0x00,			//UasEndPointUSB2[3][4]			// Maximun packet size 512
	0x02,			//UasEndPointUSB2[3][5]
	0x00,			//UasEndPointUSB2[3][6]			// Polling interval
	//
	0x04,			//Pipe[3][0]					//pipe length									//78
	0x24,			//Pipe[3][1]					//type -- pipe
	0x03,			//Pipe[3][2]					//ID -- data in
	0x00,			//Pipe[3][3]					//reserved
#endif
};

	// USB 3.0 device descriptor
u8 xdata DeviceUSB3[18] = {
	18,				//DeviceUSB3[0] = sizeof(desc.DeviceUSB3);	// Descriptor length
	DESCTYPE_DEVICE,//DeviceUSB3[1] 			// Descriptor type - Device
	0x10,			//DeviceUSB3[2] 			// Specification Version (BCD)
	0x03,			//DeviceUSB3[3]				// inidcate USB 3.0/Supper speed compilant
	0x00,			//DeviceUSB3[4] 			// Device class
	0x00,			//DeviceUSB3[5] 			// Device sub-class
	0x00,			//DeviceUSB3[6] 			// Device sub-sub-class
	0x09,			//DeviceUSB3[7] 			// Maximum packet size(512B)
	0xFD,			//DeviceUSB3[8] 			// 2 bytes Vendor ID:
	0x13,			//DeviceUSB3[9] 			//
	0x10,			//DeviceUSB3[10]  			// 2 bytes Product ID:
	0x39,			//DeviceUSB3[11]  ;			//
	0x00,			//DeviceUSB3[12]  			// Product version ID
	0x00,			//DeviceUSB3[13] 
	IMFC,			//DeviceUSB3[14] 			// iManufacture=01 (index of string describe manufacture)
	IPRODUCT,		//DeviceUSB3[15]			// iProduct=02 (index of string describe product)
	ISERIAL,		//DeviceUSB3[16] 			// iSerial=03 (index of string describe dev serial #)
	0x01			//DeviceUSB3[17] 			// Number of configurations
};

 // BOT: config +interface Descriptor+Two endpoints descriptors
u8 xdata 	CfgUSB3[] = {
	9,				//CfgUSB3[0] = sizeof(desc.CfgUSB3);								//0
	DESCTYPE_CFG,	//CfgUSB3[1]				//DESCTYPE_CFG = 0x02
#ifdef UAS_EN
	9+9+7+6+7+6+9+7+6+4 +7+6+4 +7+6+4 +7+6+4, //CfgUSB3[2] = sizeof(BOTInfUSB3) + sizeof(BOTEndPointUSB3) +sizeof(BOTSSEndPointComp)+
												//sizeof(InfUSB3) + sizeof(UasEndPointUSB3) +
												//sizeof(Pipe) + sizeof(UasSSEndPointComp);
#else
	9+9+7+6+7+6,	//CfgUSB3[2] = sizeof(desc.CfgUSB3) +sizeof(desc.BOTInfUSB3) +sizeof(desc.BotEndPointUSB3) +2 * sizeof(desc.BotSSEndPointComp); //total length of data return
#endif
	0x00,			//CfgUSB3[3]
	0x01,			//CfgUSB3[4]				// Number of interfaces
	0x01,			//CfgUSB3[5]				// Configuration number
	ICONFIG,		//CfgUSB3[6]				// iConfiguration=00 (index of string describe config)
	0xC0,			//CfgUSB3[7]				// Attributes = 1100 0000 (bit7 res=1 and bit6 =self power)
	1,				//CfgUSB3[8]				// Power comsumption of the USB device from the bus (8mA)
	9,				//CfgUSB3[9+0] BOTInfUSB3[0] = sizeof(desc.BOTInfUSB3);	// Descriptor length	//9
	DESCTYPE_INF,	//CfgUSB3[9+1] BOTInfUSB3[1]				// Descriptor type - Interface
	0x00,			//CfgUSB3[9+2] BOTInfUSB3[2]				// Zero-based index of this interface
	0x00,			//CfgUSB3[9+3] BOTInfUSB3[3]				// Alternate setting
	0x02,			//CfgUSB3[9+4] BOTInfUSB3[4]				// Number of end points
	0x08,			//CfgUSB3[9+5] BOTInfUSB3[5]				// MASS STORAGE Class.
	0x06,			//CfgUSB3[9+6] BOTInfUSB3[6]				// 06: SCSI transplant Command Set 
	0x50,			//CfgUSB3[9+7] BOTInfUSB3[7]				// BULK-ONLY TRANSPORT
	IINF,			//CfgUSB3[9+8] BOTInfUSB3[8]				// iInterface=00
	7,				//CfgUSB3[9+9+0]   BotEndPointUSB3[0][0]		// In Endpoint  Descriptor length		//18
	DESCTYPE_ENDPOINT,//CfgUSB3[9+9+1] BotEndPointUSB3[0][1]	// Descriptor type - Endpoint
	0x83,			//CfgUSB3[9+9+2] BotEndPointUSB3[0][2]		// In Endpoint number 11
	0x02,			//CfgUSB3[9+9+3] BotEndPointUSB3[0][3]		// Endpoint type (bit0-1) - Bulk
	0x00,			//CfgUSB3[9+9+4] BotEndPointUSB3[0][4]		// 2 bytes Maximun packet size (bit0-10)
	0x04,			//CfgUSB3[9+9+5] BotEndPointUSB3[0][5]
	0x00,			//CfgUSB3[9+9+6] BotEndPointUSB3[0][6]		// Polling interval endpoint for data xfer
	6,				//CfgUSB3[9+9+7+0] BotSSEndPointComp[0]		//USB SS Endpoint Companion Descriptor	//24
	DESCTYPE_ENDPOINT_COMP,	//CfgUSB3[9+9+7+1] BotSSEndPointComp[1] = ;
#ifdef	BURST_16K
	0x0F,			//CfgUSB3[9+9+7+2] BotSSEndPointComp[2]		// Max Burst : 15+1
#else
	0x07,			//CfgUSB3[9+9+7+2] BotSSEndPointComp[2]		// Max Burst: 7+1
#endif
	0x00,			//CfgUSB3[9+9+7+3] BotSSEndPointComp[3]		// Streaming is not supported.
	0x00,			//CfgUSB3[9+9+7+4] BotSSEndPointComp[4]		// Not a periodic endpoint.
	0x00,			//CfgUSB3[9+9+7+5] BotSSEndPointComp[5]
	7,				//CfgUSB3[9+9+7+6+0] BotEndPointUSB3[1][0]		// Descriptor length
	DESCTYPE_ENDPOINT,	//CfgUSB3[9+9+7+6+1] BotEndPointUSB3[1][1]	// Descriptor type - Endpoint
	0x0A,			//CfgUSB3[9+9+7+6+2] BotEndPointUSB3[1][2] 	// Endpoint number 10 and direction OUT
	0x02,			//CfgUSB3[9+9+7+6+3] BotEndPointUSB3[1][3] 	// Endpoint type - Bulk
	0x00,			//CfgUSB3[9+9+7+6+4] BotEndPointUSB3[1][4] 	// Maximun packet size
	0x04,			//CfgUSB3[9+9+7+6+5] BotEndPointUSB3[1][5] 
	0x00,			//CfgUSB3[9+9+7+6+6] BotEndPointUSB3[1][6] 	// Polling interval
	6,				//CfgUSB3[9+9+7+6+7+0] BotSSEndPointComp[0]  	//USB SS Endpoint Companion Descriptor
	DESCTYPE_ENDPOINT_COMP,	//CfgUSB3[9+9+7+6+7+1] BotSSEndPointComp[1]
#ifdef	BURST_16K
	0x0F,			//CfgUSB3[9+9+7+6+7+2] BotSSEndPointComp[2]		// Max Burst : 15+1
#else
	0x07,			//CfgUSB3[9+9+7+6+7+2] BotSSEndPointComp[2] 		// Max Burst: 7+1
#endif
	0x00,			//CfgUSB3[9+9+7+6+7+3] BotSSEndPointComp[3] 		// Streaming is not supported.
	0x00,			//CfgUSB3[9+9+7+6+7+4] BotSSEndPointComp[4] 		// Not a periodic endpoint.
	0x00			//CfgUSB3[9+9+7+6+7+5] BotSSEndPointComp[5] 
#ifdef UAS_EN
	,9,				//CfgUSB3[9+9+7+6+7+6+0] UasInfUSB3[0] = sizeof(UasInfUSB3);	// uas interface Descriptor len			//44
	DESCTYPE_INF,	//CfgUSB3[9+9+7+6+7+6+1] UasInfUSB3[1] 			// Descriptor type - Interface
	0x00,			//CfgUSB3[9+9+7+6+7+6+2] UasInfUSB3[2] 			// Zero-based index of this interface
	0x01,			//CfgUSB3[9+9+7+6+7+6+3] UasInfUSB3[3] 			// Alternate setting
	0x04,			//CfgUSB3[9+9+7+6+7+6+4] UasInfUSB3[4] 			// Number of end points
	0x08,			//CfgUSB3[9+9+7+6+7+6+5] UasInfUSB3[5] 			// MASS STORAGE Class.
	0x06,			//CfgUSB3[9+9+7+6+7+6+6] UasInfUSB3[6] 			// 06: SCSI transplant Command Set 
	0x62,			//CfgUSB3[9+9+7+6+7+6+7] UasInfUSB3[7] 			// UAS
	IINF,			//CfgUSB3[9+9+7+6+7+6+8] UasInfUSB3[8] 			// iInterface=00
					//					xmemcpy(UasEndPointUSB3[0], (u8 *)OUTBUF+53, 7);
	7,				//CfgUSB3[9+9+7+6+7+6+9+0] UasEndPointUSB3[0][0] = sizeof(UasEndPointUSB3[0]);	//command out end point Desc len		// 53
	DESCTYPE_ENDPOINT,	//CfgUSB3[9+9+7+6+7+6+9+1] UasEndPointUSB3[0][1] 	// Descriptor type - Endpoint
	0x08,			//CfgUSB3[9+9+7+6+7+6+9+2] UasEndPointUSB3[0][2] 		// Out direction IN: (bit7=0 OUT and bit7=1 IN endpoint))
	0x02,			//CfgUSB3[9+9+7+6+7+6+9+3] UasEndPointUSB3[0][3] 		// bulk
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+4] UasEndPointUSB3[0][4] 		// 2 bytes Maximun packet size (bit0-10)
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+5] UasEndPointUSB3[0][5] 
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+6] UasEndPointUSB3[0][6] 		// Polling interval endpoint for data xfer
	0x06,			//CfgUSB3[9+9+7+6+7+6+9+7+0] UasSSEndPointComp[0][0]		//USB SS Endpoint Companion Descriptor Len				//60
	DESCTYPE_ENDPOINT_COMP,	//CfgUSB3[9+9+7+6+7+6+9+7+1] UasSSEndPointComp[0][1] 
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+2] UasSSEndPointComp[0][2] 		// Max_Burst
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+3] UasSSEndPointComp[0][3] 		// Streaming is not supported.
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+4] UasSSEndPointComp[0][4] 		// Not a periodic endpoint.
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+5] UasSSEndPointComp[0][5] 
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+7+6+0] Pipe[0][0] 					//Pipe length									//66
	0x24,			//CfgUSB3[9+9+7+6+7+6+9+7+6+1] Pipe[0][1] 					//type -- pipe
	0x01,			//CfgUSB3[9+9+7+6+7+6+9+7+6+2] Pipe[0][2] 					//ID -- command pipe
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+3] Pipe[0][3] 					//reserved
					//
	7,				//CfgUSB3[9+9+7+6+7+6+9+7+6+4+0] UasEndPointUSB3[1][0] = 		// status pipe endpoint Descriptor len			//70
	DESCTYPE_ENDPOINT,	//CfgUSB3[9+9+7+6+7+6+9+7+6+4+1] UasEndPointUSB3[1][1]		// Descriptor type - Endpoint
	0x89,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+2] UasEndPointUSB3[1][2]
	0x02,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+3] UasEndPointUSB3[1][3]			// Endpoint type - Bulk
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+4] UasEndPointUSB3[1][4]			// Maximun packet size 1024
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+5] UasEndPointUSB3[1][5]
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+6] UasEndPointUSB3[1][6]			// Polling interval
	0x06,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+0] UasSSEndPointComp[1][0]		// USB SS Endpoint Companion Descriptor Len		//77
	DESCTYPE_ENDPOINT_COMP,	//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+1] UasSSEndPointComp[1][1]
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+2] UasSSEndPointComp[1][2] 		// Max Burst : 0+1 
	0x05,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+3] UasSSEndPointComp[1][3] 		// Max Stream : 32
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+4] UasSSEndPointComp[1][4] 		// Not a periodic endpoint.
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+5] UasSSEndPointComp[1][5] 
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+0] Pipe[1][0] 				// pipe length									//83
	0x24,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+1] Pipe[1][1]					// type -- pipe
	0x02,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+2] Pipe[1][2]					// ID -- status pipe
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+3] Pipe[1][3]					// reserved				
					//				xmemcpy(UasEndPointUSB3[2], (u8 *)OUTBUF+87, 7);
	7,				//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+0] UasEndPointUSB3[2][0]			// Data Out Endpoint Descriptor length			//87
	DESCTYPE_ENDPOINT,	//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+1] UasEndPointUSB3[2][1]		// Descriptor type - Endpoint
	0x0A,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+2] UasEndPointUSB3[2][2] 		// Out endpoint 10
	0x02,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+3] UasEndPointUSB3[2][3] 		// bulk
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+4] UasEndPointUSB3[2][4] 		// Maximun packet size 1024
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+5] UasEndPointUSB3[2][5] 
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+6] UasEndPointUSB3[2][6]			// Polling interval
	0x06,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+0] UasSSEndPointComp[2][0]		// USB SS Endpoint Companion Descriptor Len		//94
	DESCTYPE_ENDPOINT_COMP,	//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+1] UasSSEndPointComp[2][1]
#ifdef	BURST_16K
	0x0F,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+2] UasSSEndPointComp[2][2]		// Max Burst : 15+1
#else
	0x07,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+2] UasSSEndPointComp[2][2]		// Max Burst : 7+1
#endif
	0x05,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+3] UasSSEndPointComp[2][3]		// Max Stream : 32
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+4] UasSSEndPointComp[2][4]		// Not a periodic endpoint.
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+5] UasSSEndPointComp[2][5]
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+0] Pipe[2][0]					// pipe length									//100
	0x24,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+1] Pipe[2][1]					// type -- pipe
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+2] Pipe[2][2]					// ID -- data out
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+3] Pipe[2][3]					// reserve
					//
	7,				//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+0] UasEndPointUSB3[3][0]			// Data-In Endpoint Descriptor length			// 104
	DESCTYPE_ENDPOINT,	//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+1] UasEndPointUSB3[3][1]		// Descriptor type - Endpoint
	0x8B,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+2] UasEndPointUSB3[3][2]
	0x02,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+3] UasEndPointUSB3[3][3]			// Endpoint type - Bulk
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+4] UasEndPointUSB3[3][4]			// Maximun packet size 1024
	0x04,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+5] UasEndPointUSB3[3][5]
	0x00,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+6] UasEndPointUSB3[3][6]			// Polling interval
	0x06,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+7+0] UasSSEndPointComp[3][0]		//USB SS Endpoint Companion Descriptor Len		// 111
	DESCTYPE_ENDPOINT_COMP,	//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+7+1 UasSSEndPointComp[3][1] = ;
#ifdef	BURST_16K
	0x0F,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+7+2 UasSSEndPointComp[3][2]		// Max Burst : 15+1
#else
	0x07,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+7+3 UasSSEndPointComp[3][2]		// Max Burst : 7+1
#endif
	0x05,			//CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+7+3 UasSSEndPointComp[3][3]		// Max Stream : 32
	0x00,			//UasSSEndPointComp[3][4]		// Not a periodic endpoint.
	0x00,			//UasSSEndPointComp[3][5]
	//
	0x04,			//Pipe[3][0]					//pipe length															//117
	0x24,			//Pipe[3][1]					//type -- pipe
	0x03,			//Pipe[3][2]					//ID -- data in
	0x00,			//Pipe[3][3]					//reserved																// 121
#endif
};


// 	USB Binary Object Stor Descriptor
u8 xdata BOSS[5+7+10] = {
	5,					//BOSS[0] = sizeof(desc.BOSS);
	DESCTYPE_BOSS,		//BOSS[1] = ;				// BOSS Descriptor Type
	5+7+10,				//BOSS[2] = sizeof(BOSS) + sizeof(USB2ExtendCapabilties)+sizeof(desc.USB3Capabilties);	//total length of data return
	0,					//BOSS[3] = 0;
	2,					//BOSS[4] = 2;							// number of device Capabilties
	7,					//USB2ExtendCapabilties[0] = sizeof(desc.USB2ExtendCapabilties);
	DESCTYPE_DEV_CAP,	//USB2ExtendCapabilties[1]  			// Device Capability Descriptor Type
	0x02,				//USB2ExtendCapabilties[2] 				//USB 2.0 Extension Capability
	0x06,				//USB2ExtendCapabilties[3] 				// Link Power Management Supported
	0x00,				//USB2ExtendCapabilties[4] 
	0x00,				//USB2ExtendCapabilties[5] 
	0x00,				//USB2ExtendCapabilties[6] 
	10,					//USB3Capabilties[0] = sizeof(desc.USB3Capabilties);
	DESCTYPE_DEV_CAP,	//USB3Capabilties[1]  		// Device Capability Descriptor Type
	0x03,				//USB3Capabilties[2]  		// USB 3.0 Device Capability
	0x00,				//USB3Capabilties[3]  		// Does not support Latency Tolerance Messages
	0x0E,				//USB3Capabilties[4]		// USB 3.0 5Gb/s, USB 2.0 High and Full Speeds
	0x00,				//USB3Capabilties[5]  
	0x01,				//USB3Capabilties[6]		// All functionality supported down to USB 2.0 Full Speed
	0x0A,				//USB3Capabilties[7] 		// Less than 10us U1 Exit Latency
	0x00,				//USB3Capabilties[8] 		// Less than 512us U2 Exit Latency
	0x02				//USB3Capabilties[9] 
};

u8 xdata Str[4] = {
	4,				//0:size
	DESCTYPE_STR,	//1
	0x09,			//2
	0x04			//3
};


/****************************************\
	Ep0_2SendStatus
\****************************************/
void Ep0_2SendStatus(void)
{

//	DBG(("Ep0_2SendStatus\n"));

	*usb_Ep0TxLengh_0 = 0;
	*usb_Ep0TxLengh_1 = 0;

	// device ready to go to control status stage
	*usb_Ep0Ctrl = EP0_SRUN;
	*usb_Ep0CtrlClr = CTRL_STAT;
	while(1)
	{
		if ((*usb_Ep0Ctrl & EP0_SRUN) == 0x00)
			break;
		if (*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
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

			break;
		}
//		if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
//			break;
		if(USB_VBUS_OFF()) return;
	}
//	DBG(("end of ep0_2SendSatus!\n"));
}


/****************************************\
	Ep0_2Send
\****************************************/
void Ep0_2Send(u8 * ptr, u16 len)
{

	//*usb_Ep0CtrlClr = CTRL_DATA;
	*usb_Ep0TxLengh_0 = len;
	*usb_Ep0TxLengh_1 = len >> 8;
	*usb_Ep0BufLengh_0 = len;
	*usb_Ep0BufLengh_1 = len >> 8;

	xmemcpy(ptr, (u8 *)OUTBUF, len);
	
	//xfer data from data buffer to USB
	reg_w8(usb_Ep0Ctrl, EP0_RUN);
	while(1)
	{
		if ((*usb_Ep0Ctrl & EP0_RUN) == 0x00)
		{
			*usb_Ep0Ctrl = EP0_SRUN;
			*usb_Ep0CtrlClr = (CTRL_DATA|CTRL_STAT);
			return;
		}
		if (*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
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

			return;
		}
//		if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
//			return;
		if(USB_VBUS_OFF()) return;
	}

}

/****************************************\
	Ep0_2Send_direct
\****************************************/
void Ep0_2Send_direct(u16 len)
{

	*usb_Ep0TxLengh_0 = len;
	*usb_Ep0TxLengh_1 = len >> 8;
	*usb_Ep0BufLengh_0 = len;
	*usb_Ep0BufLengh_1 = len >> 8;

	//*usb_Ep0Ctrl = EP0_FLOW_CTRL;

	*usb_Ep0Ctrl = EP0_RUN;
//DBG(("E0\t"));
	while(1)
	{
		if ((*usb_Ep0Ctrl & EP0_RUN) == 0x00)
		{
			*usb_Ep0Ctrl = EP0_SRUN;
			*usb_Ep0CtrlClr = (CTRL_DATA|CTRL_STAT);
			return;
		}

		if (*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
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

			return;
		}
//		if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
//		{
//			return;
//		}
		if(USB_VBUS_OFF()) return;
	}
}


/****************************************\
	usb_control
\****************************************/
void usb_control()
{
	//u32 tmp;

/*
	if (Init_hardware_flag)
	{
		Init_hardware_flag = 0;
		Init_hardware_default();
	}
*/
//	reg_w32(cpu_wakeup_ctrl, CPU_USB_SUSPENDED|CPU_USB_UNSUSPENED);
	
	reg_w8(usb_Ep0CtrlClr, EP0_SETUP);	//c
	
	//DBG0(("%BX%BX%BX%BX\n", inPktSetup[0], inPktSetup[1], inPktSetup[2], inPktSetup[3]));
	
	//REQTYPE_DIR(setup) ((((setup)[BMREQTYPE]) & 0x80) >> 7) where BMREQTYPE=00h-Offset
	//where inPktSetup->6038h - 603Fh (8 bytes setup packet)
	//byte0.7 = 0: host to device(OUT) or byte0.7 = 1: device to host(IN)
	//Is byte0.7 = 00/01?
	//(1)
	if (REQTYPE_DIR(inPktSetup) == REQTYPE_D2H)		//REQTYPE_D2H=01h
	{

//DBG(("D2H\n")); 
/*
		DBG(("*************D2H***********\n"));
	
		for(i=0; i<8; i++)
		{
			DBG(("%BX ", inPktSetup[i]));
		}
		DBG(("************END***********\r"));*/

		//device to host
		//byte0.7 = 01 -> device to host:IN
		//byte 0: bit 5&6 - 0:standard, 1:class
		//(1.1) - standard
		switch (REQTYPE_TYPE(inPktSetup))
		{


		/****************************************\
			REQTYPE_STD
		\****************************************/
		case REQTYPE_STD:
			
//DBG(("\tREQTYPE_STD\n"));
			//Analyze byte 1 (bmRequestType): used USB spec. table9-3
			//used only 4:	BREQ_GET_STAT (0x00), BREQ_GET_DESC (0x06)
			//				BREQ_GET_CFG (0x08), BREQ_GET_INTERFACE	(0x0A)
			//(1.1.1)
			switch(inPktSetup[BREQ])	//BREQ=01h-Offset
			{
			/****************************************\
				BREQ_GET_STAT
			\****************************************/
			case BREQ_GET_STAT:    //(inPktSetup[BREQ=0x01])

//DBG(("\tBREQ_GET_STAT\n"));
				OUTBUF[0] = 0x00;
				OUTBUF[1] = 0x00;

				//REQTYPE_RECIP(setup) (((setup)[BMREQTYPE]) & 0x1F)
				//byte0.0-4 - 0:device, 1:interface, 2:endpoint, 3: other, 4...31: res
				switch(REQTYPE_RECIP(inPktSetup))
				{
				//(1.1.1.1) - REQTYPE_DEV=0x00 (standard request code Tbl 9-4)
				case REQTYPE_DEV:
					//DBG(("\tREQTYPE_DEV\n"));
					val = 0;
					//if ((globalNvram.USB_ATTR & 0x40))
					if (usb_self_power)
						val = 0x01;	  // self power

					tmp8 = *usb_DevStatus_0;
					if (tmp8 & U1_ENABLE)
						val |= B_BIT_2;			//U1 Enable;
					if (tmp8 & U2_ENABLE)
						val |= B_BIT_3;			//U2 Enable;
					OUTBUF[0] = val;
					Ep0_2Send_direct(2);
					return;

				//(1.1.1.2) - REQTYPE_INF=0x01 (descriptor type Tbl 9-5)
				case REQTYPE_INF:
//					DBG(("\tREQTYPE_INF\n"));
					if (*usb_Msc0Ctrl_1 & MSC_FUNC_SUSPEND)
						OUTBUF[0] = B_BIT_1;
					Ep0_2Send_direct(2);
					return;

				//(1.1.1.3) - REQTYPE_EP=0x02 (standard feature selector Tbl 9-6)
				//byte4.7 defined direction	- (i) byte4.7=0 as OUT endpoint
				//							- (ii) byte4.7=1 as IN endpoint
				//and byte4.0-3 defined endpoint number
				case REQTYPE_EP:
//DBG(("\tREQTYPE_EP\n"));
					switch(inPktSetup[WINDEX] & 0x7F) //clr direction bit
					{
					case 0x00:			// control endpoint
						if (*usb_Ep0IntEn & CTRL_HALT_STAT)
							OUTBUF[0] = 0x01;
						break;
					//byte4.7=0 imply OUT endpoint w/specific endpoint 1
					case 0x03:
					case 0x0B:
						if (*usb_Msc0DICtrl & MSC_DI_HALT)
							OUTBUF[0] = 0x01;
						break;
					//byte4.7=0 imply OUT endpoint w/specific endpoint 2
					case 0x0A:
						if (*usb_Msc0DOutCtrl & MSC_DOUT_HALT)
							OUTBUF[0] = 0x01;
						break;

	#ifdef UAS_EN
					case 0x01:			// Status endpoint
					case 0x09:			// Status endpoint
						if (*usb_Msc0StatCtrl & MSC_STAT_HALT)
							OUTBUF[0] = 0x01;
						break;

					case 0x08:			// Command  endpoint
						if (*usb_Msc0CmdCtrl & MSC_CMD_HALT)
							OUTBUF[0] = 0x01;
						break;
	#endif
					default:
						goto unsupported_request;
					}
					Ep0_2Send_direct(2);
					return;
				}
				break;

			/****************************************\
				BREQ_GET_DESC
			\****************************************/
			//(1.1.2) - continue (inPktSetup[BREQ]) where [BREQ=BREQ_GET_DESC=0x06]
			//		  - continue analyze byte 1 (bRequest): used USB spec. table9-3
			case BREQ_GET_DESC:
				
//DBG(("\tBREQ_GET_DESC\n"));
				//WLENGTH=0x06 (offset of packet 8 bytes format)
				if ((inPktSetup[WVALUE + 1] & 0xFF) == 0x7F)
				{
					usb_AdapterID = inPktSetup[WVALUE];
					usb_PortID0 = inPktSetup[WINDEX];
					usb_PortID1 = inPktSetup[WINDEX + 1];
					usb_PortID2 = inPktSetup[WLENGTH];
					Ep0_2Send_direct(1);
					return;
				}
				//tmp16 = *((u16 *)(inPktSetup + WLENGTH));
				tmp16 = (inPktSetup[WLENGTH] | (((u16)inPktSetup[WLENGTH + 1]) << 8));

				//1.1.2.1 - byte 2: used to pass a para to device according to request
				//WVALUE=0x02 (offset of packet 8 bytes format)
				switch(inPktSetup[WVALUE])  //descriptor index - lbyte of wvalue
				{							//descriptor type - hbyte of wvalue
				//Tbl 9-8
				case 0x00:	//(inPktSetup[WVALUE=0x00])

//					DBG(("\tValue: 0\n"));
					//analyze hbyte of WVALUE -> descriptor type
					switch(inPktSetup[WVALUE + 1])
					{
					//1.1.2.1.1  device descriptor
					//byte6=WLENGTH indicate number of bytes to trasnfer
					case DESCTYPE_DEVICE:	//inPktSetup[WVALUE + 1=DESCTYPE_DEVICE=0x01]

//MSG(("\tDESCTYPE_DEVICE\n"));
						tmp16 = Min(tmp16, sizeof(DeviceUSB2));
						if (*usb_DevStatus_0 & USB3_MODE)
							xmemcpy(DeviceUSB3, (u8 *)OUTBUF, tmp16);
						else
							xmemcpy(DeviceUSB2, (u8 *)OUTBUF, tmp16);
						Ep0_2Send_direct(tmp16);
						return;

					//1.1.2.1.2 get config descriptor
					case DESCTYPE_CFG:	//inPktSetup[WVALUE + 1=DESCTYPE_CFG=0x02]
					
//DBG(("\tDESCTYPE_CFG\n"));
						if (*usb_DevStatus_0 & USB3_MODE)
						{
							//DBG(("\tUSB3 MODE\n"));
							tmp16 = Min(tmp16, CfgUSB3[2]);
							if (tmp16 > 0)
							{

								{

									xmemcpy(CfgUSB3, (u8 *)OUTBUF, tmp16);
									//xmemcpy(desc.BOTInfUSB3, (u8 *)OUTBUF+9, 9);
									//xmemcpy(desc.BotEndPointUSB3[0], (u8 *)OUTBUF+18, 13);
									//xmemcpy(desc.BotSSEndPointComp, (u8 *)OUTBUF+31, 6);
									//xmemcpy(desc.BotEndPointUSB3[1], (u8 *)OUTBUF+37, 13);
									//xmemcpy(desc.BotSSEndPointComp, (u8 *)OUTBUF+50, 6);

								}
								
								if (!(*usb_DevState & USB_CONFIGURED))
								{
									// Is Bus Power
									if (!(OUTBUF[7] & 0x40))
									{	// Yes, set to 150 ma
										OUTBUF[8] = (150 >> 3);
									}
								}							
							}
						}
						else
						{
							//DBG(("\tNOT USB3 MODE\n"));
							tmp16 = Min(tmp16, CfgUSB2[2]);
							if (tmp16 > 0)
							{
								xmemcpy(CfgUSB2, (u8 *)OUTBUF, tmp16);
							
								if (!(*usb_DevState & USB_CONFIGURED))
								{
									// Is Bus Power
									if (!(OUTBUF[7] & 0x40))
									{	// Yes, set to 100 ma
										OUTBUF[8] = (100 >> 1);
									}
								}
								if ((*usb_DevStatus_0 & USB2_HS_MODE) == 0)	//full speed
								{
								#ifdef UAS_EN
									OUTBUF[2] = 9 +9 + 7 + 7;	// no USA for USB Full Speed
									tmp16 = Min(tmp16, 9 +9 + 7 + 7);
								#endif
									OUTBUF[22] = 0x40;
									OUTBUF[23] = 0x00;
									OUTBUF[29] = 0x40;
									OUTBUF[30] = 0x00;
								}							
							}
						}
						Ep0_2Send_direct(tmp16);
						return;

					//1.1.2.1.3  get string descriptor
					case DESCTYPE_STR:	//inPktSetup[WVALUE + 1=DESCTYPE_STR=0x03]
//DBG(("\tDESCTYPE_STR\n"));
						tmp16 = Min(tmp16, sizeof(Str));
						Ep0_2Send(Str, tmp16);
						return;

					//1.1.2.1.4 get device descriptor
					case DESCTYPE_DEV_QUAL:			// USB2.0 only
//						DBG(("\tDESCTYPE_DEV_QUAL\n"));
						if (*usb_DevStatus_0 & USB2_MODE)
						{
//							DBG(("\tUSB20 Mode\n"));
							xmemcpy(DeviceUSB2, (u8 *)OUTBUF, 10);
							OUTBUF[0] = 10;
							OUTBUF[1] = DESCTYPE_DEV_QUAL;
							OUTBUF[8] = DeviceUSB2[17];
							OUTBUF[9] = 0x00;
							tmp16 = Min(tmp16, 10);
							Ep0_2Send_direct(tmp16);
							return;
						}
						break;

					//1.1.2.1.5 get config descriptor
					//inPktSetup[WVALUE + 1=DESCTYPE_OTHER_SPD_CFG=0x07]
					case DESCTYPE_OTHER_SPD_CFG:
//DBG(("\tDESCTYPE_OTHER_SPD_CFG\n"));
						if (*usb_DevStatus_0 & USB2_MODE)
						{
//							DBG(("\tUSB20 Mode\n"));
							
							tmp16 = Min(tmp16, CfgUSB2[2]);
							if (tmp16 > 0)
							{
//								DBG(("\ttmp > 0\n"));
								xmemcpy(CfgUSB2, (u8 *)OUTBUF, tmp16);
								OUTBUF[1] = DESCTYPE_OTHER_SPD_CFG;

								if (!(*usb_DevState & USB_CONFIGURED))
								{
									// Is Bus Power
									if (!(OUTBUF[7] & 0x40))
									{	// Yes, set to 100 ma
										OUTBUF[8] = (100 >> 1);
									}

								}
								if (*usb_DevStatus_0 & USB2_HS_MODE)
								{
								#ifdef UAS_EN
									OUTBUF[2] = 9 +9 + 7 + 7;	// no USA for USB Full Speed
									tmp16 = Min(tmp16, 9 +9 + 7 + 7);
								#endif
									OUTBUF[22] = 0x40;
									OUTBUF[23] = 0x00;
									OUTBUF[29] = 0x40;
									OUTBUF[30] = 0x00;
								}							
							}
							Ep0_2Send_direct(tmp16);
							return;
						}
						break;

					case DESCTYPE_ENDPOINT_COMP:			// USB3.0 only
						//get endpoint descriptor
//DBG(("\tDESCTYPE_ENDPOINT_COMP\n"));
						if (*usb_DevStatus_0 & USB3_MODE)
						{
//							DBG(("\tUSB30\n"));
//							xmemcpy((&desc.BotSSEndPointComp[0]), OUTBUF, 6);
							xmemcpy((u8 *)(&CfgUSB3[24]),  (u8 *)OUTBUF, 6);
							tmp16 = Min(tmp16, 6);
							Ep0_2Send_direct(tmp16);
							return;
						}
						break;

					case DESCTYPE_BOSS:			// USB3.0 & USB2.1
						//get BOSS descriptor
						{
//							DBG(("\tUSB30\n"));
							tmp16 = Min(tmp16, BOSS[2]);
							xmemcpy(BOSS, (u8 *)OUTBUF, tmp16);
							Ep0_2Send_direct(tmp16);
							return;
						}
					default:
							break;

					}	// end of case 0x00:	//(inPktSetup[WVALUE=0x00])


				case IMFC:	//(inPktSetup[WVALUE=0x01])

//					DBG(("\tIMFC\n"));
					switch(inPktSetup[WVALUE + 1]) //descriptor type
					{
					case DESCTYPE_STR:
						{
//							DBG(("\tDESCTYPE_STR\n"));
							tmp16 = Min(tmp16, Mfc[0]);
							Ep0_2Send(Mfc, tmp16);
							return;
						}
					}
					break;

				case IPRODUCT:	//(inPktSetup[WVALUE=0x02])

//DBG(("\tIPRODUCT\n"));
					switch(inPktSetup[WVALUE + 1]) //descriptor type
					{
					case DESCTYPE_STR:
						{
//DBG(("\tDESCTYPE_STR\n"));
						
							tmp16 = Min(tmp16, Product[0]);
							Ep0_2Send(Product, tmp16);
							return;
						}
					}
					break;

				case ISERIAL:	//(inPktSetup[WVALUE=0x03])

//DBG(("\tISERIAL\n"));

					switch(inPktSetup[WVALUE + 1]) //descriptor type
					{
						case DESCTYPE_STR:
						{
//							DBG(("\tDESCTYPE_STR\n"));
							tmp16 = Min(tmp16, Serial[0]);

							Ep0_2Send(Serial, tmp16);
							return;
						}
					}
					break;



				default:

						break;
				}
				
				break;	// End of switch(inPktSetup[WVALUE])



			}	// End of switch(inPktSetup[BREQ])
			break;

		/****************************************\
			REQTYPE_CLASS
		\****************************************/
		//(1.2) - class
		case REQTYPE_CLASS:

//DBG(("\tREQTYPE_CLASS\n"));

			if (REQTYPE_RECIP(inPktSetup) == REQTYPE_INF)
			{
//				DBG(("\tREQTYPE_INF\n"));
				
				switch(inPktSetup[BREQ])
				{
				case BREQ_GET_MAX_LUN:		// 0xFE:
//					DBG(("\tBREQ_GET_MAX_LUN\n"));
					if ((inPktSetup[WVALUE] == 0) &&
						(inPktSetup[WINDEX] == 0) &&
						(inPktSetup[WLENGTH] == 1))
					{
//						DBG(("\tV:0, I:0, L:1\n"));
						//support single only
						OUTBUF[0] = 0;
						Ep0_2Send_direct(1);
						return;
					}
					else
					{
						break;
					}
				}
			}
			break;
		}	// End of switch (REQTYPE_TYPE(inPktSetup))
	 }	// End device to host
	 else	// REQTYPE_H2D	 
	 {

		//DBG(("H2D\n")); 
/*		DBG("*************H2D***********\n");

			for(i=0; i<8; i++)
			{
				DBG(("%BX ", inPktSetup[i]));
			}
		DBG(("************END***********\r"));*/
		
		//host to device
		switch (REQTYPE_TYPE(inPktSetup))
		{
		/****************************************\
			REQTYPE_STD
		\****************************************/
		case REQTYPE_STD:
			
//			DBG("\tREQTYPE_STD\n");
			switch(inPktSetup[BREQ])
			{
#if 1
			/****************************************\
				BREQ_CLR_FEATURE
			\****************************************/
			case BREQ_CLR_FEATURE:
				switch(REQTYPE_RECIP(inPktSetup))
				{
				case REQTYPE_DEV:
					switch(inPktSetup[WVALUE]) //feaure Selector
					{
					case FEAT_U1_ENABLE:			//
						if ((*usb_DevState & USB_CONFIGURED) == 0)
							goto unsupported_request;

						*usb_DevStatus_0 &= (~U1_ENABLE);
						break;

					case FEAT_U2_ENABLE:			// 
						if ((*usb_DevState & USB_CONFIGURED) == 0)
							goto unsupported_request;
						*usb_DevStatus_0 &= (~U2_ENABLE);
						usb3_u2_enable = 0;
						break;

					default:
						goto unsupported_request;
					}
					Ep0_2SendStatus();
					return;

				case REQTYPE_INF:
					switch(inPktSetup[WVALUE]) //feaure Selector
					{
					case FUNCTION_SUSPEND:			// 
						*usb_Msc0Ctrl_1 &= ~MSC_FUNC_SUSPEND;
						Ep0_2SendStatus();
						usb3_function_suspend = 0;
						usb3_device_U2_state = 0;
						return;

					default:
						goto unsupported_request;
					}

				case REQTYPE_EP:
					switch(inPktSetup[WINDEX] & 0x7F) //endpoint number
					{
					case 0x00:			// control endpoint
						*usb_Ep0IntEn &= (~CTRL_HALT_STAT);
						break;

					case 0x03:			// Bulk-In  endpoint
					case 0x0B:			// Bulk-In  endpoint
//						DBG(("\tBulk-In\n"));
#ifdef DBG_UNDRUN
						MSG(("\tBulk-In clr1212\n"));
#endif

						*usb_MscClrFea_Halt = MSC_DATAIN_CLRFEAT_HALT;
					//#ifdef INTEL_SEQNUM_PATCH
					#if 0
						if (intel_SeqNum_monitor & INTEL_SEQNUM_CHECK_CONDITION)
						{
							intel_host_flag = 1;
							intel_SeqNum_Monitor_Count = 0;
							intel_SeqNum_monitor = 0;
							MSG("ITL h\n");
						}
					#endif
						break;

					case 0x0A:			// Bulk-out  endpoint
//						DBG(("\tBulk-Out\n"));
						*usb_MscClrFea_Halt = MSC_DATAOUT_CLRFEAT_HALT;
					#if 0
					//#ifdef INTEL_SEQNUM_PATCH
						if (intel_SeqNum_monitor & INTEL_SEQNUM_CHECK_CONDITION)
						{
							intel_host_flag = 1;
							intel_SeqNum_Monitor_Count = 0;
							intel_SeqNum_monitor = 0;
							MSG("ITL h\n");
						}
					#endif
					#ifdef AMD_CLR_FEATURE_PATCH
						if ((bot_mode == 0))
						{	// UAS Mode
							if (usb_curCtxt != CTXT_NULL)
							{	//Current Command Ctxt is active
								SCTXT_INDEX = usb_curCtxt;
								if ((SCTXT_Flag & SCTXT_FLAG_DIN) == 0)
								{	// Current Command Ctxt is DO

									// reset DO
									ctxt_site = usb_curCtxt;
									usb_rst_do();

									// push ctxt site to free FIFo
									uas_push_ctxt_site_to_free_fifo();
									if (usb_post_dout_ctxt == usb_curCtxt)
									{
										usb_post_dout_ctxt = CTXT_NULL;
									}
									usb_curCtxt = CTXT_NULL;

									//abort all commands received									usb_curCtxt = CTXT_NULL;
									sata_abort_all();
									uas_abort_all();
								}
							}
						}
					#endif
						break;
	#ifdef UAS_EN
					case 0x01:			// Status endpoint
					case 0x09:			// Status endpoint
						*usb_MscClrFea_Halt = MSC_STAT_CLRFEAT_HALT;
						break;

					case 0x08:			// Command  endpoint
						*usb_MscClrFea_Halt = MSC_CMD_CLRFEAT_HALT;
						break;
	#endif
					}
					Ep0_2SendStatus();
					return;
				}
				
				break;

				DBG(("\tclear feature\n"));

				//return OK for now
				Ep0_2SendStatus();
				return;
#endif


			/****************************************\
				BREQ_SET_INTERFACE
			\****************************************/
#ifdef UAS_EN
			case BREQ_SET_INTERFACE:
				DBG(("\tS_INF\n"));
				if ((*usb_DevState & USB_CONFIGURED) == 0)
					break;

				//get Alternate Setting
				tmp8 = inPktSetup[2];
				if (tmp8 > 1)
				{
					break;
				}
				else if (tmp8 == 1)
				{	// UAS
					DBG(("\tU\n"));
					*usb_Msc0Ctrl_1 |= (MSC_SET_INTFC_RESET|MSC_ALTERNATE);
					if (sobj_ncq_mode)
						*usb_Msc0Lun_SAT_0 = (*usb_Msc0Lun_SAT_0 & ~SAT_CMD) | SATA_NCQ_CMD;
					else
						*usb_Msc0Lun_SAT_0 = (*usb_Msc0Lun_SAT_0 & ~SAT_CMD) | SATA_DMAE_CMD;
					bot_mode = 0;
					*usb_CtxtSize_1 = RST_CTXT_MEM;
					if (usb_curCtxt != usb_curCtxt)
					{
						//usb_curCtxt = CTXT_NULL;
						DBG(("\t%BX\n", usb_curCtxt));
					}
				}
				else //if (tmp8 == 0)
				{	// BOT
					DBG(("\tB\n"));
					*usb_Msc0Ctrl_1 = (*usb_Msc0Ctrl_1 & (~MSC_ALTERNATE)) | MSC_SET_INTFC_RESET;
					*usb_Msc0Lun_SAT_0 = (*usb_Msc0Lun_SAT_0 & ~SAT_CMD) | SATA_DMAE_CMD;
					bot_mode = 1;
				}
			#ifdef USB_FAST_ENUM
				if (usbMode == CONNECT_USB3)
				{
					if (sobj_init == 0)
					{
						if (sobj_State == SATA_DRV_RDY)
						{
							if (ata_fast_enum_init())
								return;
						}
					}
				}
			#endif
				Ep0_2SendStatus();
				return;
#endif


			/****************************************\
				BREQ_SET_FEATURE
			\****************************************/
			case BREQ_SET_FEATURE:
				
//				DBG("\tBREQ_SET_FEATURE\n");
				switch(REQTYPE_RECIP(inPktSetup))
				{
				case REQTYPE_DEV:

//					DBG("\tREQTYPE_DEV\n");
					switch ( inPktSetup[WVALUE])
					{
					case FEAT_SEL_TEST_MODE:
//						DBG("\tFEAT_SEL_TEST_MODE\n");
						switch(inPktSetup[WINDEX + 1])
						{
						case TEST_J:
//							DBG("\tTEST_J\n");
							Ep0_2SendStatus();
							reg_w8(usb_USB2TestMode, TM_TEST_J);
								;

						case TEST_K:
//							DBG("\tTEST_K\n");
							Ep0_2SendStatus();
							reg_w8(usb_USB2TestMode, TM_TEST_K);
							while(1)
								;

						case TEST_SE0_NAK:
							//dont set any run bits
//							DBG("\tTEST_SE0_NAK\n");
							Ep0_2SendStatus();
							reg_w8(usb_USB2TestMode, TM_TEST_SE0_NAK);
							while(1)
								;


						case TEST_PACKET:
						{

							DBG(("\tTEST_PACKET\n"));

							Ep0_2SendStatus();


//	                                          *usb_USB3StateSelect = 0x86; //control sequence state machine
                                                 
//							*usb_USB3StateCtrl = 0x0C; //						

					
							OUTBUF[0] = 0x00;
							OUTBUF[1] = 0x00;
							OUTBUF[2] = 0x00;
							OUTBUF[3] = 0x00;
							OUTBUF[4] = 0x00;
							OUTBUF[5] = 0x00;
							OUTBUF[6] = 0x00;
							OUTBUF[7] = 0x00;
							OUTBUF[8] = 0x00;
							OUTBUF[9] = 0xAA;
							OUTBUF[0xa] = 0xAA;
							OUTBUF[0xb] = 0xAA;
							OUTBUF[0xc] = 0xAA;
							OUTBUF[0xd] = 0xAA;
							OUTBUF[0xe] = 0xAA;
							OUTBUF[0xf] = 0xAA;
							OUTBUF[0x10] = 0xAA;
							OUTBUF[0x11] = 0xEE;
							OUTBUF[0x12] = 0xEE;
							OUTBUF[0x13] = 0xEE;
							OUTBUF[0x14] = 0xEE;
							OUTBUF[0x15] = 0xEE;
							OUTBUF[0x16] = 0xEE;
							OUTBUF[0x17] = 0xEE;
							OUTBUF[0x18] = 0xEE;
							OUTBUF[0x19] = 0xFE;
							OUTBUF[0x1A] = 0xFF;
							OUTBUF[0x1B] = 0xFF;
							OUTBUF[0x1C] = 0xFF;
							OUTBUF[0x1D] = 0xFF;
							OUTBUF[0x1E] = 0xFF;
							OUTBUF[0x1F] = 0xFF;
							OUTBUF[0x20] = 0xFF;
							OUTBUF[0x21] = 0xFF;
							OUTBUF[0x22] = 0xFF;
							OUTBUF[0x23] = 0xFF;
							OUTBUF[0x24] = 0xFF;
							OUTBUF[0x25] = 0x7F;
							OUTBUF[0x26] = 0xBF;
							OUTBUF[0x27] = 0xDF;

							OUTBUF[0x28] = 0xEF;
							OUTBUF[0x29] = 0xF7;
							OUTBUF[0x2A] = 0xFB;
							OUTBUF[0x2B] = 0xFD;
							OUTBUF[0x2C] = 0xFC;
							OUTBUF[0x2D] = 0x7E;
							OUTBUF[0x2E] = 0xBF;
							OUTBUF[0x2F] = 0xDF;
							OUTBUF[0x30] = 0xEF;
							OUTBUF[0x31] = 0xF7;
							OUTBUF[0x32] = 0xFB;
							OUTBUF[0x33] = 0xFD;
							OUTBUF[0x34] = 0x7E;						
						

							*usb_Ep0CtrlClr = CTRL_DATA;
							
							*usb_USB2TestMode = TM_TEST_PACKET;


							*usb_Ep0TxLengh_0 = 53;
							*usb_Ep0TxLengh_1 = 0;
							*usb_Ep0BufLengh_0 = 53;
							*usb_Ep0BufLengh_1 = 0;
							*usb_Ep0Ctrl = EP0_RUN;

							while(1)
							{
								if (USB_VBUS_OFF())
									return;
							}
						}
						

						case TEST_FORCE_ENABLE:
//							DBG("\tTEST_FORCE_ENABLE\n");
							Ep0_2SendStatus();
							break;

						default:
							goto unsupported_request;
							
						}
					case FEAT_U1_ENABLE:
						if ((*usb_DevState & USB_CONFIGURED) == 0)
							goto unsupported_request;
						*usb_DevStatus_0 |= U1_ENABLE;
						Ep0_2SendStatus();
						return;

					case FEAT_U2_ENABLE:
						if ((*usb_DevState & USB_CONFIGURED) == 0)
							goto unsupported_request;
						*usb_DevStatus_0 |= U2_ENABLE;
						usb3_u2_enable = 1;
						Ep0_2SendStatus();
						return;

					default:
						goto unsupported_request;
					}

					break;

				case REQTYPE_INF:
					switch(inPktSetup[WVALUE]) //feaure Selector
					{
					case FUNCTION_SUSPEND:			// 
						*usb_Msc0Ctrl_1 |= MSC_FUNC_SUSPEND;
						Ep0_2SendStatus();
						usb3_function_suspend = 1;
						usb3_device_U2_state = 0;
#ifdef DEBUG_LTSSM
						usb3_U2_inactivity_timer = 300;
#else
						usb3_U2_inactivity_timer = 30;
#endif
						return;

					default:
						goto unsupported_request;
					}

				case REQTYPE_EP:
					
//					DBG("\tREQTYPE_EP\n");
					//ep in inPktSetup[WINDEX]
					switch(inPktSetup[WINDEX] & 0x7F) //clr direction bit
					{				
					case 0x00:			// control endpoint
						*usb_Ep0Ctrl = EP0_HALT;
						break;

					case 0x03:			// Bulk-In  endpoint
					case 0x0B:
//						DBG(("\tBulk-In\n"));
						*usb_Msc0DICtrl = MSC_DI_HALT;
						break;

					case 0x0A:			// Bulk-out  endpoint
//						DBG(("\tBulk-Out\n"));
						*usb_Msc0DOutCtrl = MSC_DOUT_HALT;
						break;
	#ifdef UAS_EN
					case 0x01:			// UAS Status endpoint
					case 0x09:			// UAS Status endpoint
						*usb_Msc0StatCtrl =  MSC_STAT_HALT;
						break;

					case 0x08:			// UAS Command  endpoint
						*usb_Msc0CmdCtrl = MSC_CMD_HALT;
						break;
	#endif
					}
					Ep0_2SendStatus();
					return;
				}
				break;

			/****************************************\
				BREQ_SET_CFG
			\****************************************/
#if 0
			case BREQ_SET_CFG:
			case BREQ_SET_INTERFACE:

				//DBG(("\nSet interface%bx", inPktSetup[BREQ+1]));
				//curMscMode = inPktSetup[BREQ+1];

				//return OK for now
				Ep0_2SendStatus();
				return;
#endif

			case BREQ_SET_ISOCHRONOUS_DELAY:
				Ep0_2SendStatus();
				return;				
			}
			break;

		/****************************************\
			REQTYPE_CLASS
		\****************************************/
		case REQTYPE_CLASS:

//			DBG(("\tREQTYPE_CLASS\n"));
			switch(inPktSetup[BREQ])
			{
			case BREQ_SET_IDLE:		// 0x0A:
//				DBG(("\tBREQ_SET_IDLE\n"));
				//what is this?
				Ep0_2SendStatus();
				return;
			}
			break;

		} // End of switch (REQTYPE_TYPE(inPktSetup))

	}	// End of REQTYPE_DIR(inPktSetup) D2H and H2D Case


//	DBG0(("Bad SETUP\n"));
unsupported_request:
	//unsupport SETUP packet
	reg_w8(usb_Ep0Ctrl, EP0_HALT);
#if 0
	while(1)
	{
	
		if (*usb_IntStatus_shadow_0 & (HOT_RESET|WARM_RESET|USB_BUS_RST))
		{
				//DBG(("\t-1\n"));
				return;
		}

		//if (*usb_Msc0IntStatus_0 & BOT_RST_INT)
		//		return;

		if (reg_r8(usb_Ep0Ctrl) & EP0_SETUP)
		{
//			DBG(("\t-2\n"));
			reg_w8(usb_Ep0CtrlClr, EP0_HALT);
			return;
		}

		if(USB_VBUS_OFF())
		{
//			DBG(("\t-3\n"));
			reg_w8(usb_Ep0CtrlClr, EP0_HALT);
			return;
		}
		
	}
#endif	
}

/****************************************\
	Text16
\****************************************/
void Text16(u8 xdata *src, u8 xdata *dest, u8 len)
{
	u8	i;

	for (i = 0; i < len; i++)
	{
		*dest++ = *src++;
		*dest++ = 0x00;
	}
}

/****************************************\
	Hex2Text16
\****************************************/
#if 0
void Hex2Text16(u8 xdata *src, u8 xdata *dest, u8 len)
{
	u8	i;

	for (i = 0; i < len; i++)
	{
		u8 val;

		val = *src >> 4;
		if (val >= 0x0A)
			*dest = 'A' - 10 + val;
		else
			*dest = '0' + val;

		dest++;
		*dest++ = 0x00;

		val = *src & 0x0f;
		if (val >= 0x0A)
			*dest = 'A' - 10 + val;
		else
			*dest = '0' + val;

		dest++;
		*dest++ = 0x00;

		src++;
	}
}
#endif

/****************************************\
	InitDesc
\****************************************/
void InitDesc(void)			//DESCBUF xdata desc
{
	//USB2.1 device descriptor
	//DeviceUSB2[0] = sizeof(desc.DeviceUSB2);	// Descriptor length
	//DeviceUSB2[1] = DESCTYPE_DEVICE;			// Descriptor type - Device
	//DeviceUSB2[2] = 0x10;						// Specification Version (BCD)
	//DeviceUSB2[3] = 0x02;						// inidcate USB 2.0/Supper speed compilant
	//DeviceUSB2[4] = 0x00;						// Device class
	//DeviceUSB2[5] = 0x00;						// Device sub-class
	//DeviceUSB2[6] = 0x00;						// Device sub-sub-class
	//DeviceUSB2[7] = 0x40;						// Maximum packet size(64B)
	DeviceUSB2[8] = globalNvram.USB_VID[1];		// 2 bytes Vendor ID:
	DeviceUSB2[9] = globalNvram.USB_VID[0];		//
	DeviceUSB2[10] = globalNvram.USB_PID[1];	// 2 bytes Product ID:
	DeviceUSB2[11] = globalNvram.USB_PID[0];	//
	DeviceUSB2[12] = NV_VERSION(1);				// Product version ID
	DeviceUSB2[13] = NV_VERSION(0);
	//DeviceUSB2[14] = IMFC;					// iManufacture=01 (index of string describe manufacture)
	//DeviceUSB2[15] = IPRODUCT;				// iProduct=02 (index of string describe product)
	//DeviceUSB2[16] = ISERIAL;					// iSerial=03 (index of string describe dev serial #)
	//DeviceUSB2[17] = 0x01;					// Number of configurations

    //USB2.1 config descriptor
	//CfgUSB2[0] = sizeof(desc.CfgUSB2);
	//CfgUSB2[1] = DESCTYPE_CFG;					//DESCTYPE_CFG = 0x02
	//CfgUSB2[2] = sizeof(desc.CfgUSB2) + sizeof(desc.InfUSB2) + sizeof(desc.EndPointUSB2); //total length of data return
	//CfgUSB2[3] = 0x00;
	//CfgUSB2[4] = 0x01;							// Number of interfaces
	//CfgUSB2[5] = 0x01;							// Configuration number
	//CfgUSB2[6] = ICONFIG;						// iConfiguration=00 (index of string describe config)
	CfgUSB2[7] = (globalNvram.USB_ATTR & 0x40) | 0x80;	// Attributes = 1100 0000 (bit7 res=1 and bit6 =self power)
	if (globalNvram.USB2_PWR > (500/2))
		CfgUSB2[8] = 500/2;					// Power comsumption of the USB device from the bus (2mA)
	else
		CfgUSB2[8] = globalNvram.USB2_PWR;		// Power comsumption of the USB device from the bus (2mA)

	//interface descriptor
	//CfgUSB2[9+0] = sizeof(desc.InfUSB2);		// Descriptor length
	//CfgUSB2[9+1] = DESCTYPE_INF;			// Descriptor type - Interface
	//CfgUSB2[9+2] = 0x00;					// Zero-based index of this interface
	//CfgUSB2[9+3] = 0x00;					// Alternate setting
	//CfgUSB2[9+4] = 0x02;					// Number of end points
	//CfgUSB2[9+5] = 0x08;					// MASS STORAGE Class.
	//CfgUSB2[9+6] = 0x06;					// 06: SCSI transplant Command Set 
#ifdef ATAPI
	if (sobj_class == DEVICECLASS_ATAPI)
	{
		CfgUSB2[9+6] = 0x02;					//  02: MMC-5 (ATAPI) 
	}
	else
	{
		CfgUSB2[9+6] = 0x06;					// 06: SCSI transplant Command Set 
	}
#endif
											// 02: ATAPI
	//CfgUSB2[9+7] = 0x50;					// BULK-ONLY TRANSPORT
	//CfgUSB2[9+8] = IINF;					//iInterface=00

	//EndPointUSB2[0][0] = sizeof(desc.EndPointUSB2[0]);	// Descriptor length
	//EndPointUSB2[0][1] = DESCTYPE_ENDPOINT;	// Descriptor type - Endpoint
	//EndPointUSB2[0][2] = 0x8B;			// Endpoint number 11
										// direction IN: (bit7=0 OUT and bit7=1 IN endpoint))
	//EndPointUSB2[0][3] = 0x02;			// Endpoint type (bit0-1) - Bulk
	//EndPointUSB2[0][4] = 0x00;			// 2 bytes Maximun packet size (bit0-10)
	//EndPointUSB2[0][5] = 0x02;
	//EndPointUSB2[0][6] = 0x00;			// Polling interval endpoint for data xfer

	//EndPointUSB2[1][0] = sizeof(desc.EndPointUSB2[0]);	// Descriptor length
	//EndPointUSB2[1][1] = DESCTYPE_ENDPOINT;	// Descriptor type - Endpoint
	//EndPointUSB2[1][2] = 0x0A;			// Endpoint number 10 and direction OUT
	//EndPointUSB2[1][3] = 0x02;			// Endpoint type - Bulk
	//EndPointUSB2[1][4] = 0x00;			// Maximun packet size
	//EndPointUSB2[1][5] = 0x02;
	//EndPointUSB2[1][6] = 0x00;			// Polling interval

	// USB 3.0 device descriptor
	//DeviceUSB3[0] = sizeof(desc.DeviceUSB3);	// Descriptor length
	//DeviceUSB3[1] = DESCTYPE_DEVICE;			// Descriptor type - Device
	//DeviceUSB3[2] = 0x00;						// Specification Version (BCD)
	//DeviceUSB3[3] = 0x03;						// inidcate USB 3.0/Supper speed compilant
	//DeviceUSB3[4] = 0x00;						// Device class
	//DeviceUSB3[5] = 0x00;						// Device sub-class
	//DeviceUSB3[6] = 0x00;						// Device sub-sub-class
	//DeviceUSB3[7] = 0x09;						// Maximum packet size(512B)
	DeviceUSB3[8] = globalNvram.USB_VID[1];	// 2 bytes Vendor ID:
	DeviceUSB3[9] = globalNvram.USB_VID[0];	//
	DeviceUSB3[10] = globalNvram.USB_PID[1];	// 2 bytes Product ID:
	DeviceUSB3[11] = globalNvram.USB_PID[0];	//
	DeviceUSB3[12] = NV_VERSION(1);			// Product version ID
	DeviceUSB3[13] = NV_VERSION(0);
	//DeviceUSB3[14] = IMFC;					// iManufacture=01 (index of string describe manufacture)
	//DeviceUSB3[15] = IPRODUCT;				// iProduct=02 (index of string describe product)
	//DeviceUSB3[16] = ISERIAL;				// iSerial=03 (index of string describe dev serial #)
	//DeviceUSB3[17] = 0x01;					// Number of configurations




	{
		//config descriptor for BOT only
	//	CfgUSB3[0] = sizeof(desc.CfgUSB3);
	//	CfgUSB3[1] = DESCTYPE_CFG;					//DESCTYPE_CFG = 0x02
	//	CfgUSB3[2] = sizeof(desc.CfgUSB3) +
	//						sizeof(desc.BOTInfUSB3) +
	//						sizeof(desc.BotEndPointUSB3) +
	//						2 * sizeof(desc.BotSSEndPointComp); //total length of data return
	//	CfgUSB3[3] = 0x00;
	//	CfgUSB3[4] = 0x01;							// Number of interfaces
	//	CfgUSB3[5] = 0x01;							// Configuration number
	//	CfgUSB3[6] = ICONFIG;						// iConfiguration=00 (index of string describe config)
		CfgUSB3[7] = (globalNvram.USB_ATTR & 0x40) | 0x80;	// Attributes = 1100 0000 (bit7 res=1 and bit6 =self power)
		if (globalNvram.USB3_PWR > (900/8))
			CfgUSB3[8] = 900/8;					// Power comsumption of the USB device from the bus (8mA)
		else
			CfgUSB3[8] = globalNvram.USB3_PWR;		// Power comsumption of the USB device from the bus (8mA)


#ifdef A71_PATCH
	if (revision_a71)
	{
		CfgUSB3[9+9+2] = 0x83;		//BotEndPointUSB3[0][2]		// In Endpoint number 3
	#ifdef UAS_EN
		CfgUSB3[9+9+7+6+7+6+9+7+6+4+2] = 0x81;					// UasEndPointUSB3[1][2]	// In Endpoint number 1
	    CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+2] = 0x83;		//UasEndPointUSB3[3][2]	// In Endpoint number 3
	#endif

	}
#endif

#ifdef UAS_EN
	#if 0
		//if (sobj_default_cFree1.udata.u_8[0] == 0xFF)
		if (sobj_qdepth >= MIN_SATA_Q_DEPTH)
		{
			CfgUSB2[2] = 9+9+14 +9+28+16; 
				//sizeof(CfgUSB2) + sizeof(BotInfUSB2) + sizeof(EndPointUSB2)+ //total length of data return
				//								sizeof(UasInfUSB2)+ sizeof(UasEndPointUSB2)+sizeof(Pipe)
			CfgUSB3[2] = 9+9+7+6+7+6 +9+7+6+4 +7+6+4 +7+6+4 +7+6+4;
				//sizeof(BOTInfUSB3) + sizeof(BOTEndPointUSB3) +sizeof(BOTSSEndPointComp)+
				//sizeof(InfUSB3) + sizeof(UasEndPointUSB3) +
				//sizeof(Pipe) + sizeof(UasSSEndPointComp); //total length of data return
		}
		else
		{
			CfgUSB2[2] = 9 +9 + 7 + 7;
			CfgUSB3[2] = 9 + 9 + 7 + 6 + 7 + 6;
		}
	#else
		tmp8 = sobj_qdepth + 1 + 1;
		if ( (tmp8  &  B_BIT_5) == 0)
		{
			if ( (tmp8  &  B_BIT_4))
			{
				val = 4;	// Max Stream : 16
			}
			else if ( (tmp8  &  B_BIT_3))
			{
				val = 3;	// Max Stream : 8
			}
			else //if ( (tmp8  &  B_BIT_2))
			{
				val = 2;	// Max Stream : 4
			}

			CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+3] = val;				//UasSSEndPointComp[1][3] 		// Max Stream
			CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+3] = val;		//UasSSEndPointComp[2][3]		// Max Stream
			CfgUSB3[9+9+7+6+7+6+9+7+6+4+7+6+4+7+6+4+7+3] = val;	//UasSSEndPointComp[3][3]		// Max Stream
		}

	#endif
#endif
		//interface descriptor
		//BOTInfUSB3[0] = sizeof(desc.BOTInfUSB3);	// Descriptor length
		//BOTInfUSB3[1] = DESCTYPE_INF;				// Descriptor type - Interface
		//BOTInfUSB3[2] = 0x00;						// Zero-based index of this interface
		//BOTInfUSB3[3] = 0x00;						// Alternate setting
		//BOTInfUSB3[4] = 0x02;						// Number of end points
		//BOTInfUSB3[5] = 0x08;						// MASS STORAGE Class.
		//BOTInfUSB3[6] = 0x06;					// 06: SCSI transplant Command Set 
	
#ifdef ATAPI
		if (sobj_class == DEVICECLASS_ATAPI)
		{
			CfgUSB3[9+6] = 0x02;		//BOTInfUSB3[6] = 0x02;					// 02: MMC-5 (ATAPI) 
		}
		else
		{
			CfgUSB3[9+6] = 0x06;					//  06 
		}
#endif
		//BOTInfUSB3[7] = 0x50;						// BULK-ONLY TRANSPORT
		//BOTInfUSB3[8] = IINF;						// iInterface=00

		//endpoint descriptor
//		BotEndPointUSB3[0][0] = sizeof(desc.BotEndPointUSB3[0]);	// Descriptor length
//		BotEndPointUSB3[0][1] = DESCTYPE_ENDPOINT;	// Descriptor type - Endpoint
//		BotEndPointUSB3[0][2] = 0x8B;				// Endpoint number 11
														// direction IN: (bit7=0 OUT and bit7=1 IN endpoint))
//		BotEndPointUSB3[0][3] = 0x02;				// Endpoint type (bit0-1) - Bulk
//		BotEndPointUSB3[0][4] = 0x00;				// 2 bytes Maximun packet size (bit0-10)
//		BotEndPointUSB3[0][5] = 0x04;
//		BotEndPointUSB3[0][6] = 0x00;				// Polling interval endpoint for data xfer
	
	
	
		// 	USB SS Endpoint Companion Descriptor
//		BotSSEndPointComp[0] = sizeof(desc.BotSSEndPointComp);
//		BotSSEndPointComp[1] = DESCTYPE_ENDPOINT_COMP;
//		BotSSEndPointComp[2] = 0x07;						// Max Burst: 7+1
//		BotSSEndPointComp[3] = 0x00;						// Streaming is not supported.
//		BotSSEndPointComp[4] = 0x00;						// Not a periodic endpoint.
//		BotSSEndPointComp[5] = 0x00;

//		BotEndPointUSB3[1][0] = sizeof(desc.BotEndPointUSB3[0]);	// Descriptor length
//		BotEndPointUSB3[1][1] = DESCTYPE_ENDPOINT;	// Descriptor type - Endpoint
//		BotEndPointUSB3[1][2] = 0x0A;				// Endpoint number 10 and direction OUT
//		BotEndPointUSB3[1][3] = 0x02;				// Endpoint type - Bulk
//		BotEndPointUSB3[1][4] = 0x00;				// Maximun packet size
//		BotEndPointUSB3[1][5] = 0x04;
//		BotEndPointUSB3[1][6] = 0x00;				// Polling interval
	}

	// 	USB Binary Object Stor Descriptor
#if 0
	desc.BOSS[0] = sizeof(desc.BOSS);
	desc.BOSS[1] = DESCTYPE_BOSS;				// BOSS Descriptor Type
	desc.BOSS[2] = sizeof(desc.BOSS) + 
						sizeof(desc.USB2ExtendCapabilties) +
						sizeof(desc.USB3Capabilties);	//total length of data return
	desc.BOSS[3] = 0;
	desc.BOSS[4] = 2;							// number of device Capabilties
	
	desc.USB2ExtendCapabilties[0] = sizeof(desc.USB2ExtendCapabilties);
	desc.USB2ExtendCapabilties[1] = DESCTYPE_DEV_CAP;		// Device Capability Descriptor Type
	desc.USB2ExtendCapabilties[2] = 0x02;					//USB 2.0 Extension Capability
	desc.USB2ExtendCapabilties[3] = 0x02;					// Link Power Management Supported
	desc.USB2ExtendCapabilties[4] = 0x00;
	desc.USB2ExtendCapabilties[5] = 0x00;
	desc.USB2ExtendCapabilties[6] = 0x00;
	
	desc.USB3Capabilties[0] = sizeof(desc.USB3Capabilties);
	desc.USB3Capabilties[1] = DESCTYPE_DEV_CAP;		// Device Capability Descriptor Type
	desc.USB3Capabilties[2] = 0x03;						// USB 3.0 Device Capability
	desc.USB3Capabilties[3] = 0x00;						// Does not support Latency Tolerance Messages
	desc.USB3Capabilties[4] = 0x0E;						// USB 3.0 5Gb/s, USB 2.0 High and Full Speeds
	desc.USB3Capabilties[5] = 0x00;
	desc.USB3Capabilties[6] = 0x01;						// All functionality supported down to USB 2.0 Full Speed
	desc.USB3Capabilties[7] = 0x0A;						// Less than 10us U1 Exit Latency
	desc.USB3Capabilties[8] = 0x80;						// Less than 128us U2 Exit Latency
	desc.USB3Capabilties[9] = 0x00;
#endif	
	
//	Str[0] = sizeof(Str);
//	Str[1] = DESCTYPE_STR;
//	Str[2] = 0x09;
//	Str[3] = 0x04;

#if 0
	Mfc[0] = sizeof(desc.Mfc);
	Mfc[1] = DESCTYPE_STR;
	CrSetVendorText(&Module_Vendor_Text);
	Text16(&Module_Vendor_Text, Mfc + 2, 8);


	Product[0] = sizeof(Product);
	Product[1] = DESCTYPE_STR;
	CrSetModelText(Model_ID_Text);
	Text16(Model_ID_Text, Product + 2, 16);
#else
	CrSetVendorText(module_vendor_text);

	if (globalNvram.fwCtrl & VENDOR_STR_FROM_NVRAM)
	{
		if ((0 < globalNvram.vendorStrLength) && (globalNvram.vendorStrLength<= 16))
		{
			Mfc[0] = 2 + globalNvram.vendorStrLength * 2;
		}
		else
		{
			Mfc[0] = 2 + 16 * 2;
		}
	}
	else
	{
		for(i8 = 0; i8 <16; i8++)
		{
			if(module_vendor_text[i8] == ' ')
			{
				tmp8 = i8;
				break;
			}
		}
		if ((0 < tmp8) && (tmp8 <= 16))
		{
			Mfc[0] = 2 + tmp8 * 2;
		}
		else
		{
			Mfc[0] = 2 + 16 * 2;
		}	
	}
	Mfc[1] = DESCTYPE_STR;
	Text16(module_vendor_text, &Mfc[2], (Mfc[0] -2)/2);

	CrSetModelText(module_product_text);

	if(globalNvram.fwCtrl & PRODUCT_STR_FROM_NVRAM)
	{
		tmp8 = globalNvram.modelStrLength;
	}
	else
	{
		for(i8 = 0; i8 <32; i8++)
		{
			if(module_product_text[i8] == ' ')
			{
				tmp8 = i8;
				break;
			}
		}
	}
	if(tmp8 > 32)
	{
		tmp8 = 32;
	}
	else if (tmp8 == 0)
	{
		tmp8 = 32;
	}
	Product[0] = 2 + tmp8 * 2;	
	Product[1] = DESCTYPE_STR;
	Text16(module_product_text, Product + 2, tmp8);
#endif

	{
		//u8 * addr;
		//u32	i;

		Serial[0] = sizeof(Serial);
		Serial[1] = DESCTYPE_STR;
		pU8 = &Serial[2];



		for (i8 = 0; i8 < 16; i8++)
		{
			u8 serialChar;
			//u8 val;

			// the following lines takes care of byte-swap
			val = serialStr[i8];

			serialChar = (val >> 4) + '0';
			if (serialChar > '9')
				serialChar += 'A' - '9' - 1;
			*pU8 = serialChar;
			pU8++;
			*pU8 = 0;
			pU8++;

			serialChar = (val & 0x0F) + '0';
			if (serialChar > '9')
				serialChar += 'A' - '9' - 1;
			*pU8 = serialChar;
			pU8++;
			*pU8 = 0;
			pU8++;
		} // for
	}
}

