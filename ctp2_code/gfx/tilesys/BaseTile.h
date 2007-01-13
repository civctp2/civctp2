










#ifdef HAVE_PRAGMA_ONCE 
#pragma once
#endif
#ifndef __BASETILE_H__
#define __BASETILE_H__

#include "tileutils.h"

#define k_BTF_TRANSITION0		0x01
#define k_BTF_TRANSITION1		0x02
#define k_BTF_TRANSITION2		0x04
#define k_BTF_TRANSITION3		0x08

#define k_BTF_QUICKLOADED		0x80

class BaseTile {
public:
	BaseTile();
	~BaseTile();

	uint16	GetTileNum(void) const { return m_tileNum; }
	void	SetTileNum(uint16 num) { m_tileNum = num; }

	uint8	GetBaseType(void) const { return m_baseType; }
	void	SetBaseType(uint8 type) { m_baseType = type; }



	BOOL	GetTransitionFlag(sint16 flagNum) 
	{
		switch (flagNum) {
		case 0: return m_flags & k_BTF_TRANSITION0;
		case 1: return m_flags & k_BTF_TRANSITION1;
		case 2: return m_flags & k_BTF_TRANSITION2;
		case 3: return m_flags & k_BTF_TRANSITION3;
		default:
			return FALSE;
		}
	}

	void	SetTransitionFlag(sint16 flagNum, BOOL flagVal)
	{
		switch (flagNum) {
		case 0:
			if (flagVal) m_flags |= k_BTF_TRANSITION0;
			else m_flags &= ~k_BTF_TRANSITION0;
			break;
		case 1:
			if (flagVal) m_flags |= k_BTF_TRANSITION1;
			else m_flags &= ~k_BTF_TRANSITION1;
			break;
		case 2:
			if (flagVal) m_flags |= k_BTF_TRANSITION2;
			else m_flags &= ~k_BTF_TRANSITION2;
			break;
		case 3:
			if (flagVal) m_flags |= k_BTF_TRANSITION3;
			else m_flags &= ~k_BTF_TRANSITION3;
			break;
		}
	}
	




	void	SetTileDataLen(uint16 len) { m_tileDataLen = len; }
	uint16	GetTileDataLen(void) const { return m_tileDataLen; }

	Pixel16 *GetTileData(void) const { return m_tileData; }
	void	SetTileData(Pixel16 *data) { m_tileData = data; }








	void	SetHatDataLen(uint16 dataLen) { m_hatDataLen = dataLen; }
	uint16	GetHatDataLen(void) const { return m_hatDataLen; }

	void	SetHatData(Pixel16 *hatData) { m_hatData = hatData; }
	Pixel16	*GetHatData(void) const { return m_hatData; }

	BOOL	Read(FILE *file);
	BOOL	QuickRead(uint8 **dataPtr, BOOL mapped);
	BOOL	WasQuickLoaded(void) { return m_flags & k_BTF_QUICKLOADED; }
private:





	uint8		m_flags;
	uint8		m_baseType;
	uint16		m_tileNum;

	uint16		m_tileDataLen;
	uint16		m_hatDataLen;





	Pixel16		*m_tileData;
	Pixel16		*m_hatData;
};

#endif
