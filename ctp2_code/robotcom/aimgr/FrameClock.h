












  
#pragma once 
#ifndef __AI_FRAME_CLOCK__
#define __AI_FRAME_CLOCK__ 1

class CivArchive; 

class FrameClock { 

    uint32 m_goal_time_slice;
    uint32 m_max_time_slice;
    uint32 m_max_total_time;
    uint32 m_end_time_slice;
    uint32 m_start_time_slice;
    uint32 m_accumulated_frame_time;

public:

	FrameClock(uint32 goal_time_slice, uint32 max_time_slice, uint32 max_total_time); 
    FrameClock(CivArchive &archive); 
    void Serialize(CivArchive &archive); 

    void InitFrame(uint32 start_time_slice, uint32 accumulated_frame_time);
    BOOL OnTime() const; 
    BOOL OnTotalTime() const;
};

#endif