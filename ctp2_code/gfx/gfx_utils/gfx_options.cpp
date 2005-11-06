

#include "c3.h"

#include "AvlTree.h"

#include "Army.h"
#include "ArmyData.h"
#include "ArmyPool.h"

#include "gfx_options.h"

GraphicsOptions		*g_graphicsOptions;


static cmp_t CellAVLCompare(CellText *obj1, CellText *obj2)
{
	if (obj1->m_key < obj2->m_key) return MIN_CMP;
	if (obj1->m_key > obj2->m_key) return MAX_CMP;

	return EQ_CMP;
}



GraphicsOptions::GraphicsOptions()
{
	m_armyTextOn = false;
	m_cellTextOn = false;
	m_armyNameOn = false;

	m_cellAVL = new AvlTree<CellText *>;
}



GraphicsOptions::~GraphicsOptions()
{
	delete m_cellAVL;
}




void GraphicsOptions::Initialize(void)
{
	if (g_graphicsOptions)
		delete g_graphicsOptions;

	g_graphicsOptions = new GraphicsOptions;
}


void GraphicsOptions::Cleanup(void)
{
	if (g_graphicsOptions)
		delete g_graphicsOptions;

	g_graphicsOptions = NULL;
}

// ArmyName added by E
void GraphicsOptions::ArmyNameOn(void)
{
	m_armyNameOn = true;
}

void GraphicsOptions::ArmyNameOff(void)
{
	m_armyNameOn = false;
}

//bool GraphicsOptions::AddNameToArmy(const Army &army, const MBCHAR *name, const uint8 &colorMagnitude) 
//{
	
//	if (!g_theArmyPool->IsValid(army)) 
//		return FALSE;

//	ResetArmyName(army);

//	if (name == NULL)
//		return FALSE;

//	MBCHAR *myString = new MBCHAR [strlen(name) + 1];
//	memset(myString, 0, strlen(name) + 1);
//	strcpy(myString, name);

//	army.AccessData()->SetName(myString);
//	army.AccessData()->SetDebugStringColor(colorMagnitude);

//	return TRUE;
//}


//void GraphicsOptions::ResetArmyName(const Army &army)
//{
	
//	if (!g_theArmyPool->IsValid(army)) 
//		return;

	
	
//	MBCHAR *string = army.GetData()->GetName();
//	if (string != NULL) {
//		delete string;
//		army.AccessData()->SetName(NULL);
//	}
//}

void GraphicsOptions::ArmyTextOn(void)
{
	m_armyTextOn = true;
}


void GraphicsOptions::ArmyTextOff(void)
{
	m_armyTextOn = false;
}



bool GraphicsOptions::AddTextToArmy(const Army &army, const char *text, const uint8 &colorMagnitude) 
{
	
	if (!g_theArmyPool->IsValid(army)) 
		return FALSE;

	ResetArmyText(army);

	if (text == NULL)
		return FALSE;

	MBCHAR *myString = new MBCHAR [strlen(text) + 1];
	memset(myString, 0, strlen(text) + 1);
	strcpy(myString, text);

	army.AccessData()->SetDebugString(myString);
	army.AccessData()->SetDebugStringColor(colorMagnitude);

	return TRUE;
}



void GraphicsOptions::ResetArmyText(const Army &army)
{
	
	if (!g_theArmyPool->IsValid(army)) 
		return;

	
	
	MBCHAR *string = army.GetData()->GetDebugString();
	if (string != NULL) {
		delete string;
		army.AccessData()->SetDebugString(NULL);
	}
}






void GraphicsOptions::CellTextOn(void)
{
	m_cellTextOn = true;
}


void GraphicsOptions::CellTextOff(void)
{
	m_cellTextOn = false;
}


uint32 GraphicsOptions::PackCellAVLKey(MapPoint &pos)
{
	return (pos.x << 16 | pos.y);
}



CellText *GraphicsOptions::GetCellText(MapPoint &pos)
{
	
	uint32	key = PackCellAVLKey(pos);

	
	Comparable<CellText *>	*avlObject = NULL;

	
	CellText		dummyCellText;
	dummyCellText.m_key = key;
	
	
	avlObject = m_cellAVL->Search(&dummyCellText);
	if (avlObject) {
		return avlObject->Key();
	} else {
		return NULL;
	}
}



bool GraphicsOptions::AddTextToCell(const MapPoint &pos, const char *text, 
									const uint8 &colorMagnitude)
{
	if(!m_cellTextOn) return FALSE;

	
	CellText *cellText = GetCellText((MapPoint &)pos);
	if (cellText != NULL) {
		
		if (cellText->m_text) {
			delete cellText->m_text;
			cellText->m_text = NULL;
		}
	}

	if (text == NULL) 
		return TRUE;

	MBCHAR		*newText = new MBCHAR[strlen(text) + 1];

	memset(newText, 0, strlen(text) + 1);
	strcpy(newText, text);

	
	if (cellText == NULL) {
		
		cellText = new CellText;
		cellText->m_key = PackCellAVLKey((MapPoint &)pos);

		
		Comparable<CellText *> *avlObject = 
							new Comparable<CellText *>(cellText, CellAVLCompare);
		m_cellAVL->Insert(avlObject);
	}
	
	
	cellText->m_text = newText;
	cellText->m_color = colorMagnitude;

	return TRUE;
}



void GraphicsOptions::ResetCellText(const MapPoint &pos)
{
	
	CellText *cellText = GetCellText((MapPoint &)pos);
	if (cellText != NULL) {
		
		if (cellText->m_text != NULL) {
			delete cellText->m_text;
		}

		m_cellAVL->Delete(cellText);

		delete cellText;
	}
}

