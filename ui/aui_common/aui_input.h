










#ifndef __AUI_INPUT_H__
#define __AUI_INPUT_H__



class aui_Input
{
public:
	
	aui_Input(
		AUI_ERRCODE *retval );
	virtual ~aui_Input();

protected:
	aui_Input() {}
	AUI_ERRCODE InitCommon( void );

public:
	
	virtual AUI_ERRCODE GetInput( void ) { return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE ReactToInput( void ) { return AUI_ERRCODE_OK; }

	
	virtual AUI_ERRCODE Acquire( void );
	virtual AUI_ERRCODE Unacquire( void );

protected:
	BOOL IsAcquired( void ) const { return m_acquired; }

	BOOL m_acquired;
};


#endif 
