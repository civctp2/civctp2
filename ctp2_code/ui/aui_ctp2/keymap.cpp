










#include "c3.h"
#include "globals.h"
#include "keymap.h"
#include "CivPaths.h"
#include "c3files.h"
#include "c3errors.h"
#include "controlPanelWindow.h"

extern CivPaths *g_civPaths; 


static FUNC_TABLE func2key[] = {
  { 0, KEY_FUNCTION_NOOP, "NOOP"},
      
	   	 
  { 0, KEY_FUNCTION_MOVE_NORTH, "MOVE_NORTH"},
  { 0, KEY_FUNCTION_MOVE_NORTHWEST, "MOVE_NORTHWEST"},
  { 0, KEY_FUNCTION_MOVE_NORTHEAST, "MOVE_NORTHEAST"},  
  { 0, KEY_FUNCTION_MOVE_WEST, "MOVE_WEST"}, 
  { 0, KEY_FUNCTION_MOVE_EAST, "MOVE_EAST"},
  { 0, KEY_FUNCTION_MOVE_SOUTHWEST, "MOVE_SOUTHWEST"},
  { 0, KEY_FUNCTION_MOVE_SOUTH, "MOVE_SOUTH"}, 
  { 0, KEY_FUNCTION_MOVE_SOUTHEAST, "MOVE_SOUTHEAST"},
  { 0, KEY_FUNCTION_UNIT_CITY_TOGGLE, "UNIT_CITY_TOGGLE"},
  { 0, KEY_FUNCTION_NEXT_ITEM, "NEXT_ITEM"},
  { 0, KEY_FUNCTION_KEYBOARD_SELECT_UNIT, "KEYBOARD_SELECT_UNIT"},
  { 0, KEY_FUNCTION_NEXT_ROUND, "NEXTROUND"}, 
  { 0, KEY_FUNCTION_ENDTURN, "ENDTURN"},

	    	
  { 0, KEY_FUNCTION_OPEN_WORK_VIEW, "OPEN_ARMY_MGR"},				
  { 0, KEY_FUNCTION_SLEEP, "SLEEP"},
  { 0, KEY_FUNCTION_ENTRENCH, "ENTRENCH"},
  { 0, KEY_FUNCTION_SETTLE, "SETTLE"},
  { 0, KEY_FUNCTION_PILLAGE, "PILLAGE"},
  { 0, KEY_FUNCTION_BOMBARD, "BOMBARD"},
  { 0, KEY_FUNCTION_REFORM, "REFORM"},
  { 0, KEY_FUNCTION_EXPEL, "EXPEL"},
  { 0, KEY_FUNCTION_UNLOAD_TRANS, "UNLOAD_TRANS"},
  { 0, KEY_FUNCTION_INVESTIGATE_CITY, "INVESTIGATE_CITY"},
  { 0, KEY_FUNCTION_PLANT_NUKE, "PLANT_NUKE"},
  { 0, KEY_FUNCTION_BIOINFECT, "BIOINFECT"},
  { 0, KEY_FUNCTION_NANOTERROR, "NANOTERROR"},
  { 0, KEY_FUNCTION_CREATE_PARK, "CREATE_PARK"},
  { 0, KEY_FUNCTION_SPACE_LAUNCH, "SPACE_LAUNCH"},
  { 0, KEY_FUNCTION_PARADROP, "PARADROP"},

	   
  { 0, KEY_FUNCTION_OPEN_CIV_STATUS, "OPEN_CIV_STATUS"},			
  { 0, KEY_FUNCTION_OPEN_CITY_STATUS, "OPEN_CITY_STATUS"},			
  { 0, KEY_FUNCTION_CITY_MANAGEMENT, "CITY_MANAGEMENT"},			
  { 0, KEY_FUNCTION_BUILD_QUEUE, "BUILD_QUEUE"},					
  { 0, KEY_FUNCTION_CLEAR_QUEUE, "CLEAR_QUEUE"},					
  { 0, KEY_FUNCTION_OPEN_SCIENCE_STATUS, "OPEN_SCIENCE_STATUS"},	
  { 0, KEY_FUNCTION_OPEN_UNIT_STATUS, "OPEN_UNIT_STATUS"},			
  { 0, KEY_FUNCTION_OPEN_TRADE_STATUS, "OPEN_TRADE_STATUS"},		
  { 0, KEY_FUNCTION_TRADE_SUMMARY, "TRADE_SUMMARY"},				
  { 0, KEY_FUNCTION_OPEN_DIPLOMACY, "OPEN_DIPLOMACY"},				
  { 0, KEY_FUNCTION_NEW_PROPOSAL, "NEW_PROPOSAL"},					
  { 0, KEY_FUNCTION_OPEN_MESSAGE, "MESSAGE"},						
  { 0, KEY_FUNCTION_GAIA, "GAIA"},									
  { 0, KEY_FUNCTION_OPEN_GREAT_LIBRARY, "OPEN_GREAT_LIBRARY"},		
  { 0, KEY_FUNCTION_OPEN_INFO_SCREEN, "OPEN_INFO_SCREEN"},			
  { 0, KEY_FUNCTION_TIMELINE, "TIMELINE"},							
  { 0, KEY_FUNCTION_RANK, "RANK"},									
  { 0, KEY_FUNCTION_CIV_TAB, "CIVTAB"},								
  { 0, KEY_FUNCTION_CITY_TAB, "CITYTAB"},							
  { 0, KEY_FUNCTION_UNIT_TAB, "UNITTAB"},							
  { 0, KEY_FUNCTION_MSG_TAB, "MSGTAB"},								
  { 0, KEY_FUNCTION_TILE_TAB, "TILETAB"},							
  { 0, KEY_FUNCTION_CHAT_KEY, "CHAT_KEY"},							
  { 0, KEY_FUNCTION_NETWORK_PLAYERS_SCREEN, "NETWORK_PLAYERS_SCREEN"},	
  { 0, KEY_FUNCTION_OPEN_SCENARIO_EDITOR, "OPEN_SCENARIO_EDITOR"},	

	   
  { 0, KEY_FUNCTION_REMAP_KEYBOARD, "REMAP_KEYBOARD"},
  { 0, KEY_FUNCTION_OPEN_OPTIONS_SCREEN, "OPEN_OPTIONS_SCREEN"},	
  { 0, KEY_FUNCTION_SOUND_OPTIONS, "SOUND_OPTIONS"},
  // MUSIC added by ahenobarb
  { 0, KEY_FUNCTION_MUSIC_OPTIONS, "MUSIC_OPTIONS"},
  { 0, KEY_FUNCTION_GRAPHICS_OPTIONS, "GRAPHICS_OPTIONS"},
  { 0, KEY_FUNCTION_GAMEPLAY_OPTIONS, "GAMEPLAY_OPTIONS"},
  { 0, KEY_FUNCTION_ADVANCED_OPTIONS, "ADVANCED_OPTIONS"},
  { 0, KEY_FUNCTION_RESTART, "RESTART"},
  { 0, KEY_FUNCTION_NEW_GAME, "NEW_GAME"},
  { 0, KEY_FUNCTION_SAVE_GAME, "SAVE_GAME"},
  { 0, KEY_FUNCTION_LOAD_GAME, "LOAD_GAME"},
  { 0, KEY_FUNCTION_SAVE_WORLD, "SAVE_WORLD"},
  { 0, KEY_FUNCTION_LOAD_WORLD, "LOAD_WORLD"},
  { 0, KEY_FUNCTION_QUIT, "QUIT"},

	   	
  { 0, KEY_FUNCTION_TOGGLE_CITY_NAMES, "CITY_NAMES"},
  { 0, KEY_FUNCTION_TOGGLE_TRADE_ROUTES, "TRADE_ROUTES"},
  { 0, KEY_FUNCTION_CENTER_MAP, "CENTER_MAP"},
  { 0, KEY_FUNCTION_CLOSE, "CLOSE"},
  { 0, KEY_FUNCTION_TOGGLE_RADAR, "TOGGLE_RADAR"},					
  { 0, KEY_FUNCTION_TOGGLE_CONTROL_PANEL, "TOGGLE_CONTROL_PANEL"},	
  { 0, KEY_FUNCTION_TOGGLE_ALL, "TOGGLE_ALL"},						
  { 0, KEY_FUNCTION_ZOOM_IN1,  "ZOOM_IN1"},
  { 0, KEY_FUNCTION_ZOOM_OUT1, "ZOOM_OUT1"},

	   
  { 0, KEY_FUNCTION_EXECUTE_EYEPOINT, "EXECUTE_EYEPOINT"},			
  { 0, KEY_FUNCTION_YES, "YES"},
  { 0, KEY_FUNCTION_NO, "NO"},
  { 0, KEY_FUNCTION_TOGGLE_SPACE, "TOGGLE_SPACE"},					
  { 0, KEY_FUNCTION_ZOOM_OUT2, "ZOOM_OUT2"},						
  { 0, KEY_FUNCTION_ZOOM_IN2, "ZOOM_IN2"},
  { 0, KEY_FUNCTION_PROCESS_UNIT_ORDERS, "PROCESS_UNIT_ORDERS" },
  { 0, KEY_FUNCTION_HELP_MODE_TOGGLE, "HELP_MODE_TOGGLE"},
  { 0, KEY_FUNCTION_MOVE_ORDER, "MOVE_ORDER"},
  { 0, KEY_FUNCTION_EXECUTE_ORDERS, "EXECUTE_ORDERS"},				
  { 0, KEY_FUNCTION_END_UNIT_TURN, "END_UNIT_TURN"},				
  { 0, KEY_FUNCTION_GROUP_ARMY, "GROUP_ARMY"},						
  { 0, KEY_FUNCTION_UNGROUP_ARMY, "UNGROUP_ARMY"},					
  { 0, KEY_FUNCTION_MOVE_UP_OR_DOWN, "MOVE_UP_OR_DOWN"},			
  { 0, KEY_FUNCTION_OPEN_CITY_VIEW, "OPEN_CITY_VIEW"},				
  { 0, KEY_FUNCTION_DESCEND, "DESCEND"},							
  { 0, KEY_FUNCTION_CONTROL_BUILD, "CONTROL_BUILD"},
  { 0, KEY_FUNCTION_CONTROL_NEXT, "CONTROL_NEXT"},
  { 0, KEY_FUNCTION_CONTROL_PREV, "CONTROL_PREV"},

#ifdef _PLAYTEST
  { 0, KEY_FUNCTION_RAND_TEST, "RAND_TEST"},
  { 0, KEY_FUNCTION_GAMESTATE_DEBUG, "GAMESTATE_DEBUG"},
  { 0, KEY_FUNCTION_ENTER_COMMAND, "ENTER_COMMAND"},
 { 0, KEY_FUNCTION_ENTER_COMMAND_ALTERNATE, "ENTER_COMMAND_ALTERNATE"}
#endif
};

