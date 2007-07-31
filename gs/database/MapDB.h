#pragma once
#ifndef __MAPDB_H__
#define __MAPDB_H__

#include "Rec.h"
#include "DB.h"
#include "MapRecord.h"

class Token;
enum C3DIR;

class MapDatabase : public Database<MapRecord> {
public:
	MapDatabase();
	
	BOOL Initialize(char *filename, C3DIR dir);

	const MapRecord *FindBestMapSizeMatch(sint32 w, sint32 h);
};

extern MapDatabase *g_theMapDB;
#endif
