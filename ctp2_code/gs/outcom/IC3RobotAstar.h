










#pragma once

#ifndef __IC3ROBOT_ASTAR_H__
#define __IC3ROBOT_ASTAR_H__ 1


#define k_ASTAR_BIG 7654321.0f































 








enum ASTAR_ENTRY_TYPE; 

typedef BOOL RobotPathEval (
     BOOL can_enter,   
                        
                        
                        
    MapPointData *prev,  
    MapPointData *pos,    

    float *cost,       
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        
                        

    BOOL is_zoc,         
                        
                        
	ASTAR_ENTRY_TYPE &entry 
							
							
							
							
    ); 


#include <OBJBASE.H>  

DEFINE_GUID(CLSID_IC3RobotAstar, 
    0x17b66e40,
    0xcf9a,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);

#undef INTERFACE
#define INTERFACE IC3RobotAstar

enum PATH_ARMY_TYPE { 
    PATH_ARMY_TYPE_EXISTS, 
    PATH_ARMY_TYPE_DB, 
    PATH_ARMY_TYPE_MADEUP 
}; 

DECLARE_INTERFACE_(IC3RobotAstar, IUnknown)
{





    STDMETHOD_ (BOOL,  FindPath) (
        RobotPathEval *cb, 
                                
                                
       uint32 army_id, 
       PATH_ARMY_TYPE pat, 
       uint32 army_type, 

       MapPointData *start, 
       MapPointData *dest, 
       sint32 *bufSize,  
	   MapPointData ** buffer, 
                                
                                
                                
                                
   
       sint32 *nPoints, 
       float *total_cost, 
                          
     BOOL made_up_can_space_launch,  
     BOOL made_up_can_space_land,     
     BOOL check_rail_launch,         
                                    
    BOOL pretty_path,               
                                    
                                    
                                    
     sint32 cutoff,                 
                                    
     sint32 &nodes_opened,            
	 BOOL check_dest,			
     BOOL no_straigth_line,          
     const BOOL check_units_in_cell 
    ) PURE;

};

#endif __IC3ROBOT_ASTAR_H__
