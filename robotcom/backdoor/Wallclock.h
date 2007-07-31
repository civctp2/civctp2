

#ifndef __WALL_CLOCK_H__
#define __WALL_CLOCK_H__

struct Check_Point; 

struct Check_Point { 
	char m_file_name[500]; 
	sint32 m_line_number; 
	sint32 m_delta_time;
    sint32 m_count; 
    Check_Point *m_next; 
}; 

#define k_MAX_RECUR 2
#define k_MAX_GOAL_TYPES 50

class Wall_Clock { 


	Check_Point *m_head; 
	Check_Point *m_tail; 
    Check_Point *m_current_check; 
	sint32 m_start_time; 
    sint32 m_current_time; 
	BOOL m_is_first_cycle; 
    BOOL m_do_check; 

    
    Check_Point *m_multi_head; 
    Check_Point *m_multi_ptr; 
    sint32 m_start_multi; 

    sint32 m_recur_time[k_MAX_RECUR]; 
    sint32 m_recur_delta[k_MAX_RECUR];
	sint32 m_new_matches[k_MAX_GOAL_TYPES];
	sint32 m_total_matches[k_MAX_GOAL_TYPES];


public:

	Wall_Clock(); 
	~Wall_Clock(); 

	void StartCheck(BOOL do_check); 
	void EndCheck(const int &num, const bool &first); 
	void InsertCheckPoint(char *filename, sint32 line_number); 

    void ClearMulti();
    void StartMulti(char *filename, sint32 line_number);
    void EndMulti();

    void ClearRecur();
    void StartRecur(sint32 idx); 
    void StopRecur(sint32 idx);
	Check_Point *GetCurrentCheck() const {return m_current_check;};

	
	void ResetNewMatches();
	void ResetTotalMatches();
	void AddTotalMatches(const sint32 &total_matches, const sint32 &type);
	void AddNewMatches(const sint32 &new_matches, const sint32 &type);
	sint32 GetNewMatches(const sint32 &type) const;
	sint32 GetTotalMatches(const sint32 &type) const;
};



#ifdef DO_WALLCLOCK
#define CHECKCLOCK {if (g_wall_clock) g_wall_clock->InsertCheckPoint(__FILE__, __LINE__);}
#else 
#define CHECKCLOCK 
#endif

#ifdef DO_WALLCLOCK
#define START_MULTI_CLOCK { if (g_wall_clock) g_wall_clock->StartMulti(__FILE__, __LINE__);}
#define END_MULTI_CLOCK { if (g_wall_clock) g_wall_clock->EndMulti(); }
#else
#define START_MULTI_CLOCK 
#define END_MULTI_CLOCK 
#endif

#ifdef DO_WALLCLOCK
#define START_WALLCLOCK(CHECK_SIGNAL)  { if (g_wall_clock) { g_wall_clock->StartCheck(CHECK_SIGNAL); }}
#define END_WALLCLOCK(FILE_NUM, CREATE_NEW_LOG)	{ if (g_wall_clock && my_turn_is_over) { g_wall_clock->EndCheck(FILE_NUM, CREATE_NEW_LOG); }}
#else
#define START_WALLCLOCK(CHECK_SIGNAL) 
#define END_WALLCLOCK(FILE_NUM, CREATE_NEW_LOG)	
#endif

#endif __WALL_CLOCK_H__
