










#ifndef __AUI_INPUT_H__
#define __AUI_INPUT_H__



class aui_Input
{
public:
	aui_Input();
	virtual ~aui_Input();

	virtual AUI_ERRCODE GetInput( void ) { return AUI_ERRCODE_OK; }
	virtual AUI_ERRCODE ReactToInput( void ) { return AUI_ERRCODE_OK; }

	virtual AUI_ERRCODE Acquire( void );
	virtual AUI_ERRCODE Unacquire( void );

protected:
	bool IsAcquired( void ) const { return m_acquired; }

private:
	bool m_acquired;
};


#endif 