KEYMAP::KEYMAP(BOOL useDefault)

{ 
   sint32 i; 
   
   for (i=0; i< MAX_KEYPRESS_MAP; i++) { 
      key2func[i].keycode = 0;
      key2func[i].function = KEY_FUNCTION_NOOP; 
   }
   
   
   for (i=0; i < KEY_FUNCTION_LAST_NOOP; i++) { 
      func2key[i].keycode=0; 

      func2key[i].function = KEY_FUNCTION_NOOP;
   } 

   
   FILE *fin = NULL;
   if(!useDefault)
	   fin = c3files_fopen(C3DIR_DIRECT, "userkeymap.txt", "r");
   if(!fin)
	   fin = c3files_fopen(C3DIR_UIDATA, "keymap.txt", "r");
   
   if (fin == NULL) { 
     c3errors_FatalDialog ("keymap.txt", "Could not open keymap.txt or userkeymap.txt");
   }

   sint32 k, old; 
   char funcstr[80]; 
   char keystr[80]; 
   char line[1024]; 
   sint32 searching; 

   do { 
	 if(!fgets(line, 1023, fin)) {
		 fclose(fin);
		 return;
	 }
     
	 char *space = strchr(line, ' ');
	 if(!space)
		 continue;
	 if(space - line > 79)
		 continue;
	 strncpy(keystr, line, space - line);
	 keystr[space - line] = 0;

	 if(strlen(space+1) > 79)
		 continue;

	 strcpy(funcstr, space + 1);
	 char *eol = &funcstr[strlen(funcstr) - 1];
	 while((*eol == '\r' || *eol == '\n') && eol > funcstr) {
		 *eol = 0;
		 eol--;
	 }

	 
	 
	 


		unsigned long val;
		if(stricmp(keystr, "SPACE") == 0) {
			val = ' ';
		} else if(stricmp(keystr, "ENTER") == 0) {
			val = '\r' + 128;
		} else if(stricmp(keystr, "TAB") == 0) {
			val = '\t' + 128;
		} else if(stricmp(keystr, "BACKSPACE") == 0) {
			val = 8 + 128;
		} else if(keystr[0] == '^') {
			if(keystr[1] >= 'A' && keystr[1] <= 'Z') {
				val = keystr[1] - 'A' + 1; 
			} else if(keystr[1] >= 'a' && keystr[1] <= 'z') {
				val = keystr[1] - 'a' + 1;
			} else if(keystr[1] >= '0' && keystr[1] <= '9') {
				
				val = keystr[1] + 128;
			} else if(keystr[1] == '!') {
				val = keystr[1] + 128;
			} else if(keystr[1] == '@') {
				val = keystr[1] + 128;
			} else {
				val = 0;
			}
		} else {
			
			val = unsigned long (keystr[0]); 
		}

        k = hash_32to16(val) & MASK_MAX_KEYPRESS_MAP; 
        old = k;

        searching = TRUE; 

        for (i=0; i<MAX_KEYPRESS_MAP; i++) { 
           if (strcmp(funcstr, func2key[i].str) == 0)  {         
               do { 
                  if (key2func[k].function == KEY_FUNCTION_NOOP) {  
                     key2func[k].function = KEY_FUNCTION(i);
                     key2func[k].keycode = val; 
                     searching = FALSE; 
                     func2key[i].keycode = val; 
                     Assert(func2key[i].function == KEY_FUNCTION_NOOP); 
                     func2key[i].function = KEY_FUNCTION(i); 
                     
                     break;
                  } else { 
                       k = (k + 1) % MAX_KEYPRESS_MAP; 
                  } 
               } while (k != old); 
               Assert(!searching);
               break;
           } 
        }  
        Assert (!searching); 
   } while (!feof(fin)); 
	fclose(fin);

   
} 

