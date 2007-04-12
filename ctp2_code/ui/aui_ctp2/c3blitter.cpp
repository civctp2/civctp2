//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 blitter
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
// _TRY_ALL_BLITTERS
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_sdlsurface.h"
#include "aui_directsurface.h"
#include "aui_rectangle.h"

#include "c3blitter.h"


extern sint32	g_is565Format;
extern BOOL		g_useDDBlit;



unsigned which_blit=0;


C3Blitter::C3Blitter() : aui_NativeBlitter()
{
   
   
   
   if(CheckMMXTechnology())	
       _Blt16To16Fast = &Blt16To16FastMMX;
   else{
       printf("%s L%d: Using slow blitting!\n", __FILE__, __LINE__);	
       _Blt16To16Fast = &Blt16To16Fast;
       }
};


AUI_ERRCODE C3Blitter::Blt16To16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
    {
	// Blitting in assembler code is not portable //Who says so??? Sure;)))
/*#if (!defined(WIN32)) //defined(__AUI_USE_SDL__) || (!defined(WIN32))
	if (0)
        #else*/
	if ((flags & k_AUI_BLITTER_FLAG_FAST)) 
//#endif
	{
	//printf("%s L%d: Using Blt16To16FastMMX!\n", __FILE__, __LINE__);	
#ifdef _TRY_ALL_BLITTERS	
	  
        switch(which_blit)
            {
            case 0:_Blt16To16Fast=Blt16To16Fast   ; break;
  	    case 1:_Blt16To16Fast=Blt16To16FastFPU;	break;
  	    case 2:_Blt16To16Fast=Blt16To16FastMMX;	break;
            }
        
        which_blit ++;
        
        if(which_blit>2)
            which_blit = 0;
		
	   
#endif
        return (this->*_Blt16To16Fast)(destSurf, destRect, srcSurf, srcRect, flags);
	} 
        else {	
            if (g_useDDBlit)
                return aui_NativeBlitter::Blt16To16(destSurf, destRect, srcSurf, srcRect, flags);
            else{
                printf("%s L%d: Using aui_Blitter::Blt16To16!\n", __FILE__, __LINE__);
                return aui_Blitter::Blt16To16(destSurf, destRect, srcSurf, srcRect, flags);
                }
            }
    }

AUI_ERRCODE C3Blitter::ColorBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	uint32 color,
	uint32 flags )
{
	if (g_useDDBlit) 
		return aui_NativeBlitter::ColorBlt16(destSurf, destRect, color, flags);
	else {
            printf("%s L%d: Using aui_Blitter::ColorBlt16!\n", __FILE__, __LINE__);	
            return aui_Blitter::ColorBlt16(destSurf, destRect, color, flags);
            }
}

AUI_ERRCODE C3Blitter::ColorStencilBlt16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *stencilSurf,
	RECT *stencilRect,
	uint32 color,
	uint32 flags)
{
	return aui_Blitter::ColorStencilBlt16(destSurf, destRect, stencilSurf, stencilRect, color, flags);
}

AUI_ERRCODE C3Blitter::StretchBlt16To16(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	if (g_useDDBlit) 
		return aui_NativeBlitter::StretchBlt16To16(destSurf, destRect, srcSurf, srcRect, flags);
	else
		return aui_Blitter::StretchBlt16To16(destSurf, destRect, srcSurf, srcRect, flags);
}


