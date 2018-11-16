#include "c3.h"
#include "c3types.h"
#include "c3errors.h"
#include "globals.h"
#include "ic3UnitDB.h"
#include "ic3Science.h"
#include "ic3Wonder.h"
#include "ic3BlgDB.h"
#include "ic3Population.h"
#include "ic3endgamedb.h"

#include "FlatPtr.h"
#include "civarchive.h"

#include "aimain.h"
#include "gold.h"

#include "common.h"

#include "linked_list.h"
#include "semi_dynamic_array.h"
#include "sorted_array.h"
#include "matrix.h"
#include "grassfire.h"

#include "ArmyAgent.h"
#include "CityAgent.h"
#include "InstBidList.h"


#include "unitflow.h"
#include "aip.h"
#include "squad.h"

#include "goal.h"
#include "ArmyGoal.h"
#include "goal_build_supplemental.h"
#include "Goal_Build_List.h"

#include "WonderTbl.h"

#include "Scheduler.h"
#include "planner.h"
#include "strategic_map.h"

#include "dr_debug.h"

#include "ailog.h"






extern double fz_max_time_to_research;















extern double fz_no_settling;

#define LOG_AIP true








Advancement_List_Element::Advancement_List_Element()
{

	advancement_ID = -1;
}








Advancement_List_Element::Advancement_List_Element(CivArchive &archive)
{
    Serialize(archive);
}










void Advancement_List_Element::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
        archive.Store((uint8*)this, sizeof(*this));
    } else {
        archive.Load((uint8*)this, sizeof(*this));
    }
}








Building_Build_List_Element::Building_Build_List_Element()
{
    priority = 0;

	building_type_ID = -1;
	type = IS_NORMAL;


}








Building_Build_List_Element::Building_Build_List_Element(CivArchive &archive)
{
    Serialize(archive);
}










void Building_Build_List_Element::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
        archive.Store((uint8*)this, sizeof(*this));
    } else {
        archive.Load((uint8*)this, sizeof(*this));
    }
}









Build_List_Element::Build_List_Element()
{
    units_per_city = 0;

	unittype_ID = 0;
}








Build_List_Element::Build_List_Element(CivArchive &archive)
{
    Serialize(archive);
}










void Build_List_Element::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {

		Assert(fabs(units_per_city) < 100);
		if (fabs(units_per_city) > 100.0)
			units_per_city = -1.0;

        archive.Store((uint8*)this, sizeof(*this));
    } else {
        archive.Load((uint8*)this, sizeof(*this));

		Assert(fabs(units_per_city) < 100);
		if (fabs(units_per_city) > 100.0)
			units_per_city = -1.0;
    }
}








Build_List_Class::Build_List_Class()
{

	priority = 0.0;
	unit_type_count = 0;
    unit_type_list = NULL;

}







Build_List_Class::Build_List_Class(CivArchive &archive)
{
    Serialize(archive);
}









void Build_List_Class::Serialize(CivArchive &archive)
{

    CHECKSERIALIZE

    sint32 i;

    if (archive.IsStoring()) {

#ifdef _DEBUG


#endif
        archive << priority;
   	    archive << unit_type_count;

        for (i=0; i<unit_type_count; i++) {
           (*unit_type_list)[i].Serialize(archive);
        }

    } else {
#ifdef _DEBUG


#endif
        archive >> priority;
   	    archive >> unit_type_count;
        if (unit_type_count < 1) {
            unit_type_list = NULL;
        } else {

            unit_type_list = new semi_dynamic_array<Build_List_Element>(unit_type_count);

            for (i=0; i<unit_type_count; i++) {
                (*unit_type_list)[i].Serialize(archive);
            }
        }

    }
}




ZEROMEM(AIP_Flat_Data);
FLATSERIALIZE(AIP_Flat_Data);








void AIP::Load_Build_List_Class
(
	iparser * AIP_parameters,
	char * class_name,
	int class_num
)
{

	int error;
	char field_name[256];
	int i;
	char unit_type_name[MAX_AIP_STRING];

#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "....Loading Class %s\n", class_name));
#endif
#endif DBGAILOG

	(*build_list_classes)[class_num].unit_type_count =
		AIP_parameters->Get_Struct_Array_Element_Count(class_name, &error);

	Assert(error == IPARSE_NO_ERR);

	(*build_list_classes)[class_num].unit_type_list =
		new semi_dynamic_array<Build_List_Element>
			((*build_list_classes)[class_num].unit_type_count);
#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "....%d Unit Types\n", (*build_list_classes)[class_num].unit_type_count));
#endif
#endif DBGAILOG

	for (i = 0; i < (*build_list_classes)[class_num].unit_type_count; i++)
	{

		sprintf(field_name, "%s[%d].unit_type_name", class_name, i);
		AIP_parameters->Get_Runtime_String_Field(field_name, &error,
			unit_type_name);

		sprintf(field_name, "%s[%d].units_per_city", class_name, i);
		(*(*build_list_classes)[class_num].unit_type_list)[i].units_per_city =
			AIP_parameters->Get_Runtime_Double_Field(field_name, &error);

		Assert(fabs((*(*build_list_classes)[class_num].unit_type_list)[i].units_per_city) < 100);
		if (fabs((*(*build_list_classes)[class_num].unit_type_list)[i].units_per_city) > 100.0)
			(*(*build_list_classes)[class_num].unit_type_list)[i].units_per_city = -1.0;

		(*(*build_list_classes)[class_num].unit_type_list)[i].unittype_ID =
			ai->m_unitDB->Unittype_String_To_Int(
				unit_type_name);

		if ((*(*build_list_classes)[class_num].unit_type_list)[i].unittype_ID is -1)
		{
			char error_msg[500];

			_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
			_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

			sprintf(error_msg, "AIP Error: Unit %s not found in units.txt",
				unit_type_name);

			_RPT0(_CRT_WARN, error_msg);
		}
#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "    %s...%f\n", unit_type_name, (*(*build_list_classes)[class_num].unit_type_list)[i].units_per_city));
#endif
#endif DBGAILOG

	}
}













void AIP::Load_Supplemental_Build_List_Classes
(
	iparser * AIP_parameters
)
{

	int error;
	char field_name[256];
	char class_name[MAX_AIP_STRING];
	int i;
	int new_build_list_classes_count;

#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "Loading Supplemental Build List Classes\n"));
#endif
#endif DBGAILOG

	build_list_classes_loaded_flag = false;

	new_build_list_classes_count =
		AIP_parameters->Get_Struct_Array_Element_Count("Build_List_Classes",
														&error);

	if (error != IPARSE_NO_ERR) return;

	build_list_classes_count = new_build_list_classes_count;

	Delete_Build_Lists();

	Assert(build_list_classes == NULL);

	build_list_classes =
		new semi_dynamic_array<Build_List_Class> (build_list_classes_count);

#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "%d Classes\n", build_list_classes_count));
#endif
#endif DBGAILOG

	for (i = 0; i < build_list_classes_count; i++)
	{

		sprintf(field_name, "Build_List_Classes[%d].class_name", i);
		AIP_parameters->Get_Runtime_String_Field(field_name, &error,
			class_name);

		sprintf(field_name, "Build_List_Classes[%d].priority", i);
		(*build_list_classes)[i].priority =
			AIP_parameters->Get_Runtime_Double_Field(field_name, &error);

		Load_Build_List_Class(AIP_parameters, class_name, i);

	}

	build_list_classes_loaded_flag = true;
}















bool AIP::Load_Single_Building_Build_List
(
	iparser * AIP_parameters,
	char *list_name,
	semi_dynamic_array<Building_Build_List_Element> **the_list
)
{

	int error;
	char field_name[256];
	int i;
	int list_length;
	char building_type_name[MAX_AIP_STRING];

#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "\nAIP: Load_Single_Building_Build_List\n"));
#endif

#endif DBGAILOG

	list_length =
		AIP_parameters->Get_Struct_Array_Element_Count(list_name, &error);

	if (error != IPARSE_NO_ERR) return false;

	if (*the_list isnt NULL)
	{
		delete  (*the_list);
		*the_list = NULL;
	}

	*the_list =
		new semi_dynamic_array<Building_Build_List_Element> (list_length);

#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "AIP: %s has %d elements.\n", list_name, list_length));
#endif

#endif DBGAILOG

	for (i = 0; i < list_length; i++)
	{

		sprintf(field_name, "%s[%d].building_name", list_name, i);
		AIP_parameters->Get_Runtime_String_Field(field_name, &error,
			building_type_name);

		sprintf(field_name, "%s[%d].priority", list_name, i);
		(**the_list)[i].priority =
			AIP_parameters->Get_Runtime_Double_Field(field_name, &error);

#ifdef DBGAILOG

#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, "    %s...%lf\n", building_type_name,
					(**the_list)[i].priority));
#endif

#endif DBGAILOG

		if (!strcmp(list_name, WONDER_LIST_NAME))
			{

				(**the_list)[i].building_type_ID =
					ai->m_wonderDB->
					Wonder_String_To_Int(building_type_name);

			(**the_list)[i].type = IS_WONDER;


		}

		else if (!strcmp(list_name, END_GAME_OBJECT_LIST_NAME))
			{

				(**the_list)[i].building_type_ID =
					ai->m_endGameDB->
					End_Game_Object_String_To_Int(building_type_name);
			(**the_list)[i].type = IS_END_GAME_OBJECT;


			}

		else if (!strcmp(list_name, CAPITALIZATION_LIST_NAME))
			{

				(**the_list)[i].building_type_ID = 0;
			(**the_list)[i].type = IS_CAPITALIZATION;


			}
		else
		{

			(**the_list)[i].building_type_ID =
				ai->m_buildingDB->Improvementtype_String_To_Int(
					building_type_name);

			(**the_list)[i].type = IS_NORMAL;




		}

	}

	return true;
}














void AIP::Load_Building_Build_Lists
(
	iparser * AIP_parameters
)
{




	Wonders_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		WONDER_LIST_NAME, &Wonders_List);

	Transportation_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		TRANSPORTATION_LIST_NAME, &Transportation_List);

	Growth_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		GROWTH_LIST_NAME, &Growth_List);

	Happiness_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		HAPPINESS_LIST_NAME, &Happiness_List);

	Production_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		PRODUCTION_LIST_NAME, &Production_List);

	Gold_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		GOLD_LIST_NAME, &Gold_List);

	Science_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		SCIENCE_LIST_NAME, &Science_List);

	Defense_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		DEFENSE_LIST_NAME, &Defense_List);

	Miscellaneous_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		MISCELLANEOUS_LIST_NAME, &Miscellaneous_List);

	End_Game_Object_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		END_GAME_OBJECT_LIST_NAME, &End_Game_Object_List);

	Capitalization_List_loaded_flag = Load_Single_Building_Build_List(AIP_parameters,
		CAPITALIZATION_LIST_NAME, &Capitalization_List);
}














