//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : ?
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected wrap handling.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "MapCopyBuffer.h"
#include "MapPoint.h"
#include "Cell.h"
#include "World.h"
#include "CityRadius.h"

MapCopyBuffer::MapCopyBuffer()
{
	m_width = m_height = 0;
	m_cells = NULL;
}

MapCopyBuffer::~MapCopyBuffer()
{
	if(m_cells) {
		sint32 x;
		for(x = 0; x < m_width; x++) {
			delete [] m_cells[x];
		}
		delete [] m_cells;
	}
}

void MapCopyBuffer::SetSize(sint32 w, sint32 h)
{
	sint32 x;
	if(m_cells) {
		for(x = 0; x < m_width; x++) {
			delete [] m_cells[x];
		}
		delete [] m_cells;
	}

	m_width = w;
	m_height = h;

	m_cells = new CellInfo *[m_width];
	for(x = 0; x < m_width; x++) {
		m_cells[x] = new CellInfo[m_height];
	}
}

void MapCopyBuffer::Copy(MapPoint &pos, sint32 w, sint32 h)
{
	if(w == 0 || h == 0) return;

	SetSize(w, h);
	Assert(m_cells);
	if(!m_cells) return;

#if defined(ACTIVISION_ORIGINAL)
	MapPoint cur, wrapped;
	sint32 x, y;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			cur.x = pos.x + x - (y / 2);
			while(cur.x < 0)
				cur.x += (sint16)g_theWorld->GetXWidth();

			cur.y = pos.y + (sint16)y;
			if(!WrapPoint(pos, cur, wrapped))
				continue;

			Cell *cell = g_theWorld->GetCell(wrapped);
			m_cells[x][y].m_terrain = (uint8)cell->GetTerrain();
			m_cells[x][y].m_env = cell->GetEnv();
		}
	}
#else
	// x and y are orthogonal coordinates now
	for (sint32 y = 0; y < h; ++y)
	{
		for (sint32 x = (y & 1); x < (2 * w); x += 2)
		{
			OrthogonalPoint	cur(pos);
			cur.Move(MapPointData((sint16) x, (sint16) y));
			if (cur.IsValid())
			{
				Cell * cell = g_theWorld->GetCell(cur.GetRC());
				m_cells[x][y].m_terrain = (uint8) cell->GetTerrain();
				m_cells[x][y].m_env		= cell->GetEnv();
			}
		}
	}
#endif
}

void MapCopyBuffer::Paste(MapPoint &pos)
{
	Assert(m_cells);
	if(!m_cells) return;

#if defined(ACTIVISION_ORIGINAL)
	MapPoint cur, wrapped;
	sint32 x, y;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			cur.x = pos.x + x - (y / 2);
			while(cur.x < 0)
				cur.x += (sint16)g_theWorld->GetXWidth();

			cur.y = pos.y + (sint16)y;
			if(!WrapPoint(pos, cur, wrapped))
				continue;

			Cell *cell = g_theWorld->GetCell(wrapped);
			
			cell->SetEnv(m_cells[x][y].m_env);
			g_theWorld->SmartSetTerrain(wrapped, m_cells[x][y].m_terrain, 0);
		}
	}
#else
	// x and y are orthogonal coordinates now
	for (sint32 y = 0; y < m_height; ++y)
	{
		for (sint32 x = (y & 1); x < (2 * m_width); x += 2)
		{
			OrthogonalPoint	cur(pos);
			cur.Move(MapPointData((sint16) x, (sint16) y));
			if (cur.IsValid())
			{
				MapPoint	wrapped = cur.GetRC();
				Cell * cell = g_theWorld->GetCell(wrapped);
				cell->SetEnv(m_cells[x][y].m_env);
				g_theWorld->SmartSetTerrain(wrapped, m_cells[x][y].m_terrain, 0);
			}
		}
	}
#endif
}

void MapCopyBuffer::Save(const MBCHAR *fileName)
{
	FILE *f = fopen(fileName, "w");
	Assert(f);
	if(!f) return;

	fprintf(f, "CTP2CLIP\n");
	fprintf(f, "%d,%d\n", m_width, m_height);
	sint32 x, y;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			fwrite(&m_cells[x][y], 1, sizeof(CellInfo), f);
		}
	}
	fclose(f);
}


void MapCopyBuffer::Load(const MBCHAR *filename)
{
	FILE *f = fopen(filename, "r");
	Assert(f);
	if(!f) return;

	MBCHAR line[_MAX_PATH];
	if(fscanf(f, "%s\n", line) != 1) {
		Assert(FALSE);
		fclose(f);
		return;
	}

	Assert(strcmp(line, "CTP2CLIP") == 0);
	if(strcmp(line, "CTP2CLIP") != 0) {
		fclose(f);
		return;
	}

	sint32 w, h;
	if(fscanf(f, "%d,%d\n", &w, &h) != 2) {
		fclose(f);
		return;
	}

	SetSize(w, h);
	
	sint32 x, y;
	size_t r;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			r = fread(&m_cells[x][y], 1, sizeof(CellInfo), f);
			Assert(r == sizeof(CellInfo));
			if(r != sizeof(CellInfo)) {
				fclose(f);
				return;
			}
		}
	}
	fclose(f);
}
