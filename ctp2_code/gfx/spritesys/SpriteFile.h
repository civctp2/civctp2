











 
#pragma once

#include "c3.h"
#include "Action.h"


#ifndef __SPRITEFILE_H__
#define __SPRITEFILE_H__

#define k_SPRITEFILE_TAG			'SPRF'
#define k_SPRITEFILE_VERSION0		0x00010003 
#define k_SPRITEFILE_VERSION1		0x00020000 
#define k_SPRITEFILE_VERSION2		0x00020001 


enum  
{
	
	
	SPRDATA_REGULAR=0, 
	SPRDATA_LZW1, 
   	
	
	
	

	SPRDATA_MAX
};

#define  COM_BUFF_SIZE 320000


#define LZW1_FLAG_BYTES    4  
#define LZW1_FLAG_COMPRESS 0  
#define LZW1_FLAG_COPY     1  
#define LZW1_PS (*p++!=*s++)  
#define LZW1_ITEMMAX 16       

enum SPRITEFILETYPE 
{
	SPRITEFILETYPE_PLAIN,
	SPRITEFILETYPE_FACED,
	SPRITEFILETYPE_FACEDWSHADOW,
	SPRITEFILETYPE_GROUP,
	SPRITEFILETYPE_UNIT,
	SPRITEFILETYPE_PROJECTILE,
	SPRITEFILETYPE_EFFECT,
	SPRITEFILETYPE_GOOD,
	SPRITEFILETYPE_CITY,

	SPRITEFILETYPE_MAX
};

enum SPRITEFILEERR 
{
	SPRITEFILEERR_OK,

	SPRITEFILEERR_NOTFOUND,
	SPRITEFILEERR_NOCREATE,
	SPRITEFILEERR_NOOPEN,
	SPRITEFILEERR_READERR,
	SPRITEFILEERR_WRITEERR,
	SPRITEFILEERR_BADTAG,
	SPRITEFILEERR_BADVERSION,

	SPRITEFILEERR_MAX
};




typedef struct
{
  unsigned num_records;
  unsigned offset_to_special_data;


} SPRITEFILE_2_0;


#define k_SPRITEFILE_HEADER_SIZE	(sizeof(uint32) + sizeof(uint32) + sizeof(uint32))

class Sprite;
class FacedSprite;
class FacedSpriteWshadow;
class SpriteGroup;
class UnitSpriteGroup;
class ProjectileSpriteGroup;
class EffectSpriteGroup;
class GoodSpriteGroup;
class CitySpriteGroup;

class Anim;

class SpriteFile 
{
public:
	SpriteFile(MBCHAR *filename);
	~SpriteFile();

	void			WriteSpriteData(Sprite *s);
	void			WriteFacedSpriteData(FacedSprite *s);
	void			WriteFacedSpriteWshadowData(FacedSpriteWshadow *s);
	void			WriteAnimData(Anim *a);

	SPRITEFILEERR	Create(SPRITEFILETYPE type,unsigned version,unsigned compression_mode);
	SPRITEFILEERR	Write(Sprite *s, Anim *anim);
	SPRITEFILEERR	Write(FacedSprite *s, Anim *anim);
	SPRITEFILEERR	Write(FacedSpriteWshadow *s, Anim *anim);
	SPRITEFILEERR	Write(SpriteGroup *s, Anim *anim);
	SPRITEFILEERR	Write(UnitSpriteGroup *s);
	SPRITEFILEERR	Write(ProjectileSpriteGroup *s);
	SPRITEFILEERR	Write(EffectSpriteGroup *s);
	SPRITEFILEERR	Write(GoodSpriteGroup *s);
	SPRITEFILEERR	Write(CitySpriteGroup *s, Anim *anim);
	SPRITEFILEERR	CloseWrite();

	
	void			ReadSpriteDataBasic(Sprite *s);
	void			ReadSpriteDataFull(Sprite *s);
	void			SkipSpriteData();

