

#include "c3.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "SlicSegment.h"
#include "QuickSlic.h"





void QuickSlic(char *id, sint32 recipient, BOOL one_shot)
{
    
    if (one_shot) {
        if(!g_slicEngine->GetSegment(id))
            return;
        if(g_slicEngine->GetSegment(id)->HasBeenShown(recipient)) {
            return;
        }
    }

    SlicObject *so = new SlicObject(id);
    so->AddRecipient(recipient);
    g_slicEngine->Execute(so);
}
