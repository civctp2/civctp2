#include "c3.h"
#include "fliif.h"
#include "globals.h"
#include "ic3player.h"
#include "FliAction.h"
#include "globals.h"
#include "aimain.h"
#include "FliEngine.h"
#include "pointerlist.h"
#include "FliSymbol.h"
#include "common.h"
#include "linked_list.h"
#include "semi_dynamic_array.h"
#include "sorted_array.h"
#include "planner.h"
#include "AIP_Manager.h"

extern AIP_Manager * g_AIP_manager;

FliAction::FliAction(const char *name, FliEngine *engine)
{
	m_engine = engine;
	if(!stricmp(name, "load")) {
		m_action = FLI_ACTION_LOAD_AIP;
	} else {
		m_engine->ReportError("FliAction: %s is an unknown action", name);
	}
	m_arguments = new PointerList<char>;
}

FliAction::~FliAction()
{
#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

	if(m_arguments) {
		char *str;
		while((str = m_arguments->RemoveHead()) != NULL) {
Assert(++finite_loop < 100000);
			delete [] str;
            str = NULL;
		}
		delete m_arguments;
        m_arguments = NULL;
	}
}

void FliAction::AddArgument(const char *str)
{
	char *arg = new char[strlen(str) + 1];
	strcpy(arg, str);
	m_arguments->AddTail(arg);
}

void FliAction::SetBoolExp(FliSymbol *sym, BFLOP_TYPE op, double value)
{
	m_symbol = sym;
	m_op = op;
	m_value = value;
}

void FliAction::Evaluate(AiMain *ai)
{
	switch(m_op) {
		case BFLOP_LT:
			if(m_symbol->GetValue() < m_value)
				Execute(ai);
			break;
		case BFLOP_GT:
			if(m_symbol->GetValue() > m_value)
				Execute(ai);
			break;
		case BFLOP_EQ:
			if(m_symbol->GetValue() == m_value)
				Execute(ai);
			break;
	}
}

void FliAction::Execute(AiMain *ai)
{
	switch(m_action) {
		case FLI_ACTION_LOAD_AIP:

			g_AIP_manager->Update_AIP(ai, &(ai->m_planner->the_AIP), m_arguments->GetHead());
			break;
		default:
			Assert(FALSE);
			break;
	}
}
