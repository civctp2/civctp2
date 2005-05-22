












#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3WINDOWS_H__
#define __C3WINDOWS_H__



#define k_IconName								"icon.tga"
#define k_PatternName							"upba3010.tga"



#define k_ID_WINDOW_INFO						1
#define k_ID_WINDOW_STATUS						2
#define k_ID_BACKGROUND							3
#define k_ID_WINDOW_TEST						4
#define k_ID_WINDOW_CONTROLPANEL				5
#define k_ID_WINDOW_STANDARD					6
#define k_ID_WINDOW_FLOATING					7
#define k_ID_WINDOW_TIP							8
#define k_ID_WINDOW_TURN						9
#define k_ID_WINDOW_STATS						10
#define k_ID_WINDOW_GREATLIBRARY				11


#define k_ID_BUTTON_CONTROLWINDOW				1010
#define	k_ID_BUTTON_PICTURE						1020
#define k_ID_BUTTON_TESTWINDOW					1030
#define k_ID_CHECKBOX_TESTWINDOW				1040
#define k_ID_RADIO_STRONGLYMODAL				1050
#define k_ID_RADIO_WEAKLYMODAL					1060
#define k_ID_RADIOGROUP_MODAL					1070
#define k_ID_ICONBUTTON_TESTWINDOW				1080
#define k_ID_TAB_SAD							1090
#define k_ID_TAB_INDIFFERENT					1010
#define k_ID_TAB_HAPPY							1011
#define k_ID_TABGROUP_MOODY						1012
#define k_ID_TAB_TEST							1013
#define k_ID_DRAGCHECKBOX_TESTWINDOW			1014
#define k_ID_TRANSPARENTCHECKBOX_TESTWINDOW		1015
#define k_ID_SLIDER								1016
#define k_ID_SCROLLER							1017
#define k_ID_SPINNER							1018
#define k_ID_LISTBOX							1019
#define k_ID_ITEMBASE							1020


#define k_ID_TABGROUP_CITY						20
#define k_ID_TAB_PRODUCTION						21
#define k_ID_TAB_POPULATION						22
#define k_ID_TAB_TRADE							23
#define k_ID_TAB_DEBUG							24
#define k_ID_TAB_TERRAIN						25


int c3windows_MakeDebugWindow( BOOL make = TRUE );
int c3windows_MakeStatusWindow( BOOL make = TRUE );
int c3windows_MakeStandardWindow( BOOL make = TRUE );
int c3windows_MakeTestWindow( BOOL make = TRUE );
int c3windows_MakeFloatingWindow( BOOL make = TRUE );
int c3windows_MakeTipWindow( BOOL make = TRUE );

void c3windows_Cleanup( void );


#endif 
