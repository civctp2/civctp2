/*---------------------------------------------------------------------------
 Code to check whether a Greenleaf connection to a peer is working properly.

 $Log: GNLFRAMP.H $
 Revision 1.1  1996/12/13 23:26:05  jgraebner
 Initial revision
 Revision 1.2  1996/07/01 20:51:50  dkegel
 Send and check sessionID to make sure it's not the same as ours.
 Revision 1.1  1996/06/30 21:16:35  dkegel
 Initial revision
---------------------------------------------------------------------------*/

#ifndef gnlframp_h
#define gnlframp_h

#include "commlib.h"
/*---------------------------------------------------------------------------
 Send a ramp and our ID to the other system, get their ramp and ID.
 print any differences in the ramp to both DPRINT and fdprint.
 If session ID's are the same, print warning to both DPRINT and fdprint.
---------------------------------------------------------------------------*/
void exchange_ramp(PORT *port, long sessionID);

#endif