void AIP::Load_Scientific_Advancement_List
(
	iparser * AIP_parameters
)
{

	int error;
	char field_name[256];
	int i;
	int list_length;
	char advancement_name[MAX_AIP_STRING];

	#ifdef DBGAILOG
	#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "\nAIP: Load_Scientific_Advancement_List\n"));
	#endif
	#endif DBGAILOG

	Scientific_Advancements_List_loaded_flag = false;

	list_length =
		AIP_parameters->Get_Struct_Array_Element_Count(ADVANCEMENTS_LIST_NAME, &error);

	if (error != IPARSE_NO_ERR) return;

	if (Scientific_Advancements_List isnt NULL)
	{
		delete  Scientific_Advancements_List;
		Scientific_Advancements_List = NULL;
	}

	Scientific_Advancements_List =
		new semi_dynamic_array<Advancement_List_Element> (list_length);

	#ifdef DBGAILOG
	#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG((wstr, "AIP: %s has %d elements.\n", ADVANCEMENTS_LIST_NAME, list_length));
	#endif
	#endif DBGAILOG

	for (i = 0; i < list_length; i++)
	{

		sprintf(field_name, "%s[%d].advancement_name",
			ADVANCEMENTS_LIST_NAME, i);

		AIP_parameters->Get_Runtime_String_Field(field_name, &error,
			advancement_name);


		(*Scientific_Advancements_List)[i].advancement_ID =
			ai->m_science->Science_String_To_Int(advancement_name);

		#ifdef DBGAILOG
		#ifdef LOG_AIP
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "    %s...ID: %d\n",
					advancement_name,
					(*Scientific_Advancements_List)[i].advancement_ID));
		#endif
		#endif DBGAILOG


	}

	Scientific_Advancements_List_loaded_flag = true;
}














bool AIP::Load_Double_If_There
(
	iparser * AIP_parameters,
	char *parameter_name,
	double &my_variable
)
{

	int error;
	double temp_double;




	error = IPARSE_DONT_REPORT_ERROR;
	temp_double =
		AIP_parameters->Get_Runtime_Double(parameter_name, &error);
	if ((error is IPARSE_NO_ERR) or (error is WRONG_TYPE))
	{
		my_variable = temp_double;
		#ifdef DBGAILOG
		#ifdef LOG_AIP

		if (ai->AI_VERBOSITY >= LOG_MEDIUM)
			AILOG((wstr, "    %s: %g\n", parameter_name, my_variable));

		#endif
		#endif DBGAILOG
		return true;
	}




	if ((not loaded_first_AIP) and (error is VARIABLE_NOT_FOUND))
	{
		char error_msg[500];

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

		sprintf(error_msg, "Designers left out AIP parameter from default.aip: %s", parameter_name);

		_RPT0(_CRT_WARN, error_msg);

		#ifdef DBGAILOG
		#ifdef LOG_AIP

		if (ai->AI_VERBOSITY >= LOGGING_OFF)
			AILOG((wstr, "\nAIP ERROR: %s", error_msg));

		#endif
		#endif DBGAILOG

		return false;

	}

	return false;

}
















bool AIP::Load_Int_If_There
(
	iparser * AIP_parameters,
	char *parameter_name,
	int &my_variable,
	bool dont_warn
)
{

	int error;
	int temp;




	error = IPARSE_DONT_REPORT_ERROR;
	temp =
		AIP_parameters->Get_Runtime_Int(parameter_name, &error);
	if ((error is IPARSE_NO_ERR) or (error is WRONG_TYPE))
	{
		my_variable = temp;
		#ifdef DBGAILOG
		#ifdef LOG_AIP

		if (ai->AI_VERBOSITY >= LOG_MEDIUM)
			AILOG((wstr, "    %s: %d\n", parameter_name, my_variable));

		#endif
		#endif DBGAILOG
		return true;
	}

	if (dont_warn) return false;




	if (not loaded_first_AIP)
	{
		char error_msg[500];

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

		sprintf(error_msg, "Designers left out AIP parameter: %s", parameter_name);

		_RPT0(_CRT_WARN, error_msg);

		#ifdef DBGAILOG
		#ifdef LOG_AIP

		if (ai->AI_VERBOSITY >= LOGGING_OFF)
			AILOG((wstr, "\nAIP ERROR: %s", error_msg));

		#endif
		#endif DBGAILOG

	}

	return false;

}














