
#ifndef __KEYBOARD_HANDLER_H__
#define __KEYBOARD_HANDLER_H__






class KeyboardHandler {
public:
	KeyboardHandler() {}
	virtual ~KeyboardHandler() {}

	virtual void kh_Close() {}  
	virtual void kh_Select() {} 
	virtual void kh_Next() {}   
	virtual void kh_Prev() {}   
	virtual void kh_Up() {}     
	virtual void kh_Down() {}   
	virtual void kh_Left() {}   
	virtual void kh_Right() {}  
};

#endif
