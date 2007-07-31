






#pragma once
#ifndef ___BMH_GOVERNMENT_TAB_HEADER
#define ___BMH_GOVERNMENT_TAB_HEADER


#include <utility>


class aui_Control;
class ctp2_Button;
class ctp2_DropDown;
class ctp2_Static;
class GovernmentRecord;


class GovernmentTab {
public:
	
	GovernmentTab(MBCHAR *ldlBlock);

	
	void Update();

private:
	
	enum GovernmentInformationIndex {
		GII_CITIES			= 0,
		GII_GROWTH			= 1,
		GII_PRODUCTION		= 2,
		GII_RESEARCH		= 3,
		GII_ECONOMIC		= 4,
		GII_COMMERCE		= 5,
		GII_MILITARY		= 6,
		GII_LOYALTY			= 7,
		GII_MARTIAL_LAW		= 8,
		GII_ANTI_POLLUTION	= 9,
		GII_TOTAL			= 10
	};

	
	void UpdateCurrentGovernment();

	
	void UpdateCompareGovernmentDropdown();

	
	void UpdateCompareGovernment();

	
	
	void UpdateComparison(const GovernmentRecord *currentGovernment,
		const GovernmentRecord *compareGovernment);

	
	void ClearGovernmentInformation(ctp2_Static **information);

	
	void UpdateGovernmentInformation(const GovernmentRecord *government,
		ctp2_Static **information);

	
	static void CompareGovernmentActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void EnactGovernmentActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void ConfirmGovernmentChange(bool result, void *data);

	
	
	
	std::pair<bool, sint32> GetGovernmentSelection();

	
	ctp2_Static		*m_tabPanel;

	
	ctp2_Button		*m_enactButton;

	
	ctp2_Static		*m_currentGovernment;
	ctp2_Static		*m_currentInformation[GII_TOTAL];

	
	ctp2_Static		*m_comparison[GII_TOTAL];

	
	ctp2_DropDown	*m_compareGovernment;

	
	ctp2_Static		*m_compareInformation[GII_TOTAL];
};

#endif 
