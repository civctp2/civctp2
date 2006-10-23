

#include "c3.h"
#include "QuickSlic.h"

#include "SlicObject.h"
#include "SlicEngine.h"
#include "SlicSegment.h"





void QuickSlic(char const * id, sint32 recipient)
{
    if (g_slicEngine->GetSegment(id) &&
        !g_slicEngine->GetSegment(id)->HasBeenShown(recipient)
       ) 
    {
        SlicObject * so = new SlicObject(id);
        so->AddRecipient(recipient);
        g_slicEngine->Execute(so);
    }
}
