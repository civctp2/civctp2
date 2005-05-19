
#include "c3.h"
#include "Wallclock.h"


Wall_Clock::Wall_Clock()
{
	m_head = NULL; 
    m_tail = NULL; 
    m_current_check = NULL;
	m_start_time = 0; 
    m_current_time = NULL; 
	m_is_first_cycle = TRUE; 
    m_do_check = TRUE; 

    m_head = new Check_Point;
    sprintf(m_head->m_file_name, "\\total"); 
    m_head->m_line_number = 0; 
    m_head->m_next = NULL; 
    m_head->m_delta_time = 0;
    m_head->m_count = 0; 

    m_multi_head = NULL; 
    m_multi_ptr = NULL; 
    m_start_multi = 0;
    ClearRecur();

}

Wall_Clock::~Wall_Clock()
{
    
    Check_Point *ptr = m_head; 
    Check_Point *del_me; 

#ifdef _DEBUG
    sint32 finite_count=0; 
#endif

    while(ptr) { 

Assert(finite_count++ < 1000); 
        del_me = ptr; 
        ptr = ptr->m_next;
        delete del_me; 
    } 
}


void Wall_Clock::StartCheck(BOOL do_check)
{
    m_do_check = do_check; 

    if (!m_do_check) return; 

    m_current_check = m_head; 
    m_start_time = m_current_time = GetTickCount() ; 

    ClearMulti();
    ClearRecur();
	ResetNewMatches();
	ResetTotalMatches();

}

void Wall_Clock::EndCheck(const int &num, const bool &first)
{
    if (!m_do_check) return; 
    
    m_head->m_delta_time = GetTickCount() - m_start_time; 
    
#ifdef _DEBUG
    sint32 finite_count=0; 
#endif
    
    FILE *fout=NULL; 
    Check_Point *ptr=NULL; 
    char *str; 
	char filename[100];
	int i;
    if (first) { 
		sprintf(filename,"logs\\wallclock%d.txt",num);
        fout = fopen(filename, "w"); 
        Assert(fout); 
        ptr = m_head; 
		for (i = 0; i < k_MAX_GOAL_TYPES; i++)
		{
			fprintf( fout, "total_matches[%d], ",i);
		}

		for (i = 0; i < k_MAX_GOAL_TYPES; i++)
		{
			fprintf( fout, "new_matches[%d], ",i);
		}

        fprintf (fout, "execute, aux,"); 
        while (ptr) { 
            Assert(finite_count++ < 1000); 
            str = strrchr(ptr->m_file_name, '\\');
            fprintf (fout, "%s%d", str+1,  ptr->m_line_number);  
            if (ptr->m_next) { 
                fprintf (fout, ", "); 
            } 
            ptr = ptr->m_next; 
        } 
        m_is_first_cycle = FALSE;
        
        
        if (m_multi_head) { 
            fprintf( fout, ", "); 
        }
        ptr = m_multi_head; 
        while (ptr) { 
            Assert(finite_count++ < 1000); 
            str = strrchr(ptr->m_file_name, '\\');
            fprintf (fout, "%s%d, count", str+1,  ptr->m_line_number);  
            if (ptr->m_next) { 
                fprintf (fout, ", "); 
            } 
            ptr = ptr->m_next; 
        } 
        m_is_first_cycle = FALSE;
        
        fprintf (fout, "\n"); 
        
    } else { 
		sprintf(filename,"logs\\wallclock%d.txt",num);
        fout = fopen(filename, "a"); 
        Assert(fout); 
    }
    
	for (i = 0; i < k_MAX_GOAL_TYPES; i++)
		fprintf (fout, "%d, ", m_total_matches[i]);

	for (i = 0; i < k_MAX_GOAL_TYPES; i++)
		fprintf (fout, "%d, ", m_new_matches[i]);

    fprintf (fout, "%d, %d, ", m_recur_delta[0], m_recur_delta[1]); 

    ptr = m_head; 
    while (ptr) { 
        Assert(finite_count++ < 1000); 
        fprintf (fout, "%d", ptr->m_delta_time);  
        if (ptr->m_next) { 
            fprintf (fout, ", "); 
        }
        ptr = ptr->m_next; 
    } 
    
    if (m_multi_head) { 
        fprintf( fout, ", "); 
    }
    ptr = m_multi_head; 
    
    while (ptr) { 
        Assert(finite_count++ < 1000); 
        fprintf (fout, "%d, %d", ptr->m_delta_time, ptr->m_count);  
        if (ptr->m_next) { 
            fprintf (fout, ", "); 
        } 
        ptr = ptr->m_next; 
    } 
    
    fprintf (fout, "\n");
    fclose(fout); 
    
}


