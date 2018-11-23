/**
 * $Id$
 */
#ifndef __C3CPU_H__
#define __C3CPU_H__

#if defined(WIN32)
#define CPUINFODLL "dll\\util\\cpuinf32.dll"

struct FREQ_INFO
{
	unsigned long in_cycles;

	unsigned long ex_ticks;

	unsigned long raw_freq;
	unsigned long norm_freq;

};

struct TIME_STAMP
{
	DWORD High;

	DWORD Low;
};

#define CLONE_MASK		0x8000

#endif

void		c3cpu_Initialize(void);
void		c3cpu_Examine(void);
MBCHAR		*c3cpu_GetString(void);

#endif
