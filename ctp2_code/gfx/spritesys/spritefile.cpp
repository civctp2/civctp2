//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite file handling
// Id           : $Id:$
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
// __MAKESPR__
// __SPRITETEST__
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Crash prevention, small clean-ups.
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "pixelutils.h"
#include "tiffutils.h"
#include "spriteutils.h"
#include "c3files.h"
#include "c3errors.h"

#include "Sprite.h"
#include "FacedSprite.h"
#include "FacedSpriteWshadow.h"
#include "SpriteGroup.h"
#include "UnitSpriteGroup.h"
#include "GoodSpriteGroup.h"
#include "CitySpriteGroup.h"

#include "EffectSpriteGroup.h"
#include "Anim.h"
#include "SpriteFile.h"
#include "CivPaths.h"               // g_civPaths
#include "profileDB.h"              // g_theProfileDB


#ifdef __MAKESPR__
unsigned char g_compression_buff[COM_BUFF_SIZE];
#else
unsigned char *g_compression_buff=NULL;
#endif

SpriteFile::SpriteFile(MBCHAR const * name)
:
	m_version           (k_SPRITEFILE_VERSION0),
	m_spr_compression   (SPRDATA_REGULAR),
	m_file              (NULL)
{
	strcpy(m_filename, name);
}

SpriteFile::~SpriteFile()
{
    if (m_file)
    {
        c3files_fclose(m_file);
    }
}

void SpriteFile::WriteSpriteData(Sprite *s)
{
	size_t    current_pos;
	
	
	WriteData(s->GetType());
	
	WriteData(s->GetWidth());
	
	WriteData(s->GetHeight());
	
	WriteData((sint32)s->GetHotPoint().x);
	
	WriteData((sint32)s->GetHotPoint().y);

	
	WriteData(s->GetFirstFrame());

	
	WriteData(s->GetNumFrames());

	
	uint16		i;
	size_t		size,compressed_size;
	uint32		normal_ssizes[800];
	uint32		normal_msizes[800];
	uint32		compressed_ssizes[800];

	
	size_t    frame_offset_pos=GetFilePos();

	
	for (i=0; i<s->GetNumFrames(); i++) 
	{
	    normal_ssizes[i] = s->GetFrameDataSize(i); 
	    normal_msizes[i] = s->GetMiniFrameDataSize(i); 
	}
	
	WriteData((uint8 *)normal_ssizes,s->GetNumFrames()*sizeof(uint32));
	WriteData((uint8 *)normal_msizes,s->GetNumFrames()*sizeof(uint32));

	
	uint8 *CompressedData;

	
	for (i=0; i<s->GetNumFrames(); i++) 
	{
		
		
		spriteutils_ConvertPixelFormatForFile(s->GetFrameData(i), s->GetWidth(), s->GetHeight());

		size = s->GetFrameDataSize(i);
	   
		CompressedData = CompressData((void *)s->GetFrameData(i),size);

		compressed_size = size;

		if(m_version>k_SPRITEFILE_VERSION1)
		{
			size += sizeof(uint32);
			WriteData((uint32)normal_ssizes[i]);
		}

		WriteData((uint8 *)CompressedData, compressed_size);
	
		compressed_ssizes[i] = compressed_size;

		
		delete[] (unsigned char*) CompressedData;
	}
	
	
	if(m_version>k_SPRITEFILE_VERSION1)
	{
	   current_pos = GetFilePos();
       
	   SetFilePos(frame_offset_pos);
	   
	   WriteData((uint8 *)compressed_ssizes,s->GetNumFrames()*sizeof(uint32));
	
	   SetFilePos(current_pos);
	}

	for (i=0; i<s->GetNumFrames(); i++) 
	{
		
		
		spriteutils_ConvertPixelFormatForFile(s->GetMiniFrameData(i), s->GetWidth()/2, s->GetHeight()/2);
		size = s->GetMiniFrameDataSize(i);
		WriteData((uint8 *)s->GetMiniFrameData(i), size);
	}
}

void SpriteFile::WriteFacedSpriteData(FacedSprite *s)
{
	
	uint16 num_frames=s->GetNumFrames();

	
	uint32		normal_ssizes[k_MAX_FACINGS][128];
	uint32		normal_msizes[k_MAX_FACINGS][128];
	uint32		compressed_ssizes[k_MAX_FACINGS][128];
	size_t		frame_offsets[k_MAX_FACINGS];
	size_t    current_pos;

	
	WriteData(s->GetType());
	
	WriteData(s->GetWidth());
	
	WriteData(s->GetHeight());
	
	WriteData((uint8 *)s->GetHotPoints(), sizeof(POINT) * k_NUM_FACINGS);

	
	WriteData(s->GetFirstFrame());

	
	WriteData(num_frames);

	

	uint16	i, j;

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		frame_offsets[j] = GetFilePos();

		
		for (i=0; i<num_frames; i++)
		{
			normal_ssizes[j][i] = s->GetFrameDataSize(j, i);
			normal_msizes[j][i] = s->GetMiniFrameDataSize(j, i);
		}   

		
		WriteData((uint8 *)normal_ssizes[j],sizeof(uint32) * s->GetNumFrames());
		WriteData((uint8 *)normal_msizes[j],sizeof(uint32) * s->GetNumFrames());
	}	

	
	uint8 *CompressedData;

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		for (i=0; i<num_frames; i++) 
		{
			
			
			spriteutils_ConvertPixelFormatForFile(s->GetFrameData(j,i), 
													s->GetWidth(), s->GetHeight());

			size_t size,compressed_size;
			size = normal_ssizes[j][i];

			CompressedData = CompressData((void *)s->GetFrameData(j,i),size);
  
			compressed_size = size;
			
			if(m_version>k_SPRITEFILE_VERSION1)
				WriteData((uint32)normal_ssizes[j][i]);

			WriteData((uint8 *)CompressedData, compressed_size);
		
			compressed_ssizes[j][i] = size;

			
			delete[] (uint8 *) CompressedData;
		}

		
		for (i=0; i<num_frames; i++) {
			
			
			spriteutils_ConvertPixelFormatForFile(s->GetMiniFrameData(j,i), 
													s->GetWidth()/2, s->GetHeight()/2);
			WriteData((uint8 *)s->GetMiniFrameData(j,i), normal_msizes[j][i]);
		}
	}

	
	if(m_version>k_SPRITEFILE_VERSION1)
	{
	   current_pos = GetFilePos();
       
	   for (j=0; j<k_NUM_FACINGS; j++) 
	   {
	      SetFilePos(frame_offsets[j]);
	      WriteData((uint8 *)compressed_ssizes[j],num_frames*sizeof(uint32));
	   }
	   SetFilePos(current_pos);
	}
}

void SpriteFile::WriteFacedSpriteWshadowData(FacedSpriteWshadow *s)
{
	
	WriteData(s->GetType());
	
	WriteData(s->GetWidth());
	
	WriteData(s->GetHeight());
	
	WriteData((uint8 *)s->GetHotPoints(), sizeof(POINT) * k_NUM_FACINGS);

	
	WriteData(s->GetFirstFrame());

	
	WriteData(s->GetNumFrames());
	
	WriteData((uint16)s->GetHasShadow());

	

	uint16	i, j;

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size_t size;
			if(s->GetFrameData(j, i) == NULL)
			{
				size = 0;
			}
			else
			{
				size = s->GetFrameDataSize(j, i);
			}
			WriteData((uint32)size);
		}


		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size_t size;
			if(s->GetMiniFrameData(j, i) == NULL)
			{
				size = 0;
			}
			else
			{
				size = s->GetMiniFrameDataSize(j, i);
			}
			WriteData((uint32)size);
		}

		if(s->GetHasShadow())
		{
			
			for (i=0; i<s->GetNumFrames(); i++) 
			{
				size_t size;
				if(s->GetShadowFrameData(j, i) == NULL)
				{
					size = 0;
				}
				else
				{
					size = s->GetShadowFrameDataSize(j, i);
				}
				WriteData((uint32)size);
			}
			
			
			for (i=0; i<s->GetNumFrames(); i++) 
			{
				size_t size;
					if(s->GetMiniShadowFrameData(j, i) == NULL)
					{
						size = 0;
					}
					else
					{
						size = s->GetMiniShadowFrameDataSize(j, i);	
					}
				WriteData((uint32)size);
			}
		}
	}


	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size_t size;
			if(s->GetFrameData(j, i) != NULL)
			{
				size = s->GetFrameDataSize(j, i);
				WriteData((uint8 *)s->GetFrameData(j,i), size);
			}
		}

		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size_t size;
			if(s->GetMiniFrameData(j, i) != NULL)
			{
				size = s->GetMiniFrameDataSize(j, i);
				WriteData((uint8 *)s->GetMiniFrameData(j,i), size);
			}
		}
		if(s->GetHasShadow())
		{
			
			for (i=0; i<s->GetNumFrames(); i++) 
			{
				size_t size;
				if(s->GetShadowFrameData(j, i) != NULL)
				{
					size = s->GetShadowFrameDataSize(j, i);

					WriteData((uint8 *)s->GetShadowFrameData(j,i), size);
				}
			}
			
			
			for (i=0; i<s->GetNumFrames(); i++) 
			{
				size_t size;
				if(s->GetMiniShadowFrameData(j, i) != NULL)
				{
					size = s->GetMiniShadowFrameDataSize(j, i);

					WriteData((uint8 *)s->GetMiniShadowFrameData(j,i), size);
				}
			}
		}
	}
}


