#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SCORETAB_H__
#define __SCORETAB_H__

class ScoreTab;

#include "Score.h"  // SCORE_CAT_MAX
class aui_StringTable;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_Static;

class ScoreTab
{
public:
	ScoreTab(void);
    ~ScoreTab();

	void Update(void);

private:
	ctp2_Static *       m_difficulty;
	ctp2_Static *       m_rank;
	ctp2_Static *       m_total;
	ctp2_ListBox *      m_scoreList;
	ctp2_ListItem *     m_scoreElem[SCORE_CAT_MAX];
    aui_StringTable *   m_difficultyStrings; 
};

#endif
