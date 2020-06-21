//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Animation handling
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
// __SPRITETEST__
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fixed Destructor to fix some memory leaks, by Martin Gühmann.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Anim.h"

#include <algorithm> // std::copy

static const int k_FAKE_DEATH_FRAMES   = 15;
static const int k_FAKE_DEATH_DURATION = 1500;

Anim * Anim::MakeFakeDeath()
{
	Anim * animation = new Anim();

	animation->m_frames = new uint16[k_FAKE_DEATH_FRAMES];
	std::fill(animation->m_frames, animation->m_frames + k_FAKE_DEATH_FRAMES, 0);
	animation->m_numFrames = k_FAKE_DEATH_FRAMES;

	POINT point = {0, 0};
	animation->m_moveDeltas = new POINT[k_FAKE_DEATH_FRAMES];
	std::fill(animation->m_moveDeltas, animation->m_moveDeltas + k_FAKE_DEATH_FRAMES, point);

	animation->m_transparencies = new uint16[k_FAKE_DEATH_FRAMES];
	for (int i = 0; i < k_FAKE_DEATH_FRAMES; ++i) {
		animation->m_transparencies[i] = (uint16) (k_FAKE_DEATH_FRAMES - i);
	}

	animation->m_playbackTime = k_FAKE_DEATH_DURATION;
	animation->m_type = ANIMTYPE_SEQUENTIAL;

	return animation;
}

static const int k_FACEOFF_FRAMES   = 1;
static const int k_FACEOFF_DURATION = 1000;

Anim * Anim::MakeFaceoff()
{
	Anim * animation = new Anim();
	animation->m_frames = new uint16[k_FACEOFF_FRAMES];
	std::fill(animation->m_frames, animation->m_frames + k_FACEOFF_FRAMES, 0);

	POINT point = {0, 0};
	animation->m_moveDeltas = new POINT[k_FACEOFF_FRAMES];
	std::fill(animation->m_moveDeltas, animation->m_moveDeltas + k_FACEOFF_FRAMES, point);

	animation->m_transparencies = new uint16[k_FACEOFF_FRAMES];
	std::fill(animation->m_transparencies, animation->m_transparencies + k_FACEOFF_FRAMES, 15);

	animation->m_numFrames    = k_FACEOFF_FRAMES;
	animation->m_playbackTime = k_FACEOFF_DURATION;
	animation->m_type = ANIMTYPE_LOOPED;

	return animation;
}

Anim * Anim::CreateFromTokens(Token * tokens)
{
	Anim * animation = new Anim();
	if (!animation->ParseFromTokens(tokens)) {
		delete animation;
		animation = NULL;
	}
	return animation;
}

Anim * Anim::CopySetType(Anim const & copy, ANIMTYPE type)
{
	Anim * animation = new Anim(copy);
	animation->m_type = type;
	return animation;
}

Anim::Anim()
:
	m_type           (ANIMTYPE_SEQUENTIAL),
	m_numFrames      (0),
	m_frames         (NULL),
	m_moveDeltas     (NULL),
	m_transparencies (NULL),
	m_animPos        (0),
	m_playbackTime   (0),
	m_delay          (0),
	m_finished       (false)
{
}

Anim::Anim(Anim const & copy)
:
	m_type           (copy.m_type),
	m_numFrames      (copy.m_numFrames),
	m_frames         (NULL),
	m_moveDeltas     (NULL),
	m_transparencies (NULL),
	m_animPos        (copy.m_animPos),
	m_playbackTime   (copy.m_playbackTime),
	m_delay          (copy.m_delay),
	m_finished       (copy.m_finished)
{
	if (m_numFrames > 0)
	{
		m_frames         = new uint16[m_numFrames];
		m_transparencies = new uint16[m_numFrames];
		m_moveDeltas     = new POINT [m_numFrames];

		std::copy(copy.m_frames, copy.m_frames + m_numFrames, m_frames);
		std::copy(copy.m_transparencies, copy.m_transparencies + m_numFrames, m_transparencies);
		std::copy(copy.m_moveDeltas, copy.m_moveDeltas + m_numFrames, m_moveDeltas);
	}
}

Anim::~Anim()
{
	delete [] m_frames;
	delete [] m_moveDeltas;
	delete [] m_transparencies;
}

uint16 Anim::GetCurrentFrame() const
{
	if (m_numFrames == 0) {
		return 0;
	}

	Assert(m_animPos < m_numFrames);
	return m_frames[m_animPos];
}

uint16 Anim::GetCurrentTransparency() const
{
	if (m_numFrames == 0) {
		return 0;
	}

	Assert(m_animPos < m_numFrames);
	return m_transparencies[m_animPos];
}

void Anim::Process()
{
	if (m_finished) {
		return;
	}

	switch (m_type)
	{
		case ANIMTYPE_SEQUENTIAL:
			m_animPos++;

			if (m_animPos >= m_numFrames)
			{
				m_animPos  = m_numFrames-1;
				m_finished = true;
			}
			break;

		case ANIMTYPE_LOOPED:
			m_animPos++;

			if (m_animPos >= m_numFrames) {
				m_animPos = 0;
			}
			break;

		case ANIMTYPE_IDLE:
			m_animPos = m_numFrames-1;
			break;
	}
}

