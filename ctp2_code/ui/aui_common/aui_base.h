










#ifndef __AUI_BASE_H__
#define __AUI_BASE_H__



class aui_Base
{
public:
	
	aui_Base() { m_baseRefCount++; }
	virtual ~aui_Base() { m_baseRefCount--; }

	static sint32	GetBaseRefCount( void ) { return m_baseRefCount; }

protected:
	static sint32	m_baseRefCount;
};


#endif 
