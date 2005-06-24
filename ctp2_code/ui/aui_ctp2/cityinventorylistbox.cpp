









#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "aui_uniqueid.h"
#include "aui_item.h"

#include "textswitch.h"
#include "texttab.h"
#include "aui_static.h"
#include "statictextitem.h"

#include "StrDB.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "Unit.h"
#include "UnitData.h"
#include "citydata.h"
#include "Advances.h"
#include "Globals.h"

#include "BldQue.h"
#include "ObjPool.h"

#include "controlpanelwindow.h"
#include "c3listbox.h"
#include "cityinventorylistbox.h"
#include "player.h"
#include "debugwindow.h"
#include "UnitRecord.h"
#include "wonderutil.h"

extern	ControlPanelWindow	*g_controlPanel;
extern	Player				**g_player;
extern	DebugWindow			*g_debugWindow;
 

CityInventoryListBox::CityInventoryListBox(AUI_ERRCODE *retval,
			uint32 id,
			MBCHAR *ldlBlock,
			ControlActionCallback *ActionFunc,
			void *cookie )
		:
		C3ListBox(retval, id, ldlBlock, ActionFunc, cookie),
		aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_ImageBase( ldlBlock )
{
	InitCommon(ldlBlock);
}

CityInventoryListBox::CityInventoryListBox (
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc,
		void *cookie):
	C3ListBox(retval, id, x, y, width, height, pattern, ActionFunc, cookie),
	aui_TextBase(NULL),
	aui_ImageBase( (sint32)0 )
{
	InitCommon(NULL);
}

CityInventoryListBox::~CityInventoryListBox()
{
}

AUI_ERRCODE CityInventoryListBox::InitCommon(MBCHAR *ldlBlock)
{
	m_buildMode = 0;
	return AUI_ERRCODE_OK;
}

sint32 CityInventoryListBox::FillInventoryBox(const Unit &unit)
{
	AUI_ERRCODE		errcode;
	aui_Static		*item;
	MBCHAR			str[80];
	sint32			i;

	m_buildMode = 0;

	if (!unit) {
		for (i = NumItems()-1;i >= 0;i--) {
			RemoveItemByIndex(i);
		}
	} else {
		for (i = NumItems()-1;i >= 0;i--) 
			RemoveItemByIndex(i);

		m_unit = &unit;
		
		uint64 improvements = unit.GetImprovements();
		for(i = 0 ; i < g_theBuildingDB->NumRecords(); i++) {
			if(improvements & ((uint64)1 << (uint64)i)) {
				sprintf(str, "%s    %i", g_theStringDB->GetNameStr(g_theBuildingDB->Get(i)->m_name),g_theBuildingDB->Get(i)->GetUpkeep());
				
				item = new StaticTextItem(
					&errcode,

					i,
					5, 0, 100, 15,
					str,
					NULL,
					8,
					i,
					k_GAME_OBJ_TYPE_IMPROVEMENT);
				if (!item) return -i * 100;

				AddItem( (aui_Item *)item );
			}
		}

		
		uint64 wonders = unit.GetData()->GetCityData()->GetBuiltWonders();
		for(i = 0 ; i < g_theWonderDB->NumRecords(); i++) {
			if(wonders & ((uint64)1 << (uint64)i)) {
				sprintf(str, "%s", g_theStringDB->GetNameStr(wonderutil_Get(i)->m_name));

				sint32 j = aui_UniqueId();
				item = new StaticTextItem(
					&errcode,

					j,

					5, 0, 100, 15,
					str,
					NULL,
					8,
					j,
					k_GAME_OBJ_TYPE_WONDER);
				if (!item) return -i * 100;

				AddItem( (aui_Item *)item );
			}
		}




























	}

	return 0;
}

