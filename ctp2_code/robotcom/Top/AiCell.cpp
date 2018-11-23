#include "c3.h"
#include "globals.h"
#include "ic3world.h"
#include "IMapPointData.h"
#include "ic3GameState.h"
#include "aimain.h"
#include "civarchive.h"

#include "aicell.h"

#include "FzOut.h"
#include "bit_table.h"

AiCell::AiCell()
{
   memset(this, 0, sizeof(*this));
}

void AiCell::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
        archive.Store((uint8*) this, sizeof(*this));
    } else {
        archive.Load((uint8*) this, sizeof(*this));
    }
}





AiCellPtr::AiCellPtr()
{
    Init();
}

void AiCellPtr::Init()
{
    x = -1;
    y = -1;
    m_queue_idx = -1;
}

AiCellPtr::~AiCellPtr()
{
    return;
}

#include "Memory_Manager.h"
extern Memory_Manager *g_memory_AiCellPtr;
void* AiCellPtr::operator new(size_t byte_size)
{
    Assert(sizeof(AiCellPtr)==byte_size);
    return g_memory_AiCellPtr->Alloc();
}
void AiCellPtr::operator delete(void *ptr)
{
    g_memory_AiCellPtr->Dealloc(ptr);
}

AiCellPtr& AiCellPtr::operator= (AiCellPtr &set_me)
{
    x = set_me.x;
    y = set_me.y;
    m_queue_idx = set_me.m_queue_idx;
    return *this;
}

extern Bit_Table *g_static_can_settle;
extern AiMain *g_settle_ai;
extern sint16 g_settle_z;

sint32 AiCellPtr::operator<(const AiCellPtr &rhs) const
{

    sint16 right_val, left_val;
    if  (g_static_can_settle->Get(rhs.x, rhs.y)) {
        right_val = g_settle_ai->m_world->GetSettleValue(rhs.x, rhs.y, g_settle_z);
    } else {
        right_val = 0;
    }

    if  (g_static_can_settle->Get(x, y)) {
        left_val = g_settle_ai->m_world->GetSettleValue(x, y, g_settle_z);
    } else {
        left_val = 0;
    }

    if (left_val > right_val) {
        return TRUE;
    } else if (left_val < right_val) {
        return FALSE;
    } else {

        if (rhs.x < x) {
            return TRUE;
        } else if (rhs.x > x) {
            return FALSE;
        } else if (rhs.y < y) {
            return TRUE;
        } else if (rhs.y > y) {
            return FALSE;
        } else {
            return FALSE;
        }
    }
}
