//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - Moved graph functionality from other places (30-Sep-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __LINEGRAPH_H__
#define __LINEGRAPH_H__

enum GRAPH_TYPE
{
	GRAPH_TYPE_NONE=-1,
	GRAPH_TYPE_LINE,
	GRAPH_TYPE_ZEROSUM,
	GRAPH_TYPE_MAX
};

enum eRankingCategories
{
	kRankingScientific,
	kRankingMilitary,
	kRankingPollution,
	kRankingTrade,
	kRankingGold,
	kRankingPopulation,
	kRankingCities,
	kRankingGeographical,
	kRankingSpace,
	kRankingUndersea,
	kRankingUnits,
	kRankingBuildings,
	kRankingWonders,
	kRankingEconomic,
	kRankingProduction,
	kRankingOverall
};

class  LineGraph;
struct LineGraphData;

#include "aui_control.h"
#include "ctp2_inttypes.h"

class aui_EditText;
class aui_Surface;
class EventTracker;

struct LineGraphData
{
	double *    array;
	double *    topArray;
	double *    bottomArray;
	sint32      index;
	sint32      color;
};

class LineGraph : public aui_Control
{
public:

	LineGraph(AUI_ERRCODE *retval, 
	                sint32 id, 
	                MBCHAR *ldlBlock,
	                ControlActionCallback *ActionFunc = NULL,
	                void *cookie = NULL,
	                EventTracker *events = NULL);
	LineGraph(AUI_ERRCODE *retval,
	                uint32 id,
	                sint32 x,
	                sint32 y,
	                sint32 width,
	                sint32 height,
	                ControlActionCallback *ActionFunc = NULL,
	                void *cookie = NULL);
	virtual ~LineGraph();

	void		SetXBounds(double min, double max) { m_xmin = min; m_xmax = max; }
	void		SetYBounds(double min, double max) { m_ymin = min; m_ymax = max; }
	void		SetGraphBounds(double minx, double maxx, double miny, double maxy);

	void		SetLineData(sint32 numLines, sint32 numSamples, double **data, sint32 *color = NULL);

	void		SetIndicator(double val) { m_indicatorValue = val; }
	void		HasIndicator(bool has) { m_hasIndicator = has; }

	void		SetXAxisName(MBCHAR *name);
	void		SetYAxisName(MBCHAR *name);

	void		UpdateGraph(aui_Surface *surface, sint32 x, sint32 y);

	void		LabelAxes(void);
	void		DrawIndicator(void);
	void		DrawLines(int eventsOfset=0);
	void		RenderGraph(int eventsOfset=0);
	void		EnableXLabel( sint32 enable ) { m_enableXLabel = enable; };
	void		EnableYLabel( sint32 enable ) { m_enableYLabel = enable; };
	void		EnableXNumber( sint32 enable ) { m_enableXNumber = enable; };
	void		EnableYNumber( sint32 enable ) { m_enableYNumber = enable; };
	void		EnablePrecision( sint32 enable ) { m_enablePrecision = enable; };

	void		SetGraphType(GRAPH_TYPE graphType) {m_graphType=graphType;};
	void		SetEventTracker(EventTracker *events) 
	{
		m_events = events; 
		if (m_events != NULL) SetGraphType(GRAPH_TYPE_ZEROSUM);
	}

	LineGraphData *GetData(void) { return m_data; }

	virtual AUI_ERRCODE		DrawThis(aui_Surface *surface, sint32 x, sint32 y);

	aui_Surface	*   GetGraphSurface(void) const { return m_surface; }

	void GenrateGraph(sint32     &infoXCount,
	                             sint32     &infoYCount,
	                             double ***  infoGraphData,
	                             sint32      category);

private:
	void		InitCommon(void);
	void		InitCommonLdl(MBCHAR *ldlBlock);

	double			m_xmin;
	double			m_xmax;
	double			m_ymin;
	double			m_ymax;

	sint32			m_numLines;
	sint32			m_numSamples;
	double			**m_lineData;

	bool			m_hasIndicator;
	double			m_indicatorValue;

	MBCHAR			*m_xAxisName;
	MBCHAR			*m_yAxisName;

	aui_Surface *   m_surface;

	RECT			m_graphRect;
	RECT			m_surfaceRect;

	LineGraphData	*m_data;

	sint32			m_enableXLabel;
	sint32			m_enableXNumber;
	sint32			m_enableYLabel;
	sint32			m_enableYNumber;
	sint32			m_enablePrecision;

	GRAPH_TYPE		m_graphType;
	EventTracker	*m_events;
};

#endif
