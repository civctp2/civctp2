//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source file
// Description  :
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Moved CalculateHash() from aui_UI
//
//----------------------------------------------------------------------------
#include "c3.h"
#include "aui_base.h"

sint32 aui_Base::m_baseRefCount = 0;

#define POLYNOMIAL 0x04c11db7L

static uint32 *g_crcTable = NULL;


void gen_crc_table()
{
   if (!g_crcTable)
      g_crcTable = new uint32[256];
   register int i, j;
   register unsigned long crc_accum;
   
   for ( i = 0;  i < 256;  i++ ) {
      crc_accum = ( (uint32) i << 24 );
      for ( j = 0;  j < 8;  j++ ) {
         if ( crc_accum & 0x80000000L )
            crc_accum = ( crc_accum << 1 ) ^ POLYNOMIAL;
         else
            crc_accum = ( crc_accum << 1 );
      }
      
      g_crcTable[i] = crc_accum;
   }

   return;
}

void free_crc()
{
   if(g_crcTable)
      delete [] g_crcTable;
}

uint32 update_crc(uint32 crc_accum, const MBCHAR *data_blk_ptr, sint32 data_blk_size)
{
   if (!g_crcTable) {
      gen_crc_table();
   }

   register int i, j;

   for ( j = 0;  j < data_blk_size;  j++ ) {
      i = ( (int) ( crc_accum >> 24) ^ tolower(*data_blk_ptr++) ) & 0xff;
      crc_accum = ( crc_accum << 8 ) ^ g_crcTable[i];
   }

   return crc_accum;
}

uint32 aui_Base::CalculateHash( const MBCHAR *string )
{
   Assert( string != NULL );
   if ( !string ) return 0;

   return update_crc(0, string, strlen(string));
}
