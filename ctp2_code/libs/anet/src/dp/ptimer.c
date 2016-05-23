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

#include "ptimer.h"

#ifdef PTIMER_ENABLE
#include <stdio.h>
#include "eclock.h"

ptimer_t ptimer_data[NPMAX];

#ifdef _WIN32

/*------------------------------------------------------------------------
 Sets highest priority on current process.  Should be called near the
  beginning of application.
------------------------------------------------------------------------*/
/* Disable interrupts */
void ptimer_disableInterrupt()
{
#if 0
	__asm {
		__asm cli
	}
#else
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif
}

/*------------------------------------------------------------------------
 Sets normal priority on current process.  Should be called after finished
  with all timing calls.
------------------------------------------------------------------------*/
/* Enable interrupts */
void ptimer_enableInterrupt()
{
#if 0
	__asm {
		__asm sti
	}
#else
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
#endif
}

/*------------------------------------------------------------------------
 Zeroes out ptimer arrays.  Should be called before starting any timing
  calls.
------------------------------------------------------------------------*/
void ptimer_zeroAll()
{
	memset(ptimer_data, 0, NPMAX * sizeof(ptimer_t));
}

/*------------------------------------------------------------------------
 Prints out info gathered during timing calls to file.  If file is NULL,
  writes to "ptimer.log".  Should be called after finished with all timing
  calls.
------------------------------------------------------------------------*/
void ptimer_dumpToFile(char *file)
{
	FILE *fp;
	clock_t now;
	__int64 starttime, endtime, rdtsc_per_sec;
	ptimer_disableInterrupt();
	now = eclock();
	starttime = rdtsc();
	/* the following is kludgy; for more accuracy, make the time cycle longer */
	while ((long)(eclock() - now) < ECLOCKS_PER_SEC)
		;
	endtime = rdtsc();
	rdtsc_per_sec = endtime - starttime;
	if (!file)
		file = "ptimer.log";
	fp = fopen(file, "w");
	if (fp) {
		int i, j;
		fprintf(fp, "ticks per sec: %d\n", rdtsc_per_sec);
		for (i = 0; i < NPMAX; i++) {
			if (ptimer_data[i].desc)
				fprintf(fp, "%s:\n", ptimer_data[i].desc);
			else
				continue;
			for (j = 0; j < NPSUBMAX; j++) {
				if (ptimer_data[i].stat[j].n) {
					fprintf(fp, " element %d:\n", j);
					fprintf(fp, "  #times:%7d  avg time(us):%13.3f  max time(us):%13.3f\n",
							ptimer_data[i].stat[j].n, ((float)ptimer_data[i].stat[j].sum * 1000000) / (ptimer_data[i].stat[j].n * (float)rdtsc_per_sec), ((float)ptimer_data[i].stat[j].max * 1000000) / rdtsc_per_sec);
				}
			}
		}
	}
#if 0 /* time ptimer calls */
	{
		int i;
		for (i=0; i<100; i++) {
			ptimer_Enter(0, "foo");
			ptimer_Exit(0, i);
		}
	}
	{
	__int64 difftime;
	starttime = rdtsc();
	endtime = rdtsc();
	endtime = rdtsc();
	difftime = endtime - starttime;
	fprintf(fp, "time for rdtsc call: %I64d ticks %10.3f us\n", difftime, ((float)difftime * 1000000) / rdtsc_per_sec);
	ptimer_zeroAll();
	starttime = rdtsc();
	ptimer_Enter(0, "dummy call");
	ptimer_Exit(0, 0);
	endtime = rdtsc();
	difftime = endtime - starttime;
	fprintf(fp, "time for firsttime ptimer enter/exit call: %I64d ticks %10.3f us\n", difftime, ((float)difftime * 1000000) / rdtsc_per_sec);
	starttime = rdtsc();
	ptimer_Enter(0, "dummy call");
	ptimer_Exit(0, 0);
	endtime = rdtsc();
	difftime = endtime - starttime;
	fprintf(fp, "time for secondtime ptimer enter/exit call: %I64d ticks %10.3f us\n", difftime, ((float)difftime * 1000000) / rdtsc_per_sec);
	ptimer_zeroAll();
	starttime = rdtsc();
	ptimer_Enter(0, "dummy call");
	endtime = rdtsc();
	difftime = endtime - starttime;
	fprintf(fp, "time for firsttime ptimer_Enter call: %I64d ticks %10.3f us\n", difftime, ((float)difftime * 1000000) / rdtsc_per_sec);
	starttime = rdtsc();
	ptimer_Exit(0, 0);
	endtime = rdtsc();
	difftime = endtime - starttime;
	fprintf(fp, "time for firsttime ptimer_Exit call: %I64d ticks %10.3f us\n", difftime, ((float)difftime * 1000000) / rdtsc_per_sec);
	starttime = rdtsc();
	ptimer_Enter(0, "dummy call");
	endtime = rdtsc();
	difftime = endtime - starttime;
	fprintf(fp, "time for secondtime ptimer_Enter call: %I64d ticks %10.3f us\n", difftime, ((float)difftime * 1000000) / rdtsc_per_sec);
	starttime = rdtsc();
	ptimer_Exit(0, 0);
	endtime = rdtsc();
	difftime = endtime - starttime;
	fprintf(fp, "time for secondtime ptimer_Exit call: %I64d ticks %10.3f us\n", difftime, ((float)difftime * 1000000) / rdtsc_per_sec);
	}
#endif
	fclose(fp);
}

#endif /* _Win32 */
#endif /* PTIMER_ENABLE */
