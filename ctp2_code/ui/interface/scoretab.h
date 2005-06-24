#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SCORETAB_H__
#define __SCORETAB_H__

#include "Score.h"

class ctp2_Static;
class ctp2_ListBox;
class ctp2_ListItem;

class ScoreTab
{
public:
	ScoreTab(void);

	void Update(void);
private:
	
	ctp2_Static *m_difficulty;
	ctp2_Static *m_rank;
	ctp2_Static *m_total;

	
	ctp2_ListBox *m_scoreList;

	ctp2_ListItem *m_scoreElem[SCORE_CAT_MAX];
};

#endif
