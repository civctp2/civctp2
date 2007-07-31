





















#include "c3.h"
#include "AiCell.h"
#include "IC3World.h"
#include "IMapPointData.h"
#include "IC3GameState.h"
#include "IC3GameState.h"
#include "AiMain.h"
#include "CivArchive.h"

AiCell::AiCell() 

{
    flag0=0;
} 

void AiCell::Serialize(CivArchive &archive)

{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << flag0; 
    } else { 
        archive >> flag0; 
    }
} 


uint32 g_mask_gradient[NUM_GRADIENT] = {
    0x1f, 
    0x3f << 5, 
    0x7f << 11, 
    0x7f << 18, 
    0x7f << 25
}; 

   
   
   
   
   

sint32 g_max_gradient[NUM_GRADIENT] = { 
 31,
 63,
 127,
 127,
 127,
};

sint32 g_shift_gradient[NUM_GRADIENT] = { 
    0, 
    5,
    11,
    18,
    25
};


sint32 AiCell::Get(const GRADIENT_BITS g) const 
{
    Assert(0<=g); 
    Assert(g<NUM_GRADIENT); 

    return (flag0  & g_mask_gradient[g]) >> g_shift_gradient[g]; 
} 

void AiCell::Set(const GRADIENT_BITS g, sint32 val)
{  
    Assert(0<=g); 
    Assert(g<NUM_GRADIENT); 

    Assert (0<=val); 

    if (val < 0) { 
        flag0 &= ~g_mask_gradient[g]; 
    } else if (g_max_gradient[g] < val) { 
        flag0 |= g_mask_gradient[g]; 
    } else { 
        flag0 &=  ~g_mask_gradient[g];
        flag0 |= val << g_shift_gradient[g];
    }
} 

void AiCell::Clear(const GRADIENT_BITS g)
{ 
    Assert(0<=g); 
    Assert(g<NUM_GRADIENT); 

    flag0 &= ~g_mask_gradient[g]; 
} 