void SpriteFile::WriteAnimData(Anim *a)
{
	
	WriteData(a->GetType());
	
	WriteData(a->GetNumFrames());
	
	WriteData(a->GetPlaybackTime());
	
	WriteData(a->GetDelay());
	
	WriteData((uint8 *)a->GetFrames(), sizeof(uint16) * a->GetNumFrames());
	
	WriteData((uint8 *)a->GetDeltas(), sizeof(POINT) * a->GetNumFrames());
	
	WriteData((uint8 *)a->GetTransparencies(), sizeof(uint16) * a->GetNumFrames());
}




void SpriteFile::ReadSpriteDataBasic(Sprite *s)
{
	uint16		data16;
	uint32		data32;
	sint32		x, y;

	
	ReadData((void *)&data16, sizeof(data16));
	s->SetWidth(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetHeight(data16);
	
	ReadData((void *)&data32, sizeof(data32));
	x = (sint32)data32;
	
	ReadData((void *)&data32, sizeof(data32));
	y = (sint32)data32;
	s->SetHotPoint(x, y);
	
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetFirstFrame(data16);

	
	ReadData((void *)&data16, sizeof(data16));
	s->SetNumFrames((uint16)data16);

	s->AllocateFrameArrays();

	
	uint16		i;
	uint32		ssizes[800];
	uint32		msizes[800];
	Pixel16		*CompressedData,*ActualData;
	uint32		size,actual_size;

	
	
	
	ReadData((uint8 *)ssizes, sizeof(uint32) * s->GetNumFrames());
	
	ReadData((uint8 *)msizes, sizeof(uint32) * s->GetNumFrames());

	
	size = ssizes[0];

	
	CompressedData = (Pixel16 *)new uint8[size];
	
	
	if(m_version>k_SPRITEFILE_VERSION1)
		ReadData((void *)&actual_size,sizeof(uint32));
	else
		actual_size = size;

	
	ReadData((void *)CompressedData, size);

	
	ActualData = (Pixel16 *)DeCompressData(CompressedData,size,actual_size);

	
	delete CompressedData;

	
	spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth(), s->GetHeight());
	
	
	s->SetFrameData(0, ActualData);

	for (i=1; i<s->GetNumFrames(); i++) 
	{
		size = ssizes[i];

	    if(m_version>k_SPRITEFILE_VERSION1)
		  size += sizeof(size);	

		SetFilePos(GetFilePos() + size);
	}

	size        = msizes[0];
	ActualData  = (Pixel16 *)new uint8[size];

	ReadData((void *)ActualData, size);
	
	spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth()/2, s->GetHeight()/2);
	
	s->SetMiniFrameData(0, ActualData);

	for (i=1; i<s->GetNumFrames(); i++) 
	{
		size = msizes[i];
		SetFilePos(GetFilePos() + size);
	}

	s->SetNumFrames(1);
}

void SpriteFile::ReadSpriteDataFull(Sprite *s)
{
	uint16		data16;
	uint32		data32;
	sint32		x, y;

	
	ReadData((void *)&data16, sizeof(data16));
	s->SetWidth(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetHeight(data16);
	
	ReadData((void *)&data32, sizeof(data32));
	x = (sint32)data32;
	
	ReadData((void *)&data32, sizeof(data32));
	y = (sint32)data32;
	s->SetHotPoint(x, y);
	
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetFirstFrame(data16);

	
	ReadData((void *)&data16, sizeof(data16));
	s->SetNumFrames((uint16)data16);

	s->AllocateFrameArrays();

	
	uint16		i;
	uint32		ssizes[800];
	uint32		msizes[800];
	Pixel16		*CompressedData,*ActualData;
	uint32		size,actual_size;

	
	ReadData((uint8 *)ssizes, sizeof(uint32) * s->GetNumFrames());

	ReadData((uint8 *)msizes, sizeof(uint32) * s->GetNumFrames());

	
	for (i=0; i<s->GetNumFrames(); i++) 
	{
		size = ssizes[i];

		
		CompressedData = (Pixel16 *)new uint8[size];
	
		
		if(m_version>k_SPRITEFILE_VERSION1)
		   ReadData((void *)&actual_size,sizeof(uint32));
		else
			actual_size = size;
		
		
		ReadData((void *)CompressedData, size);
		
		
		ActualData = (Pixel16 *)DeCompressData(CompressedData,size,actual_size);
		
		
		delete CompressedData;

		
		spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth(), s->GetHeight());
		
		
		s->SetFrameData(i, ActualData);
	
	
	
	
	
	}

	
	for (i=0; i<s->GetNumFrames(); i++) 
	{
		size = msizes[i];
		ActualData = (Pixel16 *) new uint8[size];
		ReadData((void *)ActualData, size);
		spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth()/2, s->GetHeight()/2);
		s->SetMiniFrameData(i, ActualData);
	}
}

void SpriteFile::SkipSpriteData(void)
{
	uint16		data16;
	uint32		data32;

	uint16		numFrames;

	
	ReadData((void *)&data16, sizeof(data16));
	
	ReadData((void *)&data16, sizeof(data16));
	
	ReadData((void *)&data32, sizeof(data32));
	
	ReadData((void *)&data32, sizeof(data32));
	
	ReadData((void *)&data16, sizeof(data16));

	
	ReadData((void *)&data16, sizeof(data16));
	numFrames = (uint16)data16;

	
	uint16		i;
	uint32		ssizes[800];
	uint32		msizes[800];
	
	uint32		size;

	
	ReadData((uint8 *)ssizes, sizeof(uint32) * numFrames);
	ReadData((uint8 *)msizes, sizeof(uint32) * numFrames);

	
	for (i=0; i<numFrames; i++) 
	{
		size = ssizes[i];
		SetFilePos(GetFilePos() + size);
	}

	
	for (i=0; i<numFrames; i++) 
	{
		size = msizes[i];
		SetFilePos(GetFilePos() + size);
	}
}

void SpriteFile::ReadFacedSpriteDataBasic(FacedSprite *s)
{
	uint16		data16;
	POINT		points[k_NUM_FACINGS];

	
	ReadData((void *)&data16, sizeof(data16));
	s->SetWidth(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetHeight(data16);
	
	ReadData((void *)points, sizeof(POINT) * k_NUM_FACINGS);
	s->SetHotPoints(points);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetFirstFrame(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetNumFrames((uint16)data16);
	s->AllocateFrameArrays();

	
	uint16		i, j;
	size_t		ssizes[k_MAX_FACINGS][512];
	size_t		msizes[k_MAX_FACINGS][512];
	Pixel16		*CompressedData,*ActualData;
	size_t		size,actual_size;

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		ReadData((uint8 *)ssizes[j], sizeof(uint32) * s->GetNumFrames());
		
		ReadData((uint8 *)msizes[j], sizeof(uint32) * s->GetNumFrames());
	}

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		size = ssizes[j][0];
		
		CompressedData = (Pixel16 *)new uint8[size];
		
		
		if(m_version>k_SPRITEFILE_VERSION1)
		   ReadData((void *)&actual_size,sizeof(uint32));
		else
		   actual_size = size;

		
		ReadData((void *)CompressedData, size);

		
		ActualData = (Pixel16 *)DeCompressData(CompressedData,size,actual_size);

		
		delete CompressedData;

		
		spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth(), s->GetHeight());
		s->SetFrameData(j, 0, ActualData, actual_size);

		for (i=1; i<s->GetNumFrames(); i++) 
		{
			size = ssizes[j][i];

			if(m_version>k_SPRITEFILE_VERSION1)
			   size += sizeof(size);

			SetFilePos(GetFilePos() + size);
			s->SetFrameData(j, i, NULL, 0);
		}

		
		size = msizes[j][0];
		ActualData = (Pixel16 *)new uint8[size];
		ReadData((void *)ActualData, size);
		spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth()/2, s->GetHeight()/2);
		s->SetMiniFrameData(j, 0, ActualData, size);

		for (i=1; i<s->GetNumFrames(); i++) 
		{
			size = msizes[j][i];
			SetFilePos(GetFilePos() + size);
			s->SetFrameData(j, i, NULL, 0);
		}
	}

	s->SetNumFrames(1);
}

