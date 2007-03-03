

#include "c3.h"
#include "scoretab.h"

#include "aui_ldl.h"
#include "aui_stringtable.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "player.h"
#include "profileDB.h"
#include "Score.h"
#include "SelItem.h"

#include "GameSettings.h"

extern void cpw_NumberToCommas( uint64 number, MBCHAR *s );






ScoreTab::ScoreTab(void) 
:
	m_difficulty        (static_cast<ctp2_Static *>
                            (aui_Ldl::GetObject
                                ("InfoDialog", 
                                 "TabGroup.Tab1.TabPanel.Difficulty.Value"
                                )
                            )
                        ),
	m_rank              (static_cast<ctp2_Static *>
                            (aui_Ldl::GetObject
                                ("InfoDialog", 
                                 "TabGroup.Tab1.TabPanel.Rank.Value"
                                )
                             )
                        ),
	m_total             (static_cast<ctp2_Static *>
                            (aui_Ldl::GetObject
                                ("InfoDialog", 
                                 "TabGroup.Tab1.TabPanel.Total.Value"
                                )
                            )
                        ),
	m_scoreList         (static_cast<ctp2_ListBox *>
                            (aui_Ldl::GetObject
                                ("InfoDialog", 
                                 "TabGroup.Tab1.TabPanel.List"
                                )
                            )
                        ),
    m_difficultyStrings (NULL)
    
{
    AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_difficultyStrings = new aui_StringTable(&errcode, "strings.difficulty1strings");

    m_scoreList->Clear();
	for (int i = 0; i < SCORE_CAT_MAX; i++)
	{
		m_scoreElem[i] = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScoreElement");
	}
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_TYPE_OF_VICTORY]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_OPPONENTS_CONQUERED]);
	m_scoreList->AddItem((ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScoreElement"));
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_POPULATION]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_RANK]);
	m_scoreList->AddItem((ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScoreElement"));
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_CITIES0TO30]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_CITIES30TO100]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_CITIES100TO500]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_CITIES500PLUS]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_CITIES_RECAPTURED]);
	m_scoreList->AddItem((ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScoreElement"));
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_FEATS]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_WONDERS]);
	m_scoreList->AddItem(m_scoreElem[SCORE_CAT_ADVANCES]);
}

ScoreTab::~ScoreTab()
{
    delete m_difficultyStrings;
}

void ScoreTab::Update(void)
{
	sint32      curPlayer   = g_selected_item->GetVisiblePlayer();
	Player *    pl          = g_player[curPlayer];
	if (!pl) 
    {
		pl = Player::GetDeadPlayer(curPlayer);
	}
	if (!pl)
		return;

	Score *     score       = pl->m_score;
	MBCHAR      commaNumber[80];

	m_difficulty->SetText(m_difficultyStrings->GetString(g_theGameSettings->GetDifficulty()));

	for (int i = 0; i < SCORE_CAT_MAX; i++)
	{
		SCORE_CATEGORY cat = (SCORE_CATEGORY )i;
		((ctp2_Static *)m_scoreElem[i]->GetChildByIndex(1))->SetText(score->GetScoreString(cat));

		((ctp2_Static *)m_scoreElem[i]->GetChildByIndex(2))->SetText(score->GetPartialScoreItemized(cat));

		cpw_NumberToCommas(score->GetPartialScore(cat), commaNumber);
		((ctp2_Static *)m_scoreElem[i]->GetChildByIndex(3))->SetText(commaNumber);
	}
	cpw_NumberToCommas(score->GetTotalScore(), commaNumber);
	m_total->SetText(commaNumber);

	sint32 rank = 1;
	for (int p = 1; p < k_MAX_PLAYERS; p++) 
    {
		if ((p != curPlayer) && g_player[p] && 
            (g_player[p]->m_score->GetTotalScore() > score->GetTotalScore())
           ) 
        {
			rank++;
		}
	}

	char buf[40];
	sprintf(buf, "%d", rank);
	m_rank->SetText(buf);
}
