









#ifndef __AI_MATCH_H
#define __AI_MATCH_H



void AI_Match_Init (void);
void AI_Match_Close (void);
void AI_Match_Team_Init (Planner_ptr the_team);
void AI_Match_Team_Close (Planner_ptr the_team);




void AI_Match_Team_Unit_Strength_Recalculate (Planner_ptr the_team);










extern int AI_Match_Get_Unit_Strength 
(
	AiMain *ai,							
	Agent * the_unit					
);


#endif