void SpriteFile::ReadFacedSpriteDataFull(FacedSprite *s)
{
	uint16		data16;
	POINT		points[k_NUM_FACINGS];

	
	ReadData((void *)&data16, sizeof(data16));
	s->SetWidth(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetHeight(data16);
	
	ReadData((void *)points, sizeof(POINT) * k_NUM_FACINGS);
	s->SetHotPoints(points);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetFirstFrame(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetNumFrames((uint16)data16);
	s->AllocateFrameArrays();

	
	uint16		i, j;
	uint32		ssizes[k_MAX_FACINGS][800];
	uint32		msizes[k_MAX_FACINGS][800];
	Pixel16		*CompressedData,*ActualData;
	uint32		size,actual_size;


	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		ReadData((uint8 *)ssizes[j], sizeof(uint32) * s->GetNumFrames());
		
		ReadData((uint8 *)msizes[j], sizeof(uint32) * s->GetNumFrames());
	}

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size = ssizes[j][i];

			CompressedData = (Pixel16 *)new uint8[size];
			
			
			if(m_version>k_SPRITEFILE_VERSION1)
			   ReadData((void *)&actual_size,sizeof(uint32));
			else
			   actual_size = size;
			
			
			ReadData((void *)CompressedData, size);
			
			
			ActualData = (Pixel16 *)DeCompressData(CompressedData,size,actual_size);
			
			
			delete CompressedData;
			
			
			spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth(), s->GetHeight());
			s->SetFrameData(j, i, ActualData, actual_size);
		}

		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size = msizes[j][i];
			ActualData= (Pixel16 *) new uint8[size];
			ReadData((void *)ActualData, size);
			spriteutils_ConvertPixelFormat((Pixel16 *)ActualData, s->GetWidth()/2, s->GetHeight()/2);
			s->SetMiniFrameData(j, i, ActualData, size);
		}
	}
}

void SpriteFile::SkipFacedSpriteData(void)
{
	uint16		data16;
	POINT		points[k_NUM_FACINGS];
	uint16		numFrames;

	
	ReadData((void *)&data16, sizeof(data16));
	
	ReadData((void *)&data16, sizeof(data16));
	
	ReadData((void *)points, sizeof(POINT) * k_NUM_FACINGS);
	
	ReadData((void *)&data16, sizeof(data16));
	
	ReadData((void *)&data16, sizeof(data16));
	numFrames = data16;

	
	uint16		i, j;
	uint32		ssizes[k_NUM_FACINGS][800];
	uint32		msizes[k_NUM_FACINGS][800];
	
	uint32		size;

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		ReadData((uint8 *)ssizes[j], sizeof(uint32) * numFrames);
		
		ReadData((uint8 *)msizes[j], sizeof(uint32) * numFrames);
	}

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		for (i=0; i<numFrames; i++) 
		{
			size = ssizes[j][i];
			SetFilePos(GetFilePos() + size);
		}

		
		for (i=0; i<numFrames; i++) 
		{
			size = msizes[j][i];
			SetFilePos(GetFilePos() + size);
		}
	}
}

void SpriteFile::ReadFacedSpriteWshadowData(FacedSpriteWshadow *s)
{
	uint16		data16;
	POINT		points[k_NUM_FACINGS];

	
	ReadData((void *)&data16, sizeof(data16));
	s->SetWidth(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetHeight(data16);
	
	ReadData((void *)points, sizeof(POINT) * k_NUM_FACINGS);
	s->SetHotPoints(points);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetFirstFrame(data16);
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetNumFrames((uint16)data16);
	s->AllocateFrameArrays();
	
	ReadData((void *)&data16, sizeof(data16));
	s->SetHasShadow(data16);
	
	uint16		i, j;
	uint32		ssizes[k_NUM_FACINGS][800];
	uint32		msizes[k_NUM_FACINGS][800];
	uint32		sh_ssizes[k_NUM_FACINGS][800];
	uint32		sh_msizes[k_NUM_FACINGS][800];
	Pixel16		*data;
	uint32		size;

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		ReadData((uint8 *)ssizes[j], sizeof(uint32) * s->GetNumFrames());
		
		ReadData((uint8 *)msizes[j], sizeof(uint32) * s->GetNumFrames());
		if(s->GetHasShadow())
		{
			
			ReadData((uint8 *)sh_ssizes[j], sizeof(uint32) * s->GetNumFrames());
			
			ReadData((uint8 *)sh_msizes[j], sizeof(uint32) * s->GetNumFrames());
		}
	}

	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size = ssizes[j][i];
			if(size != 0)
			{
				data = (Pixel16 *) new uint8[size];
				ReadData((void *)data, size);
				spriteutils_ConvertPixelFormat((Pixel16 *)data, s->GetWidth(), s->GetHeight());
			}
			else
			{
				data = NULL;
			}
			s->SetFrameData(j, i, data, size);
		}

		
		for (i=0; i<s->GetNumFrames(); i++) 
		{
			size = msizes[j][i];
			if(size != 0)
			{
				data = (Pixel16 *) new uint8[size];
				ReadData((void *)data, size);
				spriteutils_ConvertPixelFormat((Pixel16 *)data, s->GetWidth()/2, s->GetHeight()/2);
			}
			else
			{
				data = NULL;
			}
			s->SetMiniFrameData(j, i, data, size);
		}
		if(s->GetHasShadow())
		{
			
			for (i=0; i<s->GetNumFrames(); i++) 
			{
				size = sh_ssizes[j][i];
				if(size != 0)
				{
					data = (Pixel16 *) new uint8[size];
					ReadData((void *)data, size);
					spriteutils_ConvertPixelFormat((Pixel16 *)data, s->GetWidth(), s->GetHeight());
				}
				else
				{
					data = NULL;
				}
				s->SetShadowFrameData(j, i, data, size);
			}
			
			
			for (i=0; i<s->GetNumFrames(); i++) 
			{
				size = sh_msizes[j][i];
				if(size != 0)
				{
					data = (Pixel16 *) new uint8[size];
					ReadData((void *)data, size);
					spriteutils_ConvertPixelFormat((Pixel16 *)data, s->GetWidth()/2, s->GetHeight()/2);
				}
				else
				{
					data = NULL;
				}
				s->SetMiniShadowFrameData(j, i, data, size);
			}
		}
	}
}

void SpriteFile::ReadSpriteDataGeneralBasic(Sprite **sprite)
{
	uint16		data16;

	
	ReadData((void *)&data16, sizeof(data16));

	if ((SPRITETYPE)data16 == SPRITETYPE_NORMAL) 
	{
		if (*sprite == NULL)
			*sprite = new Sprite;
		(*sprite)->SetType((SPRITETYPE)data16);
		ReadSpriteDataBasic(*sprite);
	} 
	else if ((SPRITETYPE)data16 == SPRITETYPE_FACED) 
	{
			if (*sprite == NULL)
				*sprite = (Sprite *) new FacedSprite;
			(*sprite)->SetType((SPRITETYPE)data16);
			ReadFacedSpriteDataBasic((FacedSprite *)*sprite);
	} 
	else 
		Assert(FALSE);

}

