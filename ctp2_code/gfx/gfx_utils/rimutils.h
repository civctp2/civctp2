#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __RIMUTILS_H__
#define __RIMUTILS_H__

#define RFLAG_BIT_ENCODING 0x0001

#ifdef __linux__
#pragma pack(push, 1)
#endif
struct RIMHeader {
    char  tag[4];
    int32_t   version;
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    uint16_t flags;
};
#ifdef __linux__
#pragma pack(pop)
#endif

extern int write_rim(char *fname, unsigned char *data, int width,
                     int height, int pitch, int is_565);

#endif