uint32 KEYMAP::hash_32to16(unsigned long val)

{
   unsigned char a, b, c, d; 

   a = unsigned char(val >> 24); 
   b = unsigned char(val >> 16); 
   c = unsigned char(val >> 8); 
   d = unsigned char(val); 


   return a^b^c^d;
}

KEY_FUNCTION KEYMAP::get_function(unsigned long wParam)

{ 
   uint32 val = hash_32to16(wParam) & MASK_MAX_KEYPRESS_MAP; 
   uint32 old = val; 

   do { 
      if (key2func[val].function == KEY_FUNCTION_NOOP) {  
         return KEY_FUNCTION_NOOP; 
      } else if (key2func[val].keycode == wParam) {  
         return key2func[val].function; 
      } else {                                     
         val = (val + 1) % MAX_KEYPRESS_MAP; 
      }
   } while (val != old); 

   Assert(0); 

   return KEY_FUNCTION_NOOP; 
}


uint32 KEYMAP::get_keycode( KEY_FUNCTION key )
{
	return func2key[key].keycode;
}

sint32 KEYMAP::remap_key( KEY_FUNCTION keyFunc, uint32 wParam, uint32 oldKeycode )
{
	uint32 val = hash_32to16( wParam ) & MASK_MAX_KEYPRESS_MAP;
	uint32 oldVal = hash_32to16( oldKeycode ) & MASK_MAX_KEYPRESS_MAP;

	
	KEY_FUNCTION oldFunc = key2func[val].function;

	
	func2key[oldFunc].keycode = 0;
	func2key[oldFunc].function = KEY_FUNCTION_NOOP;

	
	func2key[keyFunc].keycode = wParam;
	func2key[keyFunc].function = keyFunc;

	
	key2func[oldVal].keycode = 0;
	key2func[oldVal].function = KEY_FUNCTION_NOOP;
	
	
	key2func[val].keycode = wParam;		
	key2func[val].function = keyFunc;


	return 0;
}