void SpriteFile::ReadSpriteDataGeneralFull(Sprite **sprite)
{
	uint16		data16;

	
	ReadData((void *)&data16, sizeof(data16));

	if ((SPRITETYPE)data16 == SPRITETYPE_NORMAL) 
	{
		if (*sprite == NULL)
			*sprite = new Sprite;
		(*sprite)->SetType((SPRITETYPE)data16);
		ReadSpriteDataFull(*sprite);
	} 
	else if ((SPRITETYPE)data16 == SPRITETYPE_FACED) 
	{
			if (*sprite == NULL)
				*sprite = (Sprite *) new FacedSprite;
			(*sprite)->SetType((SPRITETYPE)data16);
			ReadFacedSpriteDataFull((FacedSprite *)*sprite);
	} 
	else 
		Assert(FALSE);

}

void SpriteFile::SkipSpriteDataGeneral(void)
{
	uint16		data16;

	
	ReadData((void *)&data16, sizeof(data16));

	if ((SPRITETYPE)data16 == SPRITETYPE_NORMAL) 
	{
		SkipSpriteData();
	} 
	else if ((SPRITETYPE)data16 == SPRITETYPE_FACED) 
	{
			SkipFacedSpriteData();
	} 
	else 
		Assert(FALSE);

}

void SpriteFile::ReadSpriteDataGeneral(FacedSpriteWshadow **sprite)
{
	uint16		data16;

	
	ReadData((void *)&data16, sizeof(data16));

	if ((SPRITETYPE)data16 == SPRITETYPE_FACEDWSHADOW) 
	{
			*sprite = (FacedSpriteWshadow *) new FacedSpriteWshadow;
			(*sprite)->SetType((SPRITETYPE)data16);
			ReadFacedSpriteWshadowData((FacedSpriteWshadow *)*sprite);
	} 
	else 
		Assert(FALSE);

}

void SpriteFile::ReadAnimDataBasic(Anim *a)
{
	
	
	uint16		*u;

	ReadAnimDataFull(a);

	
	a->SetPlaybackTime(a->GetPlaybackTime() / a->GetNumFrames());
	a->SetNumFrames(1);
	u = a->GetFrames();
	u[0] = 0;
}

void SpriteFile::ReadAnimDataFull(Anim *a)
{
	uint16		data16;
	POINT		*p;
	uint16		*u;

	
	ReadData(&data16, sizeof(data16));
	a->SetType(data16);

	
	ReadData(&data16, sizeof(data16));
	a->SetNumFrames(data16);

	
	ReadData(&data16, sizeof(data16));
	a->SetPlaybackTime(data16);

	
	ReadData(&data16, sizeof(data16));
	a->SetDelay(data16);

	
	u = a->GetFrames();
	if (u == NULL)
		u = new uint16[a->GetNumFrames()];
	ReadData((void *)u, sizeof(uint16) * a->GetNumFrames());
	a->SetFrames(u);

	
	p = a->GetDeltas();
	if (p == NULL)
		p = new POINT[a->GetNumFrames()];
	ReadData((void *)p, sizeof(POINT) * a->GetNumFrames());
	a->SetDeltas(p);

	
	u = a->GetTransparencies();
	if (u == NULL)
		u = new uint16[a->GetNumFrames()];
	ReadData((void *)u, sizeof(uint16) * a->GetNumFrames());
	a->SetTransparencies(u);
}

void SpriteFile::SkipAnimData(void)
{
	uint16		data16;
	
	
	uint16		numFrames;

	
	ReadData(&data16, sizeof(data16));
	
	
	ReadData(&data16, sizeof(data16));
	numFrames = data16;

	
	ReadData(&data16, sizeof(data16));
	
	
	ReadData(&data16, sizeof(data16));
	
	
	SetFilePos(GetFilePos() + sizeof(uint16) * numFrames);

	
	SetFilePos(GetFilePos() + sizeof(POINT) * numFrames);

	
	SetFilePos(GetFilePos() + sizeof(uint16) * numFrames);
}