void Anim::Rewind()
{
	m_animPos  = 0;
	m_finished = false;
}

bool Anim::ParseFromTokens(Token *tokens)
{
	sint32 tmp;
	sint32 i;

	if (!token_ParseValNext(tokens, TOKEN_ANIM, tmp)) return false;
	if (tmp == 0) return false;

	if (!token_ParseAnOpenBraceNext(tokens)) return false;

	if (!token_ParseValNext(tokens, TOKEN_ANIM_TYPE, tmp)) return false;
	m_type = (ANIMTYPE)tmp;

	if (!token_ParseValNext(tokens, TOKEN_ANIM_NUM_FRAMES, tmp)) return false;
	m_numFrames = (uint16)tmp;

	if (!token_ParseValNext(tokens, TOKEN_ANIM_PLAYBACK_TIME, tmp)) return false;
	m_playbackTime = (uint16)tmp;

	if (!token_ParseValNext(tokens, TOKEN_ANIM_DELAY, tmp)) return false;
	m_delay = (uint16)tmp;

	m_frames = new uint16[m_numFrames];
	if (!token_ParseKeywordNext(tokens, TOKEN_ANIM_FRAME_DATA)) return false;
	for (i=0; i<m_numFrames; i++)
	{
		if (tokens->Next() == TOKEN_NUMBER) tokens->GetNumber(tmp);
		else return false;
		m_frames[i] = (uint16)tmp;
	}

	m_moveDeltas = new POINT[m_numFrames];
	for (i=0; i<m_numFrames; i++)
	{
		POINT p = {0,0};
		m_moveDeltas[i] = p;
	}
	if (!token_ParseValNext(tokens, TOKEN_ANIM_MOVE_DELTAS, tmp)) return false;
	if (tmp)
	{
		if (!token_ParseAnOpenBraceNext(tokens)) return false;
		for (i=0; i<m_numFrames; i++)
		{
			POINT p;

			if (tokens->Next() == TOKEN_NUMBER) tokens->GetNumber(tmp);
			else return false;

			p.x = tmp;

			if (tokens->Next() == TOKEN_NUMBER) tokens->GetNumber(tmp);
			else return false;

			p.y = tmp;

			m_moveDeltas[i] = p;
		}
		if (!token_ParseAnCloseBraceNext(tokens)) return false;
	}

	m_transparencies = new uint16[m_numFrames];
	for (i=0; i<m_numFrames; i++)
	{
		m_transparencies[i] = 15;
	}
	if (!token_ParseValNext(tokens, TOKEN_ANIM_TRANSPARENCIES, tmp)) return false;
	if (tmp)
	{
		if (!token_ParseAnOpenBraceNext(tokens)) return false;
		for (i=0; i<m_numFrames; i++)
		{
			if (tokens->Next() == TOKEN_NUMBER) tokens->GetNumber(tmp);
			else return false;
			m_transparencies[i] = (uint16)tmp;
		}
		if (!token_ParseAnCloseBraceNext(tokens)) return false;
	}

	return token_ParseAnCloseBraceNext(tokens);
}

void Anim::Export(FILE * file)
{
	extern TokenData g_allTokens[];

	fprintf(file, "\t%s\t1\n", g_allTokens[TOKEN_ANIM].keyword);

	fprintf(file, "\t{\n");

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_ANIM_TYPE].keyword, m_type);

	fprintf(file, "\t\t%s\t%d\n", g_allTokens[TOKEN_ANIM_NUM_FRAMES].keyword, m_numFrames);

	fprintf(file, "\t\t%s\t%ld\n", g_allTokens[TOKEN_ANIM_PLAYBACK_TIME].keyword, m_playbackTime);

	fprintf(file, "\t\t%s\t%ld\n", g_allTokens[TOKEN_ANIM_DELAY].keyword, m_delay);

	fprintf(file, "\t\t%s", g_allTokens[TOKEN_ANIM_FRAME_DATA].keyword);
	for (int i = 0; i < m_numFrames; i++)
	{
		if (i%5 == 0) {
			fprintf(file, "\n\t\t\t");
		}
		fprintf(file, "%d\t", m_frames[i]);
	}
	fprintf(file, "\n\n");

	fprintf(file, "\t\t%s\t1\n\t\t{\n", g_allTokens[TOKEN_ANIM_MOVE_DELTAS].keyword);
	for (int i = 0; i < m_numFrames; i++)
	{
		fprintf(file, "\t\t\t%d %d\n", m_moveDeltas[i].x, m_moveDeltas[i].y);
	}
	fprintf(file, "\n\t\t}\n");

	fprintf(file, "\t\t%s\t1\n\t\t{", g_allTokens[TOKEN_ANIM_TRANSPARENCIES].keyword);
	for (int i = 0; i < m_numFrames; i++)
	{
		if (i%5 == 0) {
			fprintf(file, "\n\t\t\t");
		}
		fprintf(file, "%d\t", m_transparencies[i]);
	}
	fprintf(file, "\n\t\t}\n\n");

	fprintf(file, "\t}\n\n");
}
