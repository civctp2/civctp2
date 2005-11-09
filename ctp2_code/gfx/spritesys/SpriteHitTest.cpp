//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "pixelutils.h"
#include "spriteutils.h"
#include "Sprite.h"


BOOL Sprite::HitTestLow(POINT mousePt, Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
					 uint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	uint8			*surfBase;

	
	sint32 surfWidth = m_surfWidth;
	sint32 surfHeight = m_surfHeight;
	sint32 surfPitch = m_surfPitch;

	
	if (drawX < 0 - width) return FALSE;
	if (drawY < 0 - height) return FALSE;
	if (drawX >= surfWidth - width) return FALSE;
	if (drawY >= surfHeight - height) return FALSE;

	
	
	if (drawX < 0 || drawY < 0 || drawX >= surfWidth || drawY >= surfHeight) {
		return FALSE;
	}

	

	surfBase = m_surfBase + (drawY * surfPitch) + (drawX * sizeof(Pixel16));

	
	Pixel16		*table = frame+1;
	Pixel16		*dataStart = table + height;

	register	sint32 j;
	sint32		len;

	
	mousePt.x -= drawX;
	mousePt.y -= drawY;

	sint32 x = 0;

	
	for(j=0; j<height; j++) {
		if (table[j] != k_EMPTY_TABLE_ENTRY) {		
			Pixel16		*rowData;
			Pixel16		tag;

			rowData = dataStart + table[j];
			
			tag = *rowData++;
			tag = tag & 0x0FFF;	
			
			x = 0;

			while ((tag & 0xF000) == 0) {
				switch ((tag & 0x0F00) >> 8) {
					case k_CHROMAKEY_RUN_ID	:
							x += (tag & 0x00FF);
						break;
					case k_COPY_RUN_ID			: {
							len = (tag & 0x00FF);
							if (mousePt.y == j && mousePt.x >= x && mousePt.x < x + len) 
								return TRUE;
							x += len;
							rowData += len;
						}
						break;
					case k_SHADOW_RUN_ID		: {
							len = (tag & 0x00FF);
							if (mousePt.y == j && mousePt.x >= x && mousePt.x < x+len)
								return TRUE;
							x += len;
						}
						break;
					case k_FEATHERED_RUN_ID	:
						
						if (mousePt.y == j && mousePt.x == x)
							return TRUE;

						rowData++;
						x++;

						break;
					default:
						Assert(FALSE);
						break;
				}
				tag = *rowData++;
			}
		}
	}

	return FALSE;
}





BOOL Sprite::HitTestLowReversed(POINT mousePt, Pixel16 *frame, sint32 drawX, sint32 drawY, sint32 width, sint32 height,
							 uint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	uint8			*surfBase;

	
	sint32 surfWidth = m_surfWidth;
	sint32 surfHeight = m_surfHeight;
	sint32 surfPitch = m_surfPitch;

	
	if (drawX < 0 - width) return FALSE;
	if (drawY < 0 - height) return FALSE;
	if (drawX >= surfWidth - width) return FALSE;
	if (drawY >= surfHeight - height) return FALSE;

	if (drawX < 0 || drawY < 0 || drawX >= surfWidth || drawY >= surfHeight) {
		return FALSE;
	}

	
	surfBase = m_surfBase + (drawY * surfPitch) + (drawX * sizeof(Pixel16));

	
	Pixel16		*table = frame+1;
	Pixel16		*dataStart = table + height;

	register	sint32 j;
	register	sint32 len;

	mousePt.x -= drawX;
	mousePt.y -= drawY;

	sint32 x;

	
	for(j=0; j < height; j++) {
		if (table[j] != k_EMPTY_TABLE_ENTRY) {		
			Pixel16		*rowData;
			Pixel16		tag;

			rowData = dataStart + table[j];
			tag = *rowData++;
			tag = tag & 0x0FFF;	

			x = width-1;

			while ((tag & 0xF000) == 0) {
				switch ((tag & 0x0F00) >> 8) {
					case k_CHROMAKEY_RUN_ID	:
							x -= (tag & 0x00FF);
						break;
					case k_COPY_RUN_ID			: {
							len = (tag & 0x00FF);
	
							if (mousePt.y == j && mousePt.x < x && mousePt.x >= x-len)
								return TRUE;
							x -= len;
							rowData += len;
						}
						break;
					case k_SHADOW_RUN_ID		: {
							len = (tag & 0x00FF);
							if (mousePt.y == j && mousePt.x < x && mousePt.x >= x-len)
								return TRUE;
							x -= len;
						}
						break;
					case k_FEATHERED_RUN_ID	:
					
						if (mousePt.y == j && mousePt.x == x) 
							return TRUE;

						rowData++;
						x--;
						break;
				}
				tag = *rowData++;
			}
		}
	}
	
	return FALSE;
}

#define k_MEDIUM_KEY	0x4208




