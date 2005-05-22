/* 
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*------------------------------------------------------------------------
 Functions for timing function calls; uses the pentium clock to determine
  time. Records the average and maximum time for the function call and the
  number of times the function was called. As of now, only implemented on
  Win32 machines; defined as dummy macro on other platforms. Note that
  PTIMER_ENABLE must be defined below for timing stuff to work; it's disabled
  by default.

 To use this module, call ptimer_disableInterrupt() near the beginning
  of program to maximize process priority and ptimer_zeroAll() to initialize
  ptimer arrays. Pick an unique index from 0 to NPMAX-1 for your function
  call. Then call ptimer_Enter directly before the function call of interest
  and ptimer_Exit directly afterwards. The index argument for both calls
  is the index you picked and the subindex argument for ptimer_Exit allows
  you to differentiate within the same index. After you've finished timing
  everything, call ptimer_dumpToFile to write the time stats to file, by
  default "ptimer.log" and ptimer_enableInterrupt() to set normal priority
  for process.

 For sake of speed, no checking on indexes, etc is done in ptimer_Enter,
  ptimer_Exit. It is up to you to keep within array limits NPMAX, NPSUBMAX,
  to use unique indexes, and make calls in correct order (no recursion, etc).
  Note: on a P90, a ptimer_Enter, ptimer_Exit sequence takes about 5 ms.
------------------------------------------------------------------------*/
#ifndef ptimer_h
#define ptimer_h

#if 0 && defined(_WIN32) && !(defined(DEBUG) || defined(_DEBUG))
#define PTIMER_ENABLE  /* code for time measurements using Pentium clock */
#endif

#ifdef PTIMER_ENABLE

#ifdef _WIN32
#include <windows.h>

#define NPMAX 64  /* NPMAX-1 is maximum index allowed for ptimer_Enter,
					 ptimer_Exit calls */
#define NPSUBMAX 30  /*NPSUBMAX-1 is maximum subindex allowed for ptimer_Exit*/

typedef struct {
	int n;
	__int64 sum;
	__int64 max;
} ptimer_stat_t;

typedef struct ptimer_s {
	__int64 startTime;
	ptimer_stat_t stat[NPSUBMAX];
	const char *desc;
} ptimer_t;

extern ptimer_t ptimer_data[NPMAX];

/* Read realtime counter.
 * Stolen from http://www.cs.wustl.edu/~schmidt/ACE_wrappers/ace/OS.i
 * See also http://www.sandpile.org/80x86/rdtsc.shtml
 */
__int64 __inline rdtsc() 
{
	//__int64 hugetime;

	__asm {
		// __asm	rdtsc
		// VC++ doesn't know the opcode for rdtsc (OFh, 31h), so we'll
		// emit the opcode manually.
		__asm		_emit	0xf
		__asm		_emit	0x31
		//__asm		mov	dword ptr hugetime,eax
		//__asm		mov	dword ptr hugetime+4,edx
	}
	//return hugetime;
}

/*------------------------------------------------------------------------
 Sets highest priority on current process.  Should be called near the
  beginning of application.
------------------------------------------------------------------------*/
void ptimer_disableInterrupt();

/*------------------------------------------------------------------------
 Sets normal priority on current process.  Should be called after finished
  with all timing calls.
------------------------------------------------------------------------*/
void ptimer_enableInterrupt();

/*------------------------------------------------------------------------
 Zeroes out ptimer arrays.  Should be called before starting any timing
  calls.
------------------------------------------------------------------------*/
void ptimer_zeroAll();

/*------------------------------------------------------------------------
 Prints out info gathered during timing calls to file.  If file is NULL,
  writes to "ptimer.log".  Should be called after finished with all timing
  calls.
------------------------------------------------------------------------*/
void ptimer_dumpToFile(char *file);

/*------------------------------------------------------------------------
 Sets the start time and description for index; pass a constant string
  as desc, do not pass NULL or empty string. Should be called right before
  function to be timed is called.
------------------------------------------------------------------------*/
void __inline ptimer_Enter(int index, const char *desc)
{
	ptimer_data[index].desc = desc;
	ptimer_data[index].startTime = rdtsc();
}

/*------------------------------------------------------------------------
 Calculates the time difference between this call and the corresponding
  ptimer_Enter call for index.  Subindex can be used to differentiate within
  the same index. Should be called right after function to be timed returns.
------------------------------------------------------------------------*/
void __inline ptimer_Exit(int index, int subindex)
{
	__int64 diff;
	ptimer_stat_t *pstatbuf;

	diff = rdtsc() - ptimer_data[index].startTime;
	pstatbuf = (ptimer_data[index].stat) + subindex;
	(pstatbuf->n)++;
	pstatbuf->sum += diff;
	if (pstatbuf->max < diff)
		pstatbuf->max = diff;
}

#else /* not implemented for non-Win32 systems */

#endif /* _WIN32 */

#else /* not defined PTIMER_ENABLE so define dummy macros */

#define ptimer_Enter(s,t)
#define ptimer_Exit(s, t)
#define ptimer_zeroAll()
#define ptimer_dumpToFile(s)
#define ptimer_enableInterrupt()
#define ptimer_disableInterrupt()

#endif /* PTIMER_ENABLE */
#endif /* ptimer_h */
