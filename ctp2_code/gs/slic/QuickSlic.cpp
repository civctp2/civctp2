#include "ctp/c3.h"
#include "gs/slic/QuickSlic.h"

#include "gs/slic/SlicObject.h"
#include "gs/slic/SlicEngine.h"
#include "gs/slic/SlicSegment.h"




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
