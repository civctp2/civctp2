










































#include "c3.h"
#include "c3types.h"
#include "c3errors.h"
#include "globals.h"
#include "IC3Player.h"
#include "AiMain.h"


#include "common.h"

#include "list_array.h"

#include "linked_list.h"

#include "Strategic_AI_Forward.h"

#include "AIP.h"

#include "AIP_Manager.h"

#include "dr_debug.h"

#include "AILog.h"













AIP_Manager::AIP_Manager() 
{
	
	inited = false;

}













void AIP_Manager::Init
(
	AiMain *ai							
)
{
	
	char aip_directory_path[MAX_AIP_FILE_NAME];
	

	
	
	if (inited) return;

	
	AIPs = new list_array<AIP *> (INITIAL_AIP_COUNT);

	
	sprintf(aip_directory_path, "%s\\AIPs", ai->m_gs->GetDataPath());

	
	Load_All_AIPs_In_Directory(ai, aip_directory_path);

	
	inited = true;

}













AIP_Manager::~AIP_Manager()
{
	
	Clean_Up();
}













void AIP_Manager::Update_AIP
(
    AiMain *ai, 
	AIP *current_AIP,					
	char *new_AIP_name					
)
{
	
	AIP * new_aip;						
	

	
	new_aip = Look_Up_New_AIP(new_AIP_name);

	
	Assert(new_aip isnt NULL);

	
    if (new_aip is NULL)  { 
        

#ifdef _DEBUG
ai->m_player->DipLogStr(ai->m_my_player_id, "new AIP NOT LOADED - STRING NOT FOUND");
ai->m_player->DipLogStr(ai->m_my_player_id, new_AIP_name);
#endif        
        return;
    }


#ifdef _DEBUG
char tmp_str[200]; 
sprintf(tmp_str, "%s\n", new_AIP_name);
ai->m_player->DipLogStr(ai->m_my_player_id, tmp_str);
#endif        

	
	current_AIP->Switch_AIP(new_aip);

}












void AIP_Manager::Load_All_AIPs_In_Directory
(
	AiMain *ai,							
	char *directory_full_of_AIPs		
)
{
	
	WIN32_FIND_DATA find_AIP_files;	
	char files_to_find[MAX_AIP_FILE_NAME];	
    HANDLE file_find_handle;			
	AIP * new_aip;						
	

	
	sprintf(files_to_find, "%s\\*.aip", directory_full_of_AIPs);

	
    file_find_handle = FindFirstFile(files_to_find, &find_AIP_files);

	
    if (file_find_handle == INVALID_HANDLE_VALUE) 
	{
        return;

    } 
    
    
    do 
	{
		
		new_aip = new AIP(ai);

		
		new_aip->Load_AIP_File(find_AIP_files.cFileName, true);

		
		AIPs->Append_Data(new_aip);

    } while (FindNextFile(file_find_handle, &find_AIP_files) == TRUE);
}

















void AIP_Manager::Clean_Up()
{
	
	int i;
	AIP * an_AIP;						
	

	
	for (i = 0; i < AIPs->count; i++)
	{
		
		an_AIP = AIPs->Return_Data_By_Number(i);

		
        if (an_AIP isnt NULL) {
            delete an_AIP;
            an_AIP = NULL; 
        } 

	} 

	
	AIPs->Empty();

	
	delete AIPs;

	
	inited = false;
}















AIP * AIP_Manager::Look_Up_New_AIP
(
	char *new_AIP_name					
)
{
	
	int i;
	AIP * an_AIP;						
	

	
	for (i = 0; i < AIPs->count; i++)
	{
		
		an_AIP = AIPs->Return_Data_By_Number(i);

		
		if (!strcmp(new_AIP_name, an_AIP->name))
			return an_AIP;

	} 

	
	return NULL;
}