void AIP::Load_Simple_AIP_Priorities
(
	iparser * AIP_parameters
)
{

	int temp_verbosity;





	if (AI_Verbosity_loaded_flag =
			Load_Int_If_There(AIP_parameters, "AI_VERBOSITY", temp_verbosity))
	{
		ai->AI_VERBOSITY = temp_verbosity;
		AI_Verbosity = temp_verbosity;
	}




	build_troops_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"build_troops_priority",build_troops_priority);

	build_buildings_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"build_buildings_priority", build_buildings_priority);

	build_end_game_object_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"build_end_game_object_priority", build_end_game_object_priority);

	build_capitalization_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"build_capitalization_priority", build_capitalization_priority);

	seige_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"seige_priority", seige_priority);

	explore_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"explore_priority", explore_priority);

	attack_troops_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"attack_troops_priority", attack_troops_priority);

	attack_region_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"attack_region_priority", attack_region_priority);

	settle_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"settle_priority", settle_priority);

	defend_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"defend_priority", defend_priority);

	perimeter_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"perimeter_priority", perimeter_priority);

	patrol_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"patrol_priority", patrol_priority);

	chokepoint_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"chokepoint_priority", chokepoint_priority);

	harass_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"harass_priority", harass_priority);

	bombard_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"bombard_priority", bombard_priority);

	rustle_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"rustle_priority", rustle_priority);

	expel_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"expel_priority", expel_priority);

	sally_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"sally_priority", sally_priority);

	retreat_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"retreat_priority", retreat_priority);

	convert_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"convert_priority", convert_priority);

	bioterror_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"bioterror_priority", bioterror_priority);

	nanoattack_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"nanoattack_priority", nanoattack_priority);

	pillage_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"pillage_priority", pillage_priority);

	pirate_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"pirate_priority", pirate_priority);

	counter_stealth_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"counter_stealth_priority", counter_stealth_priority);

	bonus_food_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"bonus_food_priority", bonus_food_priority);

	goody_hut_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"goody_hut_priority", goody_hut_priority);

	defuse_mines_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"defuse_mines_priority", defuse_mines_priority);

	enslave_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"enslave_priority",enslave_priority);

	plant_nuke_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"plant_nuke_priority", plant_nuke_priority);

	create_park_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"create_park_priority", create_park_priority);

	underground_railway_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"underground_railway_priority", underground_railway_priority);

	establish_embassy_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"establish_embassy_priority", establish_embassy_priority);

	franchising_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"franchising_priority", franchising_priority);

	assasinate_ruler_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"assasinate_ruler_priority", assasinate_ruler_priority);

	steal_technology_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"steal_technology_priority", steal_technology_priority);

	injoin_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"injoin_priority", injoin_priority);

	incite_revolution_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"incite_revolution_priority", incite_revolution_priority);

	cause_unhappiness_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"cause_unhappiness_priority", cause_unhappiness_priority);

	nuke_city_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"nuke_city_priority", nuke_city_priority);

	nuke_city_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"wander_priority", nuke_city_priority);

	reform_city_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"reform_city_priority", reform_city_priority);

	sue_franchise_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"sue_franchise_priority", sue_franchise_priority);

	probe_wormhole_priority_loaded_flag = Load_Double_If_There(AIP_parameters,"probe_wormhole_priority", probe_wormhole_priority);




	danger_diminishment_loaded_flag = Load_Double_If_There(AIP_parameters,"danger_diminishment", danger_diminishment);

	max_danger_raw_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"max_danger_raw_bonus", max_danger_raw_bonus);

	max_power_raw_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"max_power_raw_bonus", max_power_raw_bonus);

	max_value_raw_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"max_value_raw_bonus", 	max_value_raw_bonus);

	max_threat_raw_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"max_threat_raw_bonus", max_threat_raw_bonus);

	distance_from_home_priority_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"distance_from_home_priority_modifier", distance_from_home_priority_modifier);

	distance_from_enemy_priority_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"distance_from_enemy_priority_modifier", distance_from_enemy_priority_modifier);

	distance_from_unit_priority_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"distance_from_unit_priority_modifier", distance_from_unit_priority_modifier);

	max_chokepoint_raw_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"max_chokepoint_raw_bonus", max_chokepoint_raw_bonus);

	random_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"random_modifier", random_modifier);

	per_capita_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"per_capita_modifier", per_capita_modifier);

	threat_proximity_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"threat_proximity_modifier", threat_proximity_modifier);

	human_target_raw_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"human_target_raw_bonus", human_target_raw_bonus);

	defenseless_city_raw_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"defenseless_city_raw_bonus", defenseless_city_raw_bonus);

	max_misc_bid_bonus_loaded_flag = Load_Double_If_There(AIP_parameters,"max_misc_bid_bonus", max_misc_bid_bonus);

	fzbuild_priority_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"fzbuild_priority_modifier", fzbuild_priority_modifier);

	fzwonder_priority_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"fzwonder_priority_modifier", fzwonder_priority_modifier);

	fzfreight_priority_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"fzfreight_priority_modifier", fzfreight_priority_modifier);

	persistence_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"persistence_modifier", persistence_modifier);

	better_unit_type_modifier_loaded_flag = Load_Double_If_There(AIP_parameters,"better_unit_type_modifier", better_unit_type_modifier);

	Wonders_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Wonders_List_Threshhold", Wonders_List_Threshhold);

	Transportation_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Transportation_List_Threshhold", Transportation_List_Threshhold);

	Growth_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Growth_List_Threshhold", Growth_List_Threshhold);

	Happiness_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Happiness_List_Threshhold", Happiness_List_Threshhold);

	Production_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Production_List_Threshhold", Production_List_Threshhold);

	Gold_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Gold_List_Threshhold", Gold_List_Threshhold);

	Science_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Science_List_Threshhold", Science_List_Threshhold);

	Defense_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Defense_List_Threshhold", Defense_List_Threshhold);

	Miscellaneous_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Miscellaneous_List_Threshhold", Miscellaneous_List_Threshhold);

	End_Game_Object_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"End_Game_Object_List_Threshhold", End_Game_Object_List_Threshhold);

	Capitalization_List_Threshhold_loaded_flag = Load_Double_If_There(AIP_parameters,"Capitalization_List_Threshhold", Capitalization_List_Threshhold);




	wander_rounds_from_target_loaded_flag = Load_Int_If_There(AIP_parameters, "wander_rounds_from_target", wander_rounds_from_target);


	wander_density_loaded_flag = Load_Double_If_There(AIP_parameters, "wander_density", wander_density);




	captured_city_defend_turns_loaded_flag = Load_Int_If_There(AIP_parameters, "captured_city_defend_turns", captured_city_defend_turns);

	captured_city_modifier_loaded_flag = Load_Double_If_There(AIP_parameters, "captured_city_modifier", captured_city_modifier);




	patrol_one_in_n_loaded_flag = Load_Int_If_There(AIP_parameters, "patrol_one_in_n", patrol_one_in_n);




	memory_rounds_loaded_flag = Load_Int_If_There(AIP_parameters, "memory_rounds", memory_rounds);

	memory_rounds_stealth_loaded_flag = Load_Int_If_There(AIP_parameters, "memory_rounds_stealth", memory_rounds);

	min_defense_matching_force_ratio_loaded_flag = Load_Double_If_There(AIP_parameters,"min_defense_matching_force_ratio", min_defense_matching_force_ratio);

	min_attack_matching_force_ratio_loaded_flag = Load_Double_If_There(AIP_parameters,"min_attack_matching_force_ratio", min_attack_matching_force_ratio);

	num_city_defenders_loaded_flag = Load_Int_If_There(AIP_parameters, "num_city_defenders", num_city_defenders);




	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_DEFENSE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_DEFENSE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_DEFENSE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ATTACK] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_ATTACK_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_ATTACK]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_RETREAT] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_RETREAT_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_RETREAT]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SEIGE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_SEIGE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_SEIGE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SETTLE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_SETTLE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_SETTLE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_EXPLORE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_EXPLORE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_EXPLORE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONSTRUCT_BUILDING] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_CONSTRUCT_BUILDING_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_CONSTRUCT_BUILDING]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONSTRUCT_FREIGHT] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_CONSTRUCT_FREIGHT_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_CONSTRUCT_FREIGHT]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONSTRUCT_WONDER] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_CONSTRUCT_WONDER_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_CONSTRUCT_WONDER]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_TRANSPORT] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_TRANSPORT_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_TRANSPORT]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BUILD_SUPPLEMENTAL] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_BUILD_SUPPLEMENTAL_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_BUILD_SUPPLEMENTAL]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PERIMETER] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_PERIMETER_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_PERIMETER]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ATTACK_REGION] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_ATTACK_REGION_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_ATTACK_REGION]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CHOKEPOINT] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_CHOKEPOINT_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_CHOKEPOINT]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PATROL] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_PATROL_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_PATROL]);


	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BOMBARD] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_BOMBARD_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_BOMBARD]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_RUSTLE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_RUSTLE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_RUSTLE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_HARASS_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_HARASS_CITY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_HARASS_CITY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONVERT_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_CONVERT_CITY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_CONVERT_CITY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BIOTERROR_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_BIOTERROR_CITY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_BIOTERROR_CITY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_NANOATTACK_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_NANOATTACK_CITY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_NANOATTACK_CITY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_EXPEL] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_EXPEL_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_EXPEL]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SALLY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_SALLY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_SALLY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_HURT_ARMY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_HURT_ARMY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_HURT_ARMY]);





	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PILLAGE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_PILLAGE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_PILLAGE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PIRATE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_PIRATE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_PIRATE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_COUNTER_STEALTH] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_COUNTER_STEALTH_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_COUNTER_STEALTH]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BONUS_FOOD] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_BONUS_FOOD_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_BONUS_FOOD]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_GOODY_HUT] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_GOODY_HUT_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_GOODY_HUT]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PLANT_NUKE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_PLANT_NUKE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_PLANT_NUKE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_MAKE_PARK] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_MAKE_PARK_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_MAKE_PARK]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CAUSE_UNHAPPINESS] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_CAUSE_UNHAPPINESS_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_CAUSE_UNHAPPINESS]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ESTABLISH_EMBASSY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_ESTABLISH_EMBASSY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_ESTABLISH_EMBASSY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_FRANCHISING] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_FRANCHISING_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_FRANCHISING]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_STEAL_TECHNOLOGY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_STEAL_TECHNOLOGY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_STEAL_TECHNOLOGY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_INCITE_REVOLUTION] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_INCITE_REVOLUTION_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_INCITE_REVOLUTION]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_INJOIN] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_INJOIN_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_INJOIN]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ASSASINATE_RULER] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_ASSASINATE_RULER_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_ASSASINATE_RULER]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_UNDERGROUND_RAILWAY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_UNDERGROUND_RAILWAY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_UNDERGROUND_RAILWAY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_NUKE_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_NUKE_CITY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_NUKE_CITY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ENSLAVE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_ENSLAVE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_ENSLAVE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_REFORM_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_REFORM_CITY_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_REFORM_CITY]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SUE_FRANCHISE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_SUE_FRANCHISE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_SUE_FRANCHISE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PROBE_WORMHOLE] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_PROBE_WORMHOLE_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_PROBE_WORMHOLE]);

	max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_WANDER] = Load_Double_If_There(AIP_parameters,"MAX_EVAL_WANDER_GOALS", max_eval_goals_of_type_per_city[GOAL_TYPE_WANDER]);





	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_DEFENSE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_DEFENSE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_DEFENSE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ATTACK] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_ATTACK_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_ATTACK]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_RETREAT] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_RETREAT_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_RETREAT]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SEIGE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_SEIGE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_SEIGE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SETTLE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_SETTLE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_SETTLE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_EXPLORE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_EXPLORE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_EXPLORE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONSTRUCT_BUILDING] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_CONSTRUCT_BUILDING_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_CONSTRUCT_BUILDING]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONSTRUCT_FREIGHT] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_CONSTRUCT_FREIGHT_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_CONSTRUCT_FREIGHT]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONSTRUCT_WONDER] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_CONSTRUCT_WONDER_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_CONSTRUCT_WONDER]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_TRANSPORT] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_TRANSPORT_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_TRANSPORT]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BUILD_SUPPLEMENTAL] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_BUILD_SUPPLEMENTAL_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_BUILD_SUPPLEMENTAL]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PERIMETER] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_PERIMETER_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_PERIMETER]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ATTACK_REGION] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_ATTACK_REGION_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_ATTACK_REGION]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CHOKEPOINT] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_CHOKEPOINT_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_CHOKEPOINT]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PATROL] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_PATROL_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_PATROL]);


	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BOMBARD] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_BOMBARD_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_BOMBARD]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_RUSTLE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_RUSTLE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_RUSTLE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_HARASS_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_HARASS_CITY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_HARASS_CITY]);


	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CONVERT_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_CONVERT_CITY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_CONVERT_CITY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BIOTERROR_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_BIOTERROR_CITY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_BIOTERROR_CITY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_NANOATTACK_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_NANOATTACK_CITY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_NANOATTACK_CITY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_EXPEL] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_EXPEL_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_EXPEL]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SALLY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_SALLY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_SALLY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_HURT_ARMY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_HURT_ARMY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_HURT_ARMY]);





	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PILLAGE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_PILLAGE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_PILLAGE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PIRATE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_PIRATE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_PIRATE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_COUNTER_STEALTH] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_COUNTER_STEALTH_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_COUNTER_STEALTH]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_BONUS_FOOD] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_BONUS_FOOD_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_BONUS_FOOD]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_GOODY_HUT] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_GOODY_HUT_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_GOODY_HUT]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PLANT_NUKE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_PLANT_NUKE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_PLANT_NUKE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_MAKE_PARK] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_MAKE_PARK_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_MAKE_PARK]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_CAUSE_UNHAPPINESS] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_CAUSE_UNHAPPINESS_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_CAUSE_UNHAPPINESS]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ESTABLISH_EMBASSY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_ESTABLISH_EMBASSY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_ESTABLISH_EMBASSY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_FRANCHISING] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_FRANCHISING_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_FRANCHISING]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_STEAL_TECHNOLOGY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_STEAL_TECHNOLOGY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_STEAL_TECHNOLOGY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_INCITE_REVOLUTION] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_INCITE_REVOLUTION_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_INCITE_REVOLUTION]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_INJOIN] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_INJOIN_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_INJOIN]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ASSASINATE_RULER] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_ASSASINATE_RULER_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_ASSASINATE_RULER]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_UNDERGROUND_RAILWAY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_UNDERGROUND_RAILWAY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_UNDERGROUND_RAILWAY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_NUKE_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_NUKE_CITY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_NUKE_CITY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_ENSLAVE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_ENSLAVE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_ENSLAVE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_REFORM_CITY] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_REFORM_CITY_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_REFORM_CITY]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_SUE_FRANCHISE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_SUE_FRANCHISE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_SUE_FRANCHISE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_PROBE_WORMHOLE] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_PROBE_WORMHOLE_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_PROBE_WORMHOLE]);

	max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_WANDER] = Load_Double_If_There(AIP_parameters,"MAX_EXEC_WANDER_GOALS", max_execute_goals_of_type_per_city[GOAL_TYPE_WANDER]);

#ifdef _DEBUG

	alert_to_aip_load_loaded_flag = Load_Int_If_There(AIP_parameters, "alert_to_aip_load", alert_to_aip_load, true);
#endif





	for (int i = 0 ; i < GOAL_TYPE_MAX ; i++)
	{


		if ((max_execute_goals_of_type_per_city[i] > 0) and (max_eval_goals_of_type_per_city[i] > 0))
		{
			if (max_execute_goals_of_type_per_city[i] > max_eval_goals_of_type_per_city[i])
				max_eval_goals_of_type_per_city[i] = max_execute_goals_of_type_per_city[i];

		}

		else if ((max_execute_goals_of_type_per_city[i] < 0) and (max_eval_goals_of_type_per_city[i] < 0))
		{
			if (max_execute_goals_of_type_per_city[i] < max_eval_goals_of_type_per_city[i])
				max_eval_goals_of_type_per_city[i] = max_execute_goals_of_type_per_city[i];
		}

	}


	max_build_building_rounds_loaded_flag = Load_Int_If_There(AIP_parameters, "max_build_building_rounds", max_build_building_rounds);
	max_build_unit_rounds_loaded_flag = Load_Int_If_There(AIP_parameters, "max_build_unit_rounds", max_build_unit_rounds);
	max_build_wonder_rounds_loaded_flag = Load_Int_If_There(AIP_parameters, "max_build_wonder_rounds", max_build_wonder_rounds);
}












AIP_ptr AIP::Load_AIP_File
(
	char * AIP_name,
	bool force_reload
)
{

	iparser * AIP_parameters;
	int error;
	char aip_directory_path[MAX_AIP_FILE_NAME];

#ifdef DBGAILOG

	#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
		AILOG((wstr, "\nLOADING AIP: %s\n\n", AIP_name));
	#endif

#endif DBGAILOG

	if (name[0] != 0)
	{

		if ((!strcmp(name, AIP_name)) and (not force_reload))
		{
#ifdef DBGAILOG
			#ifdef LOG_AIP
			if (ai->AI_VERBOSITY >= LOG_HEADERS)
				AILOG((wstr, "NOTE: AIP %s already in memory!\n\n", AIP_name));
			#endif
#endif DBGAILOG

			return (this);

		}

	}

	strcpy (name, AIP_name);

#ifdef _DEBUG

	if (alert_to_aip_load)
	{
		char error_msg[500];

		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);

		sprintf(error_msg, "TEAM %d LOADING AIP: %s", ai->my_player_index, name);

		_RPT0(_CRT_WARN, error_msg);

	}

