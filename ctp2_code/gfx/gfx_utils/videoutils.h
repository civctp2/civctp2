











#pragma once
#ifndef __VIDEOUTILS_H__
#define __VIDEOUTILS_H__

void videoutils_Initialize(void);
sint32 videoutils_PlayVideoInWindow(MBCHAR *name, MBCHAR *pattern);
void videoutils_Cleanup(void);

#endif