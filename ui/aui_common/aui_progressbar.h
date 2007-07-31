










#ifndef __AUI_PROGRESSBAR_H__
#define __AUI_PROGRESSBAR_H__


#include "aui_control.h"



enum AUI_PROGRESSBAR_ORIENTATION
{
	AUI_PROGRESSBAR_ORIENTATION_FIRST = 0,
	AUI_PROGRESSBAR_ORIENTATION_HORIZONTAL = 0,
	AUI_PROGRESSBAR_ORIENTATION_VERTICAL,
	AUI_PROGRESSBAR_ORIENTATION_LAST
};



enum AUI_PROGRESSBAR_TYPE
{
	AUI_PROGRESSBAR_TYPE_FIRST = 0,
	AUI_PROGRESSBAR_TYPE_STANDARD = 0,
	AUI_PROGRESSBAR_TYPE_LOOP,
	AUI_PROGRESSBAR_TYPE_BACKANDFORTH,
	AUI_PROGRESSBAR_TYPE_LAST
};



#define k_AUI_PROGRESSBAR_LDL_ORIENTATION	"orientation"
#define k_AUI_PROGRESSBAR_LDL_VERTICAL		"vertical"
#define k_AUI_PROGRESSBAR_LDL_HORIZONTAL	"horizontal"

#define k_AUI_PROGRESSBAR_LDL_TYPE			"type"
#define k_AUI_PROGRESSBAR_LDL_STANDARD		"standard"
#define k_AUI_PROGRESSBAR_LDL_LOOP			"loop"
#define k_AUI_PROGRESSBAR_LDL_BACKANDFORTH	"backandforth"

#define k_AUI_PROGRESSBAR_LDL_VPS			"valuespersecond"

#define k_AUI_PROGRESSBAR_LDL_BARRED		"barcolorred"
#define k_AUI_PROGRESSBAR_LDL_BARGREEN		"barcolorgreen"
#define k_AUI_PROGRESSBAR_LDL_BARBLUE		"barcolorblue"

#define k_AUI_PROGRESSBAR_LDL_BARIMAGE		"barimage"

#define k_AUI_PROGRESSBAR_LDL_CURVALUE		"curvalue"
#define k_AUI_PROGRESSBAR_LDL_MAXVALUE		"maxvalue"



#define k_AUI_PROGRESSBAR_DEFAULT_MAXVALUE	100
#define k_AUI_PROGRESSBAR_DEFAULT_COLOR		(RGB(192,192,192))
#define k_AUI_PROGRESSBAR_DEFAULT_VPS		50



class aui_ProgressBar : public aui_Control
{
public:
	
	aui_ProgressBar(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	aui_ProgressBar(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height );
	virtual ~aui_ProgressBar();

protected:
	aui_ProgressBar() : aui_Control() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(
		AUI_PROGRESSBAR_ORIENTATION orientation,
		AUI_PROGRESSBAR_TYPE type,
		sint32 vps,
		MBCHAR *image,
		COLORREF color,
		sint32 curValue,
		sint32 maxValue );

public:
	AUI_PROGRESSBAR_ORIENTATION GetOrientation( void ) const
		{ return m_orientation; }
	AUI_PROGRESSBAR_ORIENTATION SetOrientation(
		AUI_PROGRESSBAR_ORIENTATION orientation );

	sint32 GetCurValue( void ) const { return m_curValue; }
	sint32 SetCurValue( sint32 value );
	sint32 GetMaxValue( void ) const { return m_maxValue; }
	sint32 SetMaxValue( sint32 value );

	AUI_PROGRESSBAR_TYPE GetProgressType( void ) const { return m_type; }
	AUI_PROGRESSBAR_TYPE SetProgressType( AUI_PROGRESSBAR_TYPE type );

	sint32 GetValuesPerSecond( void ) const { return m_vps; }
	sint32 SetValuesPerSecond( sint32 vps );

	aui_Image	*SetBarImage( MBCHAR *image );
	COLORREF	SetBarColor( COLORREF color );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	
	virtual AUI_ERRCODE Idle( void );

protected:
	virtual AUI_ERRCODE CalculateIntervals( double *start, double *stop );
	virtual AUI_ERRCODE DrawBar( aui_Surface *surface, RECT *bound );

	AUI_PROGRESSBAR_ORIENTATION m_orientation;

	sint32		m_curValue;
	sint32		m_maxValue;

	AUI_PROGRESSBAR_TYPE	m_type;
	sint32					m_vps;

	aui_Image	*m_barImage;
	COLORREF	m_barColor;
};


#endif 
