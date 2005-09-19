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

#include <stdio.h>
#include <string.h>
#include "avgstat.h"

#ifdef dp_STATS

/*-------------------------------------------------------------------------
 Initialize a avgstat; call once on startup.
 return value is dp_RES_OK on success, or error code returned by
 dpGetStats on failure.
-------------------------------------------------------------------------*/
dp_result_t dp_avgstat_init(dp_t *dp, dp_avgstat_t *avgstat)
{
	int i;
    dp_result_t err;
	dp_stats_t tmp;

	if (!avgstat || !dp)
		return dp_RES_BAD;
	
	avgstat->oldest = 0;

	/* get current stats into tmp */
	err = dp_RES_OK;
	for (i = 0; i <= dp_STAT_MAX; i++) {
		 err = dpGetStats(dp, i, &tmp.stats[i], sizeof(tmp.stats[i]));
		 if (err != dp_RES_OK)
			 return err;
	}	
	tmp.t = time(0);
	/* copy tmp into every element of avgstat->stats */
	for (i = 0; i < 60; i++) 
		avgstat->stats[i] = tmp;
	/* zero out the max accumulators */
	memset(&avgstat->max1, 0, sizeof(dp_stats_t));
	memset(&avgstat->max60, 0, sizeof(dp_stats_t));
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Find the element by element maximum of the two records.
-------------------------------------------------------------------------*/
void mymax(dp_stat_t *result, const dp_stat_t *x)
{
	 if (result->in < x->in) result->in = x->in;
	 if (result->out < x->out) result->out = x->out;
	 if (result->dropped < x->dropped) result->dropped = x->dropped;
	 if (result->waiting < x->waiting) result->waiting = x->waiting;
	 result->sum_waiting = 0;
	 result->num_waiting = 0;
}

/*-------------------------------------------------------------------------
 Subtract the second dp_stat_t from the first.
 dt is the number of seconds between the gathering of the two statistics;
 use it to normalize all counts to units per minute.
 (waiting is an instantaneous value, and doen't need normalizing.)
-------------------------------------------------------------------------*/
void subtract(dp_stat_t *result, const dp_stat_t *x, time_t dt)
{
	DPRINT(("subtract +: %d in %d, out %d, dropped %d, waiting %d\n", dt, result->in, result->out, result->dropped, result->waiting)); 
	DPRINT(("subtract -: %d in %d, out %d, dropped %d, waiting %d\n", dt, x->in, x->out, x->dropped, x->waiting)); 
	 result->in -= x->in;
	 result->out -= x->out;
	 result->dropped -= x->dropped;
	 result->sum_waiting -= x->sum_waiting;
	 result->num_waiting -= x->num_waiting;
	 result->waiting = 0;
	 if (result->num_waiting > 0)
		 result->waiting = (result->sum_waiting + result->num_waiting / 2) / result->num_waiting;
	 if (dt == 0) 
		 dt = 1000000; /* avoid crashes */
	 /* multiply by 60 to get units per minute */
	 result->in = (result->in * 60) / dt;
	 result->out = (result->out * 60) / dt;
	 result->dropped = (result->dropped * 60) / dt;
}

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
		dp_stats_t *max60)
{
	int i;
	int newest;
	int oldest;
    dp_result_t err;
	dp_stats_t tmp;
	time_t	dt;

	if (!avgstat || !dp)
		return dp_RES_BAD;
	
	/* get current stats into tmp */
	err = dp_RES_OK;
	for (i = 0; i <= dp_STAT_MAX; i++) {
		 err = dpGetStats(dp, i, &tmp.stats[i], sizeof(tmp.stats[i]));
		 if (err != dp_RES_OK)
			 return err;
	}	
	tmp.t = time(0);

	oldest = avgstat->oldest;
	newest = oldest-1;
	if (newest < 0)
		newest += 60;
	DPRINT(("dp_avgstat_poll: oldest %d newest %d\n", oldest, newest));

	/* put one minute average into stats1 */	
	*stats1 = tmp;
	dt = tmp.t - avgstat->stats[newest].t;
	for (i = 0; i <= dp_STAT_MAX; i++) 
		subtract(&stats1->stats[i], &avgstat->stats[newest].stats[i], dt);

	/* put 60 minute average into stats60 */	
	*stats60 = tmp;
	dt = tmp.t - avgstat->stats[oldest].t;
	for (i = 0; i <= dp_STAT_MAX; i++) 
		subtract(&stats60->stats[i], &avgstat->stats[oldest].stats[i], dt);

	/* update stored statistics for next time */
	avgstat->stats[oldest] = tmp;
	oldest++;
	if (oldest >= 60) 
		oldest = 0;
	avgstat->oldest = oldest;
	for (i = 0; i <= dp_STAT_MAX; i++) 
		mymax(&avgstat->max1.stats[i], &stats1->stats[i]);
	*max1 = avgstat->max1;

	/* only update 60 minute max only if 60 minute data available */
	if (dt >= 60 * 60) {
		for (i = 0; i <= dp_STAT_MAX; i++) 
			mymax(&avgstat->max60.stats[i], &stats60->stats[i]);
	}
	*max60 = avgstat->max60;
	return dp_RES_OK;

}
	
/*-------------------------------------------------------------------------
 print out the statistics in the given buffer, which was obtained from
 dp_avgstat_poll.
 An ASCII report is passed line by line to the given callback function.
 This should be called e.g. when closing a session to print out the average
 statistics durning the session.
-------------------------------------------------------------------------*/
dp_result_t dp_avgstat_print(dp_t *dp,  
		const dp_stats_t *stats,
		dp_avgstat_printline_fn_t printfn, void *printcontext)
{
	int i;
	char obuf[1024];
	static char * names[dp_STAT_MAX+1] = {
		"DPIO_RX_REL_BYTES",
		"DPIO_RX_REL_PKTS",
		"DPIO_RX_UNREL_BYTES",
		"DPIO_RX_UNREL_PKTS",
		"DPIO_TX_REL_BYTES",
		"DPIO_TX_REL_PKTS",
		"DPIO_TX_UNREL_BYTES",
		"DPIO_TX_UNREL_PKTS",
		"DPIO_CONNECTING",
		"DPIO_CONNECTED",  
		"DPTAB_RX_BYTES",
		"DPTAB_RX_RECORDS",
		"DPTAB_TX_BYTES",
		"DPTAB_TX_RECORDS"
	};
 	
	if (!stats || !printfn)
		return dp_RES_BAD;

	for (i = 0; i <= dp_STAT_MAX; i++) {
		 sprintf(obuf, "%s : in %ld, out %ld, dropped %ld, waiting %ld", names[i], stats->stats[i].in, stats->stats[i].out, stats->stats[i].dropped, stats->stats[i].waiting); 
		 printfn(obuf, printcontext);
	}	
	return dp_RES_OK;
}

#endif
