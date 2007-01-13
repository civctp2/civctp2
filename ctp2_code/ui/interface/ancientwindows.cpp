

#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "c3ui.h"


#include "aui_static.h"
#include "c3_static.h"


#include "c3window.h"
#include "c3windows.h"

#include "bevellesswindow.h"
#include "radarwindow.h"
#include "statswindow.h"
#include "controlpanelwindow.h"

#include "ancientwindows.h"


extern sint32 g_ScreenWidth;
extern sint32 g_ScreenHeight;


extern C3UI						*g_c3ui;

extern StatsWindow				*g_statsWindow;
extern ControlPanelWindow	*g_controlPanel;







static C3Window		*s_lowerRight = NULL;
static aui_Static	*s_imageLowerRight;


static C3Window		*s_angelLeft = NULL;
static aui_Static	*s_imageAngelLeft;
static C3Window		*s_angelRight = NULL;
static aui_Static	*s_imageAngelRight;
static C3Window		*s_radarBorderRight = NULL;
static aui_Static	*s_imageRadarBorderRight;
static C3Window		*s_controlLeft = NULL;
static c3_Static	*s_imageControlLeft;
static C3Window		*s_controlRight = NULL;
static c3_Static	*s_imageControlRight;

static BevelLessWindow *s_controlPiece = NULL;







int AncientWindows_PreInitialize(void)
{
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		imageBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE	errcode;

return 0;

	sprintf(windowBlock, "%s", "LowerRight");

	s_lowerRight = new C3Window(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_BACKGROUND );
	Assert( AUI_NEWOK(s_lowerRight, errcode) );
	if ( !AUI_NEWOK(s_lowerRight, errcode) ) return -1;

	sprintf(imageBlock, "%s.%s", windowBlock, "Image");
	s_imageLowerRight = new aui_Static(&errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_imageLowerRight, errcode) );
	if ( !AUI_NEWOK(s_imageLowerRight, errcode) ) return -1;

	return 0;
}

sint32 ancientwindows_GetControlPieceY( void )
{


	return g_controlPanel->Y();
}

sint32 ancientwindows_GetControlPieceHeight( void )
{


	return g_controlPanel->Height();
}



BevelLessWindow		*s_controlPanelLeftHat=NULL;
BevelLessWindow		*s_controlPanelRightHat=NULL;


