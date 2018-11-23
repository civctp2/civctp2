#include "c3.h"
#include "SlicConst.h"
#include "SlicEngine.h"
#include "diplomattypes.h"
#include "civarchive.h"

static const char *slic_const_test_names[] = {
	"Continue",
	"GetInput",
	"Stop",
};

void slicconst_Initialize()
{
	sint32 i;
	for(i = 0; i < SLIC_CONST_MAX; i++) {
		g_slicEngine->AddConst(slic_const_test_names[i], i);
	}

	DiplomatTypes::InitializeSlicConsts();
}

void SlicConst::Serialize(CivArchive &archive)
{
	uint16 len;

	if(archive.IsStoring()) {
		len = strlen(m_name) + 1;
		archive << len;
		archive.Store((uint8*)m_name, len);
		archive << m_value;
	} else {
		archive >> len;
		m_name = new char[len];
		archive.Load((uint8*)m_name, len);
		archive >> m_value;
	}
}