AUI_ERRCODE C3Blitter::Blt16To16Fast(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	
	const sint32 destPitch = destSurf->Pitch() / 2;
	const sint32 srcPitch = srcSurf->Pitch() / 2;
			
	
	BOOL wasDestLocked;
	uint16 *destBuf = (uint16 *)destSurf->Buffer();
	if ((wasDestLocked = destBuf != NULL))
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	
	if ( destBuf )
	{
		
		uint16 *origDestBuf = destBuf;

		
		BOOL wasSrcLocked;
		uint16 *srcBuf = (uint16 *)srcSurf->Buffer();
		if ((wasSrcLocked = srcBuf != NULL))
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		
		if ( srcBuf )
		{
			
			uint16 *origSrcBuf = srcBuf;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				const sint32 scanWidth = 2 * ( srcRect->right - srcRect->left );

				
				const uint16 *stop = srcBuf + srcPitch * ( srcRect->bottom - srcRect->top );

				destBuf -= destPitch;

				
				do
				{
					destBuf += destPitch;
#ifndef _MSC_VER
					assert(0);
#else
					__asm {
							mov		ecx, scanWidth
				  			mov		esi, srcBuf
				  			mov     edx, ecx
				  			shr     ecx, 3
				  			mov		edi, destBuf
							test	ecx, ecx
							jz		L3
L0:						
							mov		eax, [esi]
							mov		ebx, [esi+4]
							mov		[edi], eax
							mov		[edi+4], ebx
							add		esi, 8
							add		edi, 8
							dec		ecx
							jnz		L0
L3:
							test	edx, 4
							jz		L1

							mov		eax, [esi]
							mov		[edi], eax
							add		esi, 4
							add		edi, 4
L1:
							test	edx, 2
							jz		L2
							mov		ax, [esi]
							mov		[edi], ax
L2:
					}
#endif // _MSC_VER
				} while ( (srcBuf += srcPitch) != stop );
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{
				const sint32 scanWidth = srcRect->right - srcRect->left;

				
				const sint32 destDiff = destPitch - scanWidth;
				const sint32 srcDiff = srcPitch - scanWidth;

				
				uint16 *stopHorizontal = srcBuf + scanWidth;
				const uint16 *stopVertical = srcBuf +
					srcPitch * ( srcRect->bottom - srcRect->top );
				destBuf--;

				
				const uint16 chromakey = (uint16)srcSurf->GetChromaKey();

				do
				{
					do
					{
						
						if ( *srcBuf != chromakey )
							*++destBuf = *srcBuf;
						else
							destBuf++;
					} while ( ++srcBuf != stopHorizontal );

					stopHorizontal += srcPitch;

					destBuf += destDiff;
				} while ( (srcBuf += srcDiff) != stopVertical );
			}
			else
			{
				
				
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}
			
			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );
				
				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );
			
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}
	return retcode;
}






