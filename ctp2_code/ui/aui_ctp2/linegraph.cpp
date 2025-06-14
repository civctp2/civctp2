//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Line graph
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
// - Repaired memory leaks.
// - Resolved ambigious calls of std::max.
// - Removed all warnings on .NET
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
// - Moved graph functionality from other places (30-Sep-2007 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "linegraph.h"

#include <memory>

#include "aui.h"
#include "aui_ldl.h"
#include "aui_Factory.h"
#include "aui_stringtable.h"
#include "aui_window.h"

#include "c3ui.h"
#include "c3blitter.h"

#include "pixelutils.h"
#include "colorset.h"               // g_colorSet
#include "gstypes.h"                // PLAYER_INDEX_VANDALS
#include "player.h"                 // k_MAX_PLAYERS
#include "primitives.h"
#include "textutils.h"
#include "TurnCnt.h"                // g_turn

#include "EventTracker.h"
#include "Strengths.h"

extern C3UI                    *g_c3ui;
extern PointerList<Player>     *g_deadPlayer;

static sint32                   s_minRound = 0;

namespace
{

/// Evaluate a combined "strength" value at a given turn
/// @param  a_Strengths Information per category and turn
/// @param  a_Turn      The turn
/// @param  a_Category  A strength category
/// @return The combined "strength" value
sint32 GetCombinedStrength(Strengths const & a_Strengths, sint32 a_Turn, sint32 a_Category)
{
	switch(a_Category)
	{
		case kRankingScientific:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_KNOWLEDGE,    a_Turn);
		}
		case kRankingMilitary:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_MILITARY,     a_Turn);
		}
		case kRankingPollution:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_POLLUTION,    a_Turn);
		}
		case kRankingTrade:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_TRADE,        a_Turn);
		}
		case kRankingGold:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_GOLD,         a_Turn);
		}
/*		case kRankingPopulation:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_POPULATION,   a_Turn);
		}*/
		case kRankingCities:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_CITIES,       a_Turn);
		}
		case kRankingGeographical:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_GEOGRAPHICAL, a_Turn);
		}
		case kRankingSpace:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_SPACE,        a_Turn);
		}
		case kRankingUndersea:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_UNDERSEA,     a_Turn);
		}
		case kRankingUnits:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_UNITS,        a_Turn);
		}
		case kRankingBuildings:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_BUILDINGS,    a_Turn);
		}
		case kRankingWonders:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_WONDERS,      a_Turn);
		}
		case kRankingEconomic:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_GOLD,         a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_BUILDINGS,    a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_WONDERS,      a_Turn);
		}
		case kRankingProduction:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_PRODUCTION,   a_Turn);
		}
		case kRankingOverall:
		{
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_MILITARY,     a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_GOLD,         a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_BUILDINGS,    a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_WONDERS,      a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_PRODUCTION,   a_Turn);
		}
		default:
		{
			Assert(false);
			return a_Strengths.GetTurnStrength(STRENGTH_CAT_UNITS,        a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_GOLD,         a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_BUILDINGS,    a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_WONDERS,      a_Turn)
			     + a_Strengths.GetTurnStrength(STRENGTH_CAT_PRODUCTION,   a_Turn);
		}
	}
}

} // namespace

LineGraph::LineGraph
(
	AUI_ERRCODE *           retval,
	sint32                  id,
	const MBCHAR *          ldlBlock,
	ControlActionCallback * ActionFunc,
	Cookie                  cookie,
	EventTracker *          events
)
:
    aui_Control         (retval, id, ldlBlock, ActionFunc, cookie),
	m_xmin              (0.0),
	m_xmax              (0.0),
	m_ymin              (0.0),
	m_ymax              (0.0),
	m_numLines          (0),
	m_numSamples        (0),
	m_lineData          (NULL),
	m_hasIndicator      (false),
	m_indicatorValue    (0.0),
	m_xAxisName         (NULL),
	m_yAxisName         (NULL),
    m_surface           (NULL),