void Wall_Clock::InsertCheckPoint(char *filename, sint32 line_number)
{
     if (!m_do_check) return; 


    sint32 end_time = GetTickCount(); 

    if (m_is_first_cycle) { 

        m_current_check->m_next = new Check_Point;
        m_current_check = m_current_check->m_next; 
        strcpy(m_current_check->m_file_name, filename); 
        m_current_check->m_line_number = line_number; 
        m_current_check->m_next = NULL; 
        m_current_check->m_delta_time = end_time - m_current_time;
        m_current_time = GetTickCount() ; 
        

    } else { 
        m_current_check = m_current_check->m_next; 

        Assert(m_current_check); 
        Assert(m_current_check->m_line_number == line_number); 
        Assert (0 == strcmp(filename, m_current_check->m_file_name)); 

        m_current_check->m_delta_time = end_time - m_current_time;
        m_current_time = GetTickCount() ; 
    } 
}

void Wall_Clock::ClearMulti()
{ 
    if (!m_do_check) return; 
    Check_Point *ptr=NULL; 
    for (ptr = m_multi_head; ptr; ptr = ptr->m_next) { 
        ptr->m_delta_time = 0; 
        ptr->m_count = 0; 
    } 
} 

void Wall_Clock::StartMulti(char *filename, sint32 line_number)
{
    if (!m_do_check) return; 

    m_start_multi = GetTickCount(); 

    char *tmpstr = strrchr(filename, '\\');
   
    Check_Point *ptr = m_multi_head; 

    if (NULL == ptr) { 
       ptr = new Check_Point;

        ptr->m_next = NULL;
        strcpy(ptr->m_file_name, tmpstr); 
        ptr->m_line_number = line_number; 
        ptr->m_next = NULL; 
        ptr->m_delta_time = 0;
        ptr->m_count = 0; 
        m_multi_head = ptr;
        m_multi_ptr = ptr; 
        return; 
    } 

    while(ptr) { 
        if ((line_number == ptr->m_line_number) && (0== strcmp(tmpstr, ptr->m_file_name))) { 
            m_multi_ptr = ptr; 
            return; 
        } 

        if (ptr->m_next) { 
            ptr = ptr->m_next; 
        } else { 
            break; 
        }
    }


    ptr->m_next = new Check_Point;

    ptr->m_next->m_next = NULL;
    strcpy(ptr->m_next->m_file_name, tmpstr); 
    ptr->m_next->m_line_number = line_number; 
    ptr->m_next->m_next = NULL; 
    ptr->m_next->m_delta_time = 0;
    ptr->m_next->m_count = 0; 
    m_multi_ptr = ptr->m_next; 
}

void Wall_Clock::EndMulti()
{
    if (!m_do_check) return; 
    m_multi_ptr->m_delta_time += GetTickCount() - m_start_multi; 
    m_multi_ptr->m_count++; 
}

void Wall_Clock::ClearRecur()
{
    if (!m_do_check) return; 
    sint32 i; 
    for (i=0; i<k_MAX_RECUR; i++) { 
        m_recur_time[i] = 0; 
        m_recur_delta[i] = 0;
    }
}

void Wall_Clock::StartRecur(sint32 idx)

{
    if (!m_do_check) return; 
    m_recur_time[idx] = GetTickCount(); 
}

void Wall_Clock::StopRecur(sint32 idx)
{
    if (!m_do_check) return; 
    m_recur_delta[idx] += GetTickCount() - m_recur_time[idx];
}

void Wall_Clock::ResetNewMatches()
{
	for (int i=0; i < k_MAX_GOAL_TYPES ; i++)
		m_new_matches[i] = 0;
}

void Wall_Clock::ResetTotalMatches()
{
	for (int i=0; i < k_MAX_GOAL_TYPES ; i++)
		m_total_matches[i] = 0;
}

void Wall_Clock::AddTotalMatches(const sint32 &total_matches, const sint32 &type) 
{
	Assert(type >=0 && type < k_MAX_GOAL_TYPES );
	m_total_matches[type] +=total_matches;
};

void Wall_Clock::AddNewMatches(const sint32 &new_matches, const sint32 &type)
{
	Assert(type >=0 && type < k_MAX_GOAL_TYPES );
	m_new_matches[type]+=new_matches;
};

sint32 Wall_Clock::GetNewMatches(const sint32 &type) const 
{
	Assert(type >= 0 && type < k_MAX_GOAL_TYPES );
	return m_new_matches[type];
};

sint32 Wall_Clock::GetTotalMatches(const sint32 &type) const 
{
	Assert(type >= 0 && type < k_MAX_GOAL_TYPES );
	return m_total_matches[type];
};