AUI_ERRCODE C3Blitter::Blt16To16FastMMX(
    aui_Surface *destSurf,
    RECT *destRect,
    aui_Surface *srcSurf,
    RECT *srcRect,
    uint32 flags ) {

    AUI_ERRCODE retcode = AUI_ERRCODE_OK;
    AUI_ERRCODE errcode;

    //printf("%s L%d: Using Blt16To16FastMMX!\n", __FILE__, __LINE__);
	
    const sint32 destPitch = destSurf->Pitch() / 2;
    const sint32 srcPitch = srcSurf->Pitch() / 2;
			
	
    BOOL wasDestLocked;
    uint16 *destBuf = (uint16 *)destSurf->Buffer();
    if ((wasDestLocked = destBuf != NULL)) {
        destBuf += destRect->top * destPitch + destRect->left;
	}
    else if ( destSurf->Lock(destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK ) {
        destBuf = NULL;
        retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	
    if ( destBuf ) {
		
        uint16 *origDestBuf = destBuf;
		
        BOOL wasSrcLocked;
        uint16 *srcBuf = (uint16 *)srcSurf->Buffer();
        if ((wasSrcLocked = srcBuf != NULL)) {
            srcBuf += srcRect->top * srcPitch + srcRect->left;
            }
        else if ( srcSurf->Lock(srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK ) {
            srcBuf = NULL;
            retcode = AUI_ERRCODE_SURFACELOCKFAILED;
            }

		
        if ( srcBuf ) {
            uint16 *origSrcBuf = srcBuf;

            if ( flags & k_AUI_BLITTER_FLAG_COPY ) {
                const sint32 scanWidth = 2 * ( srcRect->right - srcRect->left );
                const uint16 *stop = srcBuf + srcPitch * ( srcRect->bottom - srcRect->top );
                destBuf -= destPitch;
                do {
                    destBuf += destPitch;
#ifndef _MSC_VER
//					assert(0);
                    //printf("%s L%d: Using Blt16To16FastMMX!\n", __FILE__, __LINE__);
                    __asm__ (
                        //"movl $scanWidth, %eax       \n\t"
                        //"movl $srcBuf, %esi          \n\t"
                        //"movl $destBuf, %edi         \n\t"
                        "movl %%eax,%%ecx              \n\t"
                        "movl $8,%%edx                \n\t"
                        "shrl $3,%%eax                \n\t"
                        "andl $0x7,%%ecx              \n\t"
                        "addl $0,%%eax                \n\t"
                        "jz  MMXCopyDone             \n\t"

                        "_CopyMMX:                   \n\t"
                        "movq (%%esi),%%mm0            \n\t"
                        "movq %%mm0,(%%edi)            \n\t"
                        "addl %%edx,%%esi              \n\t"
                        "addl %%edx,%%edi              \n\t"
                        "decl %%eax                   \n\t"
                        "jnz  _CopyMMX               \n\t"

                        "emms                        \n\t"

                        "MMXCopyDone:                \n\t"


                        "shrl %%ecx                   \n\t"
                        "rep movsw                        \n\t"

                        : 
                        : "a" (scanWidth), "S" (srcBuf), "D" (destBuf)
                        : "%edx", "%ecx", "cc"
                        );
#else
                    _asm
                            {
                            mov eax, scanWidth
                                mov esi, srcBuf
                                mov ecx, eax 
                                mov edi, destBuf
                                mov edx, 8
                                shr eax, 3	 
                                and ecx, 0x7 
                                add eax, 0
                                jz  MMXCopyDone
						
				_CopyMMX:
                            movq mm0,[esi]
                                movq [edi],mm0
                                add  esi, edx
                                add  edi, edx
                                dec  eax
                                jnz  _CopyMMX
	
                                emms
	
				MMXCopyDone:
	
						
                            shr ecx,1
                                rep movsw
                                }	
#endif // _MSC_VER
                    } while ( (srcBuf += srcPitch) != stop );
                }
            else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY ) {
                const sint32 scanWidth = srcRect->right - srcRect->left;

				
                const sint32 destDiff = destPitch - scanWidth;
                const sint32 srcDiff = srcPitch - scanWidth;

				
                uint16 *stopHorizontal = srcBuf + scanWidth;
                const uint16 *stopVertical = srcBuf +
                    srcPitch * ( srcRect->bottom - srcRect->top );
                destBuf--;

				
                const uint16 chromakey = (uint16)srcSurf->GetChromaKey();

                do
                    {
                    do
                        {
						
                        if ( *srcBuf != chromakey )
                            *++destBuf = *srcBuf;
                        else
                            destBuf++;
                        } while ( ++srcBuf != stopHorizontal );

                    stopHorizontal += srcPitch;

                    destBuf += destDiff;
                    } while ( (srcBuf += srcDiff) != stopVertical );
                }
            else {
                retcode = AUI_ERRCODE_INVALIDPARAM;
                }
			
            if ( !wasSrcLocked )
                {
                errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );
				
                if ( !AUI_SUCCESS(errcode) )
                    retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
                }
            }

        if ( !wasDestLocked )
            {
            errcode = destSurf->Unlock( (LPVOID)origDestBuf );
			
            if ( !AUI_SUCCESS(errcode) )
                retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
            }
	}

    return retcode;
    }




AUI_ERRCODE C3Blitter::Blt16To16FastFPU(
	aui_Surface *destSurf,
	RECT *destRect,
	aui_Surface *srcSurf,
	RECT *srcRect,
	uint32 flags )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;

	
	const sint32 destPitch = destSurf->Pitch() / 2;
	const sint32 srcPitch = srcSurf->Pitch() / 2;
			
	
	BOOL wasDestLocked;
	uint16 *destBuf = (uint16 *)destSurf->Buffer();
	if ((wasDestLocked = destBuf != NULL))
	{
		destBuf += destRect->top * destPitch + destRect->left;
	}
	else if ( destSurf->Lock(
		destRect, (LPVOID *)&destBuf, 0 ) != AUI_ERRCODE_OK )
	{
		destBuf = NULL;
		
		retcode = AUI_ERRCODE_SURFACELOCKFAILED;
	}

	
	if ( destBuf )
	{
		
		uint16 *origDestBuf = destBuf;

		
		BOOL wasSrcLocked;
		uint16 *srcBuf = (uint16 *)srcSurf->Buffer();
		if ((wasSrcLocked = srcBuf != NULL))
		{
			srcBuf += srcRect->top * srcPitch + srcRect->left;
		}
		else if ( srcSurf->Lock(
			srcRect, (LPVOID *)&srcBuf, 0 ) != AUI_ERRCODE_OK )
		{
			srcBuf = NULL;
			
			retcode = AUI_ERRCODE_SURFACELOCKFAILED;
		}

		
		if ( srcBuf )
		{
			
			uint16 *origSrcBuf = srcBuf;

			if ( flags & k_AUI_BLITTER_FLAG_COPY )
			{
				const sint32 scanWidth = 2 * ( srcRect->right - srcRect->left );

				
				const uint16 *stop = srcBuf + srcPitch * ( srcRect->bottom - srcRect->top );

				destBuf -= destPitch;

				
				do
				{
					


					
					destBuf += destPitch;

#ifndef _MSC_VER
					assert(0);
#else
					_asm
			  		{
						mov eax, scanWidth
						mov esi, srcBuf
						mov ecx, eax 
						mov edi, destBuf
						mov edx, 8
						shr eax, 3	 
						and ecx, 0x7 
						add eax, 0
						jz  FPUCopyDone
						
					_CopyFPU:
						fild  qword ptr [esi]
						fistp qword ptr [edi]
						add esi, edx
						add edi, edx
						dec eax
						jnz _CopyFPU
					
					FPUCopyDone:
						
					   	shr ecx,1
					   	rep movsw
					}
#endif // _MSC_VER
				} while ( (srcBuf += srcPitch) != stop );
			}
			else if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY )
			{
				const sint32 scanWidth = srcRect->right - srcRect->left;

				
				const sint32 destDiff = destPitch - scanWidth;
				const sint32 srcDiff = srcPitch - scanWidth;

				
				uint16 *stopHorizontal = srcBuf + scanWidth;
				const uint16 *stopVertical = srcBuf +
					srcPitch * ( srcRect->bottom - srcRect->top );
				destBuf--;

				
				const uint16 chromakey = (uint16)srcSurf->GetChromaKey();

				do
				{
					do
					{
						
						if ( *srcBuf != chromakey )
							*++destBuf = *srcBuf;
						else
							destBuf++;
					} while ( ++srcBuf != stopHorizontal );

					stopHorizontal += srcPitch;

					destBuf += destDiff;
				} while ( (srcBuf += srcDiff) != stopVertical );
			}
			else
			{
				
				
				retcode = AUI_ERRCODE_INVALIDPARAM;
			}
			
			if ( !wasSrcLocked )
			{
				errcode = srcSurf->Unlock( (LPVOID)origSrcBuf );
				
				if ( !AUI_SUCCESS(errcode) )
					retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
			}
		}

		if ( !wasDestLocked )
		{
			errcode = destSurf->Unlock( (LPVOID)origDestBuf );
			
			if ( !AUI_SUCCESS(errcode) )
				retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
		}
	}

	return retcode;
}