#endif

	AIP_parameters = new iparser(ai);

	sprintf(aip_directory_path, "%s\\AIPs", ai->m_gs->GetDataPath());

	AIP_parameters->Append_Path(aip_directory_path);

	error = AIP_parameters->Parse_File( AIP_name );

#ifdef DBGAILOG
	#ifdef LOG_AIP
	if (ai->AI_VERBOSITY >= LOG_HEADERS)
		AILOG((wstr, "\nAIP file parsed\n\n"));
	#endif
#endif DBGAILOG

	if (error is NO_FILE_ERROR)
	{
		#ifdef LOG_AIP
		AILOG((wstr, "AIP file '%s' not found\n", AIP_name));
		#endif

		return(NULL);

	}

	loaded_first_AIP = strcmp(name, "default.aip");

	Load_Simple_AIP_Priorities(AIP_parameters);

	Load_Supplemental_Build_List_Classes(AIP_parameters);

	Load_Building_Build_Lists(AIP_parameters);

	Load_Scientific_Advancement_List(AIP_parameters);

	delete AIP_parameters;

	loaded_first_AIP = true;

	return(this);
}















void AIP::Switch_Simple_AIP_Priorities
(
	AIP_ptr new_aip
)
{




	if (new_aip->AI_Verbosity_loaded_flag)
		ai->AI_VERBOSITY = new_aip->AI_Verbosity;




	if (new_aip->build_troops_priority_loaded_flag)
		build_troops_priority = new_aip->build_troops_priority;

	if (new_aip->build_buildings_priority_loaded_flag)
		build_buildings_priority = new_aip->build_buildings_priority;

	if (new_aip->build_end_game_object_priority_loaded_flag)
		build_end_game_object_priority = new_aip->build_end_game_object_priority;

	if (new_aip->build_capitalization_priority_loaded_flag)
		build_capitalization_priority = new_aip->build_capitalization_priority;

	if (new_aip->seige_priority_loaded_flag)
		seige_priority = new_aip->seige_priority;

	if (new_aip->explore_priority_loaded_flag)
		explore_priority= new_aip->explore_priority;

	if (new_aip->attack_troops_priority_loaded_flag)
		attack_troops_priority= new_aip->attack_troops_priority;

	if (new_aip->attack_region_priority_loaded_flag)
		attack_region_priority= new_aip->attack_region_priority;

	if (new_aip->settle_priority_loaded_flag)
		settle_priority= new_aip->settle_priority;

	if (new_aip->defend_priority_loaded_flag)
		defend_priority= new_aip->defend_priority;

	if (new_aip->perimeter_priority_loaded_flag) perimeter_priority = new_aip->perimeter_priority;

	if (new_aip->patrol_priority_loaded_flag) patrol_priority = new_aip->patrol_priority;

	if (new_aip->chokepoint_priority_loaded_flag) chokepoint_priority = new_aip->chokepoint_priority;

	if (new_aip->harass_priority_loaded_flag) harass_priority = new_aip->harass_priority;

	if (new_aip->bombard_priority_loaded_flag) bombard_priority = new_aip->bombard_priority;

	if (new_aip->rustle_priority_loaded_flag) rustle_priority = new_aip->rustle_priority;

	if (new_aip->expel_priority_loaded_flag) expel_priority = new_aip->expel_priority;

	if (new_aip->sally_priority_loaded_flag) sally_priority = new_aip->sally_priority;

	if (new_aip->retreat_priority_loaded_flag) retreat_priority = new_aip->retreat_priority;

	if (new_aip->convert_priority_loaded_flag) convert_priority = new_aip->convert_priority;

	if (new_aip->bioterror_priority_loaded_flag) bioterror_priority = new_aip->bioterror_priority;

	if (new_aip->nanoattack_priority_loaded_flag) nanoattack_priority = new_aip->nanoattack_priority;

	if (new_aip->pillage_priority_loaded_flag) pillage_priority = new_aip->pillage_priority;

	if (new_aip->pirate_priority_loaded_flag) pirate_priority = new_aip->pirate_priority;

	if (new_aip->counter_stealth_priority_loaded_flag) counter_stealth_priority = new_aip->counter_stealth_priority;

	if (new_aip->bonus_food_priority_loaded_flag) bonus_food_priority = new_aip->bonus_food_priority;

	if (new_aip->goody_hut_priority_loaded_flag) goody_hut_priority = new_aip->goody_hut_priority;

	if (new_aip->defuse_mines_priority_loaded_flag) defuse_mines_priority = new_aip->defuse_mines_priority;

	if (new_aip->enslave_priority_loaded_flag) enslave_priority = new_aip->enslave_priority;

	if (new_aip->plant_nuke_priority_loaded_flag) plant_nuke_priority = new_aip->plant_nuke_priority;

	if (new_aip->create_park_priority_loaded_flag) create_park_priority = new_aip->create_park_priority;

	if (new_aip->underground_railway_priority_loaded_flag) underground_railway_priority = new_aip->underground_railway_priority;

	if (new_aip->establish_embassy_priority_loaded_flag) establish_embassy_priority = new_aip->establish_embassy_priority;

	if (new_aip->franchising_priority_loaded_flag) franchising_priority = new_aip->franchising_priority;

	if (new_aip->assasinate_ruler_priority_loaded_flag) assasinate_ruler_priority = new_aip->assasinate_ruler_priority;

	if (new_aip->steal_technology_priority_loaded_flag) steal_technology_priority = new_aip->steal_technology_priority;

	if (new_aip->injoin_priority_loaded_flag) injoin_priority = new_aip->injoin_priority;

	if (new_aip->incite_revolution_priority_loaded_flag) incite_revolution_priority = new_aip->incite_revolution_priority;

	if (new_aip->cause_unhappiness_priority_loaded_flag) cause_unhappiness_priority = new_aip->cause_unhappiness_priority;

	if (new_aip->nuke_city_priority_loaded_flag) nuke_city_priority = new_aip->nuke_city_priority;

	if (new_aip->wander_priority_loaded_flag) nuke_city_priority = new_aip->nuke_city_priority;

	if (new_aip->reform_city_priority_loaded_flag) reform_city_priority = new_aip->reform_city_priority;

	if (new_aip->sue_franchise_priority_loaded_flag) sue_franchise_priority = new_aip->sue_franchise_priority;

	if (new_aip->probe_wormhole_priority_loaded_flag) probe_wormhole_priority = new_aip->probe_wormhole_priority;




	if (new_aip->danger_diminishment_loaded_flag) danger_diminishment = new_aip->danger_diminishment;

	if (new_aip->max_danger_raw_bonus_loaded_flag) max_danger_raw_bonus = new_aip->max_danger_raw_bonus;

	if (new_aip->max_power_raw_bonus_loaded_flag) max_power_raw_bonus = new_aip->max_power_raw_bonus;

	if (new_aip->max_value_raw_bonus_loaded_flag) max_value_raw_bonus = new_aip->max_value_raw_bonus;

	if (new_aip->max_threat_raw_bonus_loaded_flag) max_threat_raw_bonus = new_aip->max_threat_raw_bonus;

	if (new_aip->distance_from_home_priority_modifier_loaded_flag) distance_from_home_priority_modifier = new_aip->distance_from_home_priority_modifier;

	if (new_aip->distance_from_enemy_priority_modifier_loaded_flag) distance_from_enemy_priority_modifier = new_aip->distance_from_enemy_priority_modifier;

	if (new_aip->distance_from_unit_priority_modifier_loaded_flag) distance_from_unit_priority_modifier = new_aip->distance_from_unit_priority_modifier;

	if (new_aip->max_chokepoint_raw_bonus_loaded_flag) max_chokepoint_raw_bonus = new_aip->max_chokepoint_raw_bonus;

	if (new_aip->random_modifier_loaded_flag) random_modifier = new_aip->random_modifier;

	if (new_aip->per_capita_modifier_loaded_flag) per_capita_modifier = new_aip->per_capita_modifier;

	if (new_aip->threat_proximity_modifier_loaded_flag) threat_proximity_modifier = new_aip->threat_proximity_modifier;

	if (new_aip->human_target_raw_bonus_loaded_flag) human_target_raw_bonus = new_aip->human_target_raw_bonus;

	if (new_aip->defenseless_city_raw_bonus_loaded_flag) defenseless_city_raw_bonus = new_aip->defenseless_city_raw_bonus;

	if (new_aip->max_misc_bid_bonus_loaded_flag) max_misc_bid_bonus = new_aip->max_misc_bid_bonus;

	if (new_aip->fzbuild_priority_modifier_loaded_flag) fzbuild_priority_modifier = new_aip->fzbuild_priority_modifier;

	if (new_aip->fzwonder_priority_modifier_loaded_flag) fzwonder_priority_modifier = new_aip->fzwonder_priority_modifier;

	if (new_aip->fzfreight_priority_modifier_loaded_flag) fzfreight_priority_modifier = new_aip->fzfreight_priority_modifier;

	if (new_aip->persistence_modifier_loaded_flag) persistence_modifier = new_aip->persistence_modifier;

	if (new_aip->better_unit_type_modifier_loaded_flag) better_unit_type_modifier = new_aip->better_unit_type_modifier;

	if (new_aip->Wonders_List_Threshhold_loaded_flag) Wonders_List_Threshhold = new_aip->Wonders_List_Threshhold;

	if (new_aip->Transportation_List_Threshhold_loaded_flag) Transportation_List_Threshhold = new_aip->Transportation_List_Threshhold;

	if (new_aip->Growth_List_Threshhold_loaded_flag) Growth_List_Threshhold = new_aip->Growth_List_Threshhold;

	if (new_aip->Happiness_List_Threshhold_loaded_flag) Happiness_List_Threshhold = new_aip->Happiness_List_Threshhold;

	if (new_aip->Production_List_Threshhold_loaded_flag) Production_List_Threshhold = new_aip->Production_List_Threshhold;

	if (new_aip->Gold_List_Threshhold_loaded_flag) Gold_List_Threshhold = new_aip->Gold_List_Threshhold;

	if (new_aip->Science_List_Threshhold_loaded_flag) Science_List_Threshhold = new_aip->Science_List_Threshhold;

	if (new_aip->Defense_List_Threshhold_loaded_flag) Defense_List_Threshhold = new_aip->Defense_List_Threshhold;

	if (new_aip->Miscellaneous_List_Threshhold_loaded_flag) Miscellaneous_List_Threshhold = new_aip->Miscellaneous_List_Threshhold;

	if (new_aip->End_Game_Object_List_Threshhold_loaded_flag) End_Game_Object_List_Threshhold = new_aip->End_Game_Object_List_Threshhold;

	if (new_aip->Capitalization_List_Threshhold_loaded_flag) Capitalization_List_Threshhold = new_aip->Capitalization_List_Threshhold;




	if (new_aip->wander_rounds_from_target_loaded_flag) wander_rounds_from_target = new_aip->wander_rounds_from_target;


	if (new_aip->wander_density_loaded_flag) wander_density = new_aip->wander_density;




	if (new_aip->captured_city_defend_turns_loaded_flag) captured_city_defend_turns = new_aip->captured_city_defend_turns;

	if (new_aip->captured_city_modifier_loaded_flag) captured_city_modifier = new_aip->captured_city_modifier;




	if (new_aip->patrol_one_in_n_loaded_flag) patrol_one_in_n = new_aip->patrol_one_in_n;




	if (new_aip->memory_rounds_loaded_flag) memory_rounds = new_aip->memory_rounds;

	if (new_aip->memory_rounds_stealth_loaded_flag) memory_rounds_stealth = new_aip->memory_rounds_stealth;

	if (new_aip->min_defense_matching_force_ratio_loaded_flag) min_defense_matching_force_ratio = new_aip->min_defense_matching_force_ratio;

	if (new_aip->min_attack_matching_force_ratio_loaded_flag) min_attack_matching_force_ratio = new_aip->min_attack_matching_force_ratio;

	if (new_aip->num_city_defenders_loaded_flag) num_city_defenders = new_aip->num_city_defenders;

#ifdef _DEBUG

	if (new_aip->alert_to_aip_load_loaded_flag) alert_to_aip_load = new_aip->alert_to_aip_load;
#endif


	for (int i = 0 ; i < GOAL_TYPE_MAX ; i++)
	{

		if (new_aip->max_execute_goals_of_type_per_city_loaded_flag[i])

			if (new_aip->max_execute_goals_of_type_per_city[i] != UNLIMITED_GOALS)
			{
				max_execute_goals_of_type_per_city[i]
					= new_aip->max_execute_goals_of_type_per_city[i];
			}

		if (new_aip->max_eval_goals_of_type_per_city_loaded_flag[i])

			if (new_aip->max_eval_goals_of_type_per_city[i] != UNLIMITED_GOALS)
			{
				max_eval_goals_of_type_per_city[i]
					= new_aip->max_eval_goals_of_type_per_city[i];
			}

	}


	if (new_aip->max_build_building_rounds_loaded_flag) max_build_building_rounds = new_aip->max_build_building_rounds;
	if (new_aip->max_build_unit_rounds_loaded_flag) max_build_unit_rounds = new_aip->max_build_unit_rounds;
	if (new_aip->max_build_wonder_rounds_loaded_flag) max_build_wonder_rounds = new_aip->max_build_wonder_rounds;





	max_eval_goals_of_type_per_city[GOAL_TYPE_RETREAT] = UNLIMITED_GOALS;




	max_execute_goals_of_type_per_city[GOAL_TYPE_RETREAT] = UNLIMITED_GOALS;

}