//  RECT m_graphRect
//  RECT m_surfaceRect
	m_data              (NULL),
	m_enableXLabel      (true),
	m_enableXNumber     (true),
	m_enableYLabel      (true),
	m_enableYNumber     (true),
	m_enablePrecision   (true),
    m_graphType         (events ? GRAPH_TYPE_ZEROSUM : GRAPH_TYPE_LINE),
    m_events            (events)
{
    InitCommon();
}

LineGraph::LineGraph
(
    AUI_ERRCODE *           retval,
	uint32                  id,
	sint32                  x,
	sint32                  y,
	sint32                  width,
	sint32                  height,
	ControlActionCallback * ActionFunc,
	Cookie                  cookie
)
:
    aui_Control         (retval, id, x, y, width, height, ActionFunc, cookie),
	m_xmin              (0.0),
	m_xmax              (0.0),
	m_ymin              (0.0),
	m_ymax              (0.0),
	m_numLines          (0),
	m_numSamples        (0),
	m_lineData          (NULL),
	m_hasIndicator      (false),
	m_indicatorValue    (0.0),
	m_xAxisName         (NULL),
	m_yAxisName         (NULL),
    m_surface           (NULL),
//  RECT m_graphRect
//  RECT m_surfaceRect
	m_data              (NULL),
	m_enableXLabel      (true),
	m_enableXNumber     (true),
	m_enableYLabel      (true),
	m_enableYNumber     (true),
	m_enablePrecision   (true),
    m_graphType         (GRAPH_TYPE_LINE)
{
	InitCommon();
}

//----------------------------------------------------------------------------
//
// Name       : LineGraph::~LineGraph
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
LineGraph::~LineGraph()
{
	delete [] m_xAxisName;
	delete [] m_yAxisName;
	delete m_surface;
	delete [] m_data;

	if (m_lineData)
	{
		for (sint32 i = 0; i < m_numLines * 3; i++)
		{
			delete [] m_lineData[i];
		}
		delete [] m_lineData;
	}
}

void LineGraph::InitCommon(void)
{
	AUI_ERRCODE			errcode = AUI_ERRCODE_OK;

	m_surface = aui_Factory::new_Surface(errcode, m_width, m_height);
	Assert( AUI_NEWOK(m_surface, errcode) );

	SetRect(&m_surfaceRect, 0, 0, m_width, m_height);

	m_graphRect = m_surfaceRect;
	InflateRect(&m_graphRect, -10, -10);
	m_graphRect.bottom -= (m_events)?40:20;
	m_graphRect.left += 20;

	m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE;
}

