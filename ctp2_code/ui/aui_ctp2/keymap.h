//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Key map handling
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
// - #01 Reordered the keymap to reflect the order needed for the display
//   of the missing orders in the keyboard mapping options 
//
//----------------------------------------------------------------------------

#pragma once
#ifndef __KEYMAP_H__
#define __KEYMAP_H__ 1

struct FUNC_TABLE {
   unsigned long keycode; 
   KEY_FUNCTION function; 
   char str[80];
};

struct KEYPRESS_MAP { 
   unsigned long keycode;
   KEY_FUNCTION function; 
};

#define MAX_KEYPRESS_MAP 512
#define MASK_MAX_KEYPRESS_MAP 0x01ff

class KEYMAP { 

	KEYPRESS_MAP key2func[MAX_KEYPRESS_MAP]; 
   

   uint32 hash_32to16(unsigned long val);

public:
	KEYMAP(BOOL useDefault);
	KEY_FUNCTION get_function(unsigned long wParam); 
	uint32 get_keycode( KEY_FUNCTION key );
	sint32 remap_key( KEY_FUNCTION key, uint32 wParam, uint32 oldKeycode );
	void save();
}; 

#endif