void AIP::Switch_AIP
(
	AIP_ptr new_aip
)
{

	Switch_Simple_AIP_Priorities(new_aip);

	if (new_aip->build_list_classes_loaded_flag)
	{

		if (units_build_list_from_saved_game) {
			Delete_Build_Lists();
			units_build_list_from_saved_game = FALSE;
		}

		build_list_classes = new_aip->build_list_classes;

		build_list_classes_count = new_aip->build_list_classes_count;

	}

	if (new_aip->Wonders_List_loaded_flag)
	{

		if (Wonders_List_from_saved_game)
			delete Wonders_List;
		Wonders_List_from_saved_game = false;
		Wonders_List = new_aip->Wonders_List;
	}
	if (new_aip->Transportation_List_loaded_flag)
	{

		if (Transportation_List_from_saved_game)
			delete Transportation_List;
		Transportation_List_from_saved_game = false;
		Transportation_List = new_aip->Transportation_List;
	}
	if (new_aip->Growth_List_loaded_flag)
	{

		if (Growth_List_from_saved_game)
			delete Growth_List;
		Growth_List_from_saved_game = false;
		Growth_List = new_aip->Growth_List;
	}
	if (new_aip->Happiness_List_loaded_flag)
	{

		if (Happiness_List_from_saved_game)
			delete Happiness_List;
		Happiness_List_from_saved_game = false;
		Happiness_List = new_aip->Happiness_List;
	}
	if (new_aip->Production_List_loaded_flag)
	{

		if (Production_List_from_saved_game)
			delete Production_List;
		Production_List_from_saved_game = false;
		Production_List = new_aip->Production_List;
	}
	if (new_aip->Gold_List_loaded_flag)
	{

		if (Gold_List_from_saved_game)
			delete Gold_List;
		Gold_List_from_saved_game = false;
		Gold_List = new_aip->Gold_List;
	}
	if (new_aip->Science_List_loaded_flag)
	{

		if (Science_List_from_saved_game)
			delete Science_List;
		Science_List_from_saved_game = false;
		Science_List = new_aip->Science_List;
	}
	if (new_aip->Defense_List_loaded_flag)
	{

		if (Defense_List_from_saved_game)
			delete Defense_List;
		Defense_List_from_saved_game = false;
		Defense_List = new_aip->Defense_List;
	}
	if (new_aip->Miscellaneous_List_loaded_flag)
	{

		if (Miscellaneous_List_from_saved_game)
			delete Miscellaneous_List;
		Miscellaneous_List_from_saved_game = false;
		Miscellaneous_List = new_aip->Miscellaneous_List;
	}
	if (new_aip->End_Game_Object_List_loaded_flag)
	{

		if (End_Game_Object_List_from_saved_game)
			delete End_Game_Object_List;
		End_Game_Object_List_from_saved_game = false;
		End_Game_Object_List = new_aip->End_Game_Object_List;
	}
	if (new_aip->Capitalization_List_loaded_flag)
	{

		if (Capitalization_List_from_saved_game)
			delete Capitalization_List;
		Capitalization_List_from_saved_game = false;
		Capitalization_List = new_aip->Capitalization_List;
	}

	if (new_aip->Scientific_Advancements_List_loaded_flag)
	{

		if (Scientific_Advancements_List_from_saved_game)
			delete Scientific_Advancements_List;
		Scientific_Advancements_List_from_saved_game = false;
		Scientific_Advancements_List = new_aip->Scientific_Advancements_List;
	}

	strcpy (name, new_aip->name);
}







int aips_inited = 0;






void AIP::Init (AiMain *init_ai)
{

	int i;
	sint32 num_unit_types;

	aips_inited++;

	build_buildings_priority = 100.0;
	build_troops_priority = 100.0;
	seige_priority = 200000.0;
	explore_priority = 30000.0;
	attack_troops_priority = 200000.0;
	attack_region_priority = attack_troops_priority/2;
	settle_priority = 100000.0;
	defend_priority = 90000.0;
	perimeter_priority = 0.0;
	chokepoint_priority = 10.0;
	patrol_priority = 20.0;
	harass_priority = 20.0;
	bombard_priority = 20.0;
	rustle_priority = 20.0;
	expel_priority = 20.0;
	sally_priority = 20.0;
	retreat_priority = 20.0;
	convert_priority = 20.0;
    bioterror_priority = 20.0;
    nanoattack_priority = 20.0;
	pillage_priority = 20.0;
	pirate_priority = 20.0;
	counter_stealth_priority = 20.0;
	counter_stealth_priority = 20.0;
	bonus_food_priority = 20.0;
	goody_hut_priority = 20.0;
	defuse_mines_priority = 20.0;
	enslave_priority = 20.0;
	plant_nuke_priority = 20.0;
	create_park_priority = 20.0;
	underground_railway_priority = 20.0;
	establish_embassy_priority = 20.0;
	franchising_priority = 20.0;
	assasinate_ruler_priority = 20.0;
	steal_technology_priority = 20.0;
	injoin_priority = 20.0;
	incite_revolution_priority = 20.0;
	cause_unhappiness_priority = 20.0;
	nuke_city_priority = 20.0;
	danger_diminishment = 0.5;
	max_danger_raw_bonus = -10.0;
	reform_city_priority = 20.0;
	sue_franchise_priority = 20.0;
	probe_wormhole_priority = 20.0;
	build_end_game_object_priority = 100.0;
	build_capitalization_priority = 100.0;





	max_threat_raw_bonus = -1.0;
	distance_from_home_priority_modifier = -1000.0;
	distance_from_enemy_priority_modifier = 1.0;
	max_chokepoint_raw_bonus = 500.0;
	random_modifier = 100.0;
	human_target_raw_bonus = 100.0;
	defenseless_city_raw_bonus = 100.0;


	fzbuild_priority_modifier = 1.0;
	fzwonder_priority_modifier = 1.0;
	fzfreight_priority_modifier = 1.0;

	per_capita_modifier = 200.0;

	threat_proximity_modifier = 100.0;

	max_misc_bid_bonus = 10000.0;

	persistence_modifier = 1000;
	distance_from_unit_priority_modifier = -10.0;


	better_unit_type_modifier = 200.0;


	patrol_one_in_n = 10;

    memory_rounds = 10;
    memory_rounds_stealth = 2;

	min_attack_matching_force_ratio = 1.5;
	min_defense_matching_force_ratio = 1.0;
	num_city_defenders = 1;




	for ( i = 0 ; i < GOAL_TYPE_MAX ; i++)
	{

		max_eval_goals_of_type[i] = UNLIMITED_GOALS;
		max_eval_goals_of_type_per_city[i] = UNLIMITED_GOALS;
		max_execute_goals_of_type[i] = UNLIMITED_GOALS;
		max_execute_goals_of_type_per_city[i] = UNLIMITED_GOALS;

	}

	build_list_classes = NULL;

	ai = init_ai;

    name[0] = 0;


	strcpy (name, "NO AIP YET!");


	num_unit_types = ai->m_unitDB->GetNumUnitTypes();

	unit_type_count = num_unit_types;

	desired_units = new semi_dynamic_array<double> (num_unit_types);

	desired_unit_priority =
		new semi_dynamic_array<sorted_array<double, double> *> (num_unit_types);

	for (i = 0; i < num_unit_types; i++)
	{

		(*desired_unit_priority)[i] = new sorted_array<double, double>
			(INITIAL_UNIT_PRIORITY_LIST_SIZE);

	}

	actual_units = new semi_dynamic_array<int> (num_unit_types);

	build_list_classes = NULL;

	Wonders_List = NULL;
	Transportation_List = NULL;
	Growth_List = NULL;
	Happiness_List = NULL;
	Production_List = NULL;
	Gold_List = NULL;
	Science_List = NULL;
	Defense_List = NULL;
	Miscellaneous_List = NULL;
	End_Game_Object_List = NULL;
	Capitalization_List = NULL;

	Scientific_Advancements_List = NULL;

	Wonders_List_Threshhold = 0.5;
	Transportation_List_Threshhold = 0.5;
	Growth_List_Threshhold = 0.5;
	Happiness_List_Threshhold = 0.5;
	Production_List_Threshhold = 0.5;
	Gold_List_Threshhold = 0.5;
	Science_List_Threshhold = 0.5;
	Defense_List_Threshhold = 0.5;
	Miscellaneous_List_Threshhold = 0.5;
	End_Game_Object_List_Threshhold = 0.5;
	Capitalization_List_Threshhold = 0.5;


	units_build_list_from_saved_game = false;
	Wonders_List_from_saved_game = false;
	Transportation_List_from_saved_game = false;
	Growth_List_from_saved_game = false;
	Happiness_List_from_saved_game = false;
	Production_List_from_saved_game = false;
	Gold_List_from_saved_game = false;
	Science_List_from_saved_game = false;
	Defense_List_from_saved_game = false;
	Miscellaneous_List_from_saved_game = false;
	End_Game_Object_List_from_saved_game = false;
	Capitalization_List_from_saved_game = false;
	Scientific_Advancements_List_from_saved_game = false;


	loaded_first_AIP = false;

	alert_to_aip_load = false;

	max_build_building_rounds = 100;
	max_build_unit_rounds = 50;
	max_build_wonder_rounds = 200;

	AI_Verbosity = LOGGING_OFF;

}

