	void			ReadFacedSpriteDataBasic(FacedSprite *s);
	void			ReadFacedSpriteDataFull(FacedSprite *s);
	void			SkipFacedSpriteData();
	
	void			ReadSpriteDataGeneralBasic(Sprite **sprite);
	void			ReadSpriteDataGeneralFull(Sprite **sprite);

	void			SkipSpriteDataGeneral(void);

	void			ReadSpriteDataGeneral(FacedSpriteWshadow **sprite);
	void			ReadFacedSpriteWshadowData(FacedSpriteWshadow *s);
	

	void			ReadAnimDataBasic(Anim *s);
	void			ReadAnimDataFull(Anim *s);
	void			SkipAnimData(void);

	SPRITEFILEERR	Open(SPRITEFILETYPE *type);
	SPRITEFILEERR	Read(Sprite **s, Anim **anim);
	SPRITEFILEERR	Read(FacedSprite **s, Anim **anim);
	SPRITEFILEERR	Read(FacedSpriteWshadow **s, Anim **anim);
	SPRITEFILEERR	Read(SpriteGroup **s, Anim **anim);

	
	SPRITEFILEERR	ReadBasic  (UnitSpriteGroup *s);
	SPRITEFILEERR	ReadBasic  (GoodSpriteGroup *s);
	
	
	SPRITEFILEERR	ReadFull   (UnitSpriteGroup *s);
	SPRITEFILEERR	ReadFull   (GoodSpriteGroup *s);
	
	
	SPRITEFILEERR	ReadIndexed(UnitSpriteGroup *s,GAME_ACTION index);
	SPRITEFILEERR	ReadIndexed(GoodSpriteGroup *s,GAME_ACTION index);
	
	SPRITEFILEERR	Read(ProjectileSpriteGroup *s);
	SPRITEFILEERR	Read(EffectSpriteGroup *s);


	SPRITEFILEERR	Read(CitySpriteGroup **s, Anim **anim);
	SPRITEFILEERR	CloseRead();

	SPRITEFILEERR	WriteData(uint8 *data, size_t bytes);
	SPRITEFILEERR	WriteData(sint16 data);
	SPRITEFILEERR	WriteData(uint16 data);
	SPRITEFILEERR	WriteData(sint32 data);
	SPRITEFILEERR	WriteData(uint32 data);

	SPRITEFILEERR	ReadData(void *data, size_t bytes);

	sint32			GetFilePos(void);
	void			SetFilePos(sint32 pos);

protected:
	unsigned	m_version;			
	unsigned	m_spr_compression;	

	
	SPRITEFILEERR	ReadFull_v13(UnitSpriteGroup *s);
	SPRITEFILEERR	ReadFull_v20(UnitSpriteGroup *s);

	SPRITEFILEERR	ReadBasic_v13(UnitSpriteGroup *s);
	SPRITEFILEERR	ReadBasic_v20(UnitSpriteGroup *s);

	SPRITEFILEERR	ReadIndexed_v13(UnitSpriteGroup *s,GAME_ACTION index);
	SPRITEFILEERR	ReadIndexed_v20(UnitSpriteGroup *s,GAME_ACTION index);

	
	SPRITEFILEERR	Write_v13(UnitSpriteGroup *s);
	SPRITEFILEERR	Write_v20(UnitSpriteGroup *s);

	
	void *CompressData  (void *Data,unsigned &DataLen);
	void *DeCompressData(void *Data,unsigned CompressedLen,unsigned ActualLen);
	
	
	void *CompressData_Default  (void *Data,unsigned &DataLen);
	void *DeCompressData_Default(void *Data,unsigned CompressedLen,unsigned ActualLen);

	
	void *CompressData_LZW1  (void *Data,unsigned &DataLen);
	void *DeCompressData_LZW1(void *Data,unsigned CompressedLen,unsigned ActualLen);

private:
	char   		*m_file_data;
	sint32       m_file_size;
	FILE		*m_file;
	MBCHAR		 m_filename[_MAX_PATH];
};

#endif
