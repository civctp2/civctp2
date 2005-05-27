






#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __AGENT_H__
#define __AGENT_H__ 1

#include "c3.h"

#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "scheduler_types.h"
#include "squad_Strength.h"



class Agent { 

public:
	
	
	
	
	

  	
  	
    
	
  	

	
	
	

  	
	Agent();

	
	Agent(const Agent &agent);
	
  	
  	virtual ~Agent();

	
	
	virtual void Init();

	
	virtual Agent& operator= (const Agent &agent);

	
    sint16 Get_Type() const; 

	
    void Set_Type(const sint16 & type);

	
    SQUAD_CLASS Get_Squad_Class() const; 

	
	bool Get_Is_Used() const;

	
	void Set_Is_Used(const bool & is_used);

	
	virtual bool Get_Is_Dead() const;

	
    void Set_Squad_Class(const SQUAD_CLASS & squad_class);

	
	virtual SQUAD_CLASS Compute_Squad_Class() = 0;

	
    virtual const Squad_Strength & Get_Squad_Strength() const;

	
	virtual const Squad_Strength & Compute_Squad_Strength() = 0;

	
	virtual void Set_Can_Be_Executed(const bool &can_be_executed);

	
	bool Get_Can_Be_Executed() const;

	
	void Set_Detached(const bool detached);

	
	bool Get_Detached() const;

    
    
    

	
    virtual void Log_Debug_Info(const int & log) const = 0;

protected:

	
	
	
	
	

	
	SQUAD_CLASS m_squad_class;
    
    
    sint16 m_agent_type; 

	
	bool m_is_used;

	
	Squad_Strength m_squad_strength;

	
	bool m_can_be_executed;

	
	bool m_detached;
};

#endif // __AGENT_H__
