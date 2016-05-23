#include "c3.h"

#include "FrameClock.h"
#include "civarchive.h"










FrameClock::FrameClock(
    uint32 goal_time_slice,
    uint32 max_time_slice,
	uint32 max_total_time
) {
    m_goal_time_slice = goal_time_slice;
    m_max_time_slice = max_time_slice;
    m_max_total_time = max_total_time;
    m_end_time_slice=0;
    m_start_time_slice=0;
    m_accumulated_frame_time=0;
}







FrameClock::FrameClock(CivArchive &archive)
{
    Serialize(archive);
}







void FrameClock::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
        archive.Store((uint8*)this, sizeof(*this));
    } else {
        archive.Load((uint8*)this, sizeof(*this));
    }
}








void FrameClock::InitFrame(
     uint32 start_time_slice,
     uint32 accumulated_frame_time
) {
    m_start_time_slice = start_time_slice;
    m_end_time_slice = m_start_time_slice + m_goal_time_slice;
    m_accumulated_frame_time = accumulated_frame_time;

}









BOOL FrameClock::OnTotalTime() const
{
   return ((GetTickCount() -  m_start_time_slice) + m_accumulated_frame_time)  < m_max_total_time;
}









BOOL FrameClock::OnTime() const

{
    return (GetTickCount() < m_end_time_slice);
}