void KEYMAP::save()
{
	FILE *file = c3files_fopen(C3DIR_DIRECT, "userkeymap.txt", "w");
	Assert(file);
	if(!file) {
		c3errors_ErrorDialog("keymap.txt", "Could not open userkeymap.txt");
		return;
	}
	
	sint32 i;
	for(i = KEY_FUNCTION_NOOP + 1; i < KEY_FUNCTION_LAST_NOOP; i++) {
		sint32 code = func2key[i].keycode;
		if(code != 0) {
			switch(code) {
				case ' ':
					fprintf(file, "SPACE ");
					break;
				case '\r' + 128:
					fprintf(file, "ENTER ");
					break;
				case '\t' + 128:
					fprintf(file, "TAB ");
					break;
				case 8 + 128:
					fprintf(file, "BACKSPACE ");
					break;
				default:
					if(isprint(code) && !(code & 0x80)) {
						fprintf(file, "%c ", code);
					} else if(code >= 'a' - 'a' + 1 && code <= 'z' - 'a' + 1) {
						fprintf(file, "^%c ", code + 'a' - 1);
					} else if(code >= ('0' | 0x80) && code <= ('9' | 0x80)) {
						fprintf(file, "^%c ", code & 0x7f);
					} else if(code == '!' + 128) {
						fprintf(file, "^! ");
					} else if(code == '@' + 128) {
						fprintf(file, "^@ ");
					} else {
						fprintf(file, "^# ");
					}
					break;
			}
			fprintf(file, "%s\n", func2key[i].str);
		}
	}
	fclose(file);
	ControlPanelWindow::RebuildMenus();
}
