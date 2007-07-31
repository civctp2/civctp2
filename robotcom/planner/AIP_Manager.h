#ifndef __AIP_Manager_h__
#define __AIP_Manager_h__
















class AIP;
template <class DATA_TYPE> class list_array;




#define INITIAL_AIP_COUNT 50








class AIP_Manager
{
public:


	
	
	
	
	
	
	AIP_Manager();


	
	
	
	
	
	
	void Init
	(
		AiMain *ai						
	);


	
	
	
	
	
	
	~AIP_Manager();


	
	
	
	
	
	
	void Update_AIP
	(
        AiMain *ai,
		AIP *current_AIP,				
		char *new_AIP_name				
	);


	
	
	
	
	
	
	void Load_All_AIPs_In_Directory
	(
		AiMain *ai,						
		char *directory_full_of_AIPs	
	);


protected:


	
	
	
	
	
	
	void Clean_Up();


	
	
	
	
	
	
	
	
	AIP * Look_Up_New_AIP
	(
		char *new_AIP_name				
	);



public:


protected:

	
	list_array<AIP *> * AIPs;

	
	bool inited;

};








#endif 


