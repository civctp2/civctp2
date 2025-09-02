// Windows specific tiffconf.h, not created by tool
// Here we simply defeine what is missing

#ifndef _TIFCONF_
#define _TIFCONF_

#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>

#if (SIZE_MAX == 0xFFFFFFFF)
// 32 bit definition
typedef int32_t TIFF_SSIZE_T;
#define SIZEOF_SIZE_T 4
#define TIFF_SSIZE_FORMAT PRId32
#elif (SIZE_MAX == 0xFFFFFFFFFFFFFFFF)
// 64 bit definition
typedef int64_t TIFF_SSIZE_T;
#define SIZEOF_SIZE_T 8
#define TIFF_SSIZE_FORMAT PRId64
#else
#error No size defined for this bit size
#endif

#endif // _TIFCONF_