SPRITEFILEERR SpriteFile::Create(SPRITEFILETYPE type,unsigned version,unsigned compression_mode)
{
	SPRITEFILEERR	err;
	uint32		data;
	MBCHAR		path[_MAX_PATH];

#if defined(__MAKESPR__) || defined(__SPRITETEST__)
	strcpy(path, m_filename);
#else
	MBCHAR fullPath[_MAX_PATH];
	g_civPaths->GetSpecificPath(C3DIR_SPRITES, fullPath, FALSE);
	sprintf(path, "%s" FILE_SEP "%s", fullPath, m_filename);
#endif

	if (m_file)
	{
		c3files_fclose(m_file);
	}
	m_file = c3files_fopen(C3DIR_DIRECT, path, "wb");

	Assert(m_file != NULL);
	
	if (m_file == NULL) 
		return SPRITEFILEERR_NOCREATE;
	
	
	data = k_SPRITEFILE_TAG;
	err  = WriteData(data);
	Assert(err == SPRITEFILEERR_OK);

	
	m_version = version;
	data = version;
	err  = WriteData(data);
	Assert(err == SPRITEFILEERR_OK);

	
	m_spr_compression = compression_mode;

	if(m_version>k_SPRITEFILE_VERSION1)
	{
		data = m_spr_compression;
		err  = WriteData(data);
		Assert(err == SPRITEFILEERR_OK);
	}

	
	data = type;
	err = WriteData(data);
	Assert(err == SPRITEFILEERR_OK);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Write(Sprite *s, Anim *anim)
{
	
	WriteSpriteData(s);
	WriteAnimData(anim);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Write(FacedSprite *s, Anim *anim)
{
	
	WriteFacedSpriteData(s);
	WriteAnimData(anim);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Write(FacedSpriteWshadow *s, Anim *anim)
{
	
	WriteFacedSpriteWshadowData(s);
	WriteAnimData(anim);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Write(SpriteGroup *s, Anim *anim)
{
	return SPRITEFILEERR_OK;
}




SPRITEFILEERR 
SpriteFile::Write_v13(UnitSpriteGroup *s)
{
	uint16		i;
	uint32		offset[UNITACTION_MAX];
	POINT		NoData;
	
	unsigned    start_of_offsets=GetFilePos();

	
	for (i=0; i<UNITACTION_MAX; i++) 
		WriteData((uint32)0);

	
	for (i=0; i<UNITACTION_MAX; i++) 
	{
		Sprite	*sprite;

		sprite = s->GetGroupSprite((GAME_ACTION)i);
		
		if (sprite != NULL) 
		{
			WriteData((uint32)TRUE);
			
			offset[i] = GetFilePos();

			
			switch(sprite->GetType())
			{
			case	SPRITETYPE_NORMAL:
					WriteSpriteData(sprite);
					break;
			case	SPRITETYPE_FACED:
					WriteFacedSpriteData((FacedSprite *)sprite);
					break;
			default:
					c3errors_ErrorDialog("SpriteFile", "\"%s\": Bad Sprite Type %d at index %d.",
										 m_filename,sprite->GetType(),i);
			}
			WriteAnimData(s->GetGroupAnim((UNITACTION)i));
		} 
		else 
		{
			WriteData((uint32)FALSE);
			offset[i] = static_cast<uint32>(-1);
			
		}
	}

	
	WriteData((uint16)0); 

	
    WriteData((uint16)0); 

	
	for(i=0;i<k_NUM_FACINGS;i++) 
  		WriteData((uint8 *)&NoData, sizeof(POINT));

	
	for (i=0; i<UNITACTION_MAX; i++) 
		WriteData((uint8 *)s->GetShieldPoints((UNITACTION)i), sizeof(POINT) * k_NUM_FACINGS);

	WriteData((uint16)s->HasDeath());
	WriteData((uint16)s->HasDirectional());

	
	SetFilePos(start_of_offsets);

	
	for (i=0; i<UNITACTION_MAX; i++) 
		WriteData(offset[i]);
	
	return SPRITEFILEERR_OK;
}




SPRITEFILEERR 
SpriteFile::Write_v20(UnitSpriteGroup *s)
{
	uint16		i;
	int  		offset[ACTION_MAX+1];

	
	unsigned    start_of_offsets=GetFilePos();

	
	for (i=0; i<ACTION_MAX+1; i++) 
		offset[i] = -1;	
	
	
	WriteData((uint8*)offset, sizeof(int) * (ACTION_MAX+1));

	
	for (i=0;i<ACTION_MAX; i++) 
	{
		Sprite	*sprite;

		sprite = s->GetGroupSprite((GAME_ACTION)i);
		
		if (sprite!=NULL) 
		{
			offset[i] = GetFilePos();
	 
			
			switch(sprite->GetType())
			{
			case	SPRITETYPE_NORMAL:
					WriteSpriteData(sprite);
					break;
			case	SPRITETYPE_FACED:
					WriteFacedSpriteData((FacedSprite *)sprite);
					break;
			default:
					c3errors_ErrorDialog("SpriteFile", "\"%s\": Bad Sprite Type %d at index %d.",
										 m_filename,sprite->GetType(),i);
			}

			WriteAnimData(s->GetGroupAnim((UNITACTION)i));
		} 
	}
	
	offset[ACTION_MAX] = GetFilePos();

	
	for (i=0; i<UNITACTION_MAX; i++) 
		WriteData((uint8 *)s->GetShieldPoints((UNITACTION)i), sizeof(POINT) * k_NUM_FACINGS);

	WriteData((uint16)s->HasDeath());
	WriteData((uint16)s->HasDirectional());

	
	SetFilePos(start_of_offsets);

	
	WriteData((uint8*)offset, sizeof(int) * (ACTION_MAX+1));

	return SPRITEFILEERR_OK;
}




SPRITEFILEERR SpriteFile::Write(UnitSpriteGroup *s)
{
	switch(m_version)
	{
	case	k_SPRITEFILE_VERSION1:
	case	k_SPRITEFILE_VERSION2:
			return Write_v20(s); 
			break;

	case	k_SPRITEFILE_VERSION0:
	default:
			return Write_v13(s);
	}
	
	return SPRITEFILEERR_OK;
}








































































SPRITEFILEERR SpriteFile::Write(EffectSpriteGroup *s)
{
	Sprite		*sprite;
	Anim		*anim;

	sprite = s->GetGroupSprite((GAME_ACTION)EFFECTACTION_PLAY);
	WriteData((uint32)(sprite != NULL));
	if (sprite != NULL) 
	{
		WriteSpriteData(sprite);
		anim = s->GetGroupAnim((GAME_ACTION)EFFECTACTION_PLAY);
		WriteData((uint32)(anim != NULL));
		if (anim != NULL) 
		{
			WriteAnimData(anim);
		}
	} 

	sprite = s->GetGroupSprite((GAME_ACTION)EFFECTACTION_FLASH);
	WriteData((uint32)(sprite != NULL));
	if (sprite != NULL) 
	{
		WriteSpriteData(sprite);
		anim = s->GetGroupAnim((GAME_ACTION)EFFECTACTION_FLASH);
		WriteData((uint32)(anim != NULL));
		if (anim != NULL) 
		{
			WriteAnimData(anim);
		}
	} 

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Write(GoodSpriteGroup *s)
{
	uint16		i;
	uint32		offset[GOODACTION_MAX];

	
	for (i=0; i<GOODACTION_MAX; i++) 
	{
		WriteData((uint32)0);
	}

	
	for (i=0; i<GOODACTION_MAX; i++) 
	{
		Sprite		*sprite;

		sprite = s->GetGroupSprite((GAME_ACTION)i);
		if (sprite != NULL) 
		{

			WriteData((uint32)TRUE);

			offset[i] = GetFilePos();

			WriteSpriteData(sprite);
			WriteAnimData(s->GetGroupAnim((GAME_ACTION)i));

		} 
		else 
		{
			
			WriteData((uint32)FALSE);
			offset[i] = static_cast<uint32>(-1);
		}

	}

	
	SetFilePos(k_SPRITEFILE_HEADER_SIZE);

	
	for (i=0; i<GOODACTION_MAX; i++) 
	{
		WriteData(offset[i]);
	}
	
	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Write(CitySpriteGroup *s, Anim *anim)
{
	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::CloseWrite()
{
	if (m_file)
    {
        c3files_fclose(m_file);
        m_file = NULL;
    }

	return SPRITEFILEERR_OK;
}



SPRITEFILEERR SpriteFile::Open(SPRITEFILETYPE *type)
{
	SPRITEFILEERR	err;
	uint32			data;

	if (m_file)
	{
		c3files_fclose(m_file);
	}
	m_file = c3files_fopen(C3DIR_SPRITES, m_filename, "rb");

	Assert(m_file != NULL);
	if (m_file == NULL) return SPRITEFILEERR_NOOPEN;

	
	err = ReadData((void *)&data, sizeof(data));

	if (data != k_SPRITEFILE_TAG) 
	{
		c3errors_ErrorDialog("SpriteFile", "BAD FILE.  Looking for valid SPR.");
		return SPRITEFILEERR_BADTAG;
	}

	
	err = ReadData((void *)&data, sizeof(data));


	m_version = data;

	
	switch(m_version)
	{
	   case k_SPRITEFILE_VERSION2: 
			err = ReadData((void *)&data, sizeof(data));
			m_spr_compression = data;
			m_spr_compression = SPRDATA_LZW1;
	  
	   case k_SPRITEFILE_VERSION0:
	   case k_SPRITEFILE_VERSION1:
			break; 
			
	   default:
		c3errors_ErrorDialog("SpriteFile", "\"%s\": Bad Sprite File Version %d.%d.", 
						   	 m_filename,(data >> 16), data & 0x0000FFFF);
		return SPRITEFILEERR_BADVERSION;

	}

	
	err = ReadData((void *)&data, sizeof(data));
	Assert(err == SPRITEFILEERR_OK);
	*type = (SPRITEFILETYPE)data;

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Read(Sprite **s, Anim **anim)
{
	uint32			soffset=0, aoffset=0;
	SPRITEFILEERR	err;

	
	err = ReadData((void *)&soffset, sizeof(soffset));			
	err = ReadData((void *)&aoffset, sizeof(aoffset));			

	
	*s = new Sprite;
	ReadSpriteDataFull(*s);

	*anim = new Anim;
	ReadAnimDataFull(*anim);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Read(FacedSprite **s, Anim **anim)
{
	uint32			soffset=0, aoffset=0;
	SPRITEFILEERR	err;

	
	err = ReadData((void *)&soffset, sizeof(soffset));			
	err = ReadData((void *)&aoffset, sizeof(aoffset));			

	
	*s = new FacedSprite;
	ReadFacedSpriteDataFull(*s);

	*anim = new Anim;
	ReadAnimDataFull(*anim);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Read(FacedSpriteWshadow **s, Anim **anim)
{
	uint32			soffset=0, aoffset=0;
	SPRITEFILEERR	err;

	
	err = ReadData((void *)&soffset, sizeof(soffset));			
	err = ReadData((void *)&aoffset, sizeof(aoffset));			

	
	*s = new FacedSpriteWshadow;
	ReadFacedSpriteWshadowData(*s);

	*anim = new Anim;
	ReadAnimDataFull(*anim);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::Read(SpriteGroup **s, Anim **anim)
{
	return SPRITEFILEERR_OK;
}









SPRITEFILEERR 
SpriteFile::ReadBasic_v13(UnitSpriteGroup *s)
{
	uint16	i;
	uint32	data32;
	uint16	data16;

	
	for (i=0; i<UNITACTION_MAX; i++) 
	{
		ReadData((void *)&data32, sizeof(data32));
	}

	for (i=0; i<UNITACTION_MAX; i++) 
	{
		Sprite		*sprite;

		ReadData((void *)&data32, sizeof(data32));

		if (data32) {
			if (i==UNITACTION_MOVE) {
				sprite = s->GetGroupSprite((GAME_ACTION)i);
				ReadSpriteDataGeneralBasic(&sprite);
				s->SetGroupSprite((GAME_ACTION)i, sprite);

				Anim *anim = s->GetGroupAnim((GAME_ACTION)i);

				if (anim == NULL)
					anim = new Anim;
				
				ReadAnimDataBasic(anim);

				s->SetGroupAnim((GAME_ACTION)i, anim);
			} else
			if (i==UNITACTION_IDLE) {
				if (g_theProfileDB && g_theProfileDB->IsUnitAnim()) {
					
					sprite = s->GetGroupSprite((GAME_ACTION)i);
					ReadSpriteDataGeneralFull(&sprite);
					s->SetGroupSprite((GAME_ACTION)i, sprite);

					Anim *anim = s->GetGroupAnim((GAME_ACTION)i);
					if (anim == NULL)
						anim = new Anim;
					ReadAnimDataFull(anim);
					s->SetGroupAnim((GAME_ACTION)i, anim);
				} else {
					
					sprite = s->GetGroupSprite((GAME_ACTION)i);
					ReadSpriteDataGeneralBasic(&sprite);
					s->SetGroupSprite((GAME_ACTION)i, sprite);

					Anim *anim = s->GetGroupAnim((GAME_ACTION)i);
					if (anim == NULL)
						anim = new Anim;
					ReadAnimDataBasic(anim);

					s->SetGroupAnim((GAME_ACTION)i, anim);
				}
			} else {














				
				SkipSpriteDataGeneral();
				s->SetGroupSprite((GAME_ACTION)i, NULL);

				SkipAnimData();
				s->SetGroupAnim((GAME_ACTION)i, NULL);
			}
		}
	}

	POINT		*thePoints;
	POINT		pointBuffer[k_NUM_FACINGS];

	
	ReadData((void *)&data16, sizeof(uint16));


 	for (i=0; i<k_NUM_FIREPOINTS; i++) 
 	{
 		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
 
 
 	}

	
	ReadData((void *)&data16, sizeof(uint16));
 

	for (i=0; i<k_NUM_FIREPOINTS; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
   
   
	}

	ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
   
   

	for (i=0; i<UNITACTION_MAX; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
		thePoints = s->GetShieldPoints((UNITACTION)i);
		memcpy(thePoints, pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
	}

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDeath(data16);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDirectional(data16);

	return SPRITEFILEERR_OK;
}



SPRITEFILEERR 
SpriteFile::ReadBasic_v20(UnitSpriteGroup *s)
{
	int		i;
	uint16	data16;
	int     offsets[ACTION_MAX+1];

	
	Sprite		*sprite;
	Anim		*anim;

	
    ReadData((void *)offsets, sizeof(int) * (ACTION_MAX+1));

	
	for (i=0; i<ACTION_MAX; i++) 
	{
	  s->SetGroupSprite((GAME_ACTION)i,NULL);
	  s->SetGroupAnim  ((GAME_ACTION)i,NULL);
	}

	
	if(offsets[UNITACTION_IDLE]>0)
	{
		
 		SetFilePos(offsets[UNITACTION_IDLE]);

		sprite = s->GetGroupSprite((GAME_ACTION)UNITACTION_IDLE);
		anim   = s->GetGroupAnim  ((GAME_ACTION)UNITACTION_IDLE);
	
		if(	anim == NULL)
			anim = new Anim;

#ifdef __MAKESPR__
	   ReadSpriteDataGeneralBasic(&sprite);
	   ReadAnimDataBasic(anim);
#else
		if (g_theProfileDB && g_theProfileDB->IsUnitAnim())
		{
			ReadSpriteDataGeneralFull(&sprite);
			ReadAnimDataFull(anim);
		}
		else 
		{
			ReadSpriteDataGeneralBasic(&sprite);
			ReadAnimDataBasic(anim);
		}
#endif
	    s->SetGroupSprite((GAME_ACTION)UNITACTION_IDLE,sprite);
	    s->SetGroupAnim  ((GAME_ACTION)UNITACTION_IDLE,anim);
	}

	
	if(offsets[UNITACTION_MOVE]>0)
	{
		
 		SetFilePos(offsets[UNITACTION_MOVE]);
		
		sprite = s->GetGroupSprite((GAME_ACTION)UNITACTION_MOVE);
		anim   = s->GetGroupAnim  ((GAME_ACTION)UNITACTION_MOVE);
	
		if(	anim == NULL)
			anim = new Anim;

		 ReadSpriteDataGeneralBasic(&sprite);
		 ReadAnimDataBasic(anim);
	
	     s->SetGroupSprite((GAME_ACTION)UNITACTION_MOVE,sprite);
	     s->SetGroupAnim  ((GAME_ACTION)UNITACTION_MOVE,anim);
	}

	
	SetFilePos(offsets[ACTION_MAX]);

	
	for (i=0; i<UNITACTION_MAX; i++) 
		ReadData((void *)s->GetShieldPoints((UNITACTION)i), sizeof(POINT) * k_NUM_FACINGS);

	
	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDeath(data16);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDirectional(data16);

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR 
SpriteFile::ReadBasic(UnitSpriteGroup *s)
{
	switch(m_version)
	{
	case	k_SPRITEFILE_VERSION1:
	case	k_SPRITEFILE_VERSION2:
			return ReadBasic_v20(s); 
			break;

	case	k_SPRITEFILE_VERSION0:
	default:
			return ReadBasic_v13(s);
	}
	
	return SPRITEFILEERR_OK;
}


SPRITEFILEERR SpriteFile::ReadFull_v13(UnitSpriteGroup *s)
{
	uint16	i;
	uint32	data32;
	uint16	data16;

	
	for (i=0; i<UNITACTION_MAX; i++) 
	{
		ReadData((void *)&data32, sizeof(data32));
	}

	for (i=0; i<UNITACTION_MAX; i++) 
	{
		Sprite		*sprite;

		ReadData((void *)&data32, sizeof(data32));
		if (data32) 
		{
			sprite = s->GetGroupSprite((GAME_ACTION)i);

			ReadSpriteDataGeneralFull(&sprite);
			s->SetGroupSprite((GAME_ACTION)i, sprite);

			Anim *anim = s->GetGroupAnim((GAME_ACTION)i);

			if (anim== NULL) {
				anim = new Anim;
			}
			ReadAnimDataFull(anim);
			s->SetGroupAnim((GAME_ACTION)i, anim);
		}
	}

	POINT		*thePoints;
	POINT		pointBuffer[k_NUM_FACINGS];

	
	ReadData((void *)&data16, sizeof(uint16));


	for (i=0; i<k_NUM_FIREPOINTS; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);


	}

	
	ReadData((void *)&data16, sizeof(uint16));
 

	for (i=0; i<k_NUM_FIREPOINTS; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
   
   
	}

	ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);



	for (i=0; i<UNITACTION_MAX; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
		thePoints = s->GetShieldPoints((UNITACTION)i);
		memcpy(thePoints, pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
	}

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDeath(data16);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDirectional(data16);

	return SPRITEFILEERR_OK;
}


SPRITEFILEERR SpriteFile::ReadFull_v20(UnitSpriteGroup *s)
{
	uint16	i;
	uint16	data16;
	int     offsets[ACTION_MAX];

	
    ReadData((void *)offsets, sizeof(int) * (ACTION_MAX+1));

	
	for (i=0; i<ACTION_MAX; i++) 
	{
		Sprite		*sprite;

		if (offsets[i]>0) 
		{
			sprite = s->GetGroupSprite((GAME_ACTION)i);

			ReadSpriteDataGeneralFull(&sprite);
		   
			s->SetGroupSprite((GAME_ACTION)i, sprite);

			Anim *anim = s->GetGroupAnim((GAME_ACTION)i);

			if (anim== NULL) {
				anim = new Anim;
			}
			ReadAnimDataFull(anim);
			s->SetGroupAnim((GAME_ACTION)i, anim);
		}
	}

	
	for (i=0; i<UNITACTION_MAX; i++) 
		ReadData((void *)s->GetShieldPoints((UNITACTION)i), sizeof(POINT) * k_NUM_FACINGS);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDeath(data16);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDirectional(data16);

	return SPRITEFILEERR_OK;
}




SPRITEFILEERR SpriteFile::ReadFull(UnitSpriteGroup *s)
{
	switch(m_version)
	{
	case	k_SPRITEFILE_VERSION1:
	case	k_SPRITEFILE_VERSION2:
			return ReadFull_v20(s); 
			break;

	case	k_SPRITEFILE_VERSION0:
	default:
			return ReadFull_v13(s);
	}
	
	return SPRITEFILEERR_OK;
}




SPRITEFILEERR 
SpriteFile::ReadIndexed_v13(UnitSpriteGroup *s,GAME_ACTION action)
{
	uint16	i;
	uint32  offsets[ACTION_MAX+1];
	uint32	data32;
	uint16	data16;

	

	data32 = GetFilePos();

	SetFilePos(offsets[action]-(action+1)*sizeof(uint32));

	Sprite		*sprite;

	ReadData((void *)&data32, sizeof(data32));
	
	if (data32) 
	{
		sprite = s->GetGroupSprite(action);

		ReadSpriteDataGeneralFull(&sprite);
		s->SetGroupSprite(action, sprite);

		Anim *anim = s->GetGroupAnim(action);

		if (anim== NULL) 
			anim = new Anim;

		ReadAnimDataFull(anim);
		s->SetGroupAnim(action, anim);
	}
	

	POINT		*thePoints;
	POINT		pointBuffer[k_NUM_FACINGS];

	
	ReadData((void *)&data16, sizeof(uint16));


	for (i=0; i<k_NUM_FIREPOINTS; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
  
  
	}

	
	ReadData((void *)&data16, sizeof(uint16));
  

	for (i=0; i<k_NUM_FIREPOINTS; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
   
   
	}

	ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
   
   

	for (i=0; i<UNITACTION_MAX; i++) 
	{
		ReadData((void *)pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
		thePoints = s->GetShieldPoints((UNITACTION)i);
		memcpy(thePoints, pointBuffer, sizeof(POINT) * k_NUM_FACINGS);
	}

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDeath(data16);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDirectional(data16);

	return SPRITEFILEERR_OK;
}




SPRITEFILEERR 
SpriteFile::ReadIndexed_v20(UnitSpriteGroup *s,GAME_ACTION action)
{
	uint16	i;
	uint16	data16;
	int     offsets[ACTION_MAX];

	
    ReadData((void *)offsets, sizeof(int) * (ACTION_MAX+1));

	
    if (offsets[action]>0)
	{	
		Sprite	*sprite;
		Anim	*anim;

		sprite = s->GetGroupSprite(action);
		anim   = s->GetGroupAnim  (action);

		if (anim== NULL)
			anim = new Anim;

		ReadSpriteDataGeneralFull(&sprite);
	    ReadAnimDataFull(anim);
		   
		s->SetGroupSprite(action, sprite);
		s->SetGroupAnim  (action, anim);
	}

	
	SetFilePos(offsets[ACTION_MAX]);

	
	for (i=0; i<UNITACTION_MAX; i++) 
		ReadData((void *)s->GetShieldPoints((UNITACTION)i), sizeof(POINT) * k_NUM_FACINGS);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDeath(data16);

	ReadData((void *)&data16, sizeof(uint16));
	s->SetHasDirectional(data16);

	return SPRITEFILEERR_OK;
}





SPRITEFILEERR SpriteFile::ReadIndexed(UnitSpriteGroup *s,GAME_ACTION action)
{
	switch(m_version)
	{
	case	k_SPRITEFILE_VERSION1:
	case	k_SPRITEFILE_VERSION2:
			return ReadIndexed_v20(s,action); 
			break;

	case	k_SPRITEFILE_VERSION0:
	default:
			return ReadFull_v13(s);
	}
	
	return SPRITEFILEERR_OK;
}




















































SPRITEFILEERR SpriteFile::Read(EffectSpriteGroup *s)
{
	uint32	data32;

	Sprite	*sprite = NULL;
	Anim	*anim;

	ReadData((void *)&data32, sizeof(data32));
	if (data32) {
		ReadSpriteDataGeneralFull(&sprite);
		s->SetGroupSprite((GAME_ACTION)EFFECTACTION_PLAY, sprite);
	
		ReadData((void *)&data32, sizeof(data32));
		if(data32) {
			anim = new Anim;
			ReadAnimDataFull(anim);
			s->SetGroupAnim((GAME_ACTION)EFFECTACTION_PLAY, anim);
		}
	}

	sprite = NULL;

	ReadData((void *)&data32, sizeof(data32));
	if (data32) {
		ReadSpriteDataGeneralFull(&sprite);
		s->SetGroupSprite((GAME_ACTION)EFFECTACTION_FLASH, sprite);
	
		ReadData((void *)&data32, sizeof(data32));
		if(data32) {
			anim = new Anim;
			ReadAnimDataFull(anim);
			s->SetGroupAnim((GAME_ACTION)EFFECTACTION_FLASH, anim);
		}
	}

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::ReadBasic(GoodSpriteGroup *s)
{
	uint16	i;
	uint32	data32;

	
	for (i=0; i<GOODACTION_MAX; i++) 
	{
		ReadData((void *)&data32, sizeof(data32));
	}

	for (i=0; i<GOODACTION_MAX; i++) 
	{
		Sprite *sprite;

		ReadData((void *)&data32, sizeof(data32));
		if (data32) 
		{
			sprite = s->GetGroupSprite((GAME_ACTION)i);
			ReadSpriteDataGeneralBasic(&sprite);
			s->SetGroupSprite((GAME_ACTION)i, sprite);

			Anim *anim = s->GetGroupAnim((GAME_ACTION)i);
			if (anim == NULL)
				anim = new Anim;

			ReadAnimDataBasic(anim);
			s->SetGroupAnim((GAME_ACTION)i, anim);
		}
	}

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::ReadFull(GoodSpriteGroup *s)
{
	uint16	i;
	uint32	data32;

	
	for (i=0; i<GOODACTION_MAX; i++) 
	{
		ReadData((void *)&data32, sizeof(data32));
	}

	for (i=0; i<GOODACTION_MAX; i++) 
	{
		Sprite *sprite;

		ReadData((void *)&data32, sizeof(data32));
		if (data32) 
		{
			sprite = s->GetGroupSprite((GAME_ACTION)i);
			ReadSpriteDataGeneralFull(&sprite);
			s->SetGroupSprite((GAME_ACTION)i, sprite);

			Anim *anim = s->GetGroupAnim((GAME_ACTION)i);
			if (anim == NULL)
				anim = new Anim;

			ReadAnimDataFull(anim);
			s->SetGroupAnim((GAME_ACTION)i, anim);
		}
	}

	return SPRITEFILEERR_OK;
}



SPRITEFILEERR SpriteFile::ReadIndexed(GoodSpriteGroup *s,GAME_ACTION index)
{
	uint16	i;
	uint32	data32;
	uint32 	offsets[GOODACTION_MAX];

	
	for (i=0; i<GOODACTION_MAX; i++) 
		ReadData((void *)&(offsets[i]), sizeof(uint32));

	SetFilePos(GetFilePos()+offsets[index]);

	Sprite *sprite;

	ReadData((void *)&data32, sizeof(data32));
	
	if (data32) 
	{
		sprite = s->GetGroupSprite(index);
		ReadSpriteDataGeneralFull(&sprite);
		s->SetGroupSprite(index, sprite);

		Anim *anim = s->GetGroupAnim(index);

		if (anim == NULL)
			anim = new Anim;

		ReadAnimDataFull(anim);
		s->SetGroupAnim(index, anim);
	}
	

	return SPRITEFILEERR_OK;
}




SPRITEFILEERR SpriteFile::Read(CitySpriteGroup **s, Anim **anim)
{
	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::CloseRead()
{
	if (m_file)
    {
        c3files_fclose(m_file);
        m_file = NULL;
    }

	return SPRITEFILEERR_OK;
}


SPRITEFILEERR SpriteFile::WriteData(uint8 *data, size_t bytes)
{
	size_t		countWritten;

	countWritten = c3files_fwrite(data, 1, bytes, m_file);
	Assert(countWritten == bytes);

	if (countWritten != bytes) return SPRITEFILEERR_WRITEERR;

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::WriteData(sint16 data)
{
	size_t		countWritten;

	countWritten = c3files_fwrite((void *)&data, 1, 2, m_file);
	Assert(countWritten == 2);
	if (countWritten != 2) return SPRITEFILEERR_WRITEERR;

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::WriteData(uint16 data)
{
	size_t		countWritten;

	countWritten = c3files_fwrite((void *)&data, 1, 2, m_file);
	Assert(countWritten == 2);
	if (countWritten != 2) return SPRITEFILEERR_WRITEERR;

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::WriteData(sint32 data)
{
	size_t		countWritten;

	countWritten = c3files_fwrite((void *)&data, 1, 4, m_file);
	Assert(countWritten == 4);
	if (countWritten != 4) return SPRITEFILEERR_WRITEERR;

	return SPRITEFILEERR_OK;
}

SPRITEFILEERR SpriteFile::WriteData(uint32 data)
{
	size_t		countWritten;

	countWritten = c3files_fwrite((void *)&data, 1, 4, m_file);
	Assert(countWritten == 4);
	if (countWritten != 4) return SPRITEFILEERR_WRITEERR;

	return SPRITEFILEERR_OK;
}


SPRITEFILEERR SpriteFile::ReadData(void *data, size_t bytes)
{
	size_t		countRead;

	countRead = c3files_fread(data, 1, bytes, m_file);
	Assert(countRead == bytes);

	if (countRead != bytes) return SPRITEFILEERR_READERR;

	return SPRITEFILEERR_OK;
}

size_t SpriteFile::GetFilePos(void)
{	
	fpos_t		pos;
	sint32		err;

	err = c3files_fgetpos(m_file, &pos);
	Assert(err == 0);

#ifdef WIN32
	return  pos;
#elif defined(LINUX)
	return pos.__pos;
#endif
}

void SpriteFile::SetFilePos(size_t pos)
{
	fpos_t		filePos;
	sint32		err;
	
#ifdef WIN32
	filePos = pos;
#elif defined(LINUX)
	err = c3files_fgetpos(m_file, &filePos);
	Assert(err == 0);
	filePos.__pos = pos;
#endif

	err = c3files_fsetpos(m_file, &filePos);
	Assert(err == 0);
}




uint8 *
SpriteFile::CompressData  (void *Data, size_t &DataLen)
{
	uint8 *ReturnVal=NULL;

	switch(m_spr_compression)
	{
	
	case	SPRDATA_REGULAR:
			ReturnVal = CompressData_Default(Data,DataLen);
			break;
	
	case	SPRDATA_LZW1:
			ReturnVal = CompressData_LZW1(Data,DataLen);
			break;

	default:
		
		DataLen = 0;
		c3errors_ErrorDialog("SpriteFile: ", "Bad Compression Mode %d",m_spr_compression);
	};

	return ReturnVal;
}


uint8 *
SpriteFile::DeCompressData(void *Data, size_t CompressedLen, size_t ActualLen)
{
	uint8 *ReturnVal = (uint8 *)Data;

	switch(m_spr_compression)
	{
	
	case	SPRDATA_REGULAR:
			ReturnVal = DeCompressData_Default(Data,CompressedLen,ActualLen);
			break;

	
	case	SPRDATA_LZW1:
			ReturnVal = DeCompressData_LZW1(Data,CompressedLen,ActualLen);
			break;


	default:
		 
		c3errors_ErrorDialog("SpriteFile:", "Bad Compression Mode %d",m_spr_compression);
	};

	return ReturnVal;
}




uint8 *
SpriteFile::CompressData_Default  (void *Data, size_t &DataLen)
{
	uint8 *ReturnVal = new uint8[DataLen];
			
	memcpy((void *)ReturnVal,Data,DataLen);

	return ReturnVal;
}


uint8 *
SpriteFile::DeCompressData_Default(void *Data, size_t CompressedLen, size_t ActualLen)
{
  uint8 *ReturnVal = new uint8[ActualLen];
			
  memcpy((void *)ReturnVal,Data,ActualLen);

  return ReturnVal;
}














uint8 *
SpriteFile::CompressData_LZW1(void *Data, size_t &DataLen)
{
 uint8  *p_src_first=(uint8 *)Data;
 uint8  *p_dst_first=(uint8 *)g_compression_buff;
 
 size_t  src_len=DataLen;
 uint32     p_dst_len=COM_BUFF_SIZE;

 uint8 *p_src=p_src_first,*p_dst=p_dst_first;
 uint8 *p_src_post=p_src_first+src_len,*p_dst_post=p_dst_first+src_len;
 uint8 *p_src_max1=p_src_post-LZW1_ITEMMAX,*p_src_max16=p_src_post-16*LZW1_ITEMMAX;
 uint8 *hash[4096],*p_control; 
 uint16 control=0,control_bits=0;
 
 *p_dst=LZW1_FLAG_COMPRESS; 
 p_dst+=LZW1_FLAG_BYTES; 
 p_control=p_dst; 
 p_dst+=2;
 
 while (true)
 {
	uint8 *p,*s; 
	uint16 unroll=16,len;
	uint32 index; 
	uint32 offset;
    
	if (p_dst>p_dst_post) 
		goto overrun;

    if (p_src>p_src_max16)
    {
		unroll=1;
		if (p_src>p_src_max1)
        {
			if (p_src==p_src_post) 
				break; 
			goto literal;
		}
	}

begin_unrolled_loop:

    index=((40543*((((p_src[0]<<4)^p_src[1])<<4)^p_src[2]))>>4) & 0xFFF;
    
	p=hash[index]; 
	hash[index]=s=p_src; 
	offset=s-p;
    
	if ((offset>4095) || (p<p_src_first) || (offset==0) || LZW1_PS || LZW1_PS || LZW1_PS)
    {
literal: 
	 	*p_dst++=*p_src++; 
	 	control>>=1; 
	 	control_bits++;
	}
    else
    {
	   LZW1_PS || LZW1_PS || LZW1_PS || LZW1_PS || LZW1_PS || LZW1_PS || LZW1_PS ||
       LZW1_PS || LZW1_PS || LZW1_PS || LZW1_PS || LZW1_PS || LZW1_PS || s++; 
	   len=s-p_src-1;
       *p_dst++=(unsigned char)(((offset&0xF00)>>4)+(len-1)); 
	   *p_dst++=(unsigned char)(offset&0xFF);
       p_src+=len; 
	   control=(control>>1)|0x8000; control_bits++;
	}



	if (--unroll) 
	   goto begin_unrolled_loop;
    
	if (control_bits==16)
	{
		*p_control=control&0xFF; 
		*(p_control+1)=control>>8;
		p_control=p_dst; 
		p_dst+=2; 
		control=control_bits=0;
	}
 }

 control>>=16-control_bits;
 *p_control++=control&0xFF; 
 *p_control++=control>>8;
 
 if (p_control==p_dst) 
	 p_dst-=2;
 
 p_dst_len=p_dst-p_dst_first;

 goto end_of_compression;

  
overrun: memcpy(p_dst_first+LZW1_FLAG_BYTES,p_src_first,src_len);
          
   *p_dst_first=LZW1_FLAG_COPY; 
	p_dst_len=src_len+LZW1_FLAG_BYTES;

end_of_compression:

 uint8 *retval=new uint8[p_dst_len];
 DataLen = p_dst_len;

 memcpy(retval,g_compression_buff,DataLen);

 return (uint8 *)retval;
}



uint8 *
SpriteFile::DeCompressData_LZW1(void *Data, size_t CompressedLen, size_t ActualLen)
{
 size_t  src_len=CompressedLen;
 size_t  dst_len=ActualLen;

 uint8  *ReturnVal  = new uint8[ActualLen];
 uint8  *p_src_first=(uint8 *)Data;
 uint8  *p_dst_first=ReturnVal;
	 
 uint32 loops=0,subloops=0;
	
 uint16 controlbits=0, control=0;
 uint8 *p_src=p_src_first+LZW1_FLAG_BYTES, *p_dst=p_dst_first,
       *p_src_post=p_src_first+src_len;
 
 if (*p_src_first==LZW1_FLAG_COPY)
 {
	memcpy(p_dst_first,p_src_first+LZW1_FLAG_BYTES,src_len-LZW1_FLAG_BYTES);
    dst_len=src_len-LZW1_FLAG_BYTES; 
	return ReturnVal;
 }

 while (p_src<p_src_post)
 {
	subloops++;

	if(subloops>25)
	{
		loops ++;
		subloops=0;
	}
	if (controlbits==0)
	{
		control=*p_src++; 
		control|=(*p_src++)<<8; 
		controlbits=16;
	}
    if (control&1)
    {
		uint16 offset,len; 
		uint8 *p;
		
		offset=(*p_src&0xF0)<<4; 
		len=1+(*p_src++&0xF);
		offset+=*p_src++&0xFF; 
		p=p_dst-offset;
		
		while (len--) 
			*p_dst++=*p++;
	}
    else
       *p_dst++=*p_src++;
    
	control>>=1; 
	controlbits--;
 }
 
 

 return ReturnVal;
}