AIP::AIP (AiMain *init_ai,  bool i_dont_delete_ptr_fields)
{

	Init(init_ai);


	dont_delete_ptr_fields = i_dont_delete_ptr_fields;




}













AIP::AIP (AiMain *init_ai, CivArchive &archive)
{

	Init(init_ai);





	Serialize(init_ai, archive);

}





AIP::~AIP()
{









	if (dont_delete_ptr_fields) return;


    if (Wonders_List) {
		delete Wonders_List;
        Wonders_List = NULL;
    }

    if (Transportation_List) {
		delete Transportation_List;
        Transportation_List = NULL;
    }

    if (Growth_List) {
		delete Growth_List;
        Growth_List = NULL;
    }

    if (Happiness_List) {
		delete Happiness_List;
        Happiness_List = NULL;
    }

    if (Production_List) {
		delete Production_List;
        Production_List = NULL;
    }

    if (Gold_List) {
		delete Gold_List;
        Gold_List = NULL;
    }

    if (Science_List) {
		delete Science_List;
        Science_List = NULL;
    }

    if (Defense_List) {
		delete Defense_List;
        Defense_List = NULL;
    }

    if (Miscellaneous_List) {
		delete Miscellaneous_List;
        Miscellaneous_List = NULL;
    }

    if (End_Game_Object_List) {
		delete End_Game_Object_List;
        End_Game_Object_List = NULL;
    }

    if (Capitalization_List) {
		delete Capitalization_List;
        Capitalization_List = NULL;
    }

    if (Scientific_Advancements_List) {
		delete Scientific_Advancements_List;
        Scientific_Advancements_List = NULL;
    }
}

int aip_ptr_allocated = 0;
int aip_ptr_deleted = 0;

AIP_Ptr_Data::AIP_Ptr_Data()
{

    build_list_classes = NULL;
	desired_units = NULL;
    desired_unit_priority = NULL;
    actual_units = NULL;




	Wonders_List = NULL;
	Transportation_List = NULL;
	Growth_List = NULL;
	Happiness_List = NULL;
	Production_List = NULL;
	Gold_List = NULL;
	Science_List = NULL;
	Defense_List = NULL;
	Miscellaneous_List = NULL;
	End_Game_Object_List = NULL;
	Capitalization_List = NULL;
	Scientific_Advancements_List = NULL;

	name[0] = '\0';

    ai = NULL;

	aip_ptr_allocated++;
}






AIP_Ptr_Data::~AIP_Ptr_Data()
{

	sorted_array<double, double> *a_priority_list;
	int i;

	delete desired_units;
    desired_units = NULL;


	for (i = 0; i < unit_type_count; i++)
	{

		a_priority_list = (*desired_unit_priority)[i];

		delete a_priority_list;

	}

	delete desired_unit_priority;
    desired_unit_priority = NULL;

	delete actual_units;
    actual_units = NULL;

	aip_ptr_deleted++;


	if (dont_delete_ptr_fields)
	{


		if (units_build_list_from_saved_game)
			Delete_Build_Lists();

		if (Wonders_List_from_saved_game)
			delete Wonders_List;

		if (Transportation_List_from_saved_game)
			delete Transportation_List;

		if (Growth_List_from_saved_game)
			delete Growth_List;

		if (Happiness_List_from_saved_game)
			delete Happiness_List;

		if (Production_List_from_saved_game)
			delete Production_List;

		if (Gold_List_from_saved_game)
			delete Gold_List;

		if (Science_List_from_saved_game)
			delete Science_List;

		if (Defense_List_from_saved_game)
			delete Defense_List;

		if (Miscellaneous_List_from_saved_game)
			delete Miscellaneous_List;

		if (End_Game_Object_List_from_saved_game)
			delete End_Game_Object_List;

		if (Capitalization_List_from_saved_game)
			delete Capitalization_List;

		if (Scientific_Advancements_List_from_saved_game)
			delete Scientific_Advancements_List;

		return;
	}

	Delete_Build_Lists();

}













void AIP_Ptr_Data::Delete_Build_Lists()
{

	int i;


	if (build_list_classes isnt NULL)
	{

		for (i = build_list_classes_count-1; 0 <= i; i--)
        {

            if ((*build_list_classes)[i].unit_type_list isnt NULL) {
				delete (*build_list_classes)[i].unit_type_list;
                (*build_list_classes)[i].unit_type_list = NULL;
            }

		}

		delete build_list_classes;
        build_list_classes = NULL;
	}

	build_list_classes = NULL;

}






void AIP_Ptr_Data::Load_Building_List
(
	CivArchive &archive,
	semi_dynamic_array<Building_Build_List_Element> **the_list
)
{
	sint32 list_length;
	int i;
	sint32 list_exists = false;

	if (*the_list isnt NULL)
	{
		delete  (*the_list);
		*the_list = NULL;
	}

	list_exists = archive.GetSINT32();

	if (not list_exists) return;

	archive >> list_length;

	*the_list =
		new semi_dynamic_array<Building_Build_List_Element> (list_length);

	for (i = 0; i < list_length; i++)
	{
        (**the_list)[i].Serialize(archive);
	}
}

void AIP_Ptr_Data::Save_Building_List
(
	CivArchive &archive,
	semi_dynamic_array<Building_Build_List_Element> *the_list
)
{
	int i;

	if (the_list is NULL)
	{
        archive.PutSINT32(0);
		return;

	}

	archive.PutSINT32(1);

	archive << (sint32) the_list->Get_Size();
	for (i = 0; i < the_list->Get_Size(); i++)
	{
        (*the_list)[i].Serialize(archive);
	}

}

void AIP_Ptr_Data::Load_Science_List(CivArchive &archive,
	semi_dynamic_array<Advancement_List_Element> **the_list)
{
	sint32 list_length;
	int i;
	sint32 list_exists = false;

	if (*the_list isnt NULL)
	{
		delete  (*the_list);
		*the_list = NULL;
	}

	list_exists = archive.GetSINT32();

	if (not list_exists) return;

	archive >> list_length;

	*the_list =
		new semi_dynamic_array<Advancement_List_Element> (list_length);

	for (i = 0; i < list_length; i++)
	{
		(**the_list)[i].Serialize(archive);
	}
}

void AIP_Ptr_Data::Save_Science_List(CivArchive &archive,
	semi_dynamic_array<Advancement_List_Element> *the_list)
{
	int i;

	if (the_list is NULL)
	{
        archive.PutSINT32(0);
		return;

	}

	archive.PutSINT32(1);

	archive << (sint32) the_list->Get_Size();
	for (i = 0; i < the_list->Get_Size(); i++)
	{
        (*the_list)[i].Serialize(archive);
	}
}









void AIP_Ptr_Data::Serialize(CivArchive &archive)
{
    sint32 i;

    CHECKSERIALIZE

    if (archive.IsStoring()) {
	    archive << build_list_classes_count;

	     for (i = 0; i < build_list_classes_count; i++)
	     {
            Assert(build_list_classes);
            (*build_list_classes)[i].Serialize(archive);
         }





















		Save_Building_List(archive, Wonders_List);
		Save_Building_List(archive, Transportation_List);
		Save_Building_List(archive, Growth_List);
		Save_Building_List(archive, Happiness_List);
		Save_Building_List(archive, Production_List);
		Save_Building_List(archive, Gold_List);
		Save_Building_List(archive, Science_List);
		Save_Building_List(archive, Defense_List);
		Save_Building_List(archive, Miscellaneous_List);
		Save_Building_List(archive, End_Game_Object_List);
		Save_Building_List(archive, Capitalization_List);

		Save_Science_List(archive, Scientific_Advancements_List);


        sint32 n = strlen(name) + 1;
        archive << n;

	    archive.Store((uint8*)name, n);

    } else {

		Delete_Build_Lists();

 	     archive >> build_list_classes_count;

         if (0 < build_list_classes_count) {
             build_list_classes = new semi_dynamic_array<Build_List_Class> (build_list_classes_count);

         }

	     for (i = 0; i < build_list_classes_count; i++)
	     {
             ((*build_list_classes)[i]).Serialize(archive);
         }















































		Load_Building_List(archive, &Wonders_List);
		Load_Building_List(archive, &Transportation_List);
		Load_Building_List(archive, &Growth_List);
		Load_Building_List(archive, &Happiness_List);
		Load_Building_List(archive, &Production_List);
		Load_Building_List(archive, &Gold_List);
		Load_Building_List(archive, &Science_List);
		Load_Building_List(archive, &Defense_List);
		Load_Building_List(archive, &Miscellaneous_List);
		Load_Building_List(archive, &End_Game_Object_List);
		Load_Building_List(archive, &Capitalization_List);

		if (Scientific_Advancements_List isnt NULL)
		{
			delete  (Scientific_Advancements_List);
			Scientific_Advancements_List = NULL;
		}
		Load_Science_List(archive, &Scientific_Advancements_List);


		units_build_list_from_saved_game = true;
		Wonders_List_from_saved_game = true;
		Transportation_List_from_saved_game = true;
		Growth_List_from_saved_game = true;
		Happiness_List_from_saved_game = true;
		Production_List_from_saved_game = true;
		Gold_List_from_saved_game = true;
		Science_List_from_saved_game = true;
		Defense_List_from_saved_game = true;
		Miscellaneous_List_from_saved_game = true;
		End_Game_Object_List_from_saved_game = true;
		Capitalization_List_from_saved_game = true;
		Scientific_Advancements_List_from_saved_game = true;

        sint32 n;
        archive >> n;

	    archive.Load((uint8*)name, n);

    }

}







