#pragma once

























#ifndef _TIFFCONF_
#define	_TIFFCONF_



























#ifndef HAVE_IEEEFP
#define	HAVE_IEEEFP	1
#endif
#ifndef HOST_FILLORDER
#define	HOST_FILLORDER	FILLORDER_MSB2LSB
#endif
#ifndef	HOST_BIGENDIAN
#define	HOST_BIGENDIAN	1
#endif

#ifndef FEATURE_SUPPORT








#define	COLORIMETRY_SUPPORT
#define	YCBCR_SUPPORT
#define	CMYK_SUPPORT
#define	ICC_SUPPORT
#endif 

#ifndef COMPRESSION_SUPPORT





















#define	PACKBITS_SUPPORT
#define	LZW_SUPPORT
#define	THUNDER_SUPPORT
#define	NEXT_SUPPORT




#endif 





#ifdef JPEG_SUPPORT
#ifndef YCBCR_SUPPORT
#define	YCBCR_SUPPORT
#endif
#ifndef COLORIMETRY_SUPPORT
#define	COLORIMETRY_SUPPORT
#endif
#endif 









#ifndef STRIPCHOP_DEFAULT
#define	STRIPCHOP_DEFAULT	TIFF_STRIPCHOP	
#endif
#ifndef SUBIFD_SUPPORT
#define	SUBIFD_SUPPORT		1	
#endif
#endif 