int AncientWindows_Initialize( void )
{

	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		imageBlock[ k_AUI_LDL_MAXBLOCK + 1 ];



	
	

































return 0;

	
	

	
	sprintf(windowBlock, "%s", "ControlPiece");
	s_controlPiece = new BevelLessWindow(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_BACKGROUND );
	Assert( AUI_NEWOK(s_controlPiece, errcode) );
	if ( !AUI_NEWOK(s_controlPiece, errcode) ) return -1;

	
	if ( g_ScreenWidth > 640 || g_ScreenHeight > 480 ) {
		
		s_controlPiece->Resize(g_controlPanel->Width(), s_controlPiece->Height());
		s_controlPiece->Move(s_controlPiece->X(), g_ScreenHeight - g_controlPanel->Height() - s_controlPiece->Height());
	}
	
	errcode = g_c3ui->AddWindow( s_controlPiece );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;


	
	sprintf(windowBlock, "%s", "AngelLeft");
	s_angelLeft = new C3Window(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_SINKING );
	Assert( AUI_NEWOK(s_angelLeft, errcode) );
	if ( !AUI_NEWOK(s_angelLeft, errcode) ) return -1;

	sint32 nudge = 0;

	
	if ( g_ScreenWidth > 640 || g_ScreenHeight > 480 ) {
		
		if ( g_ScreenHeight < 768 ) nudge = 2;
		
		extern aui_Static	*s_leftBorder;	
		s_angelLeft->Move(s_angelLeft->X(), g_ScreenHeight - s_leftBorder->Height() - s_angelLeft->Height() + nudge);
	}

	sprintf(imageBlock, "%s.%s", windowBlock, "Image");
	s_imageAngelLeft = new aui_Static(&errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_imageAngelLeft, errcode) );
	if ( !AUI_NEWOK(s_imageAngelLeft, errcode) ) return -1;
	
	errcode = s_angelLeft->AddControl(s_imageAngelLeft);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -4;

	s_angelLeft->Enable( FALSE );
	s_angelLeft->SetTransparent( TRUE );

	errcode = g_c3ui->AddWindow( s_angelLeft );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;

	

	sprintf(windowBlock, "%s", "AngelRight");
	s_angelRight = new C3Window(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_SINKING );
	Assert( AUI_NEWOK(s_angelRight, errcode) );
	if ( !AUI_NEWOK(s_angelRight, errcode) ) return -1;

	sint32 downwardNudge = 3;	
	if ( g_ScreenHeight < 768 ) {
		if ( g_ScreenHeight <= 480) downwardNudge = 0;
		else downwardNudge = 13;
	} 

	
	if ( g_ScreenWidth > 640 || g_ScreenHeight > 480 ) {
		
		s_angelRight->Move(s_angelRight->X(), 
			g_ScreenHeight - g_controlPanel->Height() - s_angelRight->Height() + downwardNudge);
	}

	sprintf(imageBlock, "%s.%s", windowBlock, "Image");
	s_imageAngelRight = new aui_Static(&errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_imageAngelRight, errcode) );
	if ( !AUI_NEWOK(s_imageAngelRight, errcode) ) return -1;
	
	errcode = s_angelRight->AddControl(s_imageAngelRight);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -4;

	s_angelRight->Enable( FALSE );
	s_angelRight->SetTransparent( TRUE );

	errcode = g_c3ui->AddWindow( s_angelRight );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;

	

	



	errcode = s_lowerRight->AddControl(s_imageLowerRight);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -4;

	s_lowerRight->Enable( FALSE );
	s_lowerRight->SetTransparent( TRUE );

	errcode = g_c3ui->AddWindow( s_lowerRight );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;

	

	sprintf(windowBlock, "%s", "RadarBorderRight");
	s_radarBorderRight = new C3Window(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_SINKING );
	Assert( AUI_NEWOK(s_radarBorderRight, errcode) );
	if ( !AUI_NEWOK(s_radarBorderRight, errcode) ) return -1;
	
	
	s_radarBorderRight->Move(s_radarBorderRight->X(), g_ScreenHeight - s_radarBorderRight->Height());

	sprintf(imageBlock, "%s.%s", windowBlock, "Image");
	s_imageRadarBorderRight = new aui_Static(&errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_imageRadarBorderRight, errcode) );
	if ( !AUI_NEWOK(s_imageRadarBorderRight, errcode) ) return -1;
	
	errcode = s_radarBorderRight->AddControl(s_imageRadarBorderRight);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -4;

	s_radarBorderRight->Enable( FALSE );



	errcode = g_c3ui->AddWindow( s_radarBorderRight );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;

	
	
	if ( g_ScreenWidth < 1024 || g_ScreenHeight < 768 ) {
		return 0;
	}

	

	sprintf(windowBlock, "%s", "ControlLeft");
	s_controlLeft = new C3Window(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_SINKING );
	Assert( AUI_NEWOK(s_controlLeft, errcode) );
	if ( !AUI_NEWOK(s_controlLeft, errcode) ) return -1;

	
	s_controlLeft->Move(s_controlLeft->X(), g_ScreenHeight - s_controlLeft->Height());
	
	sprintf(imageBlock, "%s.%s", windowBlock, "Image");
	s_imageControlLeft = new c3_Static(&errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_imageControlLeft, errcode) );
	if ( !AUI_NEWOK(s_imageControlLeft, errcode) ) return -1;
	
	errcode = s_controlLeft->AddControl(s_imageControlLeft);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -4;

	s_controlLeft->Enable( FALSE );



	errcode = g_c3ui->AddWindow( s_controlLeft );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;

	

	sprintf(windowBlock, "%s", "ControlRight");
	s_controlRight = new C3Window(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_SINKING );
	Assert( AUI_NEWOK(s_controlRight, errcode) );
	if ( !AUI_NEWOK(s_controlRight, errcode) ) return -1;
	
	
	s_controlRight->Move(s_controlRight->X(), g_ScreenHeight - s_controlRight->Height());

	sprintf(imageBlock, "%s.%s", windowBlock, "Image");
	s_imageControlRight = new c3_Static(&errcode, aui_UniqueId(), imageBlock);
	Assert( AUI_NEWOK(s_imageControlRight, errcode) );
	if ( !AUI_NEWOK(s_imageControlRight, errcode) ) return -1;
	
	errcode = s_controlRight->AddControl(s_imageControlRight);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -4;

	s_controlRight->Enable( FALSE );



	errcode = g_c3ui->AddWindow( s_controlRight );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;


	return 0;
}

int AncientWindows_Cleanup( void )
{

	if (s_controlPanelLeftHat) {
		g_c3ui->RemoveWindow(s_controlPanelLeftHat->Id());
		delete s_controlPanelLeftHat;
	}
	if (s_controlPanelRightHat) {
		g_c3ui->RemoveWindow(s_controlPanelRightHat->Id());
		delete s_controlPanelRightHat;
	}


return 0;



	g_c3ui->RemoveWindow( s_lowerRight->Id() );

	g_c3ui->RemoveWindow( s_angelLeft->Id() );
	g_c3ui->RemoveWindow( s_angelRight->Id() );
	if ( s_controlLeft ) g_c3ui->RemoveWindow( s_controlLeft->Id() );
	if ( s_controlRight ) g_c3ui->RemoveWindow( s_controlRight->Id() );
	g_c3ui->RemoveWindow( s_radarBorderRight->Id() );

	g_c3ui->RemoveWindow( s_controlPiece->Id() );








	delete s_imageLowerRight;
	s_imageLowerRight = NULL;




	delete s_imageAngelLeft;
	s_imageAngelLeft = NULL;

	delete s_imageAngelRight;
	s_imageAngelRight = NULL;

	delete s_imageRadarBorderRight;
	s_imageRadarBorderRight = NULL;

	if ( s_imageControlLeft ) delete s_imageControlLeft;
	s_imageControlLeft = NULL;

	if ( s_imageControlRight ) delete s_imageControlRight;
	s_imageControlRight = NULL;







	delete s_lowerRight;
	s_lowerRight = NULL;




	delete s_angelLeft;
	s_angelLeft = NULL;

	delete s_angelRight;
	s_angelRight = NULL;

	if ( s_controlLeft ) delete s_controlLeft;
	s_controlLeft = NULL;

	if ( s_controlRight ) delete s_controlRight;
	s_controlRight = NULL;

	delete s_radarBorderRight;
	s_radarBorderRight = NULL;




	delete s_controlPiece;
	s_controlPiece = NULL;




	return 0;
}
