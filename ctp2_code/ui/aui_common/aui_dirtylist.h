










#ifndef __AUI_DIRTYLIST_H__
#define __AUI_DIRTYLIST_H__


#include "aui_base.h"
#include "tech_wllist.h"


struct aui_Span
{
	sint16 run;		
	sint16 length;	
};







#define k_DIRTYLIST_MAXSPANS	15 

struct aui_SpanList
{
	sint32 num; 
	aui_Span spans[ k_DIRTYLIST_MAXSPANS ];
};



class aui_DirtyList : public aui_Base, public tech_WLList<RECT *>
{
public:
	
	aui_DirtyList(
		BOOL useSpans = FALSE,
		sint32 width = 0,
		sint32 height = 0 );
	virtual ~aui_DirtyList();

	AUI_ERRCODE	AddRect(
		sint32 left,
		sint32 top,
		sint32 right,
		sint32 bottom );
	AUI_ERRCODE	AddRect(
		RECT *rect );

	AUI_ERRCODE	SubtractRect(
		sint32 left,
		sint32 top,
		sint32 right,
		sint32 bottom );
	AUI_ERRCODE	SubtractRect( RECT *sub );

	AUI_ERRCODE	Minimize( void );
	void		Flush( void );

	sint32 GetWidth() const { return m_width; }
	sint32 GetHeight() const { return m_height; }

	
	aui_SpanList *GetSpans() const { return m_spanListArray; }
	AUI_ERRCODE SetSpans( aui_DirtyList *newDirtyList );

	
	BOOL IsEmpty( void ) const { return m_isEmpty; }

	
	AUI_ERRCODE ComputeSpans( RECT *newRect );

protected:
	tech_Memory<RECT> *m_rectMemory;

	
	sint32 m_width;
	sint32 m_height;

	
	aui_SpanList *m_spanListArray;
	sint32 m_isEmpty;
};


#endif 