AUI_ERRCODE LineGraph::DrawThis(aui_Surface *surface, sint32 x,	sint32 y )
{
	if ( !m_draw )
		m_draw = k_AUI_REGION_DRAWFLAG_UPDATE;

	if (!(m_draw & k_AUI_REGION_DRAWFLAG_UPDATE)) return AUI_ERRCODE_OK;

	if ( !surface )
		surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	UpdateGraph(surface, rect.left, rect.top);

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

void LineGraph::UpdateGraph(aui_Surface * surface, sint32 x, sint32 y)
{
	RECT		rect = {0, 0, m_width, m_height};

	g_c3ui->TheBlitter()->Blt(surface, x, y, m_surface, &rect, k_AUI_BLITTER_FLAG_COPY);
}

void LineGraph::LabelAxes(void)
{
	MBCHAR		s[80];

	if (m_enableXNumber)
	{
		if (m_enablePrecision) sprintf(s, "%#.3f", m_xmin);
		else sprintf(s, "%d", (sint32)m_xmin);

		primitives_DrawText(m_surface, m_graphRect.left, m_graphRect.bottom + (m_events?20:0),
								s, g_colorSet->GetColorRef(COLOR_WHITE), TRUE);

		if (m_enablePrecision) sprintf(s, "%#.3f", m_xmax);
		else sprintf(s, "%d", (sint32)m_xmax);

		primitives_DrawText(m_surface, std::max(0L, m_graphRect.right-35L), m_graphRect.bottom + (m_events?20:0),
								s, g_colorSet->GetColorRef(COLOR_WHITE), TRUE);
	}

	if (m_enableXLabel)
	{
		sint32 len = textutils_GetWidth(m_surface, m_xAxisName);
		sint32 xpos = m_graphRect.left + (m_graphRect.right - m_graphRect.left) / 2 - len / 2;
		primitives_DrawText(m_surface, xpos, m_graphRect.bottom + 2 + (m_events?20:0), m_xAxisName,	g_colorSet->GetColorRef(COLOR_WHITE), TRUE);
	}

	if (m_enableYNumber)
	{
		if (m_enablePrecision) sprintf(s, "%#.1f", m_ymin);
		else sprintf(s, "%d", (sint32)m_ymin);

        primitives_DrawText(m_surface,
                            std::max(0L, m_graphRect.left-45L),
                            std::max(0L, m_graphRect.bottom-15L),
							s,
                            g_colorSet->GetColorRef(COLOR_WHITE),
                            TRUE
                           );

		if (m_enablePrecision) sprintf(s, "%#.1f", m_ymax);
		else sprintf(s, "%d", (sint32)m_ymax);

        primitives_DrawText(m_surface,
                            std::max(0L, m_graphRect.left-45L),
                            m_graphRect.top,
							s,
                            g_colorSet->GetColorRef(COLOR_WHITE),
                            TRUE
                           );
	}
}

void LineGraph::DrawIndicator(void)
{
	sint32 xpos;
	MBCHAR s[80];

    double xnorm = (m_indicatorValue - m_xmin) / (m_xmax - m_xmin);
	xpos = sint32(m_graphRect.left + xnorm * (m_graphRect.right-m_graphRect.left));

	primitives_DrawLine16(m_surface, xpos, m_graphRect.top + 1,
							xpos, m_graphRect.bottom - 1,
							g_colorSet->GetColor(COLOR_YELLOW));

	sprintf(s, "%#.3f", m_indicatorValue);
	sint32 len = textutils_GetWidth(m_surface, s);
	primitives_DrawText(m_surface, xpos - (len/2), m_graphRect.top+15, s, g_colorSet->GetColorRef(COLOR_YELLOW), TRUE);
}

void LineGraph::DrawLines(int eventsOfset)
{
	sint32		color = (sint32)COLOR_RED;
	sint32		i, j;
	sint32		xpos = 0, ypos = 0, oldxpos = 0, oldypos = 0;
	double		point;

    Assert(m_ymin <= m_ymax);
	if(m_graphType == GRAPH_TYPE_LINE)
	{
		for (i=0; i<m_numLines; i++) {
			bool first = true;
			for (j=0; j<m_numSamples; j++)
            {
                point = std::min(m_ymax, m_lineData[i][j]);
                point = std::max(m_ymin, point);

				sint32 num = m_numSamples-1;
				if (num == 0) num = 1;

				xpos = m_graphRect.left + ((m_graphRect.right-m_graphRect.left) * j) / num;
				ypos = (sint32)(m_graphRect.bottom - ((point-m_ymin) / (m_ymax - m_ymin)) * (m_graphRect.bottom - m_graphRect.top));

                sint32  l_Color = (m_data) ? m_data[i].color : color;

                if (first)
                {
					primitives_DrawLine16(m_surface, xpos, ypos, xpos, ypos, g_colorSet->GetColor((COLOR) l_Color));
					first = false;
				}
                else
                {
					primitives_DrawLine16(m_surface, oldxpos, oldypos, xpos, ypos, g_colorSet->GetColor((COLOR) l_Color));
				}

                oldxpos = xpos;
				oldypos = ypos;
			}

			color++;
		}
	}
	else if(m_graphType==GRAPH_TYPE_ZEROSUM)
	{
		double		top,bottom;
		sint32		width,height,ypos2;

		width=m_graphRect.right-m_graphRect.left;
		height=m_graphRect.bottom-m_graphRect.top;
		for (i=0; i<m_numLines; i++)
		{
			for(j=0; j<width; j++)
			{

				bottom = m_data[i].bottomArray[j*m_numSamples/width];
				top = m_data[i].topArray[j*m_numSamples/width];

				sint32 num = m_numSamples-1;
				if (num == 0) num = 1;

				xpos = m_graphRect.left + j;
				ypos = (sint32)(m_graphRect.top + top * height);
				ypos2 = (sint32)(m_graphRect.top + bottom * height);

				primitives_DrawLine16(m_surface, xpos, ypos, xpos, ypos2, g_colorSet->GetColor((COLOR)m_data[i].color));
			}

			color++;
		}

		if (m_events)
		{
			int currentEventNum = 0;
			for
            (
                EventData * curData = m_events->GetEvents(true);
                curData;
                curData = m_events->GetEvents(false)
            )
			{
				xpos=m_graphRect.left+(curData->m_turn-1)*width/m_numSamples;
				ypos=((sint32)((m_data[curData->m_playerNum-1].bottomArray[curData->m_turn-1] +
					m_data[curData->m_playerNum-1].topArray[curData->m_turn-1])/2.0)*height)+
					m_graphRect.top;
				primitives_DrawLine16(m_surface, xpos-1, ypos-1, xpos+1, ypos+1, g_colorSet->GetColor((COLOR)(m_data[curData->m_playerNum].color+1)));
				primitives_DrawLine16(m_surface, xpos+1, ypos-1, xpos-1, ypos+1, g_colorSet->GetColor((COLOR)(m_data[curData->m_playerNum].color+1)));
				if((currentEventNum-eventsOfset)>=0 && (currentEventNum-eventsOfset)<=17)
				{
					primitives_DrawLine16(m_surface, xpos, ypos, m_graphRect.left+18+(currentEventNum-eventsOfset)*20, m_graphRect.bottom, g_colorSet->GetColor(COLOR_GRAY));
				}
				currentEventNum++;
			}
		}
	}
	else
	{
		Assert(false);
	}
}

void LineGraph::RenderGraph(int eventsOfset)
{
	primitives_PaintRect16(m_surface, &m_surfaceRect, g_colorSet->GetColor(COLOR_BLACK));
	primitives_FrameRect16(m_surface, &m_surfaceRect, g_colorSet->GetColor(COLOR_WHITE));
	primitives_FrameRect16(m_surface, &m_graphRect, g_colorSet->GetColor(COLOR_WHITE));

	DrawThisStateImage(
		0,
		m_surface,
		&m_surfaceRect );

	LabelAxes();
	DrawLines(eventsOfset);
	if (m_hasIndicator) DrawIndicator();
}

void LineGraph::SetLineData(sint32 numLines, sint32 numSamples, double **data, sint32 *color)
{
	sint32		i,j;
	double		sum,curYPos;

	if (m_lineData)
	{
		for (i = 0; i < (m_numLines * 3); i++)
		{
			delete [] m_lineData[i];
		}
		delete [] m_lineData;
	}
	m_lineData  = new double *[numLines*3];
	m_numLines	= numLines;

	delete [] m_data;
	m_data = new LineGraphData[numLines];

	sint32 defaultColor = (sint32)COLOR_RED;

	for (i=0; i<numLines; i++) {
		m_lineData[i] = new double[numSamples];
		m_lineData[i+numLines] = new double[numSamples];
		m_lineData[i+numLines*2] = new double[numSamples];

		m_data[i].array = m_lineData[i];
		m_data[i].topArray = m_lineData[i+numLines];
		m_data[i].bottomArray = m_lineData[i+numLines*2];
		m_data[i].index = i;
		if (color) m_data[i].color = color[i];
		else m_data[i].color = defaultColor++;

		memcpy(m_lineData[i], data[i], sizeof(double) * numSamples);
	}

	for(i=0; i<numSamples; i++)
	{
		sum=0.0;
		for(j=0; j<numLines; j++)
		{
			sum+=m_data[j].array[i]+1.0;
		}
		curYPos=0.0;
		for(j=0; j<numLines; j++)
		{
			m_data[j].bottomArray[i]=curYPos;
			if(j==(numLines-1))
			{
				curYPos=m_data[j].topArray[i]=1.0;
			}
			else
			{
				curYPos=m_data[j].topArray[i]=curYPos+((m_data[j].array[i]+1.0)/sum);
			}
		}
	}
	m_numSamples = numSamples;
}

void LineGraph::SetGraphBounds(double minx, double maxx, double miny, double maxy)
{
	m_xmin = minx;
	m_xmax = maxx;
	m_ymin = miny;
	m_ymax = maxy;
}

void LineGraph::SetXAxisName(const MBCHAR *name)
{
	Assert(name);
	if (!name) return;

	delete [] m_xAxisName;
	m_xAxisName = new MBCHAR[strlen(name)+1];
	strcpy(m_xAxisName, name);
}

void LineGraph::SetYAxisName(const MBCHAR *name)
{
	Assert(name);
	if (!name) return;

	delete[] m_yAxisName;
	m_yAxisName = new MBCHAR[strlen(name)+1];
	strcpy(m_yAxisName, name);
}

void LineGraph::GenrateGraph(sint32     &infoXCount,
                             sint32     &infoYCount,
                             double ***  infoGraphData,
                             sint32      category)
{
	infoYCount = 0;
	infoXCount = 0;

	AUI_ERRCODE                     errcode     = AUI_ERRCODE_OK;
	std::unique_ptr<aui_StringTable>  stringTable (new aui_StringTable(&errcode, "InfoStrings"));

	SetXAxisName(stringTable->GetString(6));
	SetYAxisName("Power");

	double minRound = s_minRound;
	double curRound = g_turn->GetRound();
	double minPower = 0;
	double maxPower = 10;

	SetGraphBounds(minRound, curRound, minPower, maxPower);
	HasIndicator(false);

	sint32 i;
	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			infoYCount++;
		}
	}

	sint32* color = new sint32[infoYCount + g_deadPlayer->GetCount()];

	infoYCount = 0;

	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			color[infoYCount++] = g_colorSet->ComputePlayerColor(i);
		}
	}

	for
	(
	    PointerList<Player>::Walker walk(g_deadPlayer);
	    walk.IsValid();
	    walk.Next()
	)
	{
		color[infoYCount++] = g_colorSet->ComputePlayerColor(walk.GetObj()->GetOwner());
	}

	infoXCount = static_cast<sint32>(curRound) - static_cast<sint32>(minRound);
	if (infoXCount == 0)
	{
		RenderGraph();
		return;
	}

	infoXCount = std::max<sint32>(1, infoXCount);
	infoYCount = std::max<sint32>(1, infoYCount);

	Assert(!*infoGraphData);
	*infoGraphData = new double *[infoYCount];

	for (i = 0 ; i < infoYCount; i++)
	{
		(*infoGraphData)[i] = new double[infoXCount];
		std::fill((*infoGraphData)[i], (*infoGraphData)[i] + infoXCount, 0.0);
	}

	sint32 playerCount = 0;
	for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
	{
		if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
		{
			for (sint32 round = 0 ; round < infoXCount ; ++round)
			{
				sint32 strValue = GetCombinedStrength(*g_player[i]->m_strengths, round, category);
				(*infoGraphData)[playerCount][round] = strValue;

				while (strValue > maxPower)
					maxPower += 10.0;
			}

			playerCount++;
		}
	}

	for
	(
	    PointerList<Player>::Walker walk2(g_deadPlayer);
	    walk2.IsValid();
	    walk2.Next()
	)
	{
		for (sint32 round = 0 ; round < infoXCount ; ++round)
		{
			sint32 strValue = GetCombinedStrength(*walk2.GetObj()->m_strengths, round, category);
			(*infoGraphData)[playerCount][round] = strValue;

			while (strValue > maxPower)
				maxPower += 10.0;
		}

		playerCount++;
	}

	Assert(playerCount == infoYCount);

	SetLineData(infoYCount, infoXCount, (*infoGraphData), color);
	SetGraphBounds(minRound, curRound, minPower, maxPower);
	RenderGraph();

	delete color;
}
