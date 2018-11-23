#ifndef MAP_COPY_BUFFER_H__
#define MAP_COPY_BUFFER_H__

class Cell;
class MapPoint;

struct CellInfo {
	uint8 m_terrain;
	uint32 m_env;
};

class MapCopyBuffer {
  private:
	sint32 m_width, m_height;
	CellInfo **m_cells;

  public:
	MapCopyBuffer();
	~MapCopyBuffer();

	void SetSize(sint32 w, sint32 h);
	void Copy(MapPoint &pos, sint32 w, sint32 h);
	void Paste(MapPoint &pos);

	void Load(const MBCHAR *fileName);
	void Save(const MBCHAR *filename);

};

#endif
