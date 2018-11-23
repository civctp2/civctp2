#include "c3.h"
#include "globals.h"
#include "aimain.h"

#include "FSOppAction.h"
#include "Oa.h"

#include "Wallclock.h"
extern Wall_Clock *g_wall_clock;









BOOL FS_Opportunity_Actions::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{












    for (int i=0; i< OPPORTUNITY_ACTION_MAX; i++) {
		ai->m_opportunity_action[i]->Execute(ai);

        if (ai->m_i_am_dead) {
            return FALSE;
        }
    }


    return TRUE;
}


void FS_Opportunity_Actions::Serialize(CivArchive &archive)
{

}
