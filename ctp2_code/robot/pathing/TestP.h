#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TEST_PATH__
#define __TEST_PATH__

#include "Astar.h"

class TestPath : public Astar

{

public:

    double EntryCost(const MapPoint pos) { return 1.0 ;}    
    


};

#endif
