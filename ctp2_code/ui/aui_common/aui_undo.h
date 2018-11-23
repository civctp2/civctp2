#ifndef __AUI_UNDO_H__
#define __AUI_UNDO_H__

class aui_Region;

class aui_Undo
{
public:

	aui_Undo( aui_Region *region,
		    RECT rect );
	~aui_Undo();

public:
	aui_Region *GetUndoRegion( void ) { return m_region; }
	void SetUndoRegion( aui_Region *region ) { m_region = region; }

	RECT GetUndoRect( void ) { return m_rect; }
	void SetUndoRect( RECT *rect ) { SetRect( &m_rect, rect->left, rect->top,
											  rect->right, rect->bottom); }

protected:
	aui_Region	*m_region;
	RECT		m_rect;
};

#endif
