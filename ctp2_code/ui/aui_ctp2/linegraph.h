

#pragma once

#ifndef __LINEGRAPH_H__
#define __LINEGRAPH_H__

#include "aui_control.h"

class aui_EditText;
class aui_DirectSurface;
class EventTracker;

enum GRAPH_TYPE
{
	GRAPH_TYPE_NONE=-1,
	GRAPH_TYPE_LINE,
	GRAPH_TYPE_ZEROSUM,
	GRAPH_TYPE_MAX
};

typedef struct LINE_GRAPH_DATA {
	double *array;
	double *topArray,*bottomArray;
	sint32 index;
	sint32 color;
} LineGraphData;

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

	void		InitCommonLdl(MBCHAR *ldlBlock);
	void		InitCommon(void);

	void		SetXBounds(double min, double max) { m_xmin = min; m_xmax = max; }
	void		SetYBounds(double min, double max) { m_ymin = min; m_ymax = max; }
	void		SetGraphBounds(double minx, double maxx, double miny, double maxy);

	void		SetLineData(sint32 numLines, sint32 numSamples, double **data, sint32 *color = NULL);

	void		SetIndicator(double val) { m_indicatorValue = val; }
	void		HasIndicator(BOOL has) { m_hasIndicator = has; }

	void		SetXAxisName(MBCHAR *name);
	void		SetYAxisName(MBCHAR *name);

	void		UpdateGraph(aui_DirectSurface *surface, sint32 x, sint32 y);

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

	
	aui_DirectSurface	*GetGraphSurface(void) const { return m_surface; }

private:
	double			m_xmin;
	double			m_xmax;
	double			m_ymin;
	double			m_ymax;

	sint32			m_numLines;
	sint32			m_numSamples;
	double			**m_lineData;

	BOOL			m_hasIndicator;
	double			m_indicatorValue;

	MBCHAR			*m_xAxisName;
	MBCHAR			*m_yAxisName;

	aui_DirectSurface *m_surface;

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