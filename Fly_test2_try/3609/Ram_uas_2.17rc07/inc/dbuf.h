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
 * 3610		2010/04/13	Odin		Function list
 * 3610		2010/04/27	Odin		USB2.0 BOT Debugging
 *
 *****************************************************************************/
#ifndef DBUF_H
#define DBUF_H

#if 0 //def DEBUG
void dbuf_dbg(u8 segment_num);
#endif

// Get up to 512 byte to from DBUF to mc_buffer thru CPU Port
void dbuf_get_data(u8 segment_num);
#if 0
// Write 512 byte to from mc_buffer to Seg 2 of DBUF thru CPU W Port
void dbuf_write_data();
#endif

#ifdef UAS_EN
u8 dbuf_GetIdleSeg();
void dbuf_uas_mode();
#endif
void DBUF_SegFree(u8 segIndex);

void dbuf_Reset();

void dbuf_bot_mode();


void dbuf_init(void);
 
#endif
