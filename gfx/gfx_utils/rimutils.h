#pragma once

#ifndef __RIMUTILS_H__
#define __RIMUTILS_H__

#define RFLAG_BIT_ENCODING 0x0001 

struct RIMHeader {
    char  tag[4];                       
    int   version;                      
    short width;                        
    short height;                       
    short pitch;                        
    short flags;
};

extern int write_rim(char *fname, unsigned char *data, int width,
                     int height, int pitch, int is_565);

#endif

