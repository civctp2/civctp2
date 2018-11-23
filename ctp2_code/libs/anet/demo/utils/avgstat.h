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

#ifndef avgstat_H
#define avgstat_H

#include <time.h>
#include "anet.h"

#ifdef dp_STATS

/*-------------------------------------------------------------------------
 Structure to hold dp statistics at a particular momement in time.
-------------------------------------------------------------------------*/
typedef struct {
  dp_stat_t stats[dp_STAT_MAX+1];
  time_t t;
} dp_stats_t;

/*-------------------------------------------------------------------------
 Structure to keep track of dp statistics over the course of an hour.
-------------------------------------------------------------------------*/
typedef struct {
  dp_stats_t stats[60];
  int oldest; /* 0..59; oldest element of stats */
  dp_stats_t max1;	/* worst one minute statistics seen so far */
  dp_stats_t max60;	/* worst sixty minute statistics seen so far */
} dp_avgstat_t;

/*-------------------------------------------------------------------------
 callback function to print a single line of report, e.g. to a log file.
-------------------------------------------------------------------------*/
typedef void (*dp_avgstat_printline_fn_t)(const char *buf, void *context);

/*-------------------------------------------------------------------------
 Initialize a avgstat; call once on startup.
 return value is dp_RES_OK on success, or error code returned by
 dpGetStats on failure.
-------------------------------------------------------------------------*/
dp_result_t dp_avgstat_init(dp_t *dp, dp_avgstat_t *avgstat);

/*-------------------------------------------------------------------------
 Call once a minute to update statistics.
 Places one minute and sixty minute average statistics in the given buffers.
 stats1 and stats60 get the average over the last minute and hour;
 max1 and max60 get the worst minute and hour seen so far.
 return value is dp_RES_OK on success, or error code returned by
 dpGetStats on failure.
-------------------------------------------------------------------------*/
dp_result_t dp_avgstat_poll(dp_t *dp, dp_avgstat_t *avgstat,
		dp_stats_t *stats1,
		dp_stats_t *stats60,
		dp_stats_t *max1,
		dp_stats_t *max60);

/*-------------------------------------------------------------------------
 print out the statistics in the given buffer, which was obtained from
 dp_avgstat_poll.
 An ASCII report is passed line by line to the given callback function.
 This should be called e.g. when closing a session to print out the average
 statistics durning the session.
-------------------------------------------------------------------------*/
dp_result_t dp_avgstat_print(dp_t *dp,
		const dp_stats_t *stats,
		dp_avgstat_printline_fn_t printfn, void *printcontext);
#endif
#endif
