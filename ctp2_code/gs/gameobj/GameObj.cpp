//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The object pool
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected return types
// - Repaired crashes
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GameObj.h"

#include "c3errors.h"
#include "civarchive.h"

GAMEOBJ::GAMEOBJ(uint32 i) 
:
    m_id            (i),
    m_lesser        (NULL),
    m_greater       (NULL),
    m_isFromPool    (FALSE)
{ 
}

GAMEOBJ::~GAMEOBJ()
{
	delete m_lesser;
	delete m_greater;
}

void GAMEOBJ::operator delete(void *ptr)
{
	GAMEOBJ *obj = (GAMEOBJ *)ptr;
	if(!obj->m_isFromPool)
		::delete obj;
}

void GAMEOBJ::operator delete(void *ptr, size_t size)
{
	GAMEOBJ *obj = (GAMEOBJ *)ptr;
	if(!obj->m_isFromPool)
		::delete obj;
}

void GAMEOBJ::operator delete[] (void *ptr, size_t size)
{
	::delete[] ((GAMEOBJ*)ptr);
}

GameObj * GameObj_Access
(
    GameObj *   p,   
    uint32      id
) 
{
	while (p && (id != p->m_id))
	{
		if (id < p->m_id) 
        {       
			p = p->m_lesser;
		} 
        else 
        {
			p = p->m_greater;
		}
	}
     
    if (!p) 
    { 
        DPRINTF(k_DBG_GAMESTATE, ("No such object %lx\n", id));
        c3errors_ErrorDialog("GameObj.cpp", "No such object"); 
    }

 	return p; 
}

GameObj const * GameObj_Get(GameObj *p, uint32 id) 
{
    return GameObj_Access(p, id);	
}











bool GameObj_Valid(GameObj * p, uint32 id) 
{
    for (GameObj * l_p = p; l_p; )
    {
		if (id == l_p->m_id) 
        {
            return true;
        }
        else if (id < l_p->m_id) 
        {
			l_p = l_p->m_lesser;
		}
		else  
        {
			l_p = l_p->m_greater;
		}
	}

	return false;	
}








void GameObj_Insert
(
   GameObj **   p,  
   GameObj *    ins
)
{
    if (*p == NULL) 
    { 
        *p = ins; 
    } 
    else if (ins)
    {
        if (ins->m_id < (*p)->m_id) 
        {       
            GameObj_Insert(&((*p)->m_lesser), ins); 
        } 
        else if ((*p)->m_id < ins->m_id)
        { 
            GameObj_Insert(&((*p)->m_greater), ins);
        }
        else
        {
			DPRINTF(k_DBG_GAMESTATE, ("Insert duplicate %lx", ins->m_id));
            c3errors_ErrorDialog
                ("GameObj_Insert.cpp", "insert duplicate %d ", ins->m_id); 
        }
    }
}








void GameObj_Delete(GameObj **p, uint32 id)
{
   if (*p == NULL) {       
	   DPRINTF(k_DBG_GAMESTATE, ("No such object %lx\n", id));
      c3errors_ErrorDialog ("GameObj.cpp", "No such id %d", id); 
   } else {
      if (id < (*p)->m_id) {       
         GameObj_Delete(&((*p)->m_lesser), id); 
      } else if ((*p)->m_id < id) { 
         GameObj_Delete(&((*p)->m_greater), id); 
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