bool C3Blitter::CheckMMXTechnology(void)
{
#ifdef _MSC_VER
    bool retval = true;
    DWORD RegEDX = 0;

    __try {
            _asm {
                    mov eax, 1      
                                    
					_emit 0fh		
					_emit 0a2h
                    mov RegEDX, edx 
            }
    } __except(EXCEPTION_EXECUTE_HANDLER) { retval = false; }

    if(retval == false)
       return false;           

    if (RegEDX & 0x800000)          
    {
       __try { _asm emms }          
       __except(EXCEPTION_EXECUTE_HANDLER) { retval = false; }
    }
#else
    bool retval = true;
    DWORD RegEDX = 0;

    try {
        __asm__ ( // what's this good for??? Setting an opcode?
            "movl $1,%%eax                \n\t"
            ".byte 0x0f		         \n\t"
            ".byte 0xa2                  \n\t"
            : "=d" (RegEDX)
            :
            : "%eax", "memory"
            );
        } 
    catch(...) {
        printf("%s L%d: MMX-Test FAILED!\n", __FILE__, __LINE__);	
        retval = false; 
        }

    if(retval == false)
       return false;           

    if (RegEDX & 0x800000)          
    {
    try { __asm__ (" emms            \n\t"); }          
    catch(...) {
        printf("%s L%d: MMX-Test FAILED!\n", __FILE__, __LINE__);	
        retval = false; 
        }
    }
    
    printf("%s L%d: MMX-Test succeded!\n", __FILE__, __LINE__);	
#endif // _MSC_VER
    
    
    return retval;
}


void BlockCopy(uint8 *src, uint8 *dest, uint32 len)
{
#ifdef _MSC_VER
	__asm {
		mov		esi, src
		mov		edi, dest
		mov		ecx, len

		mov		eax, ecx
		sub		ecx, edi
		sub		ecx, eax
		and		ecx, 3
		sub		eax, ecx
		jle		short End
	rep movsb
		mov		ecx, eax
		and		eax, 3
		shr		ecx, 2
	rep movsd
End:	add		ecx, eax
	rep movsb
	}
#else
	assert(0);
#endif // _MSC_VER
}

