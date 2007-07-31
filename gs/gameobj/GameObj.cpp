











#include "c3.h"
#include "c3errors.h"

#include "GameObj.h"
#include "CivArchive.h"

   







GAMEOBJ::GAMEOBJ (const uint32 i) 

{ 
   m_id = i; 
   m_lesser = NULL; 
   m_greater = NULL; 
   m_isFromPool = FALSE;
}

GAMEOBJ::~GAMEOBJ()
{
	if(m_lesser)
		delete m_lesser;
	
	if(m_greater)
		delete m_greater;
}

void GAMEOBJ::operator delete(void *ptr)
{
	GAMEOBJ *obj = (GAMEOBJ *)ptr;
	if(!obj->m_isFromPool)
		::delete ptr;
}

void GAMEOBJ::operator delete(void *ptr, size_t size)
{
	GAMEOBJ *obj = (GAMEOBJ *)ptr;
	if(!obj->m_isFromPool)
		::delete ptr;
}

void GAMEOBJ::operator delete[] (void *ptr, size_t size)
{
	::delete[] (ptr);
}








GameObj * GameObj_Access(
    GameObj *p,   
    const uint32 id) 

{
	
	
	
	
#ifdef _DEBUG
   if (p == NULL) { 
	   DPRINTF(k_DBG_GAMESTATE, ("No such object %lx\n", id));
      c3errors_FatalDialog ("GameObj.cpp", "No such object"); 
      return NULL; 
	}
#endif

	while (id != p->m_id)
	{
		if (id < p->m_id) {       
			p = p->m_lesser;
		} else {
			p = p->m_greater;
		}

#ifdef _DEBUG
		if (p == NULL) { 
			DPRINTF(k_DBG_GAMESTATE, ("No such object %lx\n", id));
			c3errors_FatalDialog ("GameObj.cpp", "No such object"); 
			return NULL; 
		}
#endif
	}
     
 	return p; 
}








const GameObj * GameObj_Get(GameObj *p, const uint32 id) 
{
	
	
	
	
	
#ifdef _DEBUG
	if (p == NULL) { 
		DPRINTF(k_DBG_GAMESTATE, ("No such id %lx\n", id));
		c3errors_FatalDialog ("GameObj.cpp", "No such id %d", id); 
		return NULL; 
	}
#endif 
	
	while (id != p->m_id)
	{

	   if (id < p->m_id) p = p->m_lesser;
	   else p = p->m_greater;
#ifdef _DEBUG
		if (p == NULL) { 
			DPRINTF(k_DBG_GAMESTATE, ("No such id %lx\n", id));
			c3errors_FatalDialog ("GameObj.cpp", "No such id %d", id); 
			return NULL; 
		}
#endif
	} 

#if 0
   
   } else {
      if (id < p->m_id) {       
         return GameObj_Get(p->m_lesser, id); 
      } else if (p->m_id < id) { 
         return GameObj_Get(p->m_greater, id); 
      } else { 
         return p; 
      }
   }
#endif

   return p;
}











const sint32 GameObj_Valid(GameObj *p, const uint32 id) 
{
	
	
 	while (p!=NULL)
	{
		if (id==p->m_id) return TRUE;
		if (id < p->m_id) {
			p = p->m_lesser;
		}
		else  {
			p = p->m_greater;
		}
	}
	return FALSE;	
}








void GameObj_Insert(
   GameObj **p,  
   GameObj *ins) 

{
   if (*p == NULL) { 
      *p = ins; 
   } else {
      if (ins->m_id < (*p)->m_id) {       
         GameObj_Insert (&((*p)->m_lesser), ins); 
      } else { 
         if (ins->m_id == (*p)->m_id) {   
			 DPRINTF(k_DBG_GAMESTATE, ("Insert duplicate %lx", ins->m_id));
           c3errors_FatalDialog ("GameObj_Insert.cpp", "insert duplicate %d ", ins->m_id); 
         } else { 
            GameObj_Insert(&((*p)->m_greater), ins);
         }
      }
   }
}








sint32 GameObj_Delete(GameObj **p, const uint32 id)

{
   if (*p == NULL) {       
	   DPRINTF(k_DBG_GAMESTATE, ("No such object %lx\n", id));
      c3errors_FatalDialog ("GameObj.cpp", "No such id %d", id); 
      return FALSE; 
   } else {
      if (id < (*p)->m_id) {       
         return GameObj_Delete(&((*p)->m_lesser), id); 
      } else if ((*p)->m_id < id) { 
         return GameObj_Delete(&((*p)->m_greater), id); 
      } else { 

         GameObj *tmp = *p; 
         if ((*p)->m_greater == NULL) { 
            *p = (*p)->m_lesser; 
         } else if ((*p)->m_lesser == NULL) { 
            *p = (*p)->m_greater; 
         } else { 
            *p = (*p)->m_lesser; 
            GameObj_Insert(p, tmp->m_greater); 
         } 
		 tmp->m_greater = NULL;
		 tmp->m_lesser = NULL;
         delete tmp; 
         return TRUE;
      }
   }
}












void GameObj::Serialize(CivArchive &archive)
{
	
	



	if (archive.IsStoring()) {
		archive << m_id;		
	} else {
		archive >> m_id;
	}
}














uint32 GameObj_GameObj_GetVersion(void)
	{
	return (k_GAMEOBJ_VERSION_MAJOR<<16 | k_GAMEOBJ_VERSION_MINOR) ;
	}