void AIP::Serialize(AiMain *init_ai, CivArchive &archive)
{
    CHECKSERIALIZE

    AIP_Flat_Data::Serialize(archive);
    AIP_Ptr_Data::Serialize(archive);

    ai = init_ai;
}

















void AIP::Update_Max_Goals()
{

	int i;
	int num_cities;
	bool dont_limit_settle_goals = false;


	num_cities = ai->m_planner->the_strategic_map->Get_Our_City_Count();

	if (num_cities == 0)
	{
		num_cities = 1;

		dont_limit_settle_goals = true;

	}

	for ( i = 0 ; i < GOAL_TYPE_MAX ; i++)
	{






		if (max_eval_goals_of_type_per_city[i] <= (UNLIMITED_GOALS+0.5))
		{
			max_eval_goals_of_type[i] = UNLIMITED_GOALS;
		}
		else if (max_eval_goals_of_type_per_city[i] < 0)
		{
			max_eval_goals_of_type[i] =
				(int) ceil (- max_eval_goals_of_type_per_city[i]);
		}
		else
		{

			max_eval_goals_of_type[i] = (int) ceil
				(max_eval_goals_of_type_per_city[i] * num_cities);

		}







		if (max_execute_goals_of_type_per_city[i] <= (UNLIMITED_GOALS+0.5))
		{
			max_execute_goals_of_type[i] = UNLIMITED_GOALS;
		}
		else if (max_execute_goals_of_type_per_city[i] < 0)
		{
			max_execute_goals_of_type[i] =
				(int) ceil(-max_execute_goals_of_type_per_city[i]);
		}
		else
		{

			max_execute_goals_of_type[i] = (int) ceil
				(max_execute_goals_of_type_per_city[i] * num_cities);

		}
	}

	if (dont_limit_settle_goals)
	{
		max_eval_goals_of_type[GOAL_TYPE_SETTLE] = UNLIMITED_GOALS;
		max_execute_goals_of_type[GOAL_TYPE_SETTLE] = UNLIMITED_GOALS;
	}

}













void AIP::Update_Actual_Units()
{

	int i;
	sint32 num_unit_types;
    ArmyAgent *agent = NULL;
    CityAgent *ca = NULL;
    BSetID id;
	sint32 army_type;


	num_unit_types = ai->m_unitDB->GetNumUnitTypes();

	for (i= 0; i < num_unit_types; i++)
	{

		(*actual_units)[i] = 0;

	}




    for (agent = ai->m_army_set->First(id); ai->m_army_set->Last(); agent = ai->m_army_set->Next(id))
	{
        switch (agent->GetState())
		{
        case AGENT_STATE_MARKED_SETTLE:
        case AGENT_STATE_MARKED_DEAD:

             continue;
        default:

			agent->CountUnitTypes(actual_units);
        }
    }


	for ( ca = ai->m_city_set->First(id);
		  ai->m_city_set->Last();
		  ca = ai->m_city_set->Next(id) )
	{

		army_type = ca->What_Army_Am_I_Building(ai);

		if (army_type != -1)
		{

			(*actual_units)[army_type] = (*actual_units)[army_type]+1;
		}

	}

}












sint32 AIP::Get_Build_List_Classes_Count()
{
	return build_list_classes_count;
};













sint32 AIP::Get_Best_Unit_From_List(const sint32 &build_list_class)
{
	Assert(build_list_class < build_list_classes_count);
	Assert(build_list_class >= 0);

	sint32 build_this_unit;
	sint32 list_class = build_list_class;
	Choose_Unit_From_List(
		(*build_list_classes)[list_class].unit_type_list,
		build_this_unit);

	return build_this_unit;
}













double AIP::Choose_Unit_From_List
(

	semi_dynamic_array<Build_List_Element> * unit_type_list,
	sint32 & build_this_unit
)
{

	int i;
	int can_build;
	double build_this_many = -1.0;


	build_this_unit = -1;

	for (i = 0; i < unit_type_list->Get_Size(); i++)
	{

		if ((*unit_type_list)[i].unittype_ID is -1)
		{

			can_build = false;

			continue;

		}

		can_build =
			ai->m_unitDB->CityCanBuildUnit((*unit_type_list)[i].unittype_ID);


		if (can_build)
		{

			build_this_unit = (*unit_type_list)[i].unittype_ID;

			build_this_many = (*unit_type_list)[i].units_per_city;

			Assert(fabs(build_this_many) < 100);
			if (fabs(build_this_many) > 100.0)
				build_this_many = -1.0;

		}

	}

	return build_this_many;
}














void AIP::Update_Desired_Units()
{

	int i, j;
	int num_cities;
	sint32 num_unit_types;
	sint32 build_this_unit;
	double build_this_many;
	double a_units_priority;


	num_unit_types = ai->m_unitDB->GetNumUnitTypes();

	for (i= 0; i < num_unit_types; i++)
	{

		(*desired_units)[i] = 0;

		((*desired_unit_priority)[i])->Empty();

	}

	num_cities = ai->m_planner->the_strategic_map->Get_Our_City_Count();

	if (num_cities < 1) num_cities = 1;

	if (build_list_classes isnt NULL)
	{

		for (i = 0; i < build_list_classes_count; i++)
		{

			if ((*build_list_classes)[i].unit_type_list isnt NULL)
			{

				build_this_many =
					Choose_Unit_From_List(
						(*build_list_classes)[i].unit_type_list,
						build_this_unit
						);

				Assert(fabs(build_this_many) < 5000);
				if (fabs(build_this_many) > 5000)
					build_this_many = 1;

				if (build_this_unit < 0)
					continue;


				if (build_this_many < 0.0)
				{
					build_this_many = -build_this_many;

				}
				else
				{

					build_this_many *= num_cities;

				}

				if (build_this_many > 0.0)
				{

					(*desired_units)[build_this_unit] += ceil(build_this_many);


					for (j = 0; j < ceil(build_this_many); j++)
					{

						if (j == 0)
							a_units_priority = (*build_list_classes)[i].priority;
						else
						{


							a_units_priority = (*build_list_classes)[i].priority
								+ j * ((*build_list_classes)[i].priority/
										(build_this_many-1));
						}

						((*desired_unit_priority)[build_this_unit])->Add_Data(
							a_units_priority, a_units_priority);

					}

				}

			}

		}

	}

}



















void AIP::Add_Supplemental_Unit_Build_Goals()
{

	sint32 i;
	int j;
	sint32 num_unit_types;
	int discrepancy;
	double build_this_unit_priority;
	Goal_Build_Supplemental * build_this;


	num_unit_types = ai->m_unitDB->GetNumUnitTypes();

	for (i= 0; i < num_unit_types; i++)
	{

		discrepancy =  (int) ((*desired_units)[i] - (*actual_units)[i]);

		if (discrepancy > 0)
		{

			if (fz_no_settling > 0.8)
				{
					if (ai->m_unitDB->IsSettler(i))

						continue;
				}


			for (j = (int) ((*desired_units)[i] - (*actual_units)[i]) - 1; j >= 0; j--)
			{

				build_this_unit_priority =
					((*desired_unit_priority)[i])->Return_Data_By_Number(j);

				build_this = new Goal_Build_Supplemental(ai,  i,
								build_this_unit_priority);

				ai->m_planner->Add_Goal(build_this);

			}

		}

	}


	int free_cities = (int) ceil(
		(ai->m_planner->the_strategic_map->free_city_count + 1) * 1.5);




	if (max_eval_goals_of_type[GOAL_TYPE_BUILD_SUPPLEMENTAL] > free_cities)
	{
		max_eval_goals_of_type[GOAL_TYPE_BUILD_SUPPLEMENTAL] = free_cities;
	}
	if (max_execute_goals_of_type[GOAL_TYPE_BUILD_SUPPLEMENTAL] > free_cities)
	{
		max_execute_goals_of_type[GOAL_TYPE_BUILD_SUPPLEMENTAL] = free_cities;
	}

}














void AIP::Update_City_Distributions
(
	sorted_array<CityAgent *, sint32> *production_rank,
	sorted_array<CityAgent *, sint32> *growth_rank,
	sorted_array<CityAgent *, sint32> *gold_rank,
	sorted_array<CityAgent *, sint32> *science_rank,
	sorted_array<CityAgent *, sint32> *happiness_rank,
	int num_cities

)
{

    CityAgent *the_city = NULL;
    BSetID id;
	sint32 happiness;
	sint32 growth;
	sint32 science;
	sint32 production;
	sint32 gold;


	for ( the_city = ai->m_city_set->First(id);
		  ai->m_city_set->Last();
		  the_city = ai->m_city_set->Next(id) )
	{

		production = the_city->GetGrossProduction();
		growth = the_city->GetGrossGrowth();
		gold = the_city->GetGrossGold();
		science = the_city->GetGrossScience();
		happiness = the_city->GetHappiness();

		production_rank->Add_Data(the_city, production);
		growth_rank->Add_Data(the_city, growth);
		gold_rank->Add_Data(the_city, gold);
		science_rank->Add_Data(the_city, science);
		happiness_rank->Add_Data(the_city, happiness);

		the_city->best_build_bid = 0.0;

	}
}














