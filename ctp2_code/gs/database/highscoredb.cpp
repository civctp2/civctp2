#include "c3.h"
#include "c3errors.h"
#include "c3files.h"

#include "Token.h"
#include "ErrMsg.h"
#include "civarchive.h"
#include "StrDB.h"

#include "highscoredb.h"

extern sint32	g_abort_parse;
extern StringDB	*g_theStringDB;

HighScoreDB::HighScoreDB()
{
	Initialize();
}

HighScoreDB::~HighScoreDB()
{

	SaveHighScores();

	HighScoreInfo *prevScore = m_highScoreInfo;
	HighScoreInfo *nextScore = NULL;

	if (prevScore)
	{
		for (sint32 i = 0; i < m_nHighScores ; i++)
		{
			nextScore = prevScore->m_next;
			delete prevScore;
			prevScore = nextScore;
		}

		Assert( prevScore == NULL );
	}
}

void HighScoreDB::Initialize(void)
{
	m_nHighScores = 0;
	m_highScoreInfo = NULL;
	LoadHighScores();
}

sint32 HighScoreDB::AddHighScore(MBCHAR *name, sint32 score)
{
	HighScoreInfo *nextScore = NULL;
	HighScoreInfo *prevScore = NULL;
	sint32 i = 0;

	HighScoreInfo *newScore = new HighScoreInfo;
	strcpy(newScore->m_name, name);
	newScore->m_score = score;
	newScore->m_next = NULL;

	if (!m_nHighScores)
	{

		m_highScoreInfo = newScore;
		m_nHighScores++;
		return 0;
	}

	if (m_highScoreInfo->m_score < newScore->m_score)
	{
		newScore->m_next = m_highScoreInfo;
		m_highScoreInfo = newScore;
		m_nHighScores++;

		CheckMaxScores();
		return 0;
	}

	nextScore = m_highScoreInfo->m_next;
	prevScore = m_highScoreInfo;
	for ( i = 0 ; i < m_nHighScores ; i++ )
	{

		if (!nextScore)
		{
			prevScore->m_next = newScore;
			m_nHighScores++;

			CheckMaxScores();
			return 0;
		}

		if (nextScore->m_score < newScore->m_score)
		{
			newScore->m_next = nextScore;
			prevScore->m_next = newScore;
			m_nHighScores++;

			CheckMaxScores();
			return 0;
		}
		prevScore = nextScore;
		nextScore = nextScore->m_next;
	}

	return 0;
}

sint32 HighScoreDB::CheckMaxScores( void )
{
	HighScoreInfo *nextScore = m_highScoreInfo;
	HighScoreInfo *prevScore = NULL;

	if (m_nHighScores > k_MAX_HIGH_SCORES)
	{

		Assert(m_nHighScores == k_MAX_HIGH_SCORES+1);
		for (sint32 i = 0 ; i < k_MAX_HIGH_SCORES; i++)
		{
			prevScore = nextScore;
			nextScore = nextScore->m_next;
		}

		prevScore->m_next = NULL;
		delete nextScore;
		nextScore = NULL;
		m_nHighScores--;
	}

	return 0;
}

HighScoreInfo *HighScoreDB::GetHighScoreInfo( sint32 index )
{

	if (index > m_nHighScores) index = m_nHighScores;
	if (index < 0) index = 0;

	if (!m_highScoreInfo) return NULL;

	HighScoreInfo *nextScore = m_highScoreInfo;

	for (sint32 i = 0 ; i < index ; i++)
	{
		nextScore = nextScore->m_next;
	}

	return nextScore;
}


void HighScoreDB::LoadHighScores( void )
{
	MBCHAR strbuf[256];
	MBCHAR scorebuf[256];
	MBCHAR rankname[256];
	sint32 score = 0;
	MBCHAR c = '\0';
	sint32 i = 0;
	sint32 j = 0;

	FILE *fp;

	fp = c3files_fopen(C3DIR_GAMEDATA, "hscore.txt", "r");

	if (!fp) return;

	while(1)
	{
		i = 0;
		j = 0;

		if (!fgets(strbuf, 255, fp)) break;

		while(i < 256)
		{

			c = strbuf[i++];
			if (c == '{')
			{
				while ((i < 256) && (j < 256))
				{

					c = strbuf[i++];
					if (c == '}') break;
					rankname[j++] = c;
				}

				rankname[j] = '\0';

				j = 0;
				while((i < 256) && (c != '\n'))
				{
					c = strbuf[i++];
					scorebuf[j++] = c;
				}
				scorebuf[j] = '\0';
				break;
			}
		}

		if (i == 256) break;

		if (sscanf(scorebuf, "%d", &score) != 1) break;

		AddHighScore(rankname,score);
	}

	fclose(fp);
}

void HighScoreDB::SaveHighScores( void )
{
	FILE *fp;

	fp = c3files_fopen(C3DIR_GAMEDATA, "hscore.txt", "w");

	if (!fp) return;

	HighScoreInfo *nextScore = m_highScoreInfo;

	for ( sint32 i = 0 ; i < m_nHighScores ; i++)
	{

		fprintf(fp, "{%s} %d\n", nextScore->m_name, nextScore->m_score);
		nextScore = nextScore->m_next;
	}

	fclose(fp);
}