void CityInventoryListBox::UpdateInventoryBox( const Unit &unit )
{
	sint32 i, n;
	Player *p = g_player[unit.GetOwner()];
	sint32 enable;
	MBCHAR str[80];
	StaticTextItem *item;
	AUI_ERRCODE errcode;
	BOOL isObsolete;
	sint32 o;

	m_buildMode = 1;

	n = g_theUnitDB->NumRecords();

	for (i = NumItems()-1;i >= 0;i--) {
		RemoveItemByIndex(i);
	}

	for(i = 0; i < n; i++) {
		const UnitRecord *rec = g_theUnitDB->Get(i);
		enable = rec->GetEnableAdvanceIndex();
		isObsolete = FALSE;
		for(o = 0; o < rec->GetNumObsoleteAdvance(); o++) {
			if(p->m_advances->HasAdvance(rec->GetObsoleteAdvanceIndex(o))) {
				isObsolete = TRUE;
			}
		}
		if(isObsolete)
			continue;
		if((p->m_advances->HasAdvance(enable) || (enable < 0))) {
			sprintf(str, "%s",g_theStringDB->GetNameStr(g_theUnitDB->Get(i)->m_name));
				sint32 j = aui_UniqueId();
				item = new StaticTextItem(
					&errcode,

					i,

					5, 0, 100, 15,
					str,
					NULL,
					8,
					i,
					k_GAME_OBJ_TYPE_UNIT,
					i);
				if (!item) return;

				AddItem( (aui_Item *)item );
		}
	}
	
	n = g_theBuildingDB->NumRecords();
	for(i = 0; i < n; i++) {
		const BuildingRecord *rec = g_theBuildingDB->Get(i);
		enable = rec->GetEnableAdvanceIndex();
		isObsolete = FALSE;
		for(o = 0; o < rec->GetNumObsoleteAdvance(); o++) {
			if(p->m_advances->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
				isObsolete = TRUE;
		}
		if(isObsolete)
			continue;
		if((p->m_advances->HasAdvance(enable) || (enable < 0))) {
			sprintf(str, "%s",g_theStringDB->GetNameStr(g_theBuildingDB->Get(i)->m_name));
				sint32 j = aui_UniqueId();
				item = new StaticTextItem(
					&errcode,

					i,

					5, 0, 100, 15,
					str,
					NULL,
					8,
					i,
					k_GAME_OBJ_TYPE_IMPROVEMENT,
					i);
				if (!item) return;

				AddItem( (aui_Item *)item );
		}
	}

	n = g_theWonderDB->NumRecords();
	for(i = 0; i < n; i++) {
		const WonderRecord *rec = wonderutil_Get(i);
		enable = rec->GetEnableAdvanceIndex();
		isObsolete = FALSE;
		for(o = 0; o < rec->GetNumObsoleteAdvance(); o++) {
			if(p->m_advances->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
				isObsolete = TRUE;
		}
		if(isObsolete)
			continue;
		if((p->m_advances->HasAdvance(enable) || (enable < 0))) {
			sprintf(str, "%s",g_theStringDB->GetNameStr(rec->m_name));
				sint32 j = aui_UniqueId();
				item = new StaticTextItem(
					&errcode,

					i,

					5, 0, 100, 15,
					str,
					NULL,
					8,
					i,
					k_GAME_OBJ_TYPE_WONDER,
					i);
				if (!item) return;

				AddItem( (aui_Item *)item );
		}
	}
}

sint32 CityInventoryListBox::UpdateImage( const Unit &unit )
{
	MBCHAR str[80];
	StaticTextItem *text;
	BuildNode *bn = NULL;
	BuildQueue *bq = NULL;

	text = (StaticTextItem *)GetSelectedItem();

	if (!unit && !m_buildMode) {
	}
	else {

		if (m_buildMode) {
			m_buildMode = 0;
			if (text) {
				unit.GetData()->GetCityData()->ChangeCurrentlyBuildingItem(text->GetCategory(),text->GetItemType());
				text = NULL;
			}

		}
		FillInventoryBox(NULL);
		FillInventoryBox(unit);
		bq = unit.GetData()->GetCityData()->GetBuildQueue();
		bn = bq->GetHead();
	}


	if (text) {



		sprintf(str,"");

	}

	else if (bn) {



		sint32 completed = bq->GetPercentCompleted(unit.GetData()->GetCityData()->GetStoredCityProduction());

		if (bn->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT) {
			sprintf(str, "%s", g_theStringDB->GetNameStr(g_theBuildingDB->Get(bn->m_type)->m_name));


		}
		else if (bn->m_category == k_GAME_OBJ_TYPE_UNIT) {
			sprintf(str, "%s", g_theStringDB->GetNameStr(g_theUnitDB->Get(bn->m_type)->m_name));


		}

	}

	else {


		sprintf(str,"");

	}

	return 0;
}

void CityInventoryListBox::CutBuilding( const Unit &unit)
{
	StaticTextItem *text;
	text = (StaticTextItem *)GetSelectedItem();
	sint32 category = text->GetCategory();
	sint32 id = text->GetId();
	uint64 improvements = 0;
	uint64 wonders = 0;

	if (category == k_GAME_OBJ_TYPE_IMPROVEMENT) {
		improvements = unit.GetData()->GetCityData()->GetImprovements();
		improvements ^= ((uint64)1 << (uint64)id);
		unit.GetData()->GetCityData()->SetImprovements(improvements);
	}
	else if (category == k_GAME_OBJ_TYPE_WONDER) {
		wonders = unit.GetData()->GetCityData()->GetBuiltWonders();
		wonders ^= ((uint64)1 << (uint64)id);
		unit.GetData()->GetCityData()->SetWonders(wonders);
	}

	FillInventoryBox(unit);



}