void AIP::Update_City_Needs
(
	sorted_array<CityAgent *, sint32> *production_rank,
	sorted_array<CityAgent *, sint32> *growth_rank,
	sorted_array<CityAgent *, sint32> *gold_rank,
	sorted_array<CityAgent *, sint32> *science_rank,
	sorted_array<CityAgent *, sint32> *happiness_rank,
	int num_cities

)
{

	CityAgent *the_city = NULL;
	int i;
	double my_rank;
	double my_bid;


	for (i = 0; i < num_cities; i++)
	{

		the_city = production_rank->Return_Data_By_Number(i);

		my_rank = (double) (i) / (double) num_cities;

		if (the_city->HasLotOfStoredProduction() ||
            my_rank >= Wonders_List_Threshhold)
		{
			Add_Supplemental_Building_Build_Goal( the_city, Wonders_List, my_rank,
				BUILD_LIST_WONDER );

			if (my_rank > the_city->best_build_bid) the_city->best_build_bid = my_rank;
		}

		if (my_rank >= Transportation_List_Threshhold)
		{
			Add_Supplemental_Building_Build_Goal( the_city, Transportation_List, my_rank,
				BUILD_LIST_TRANSPORTATION );

			if (my_rank > the_city->best_build_bid) the_city->best_build_bid = my_rank;
		}

		if (my_rank >= Defense_List_Threshhold)
		{
			Add_Supplemental_Building_Build_Goal( the_city, Defense_List, my_rank,
				BUILD_LIST_DEFENSE );

			if (my_rank > the_city->best_build_bid) the_city->best_build_bid = my_rank;
		}

		if (my_rank >= Production_List_Threshhold)
		{
			my_bid = my_rank;

			Add_Supplemental_Building_Build_Goal( the_city, Production_List, my_bid,
				BUILD_LIST_PRODUCTION );

			if (my_bid > the_city->best_build_bid) the_city->best_build_bid = my_bid;
		}

		the_city = growth_rank->Return_Data_By_Number(i);

		if ((1 - my_rank) >= Growth_List_Threshhold)
		{
			my_bid = 1 - my_rank;

			Add_Supplemental_Building_Build_Goal( the_city, Growth_List, my_bid,
				BUILD_LIST_GROWTH );

			if (my_bid > the_city->best_build_bid) the_city->best_build_bid = my_bid;
		}

		the_city = gold_rank->Return_Data_By_Number(i);

		if (my_rank >= Gold_List_Threshhold)
		{
			Add_Supplemental_Building_Build_Goal( the_city, Gold_List, my_bid,
				BUILD_LIST_GOLD );

			if (my_bid > the_city->best_build_bid)
				the_city->best_build_bid = my_bid;
		}

		the_city = science_rank->Return_Data_By_Number(i);

		if (my_rank >= Science_List_Threshhold)
		{
			my_bid = my_rank;

			Add_Supplemental_Building_Build_Goal( the_city, Science_List, my_bid,
				BUILD_LIST_SCIENCE );

			if (my_bid > the_city->best_build_bid) the_city->best_build_bid = my_bid;
		}

		the_city = happiness_rank->Return_Data_By_Number(i);

		if ((1 - my_rank) >= Happiness_List_Threshhold)
		{

			if(	the_city->GetHappiness() < ai->m_gs->GetConstRiotLevel()+1 ||
				the_city->GetNumMusicians() > 0)
			{
				my_bid = 1.0 - my_rank;
				Add_Supplemental_Building_Build_Goal( the_city, Happiness_List,
												  my_bid,
												  BUILD_LIST_HAPPINESS );

				if (my_bid > the_city->best_build_bid)
					the_city->best_build_bid = my_bid;
			}

		}

		my_bid = 1.0 - the_city->best_build_bid;

		if (my_bid >= Miscellaneous_List_Threshhold)
		{
			Add_Supplemental_Building_Build_Goal( the_city, Miscellaneous_List, my_bid,
				BUILD_LIST_MISCELLANEOUS );
		}

		if (my_rank >= End_Game_Object_List_Threshhold)
		{
			my_bid = my_rank;

			Add_Supplemental_Building_Build_Goal( the_city, End_Game_Object_List, my_bid,
				BUILD_LIST_END_GAME_OBJECT);

			if (my_bid > the_city->best_build_bid) the_city->best_build_bid = my_bid;
		}

		if ((1 - my_rank) >= Capitalization_List_Threshhold)
		{
			my_bid = 1.0 - my_rank;

			Add_Supplemental_Building_Build_Goal( the_city, Capitalization_List, my_bid,
				BUILD_LIST_CAPITALIZATION);

			if (my_bid > the_city->best_build_bid) the_city->best_build_bid = my_bid;
		}

	}
}














void AIP::Add_Supplemental_Building_Build_Goal
(
	CityAgent *the_city,
	semi_dynamic_array<Building_Build_List_Element> *the_list,
	double bid_coefficient,
	BUILD_LIST_TYPE which_list
)
{

	int list_pos;
	double priority = BAD_UTILITY;
	int list_length;
	bool found_something_to_build = false;
	Goal_Build_List *new_goal;
	sint32 building_type;
	double gold_bonus;
	double gold_pop_bonus;
	sint16 upkeep;
	double sci_per_pop;
	double sci_bonus;
	sint32 city_science;
	sint16 city_pop;


	if (the_city->GetQueueLen() > 0)
	{
        return;
    }

	Assert(the_list);

	if (!the_list)
		return;
	list_length = the_list->Get_Size();

	list_pos = 0;


	while (( list_pos < list_length) && (not found_something_to_build))
	{

		building_type = (*the_list)[list_pos].building_type_ID;





		if ((*the_list)[list_pos].type is IS_WONDER)
		{

			if (ai->m_wonder_table->CanInsertWonderGoalForCity(ai, the_city, building_type) &&
				the_city->PretestContructWonder(ai, building_type))
			{

				found_something_to_build = true;

				priority = (*the_list)[list_pos].priority;

				new_goal = new Goal_Build_List(ai, building_type, priority, TRUE, FALSE, FALSE, the_city);

				ai->m_planner->the_scheduler->Add_Single_Plan(the_city, new_goal);

				ai->m_planner->Add_Single_Plan_Goal( new_goal);




			}

		}

		else if ((*the_list)[list_pos].type is IS_END_GAME_OBJECT)
			{

			if (the_city->PretestConstructEndGameObject(ai, building_type))
				{

					found_something_to_build = true;

					priority = (*the_list)[list_pos].priority;

					new_goal = new Goal_Build_List(ai, building_type, priority, FALSE, TRUE, FALSE, the_city);

					ai->m_planner->the_scheduler->Add_Single_Plan(the_city, new_goal);

					ai->m_planner->Add_Single_Plan_Goal( new_goal);




				}
			}
		else if ((*the_list)[list_pos].type is IS_CAPITALIZATION)
			{

			if (the_city->PretestConstructCapitalization(ai))
				{

					found_something_to_build = true;

					priority = (*the_list)[list_pos].priority;

					new_goal = new Goal_Build_List(ai, 0, priority, FALSE, FALSE, TRUE, the_city);

					ai->m_planner->the_scheduler->Add_Single_Plan(the_city, new_goal);

					ai->m_planner->Add_Single_Plan_Goal( new_goal);
				}
			}

		else
		{

			if (the_city->Can_Build_Building(ai, building_type))
			{

				if (not (the_city->Already_Has_One(building_type)))
				{

					found_something_to_build = true;

					priority = (*the_list)[list_pos].priority;

					city_pop = (sint16) the_city->GetTotalNumCitizens();

					upkeep = (sint16) ai->m_buildingDB->
						GetGoldUpkeep(building_type);


					if (which_list == BUILD_LIST_SCIENCE)
						{
							sci_per_pop = ai->m_buildingDB->
								GetIncreaseSciencePerPop(building_type);

							sci_bonus = ai->m_buildingDB->
								GetScienceBonus(building_type);

							city_science = the_city->GetGrossScience();

							if ((city_pop * sci_per_pop +
								city_science * sci_bonus) < upkeep / 2)
								{

									found_something_to_build = false;
								}
						}

					if (which_list == BUILD_LIST_GOLD)
						{
							gold_bonus = ai->m_buildingDB->
								GetGoldBonus(building_type)
								* the_city->GetGrossGold();
							gold_pop_bonus = (ai->m_buildingDB->
											  GetGoldPerCitizen(building_type)
											  * city_pop);
							if((gold_bonus + gold_pop_bonus ) >= upkeep)
								{

									priority = priority/2;
								}
						}

					if (!the_city->PretestBuildingBuildTime(ai, building_type))
						{

							double time = the_city->
								EstimateRoundsToBuildBuilding(ai, building_type);
							double ratio =
								(time / ai->m_planner->the_AIP.max_build_building_rounds);
							if (ratio < 2.0)
								priority = priority/2.0;
							else
								priority = priority / (ratio + 8.0);
						}

					if (found_something_to_build)
					{

						new_goal = new Goal_Build_List(ai, building_type, priority, FALSE, FALSE, FALSE, NULL);

						ai->m_planner->the_scheduler->Add_Single_Plan(the_city, new_goal);

						ai->m_planner->Add_Single_Plan_Goal( new_goal);
					}

				}

			}

		}

		list_pos++;

	}

}













void AIP::Update_Building_Build_Lists()
{

	sorted_array<CityAgent *, sint32> *production_rank;
	sorted_array<CityAgent *, sint32> *growth_rank;
	sorted_array<CityAgent *, sint32> *gold_rank;
	sorted_array<CityAgent *, sint32> *science_rank;
	sorted_array<CityAgent *, sint32> *happiness_rank;
	int num_cities;


	num_cities = ai->m_planner->the_strategic_map->Get_Our_City_Count();

	if (num_cities < 1) return;

	production_rank = new sorted_array<CityAgent *, sint32> (num_cities);
	growth_rank = new sorted_array<CityAgent *, sint32> (num_cities);
	gold_rank = new sorted_array<CityAgent *, sint32> (num_cities);
	science_rank=  new sorted_array<CityAgent *, sint32> (num_cities);
	happiness_rank =  new sorted_array<CityAgent *, sint32> (num_cities);


	Update_City_Distributions(production_rank, growth_rank, gold_rank,
		science_rank, happiness_rank, num_cities);

	Update_City_Needs(production_rank, growth_rank, gold_rank,
		science_rank, happiness_rank, num_cities);

	delete production_rank;
    production_rank = NULL;
	delete growth_rank;
    growth_rank = NULL;
	delete gold_rank;
    gold_rank = NULL;
	delete science_rank;
    science_rank = NULL;

	delete happiness_rank;
    happiness_rank = NULL;
}













void AIP::Update()
{

	Update_Max_Goals();

	Update_Actual_Units();

	Update_Desired_Units();

	Add_Supplemental_Unit_Build_Goals();

	Update_Building_Build_Lists();

}















sint32 AIP::Choose_Next_Science()
{

	sint32 list_pos;
	int list_length;
	sint32 the_science;


	if (Scientific_Advancements_List is NULL) return -1;

	list_length = Scientific_Advancements_List->Get_Size();

	list_pos = 0;

	int fail_safe_science = -1;


	while (list_pos < list_length)
	{

		the_science = (*Scientific_Advancements_List)[list_pos].advancement_ID;

		if (!ai->m_science->HasAdvance(the_science))
		{

			if (ai->m_science->GetMinPrerequisites(the_science) is 1)
			{

				if(fail_safe_science is -1)
				{
					fail_safe_science = the_science;
				}

				int cost = ai->m_science->GetCost(the_science);

				double last_turn_science = max(ai->m_gold->GetOldScience(), 1.0);

				double time_to_get_advance = cost / last_turn_science;

				if (time_to_get_advance < fz_max_time_to_research)
				{

					return the_science;
				}

			}

		}

		list_pos++;

	}


	if(fail_safe_science isnt -1)
	{
		return fail_safe_science;

	}

	return -1;

}





void  AIP::GetCurrentAIPname(char *str)
{
    strcpy (str, name);
}