BOOL Sprite::HitTestScaledLow(POINT mousePt, Pixel16 *data, sint32 x, sint32 y, sint32 destWidth, sint32 destHeight,
							 uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL reverse)
{
	uint8			*surfBase;

	
	Pixel16		emptyRow[2];

	emptyRow[0] = (Pixel16)((k_CHROMAKEY_RUN_ID << 8) | m_width);
	emptyRow[1] = (Pixel16)(k_EOLN_ID << 8);

	RECT destRect = { x, y, x + destWidth, y + destHeight };

	
	sint32 surfWidth = m_surfWidth;
	sint32 surfHeight = m_surfHeight;
	sint32 surfPitch = m_surfPitch;

	surfBase = m_surfBase + (y * surfPitch) + (x * sizeof(Pixel16));

	if (destRect.left < 0) return FALSE;
	if (destRect.top < 0) return FALSE;
	if (destRect.right > surfWidth) return FALSE;
	if (destRect.bottom > surfHeight) return FALSE;

	
	Pixel16			*destPixel;

	Pixel16			*table = data+1;
	Pixel16			*dataStart = table + m_height;

	
	sint32				vaccum;
	sint32				vincx, vincxy;
	sint32				vend;
	sint32				vdestpos;
	sint32				vpos1, vpos2;

	vaccum = destHeight*2 - m_height;
	vincx = destHeight*2;
	vincxy = (destHeight - m_height) * 2 ;

	vpos1 = 0;
	vpos2 = (sint32)((double)(m_height - destHeight) / (double)destHeight);

	vdestpos = y;
	vend = m_height - 1;

	mousePt.x -= x;
	mousePt.y -= y;

	
	while ( vpos1 < vend) {
		if (vaccum < 0) {
			vaccum += vincx;
		} else {
			
			Pixel16		*rowData1, *rowData2;
			Pixel16		pixel1, pixel2;

			sint32		haccum;
			sint32		hincx, hincxy;
			sint32		hend;
			sint32		hpos;
			sint32		hdestpos;

			
			if (table[vpos1] == k_EMPTY_TABLE_ENTRY) rowData1 = emptyRow;
			else rowData1 = dataStart + table[vpos1];	
			if (table[vpos2] == k_EMPTY_TABLE_ENTRY) rowData2 = emptyRow;
			else rowData2 = dataStart + table[vpos2];

			haccum = destWidth*2 - m_width;
			hincx = destWidth*2;
			hincxy = (destWidth - m_width) * 2;
			
			hpos = 0;
			if (reverse) hdestpos = x + destWidth;
			else hdestpos = x;
			hend = m_width-1;

			sint32		mode1;
			sint32		mode2;
			sint32		pos1 = 0;
			sint32		pos2 = 0;
			sint32		end1, 
						end2;
			sint32		alpha1, 
						alpha2;
			sint32		oldend1 = 0, 
						oldend2 = 0;

			end1 = ReadTag(&mode1, &rowData1, &alpha1);
			end2 = ReadTag(&mode2, &rowData2, &alpha2);

			pixel1 = k_MEDIUM_KEY;
			pixel2 = k_MEDIUM_KEY;

			while (hpos < hend) {
				if (haccum < 0) {
					haccum += hincx;
				} else {
					haccum += hincxy;

					destPixel = (Pixel16 *)(surfBase + ((vdestpos-y) * surfPitch) + ((hdestpos-x) * 2));

					
					while (pos1 <= hpos) {
						switch (mode1) {
							case k_CHROMAKEY_RUN_ID	:
								break;
							case k_COPY_RUN_ID			:
									if (mousePt.x == (hdestpos-x) && mousePt.y == (vdestpos-y))
										return TRUE;
									rowData1++;
								break;
							case k_SHADOW_RUN_ID		:
									if (mousePt.x == (hdestpos-x) && mousePt.y == (vdestpos-y))
										return TRUE;
								break;			
							case k_FEATHERED_RUN_ID	:
									if (mousePt.x == (hdestpos-x) && mousePt.y == (vdestpos-y))
										return TRUE;
									rowData1++;
								break;
							default:
								Assert(mode1 == k_CHROMAKEY_RUN_ID || mode1 == k_COPY_RUN_ID || mode1 == k_SHADOW_RUN_ID || mode1 == k_FEATHERED_RUN_ID);
						}
						
						pos1++;

						if (pos1 >= end1) {
							oldend1 = end1;
							end1 = oldend1 + ReadTag(&mode1, &rowData1, &alpha1);
						}
					}

					
					while (pos2 <= hpos) {
					
						switch (mode2) {
							case k_CHROMAKEY_RUN_ID	:
								break;
							case k_COPY_RUN_ID			:
									if (mousePt.x == (hdestpos-x) && mousePt.y == (vdestpos-y))
										return TRUE;
									rowData2++;
								break;
							case k_SHADOW_RUN_ID		:
									if (mousePt.x == (hdestpos-x) && mousePt.y == (vdestpos-y))
										return TRUE;
								break;
							case k_FEATHERED_RUN_ID	:
									if (mousePt.x == (hdestpos-x) && mousePt.y == (vdestpos-y))
										return TRUE;
									rowData2++;
								break;
							default : 
								Assert(mode2 == k_CHROMAKEY_RUN_ID || mode2 == k_COPY_RUN_ID || mode2 == k_SHADOW_RUN_ID || mode2 == k_FEATHERED_RUN_ID);
						}

						pos2++;

						if (pos2 >= end2) {
							oldend2 = end2;
							end2 = oldend2 + ReadTag(&mode2, &rowData2, &alpha2);
						}
					}

					if (reverse) hdestpos--;
					else hdestpos++;
				}
				hpos++;
			}

			vaccum += vincxy;
			vdestpos++;
		} 
		vpos1++;
		vpos2++;
	}

	return FALSE;
}
