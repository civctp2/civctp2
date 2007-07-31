



#include "c3.h"

#include "AiStats.h"
#include "FlatPtr.h"
#include "CivArchive.h"

#include "DynArr.h"

#include "IC3GameState.h"
#include "AiMain.h"
#include "BSet.h"
#include "AiRndCnt.h"
#include "CityAgent.h"
#include "ArmyAgent.h"
#include "Gold.h"

char g_label[k_NUM_STATS][100] =
{ 
   "production", 
   "gold_incom",
   "food",
   "units",
};

enum AI_GRAPH { 
    AI_GRAPH_PROD = 0, 
    AI_GRAPH_GOLD, 
    AI_GRAPH_FOOD, 
    AI_GRAPH_UNIT
}; 
    
ZEROMEM(AiStatsFlat); 
ZEROMEM(AiStatsPtr); 
ZERODEL(AiStatsPtr); 
FLATSERIALIZE(AiStatsFlat); 
PTRSERIALIZE(AiStatsPtr); 








AiStats::AiStats()
{ 
    sint32 i, j; 

    m_num_turns=0; 

    m_buffer = new double*[k_NUM_STATS]; 
    m_tmp = new double *[10]; 
    for (i=0; i<10; i++) { 
        m_tmp[i] = NULL; 
    } 

    for (i=0; i<k_NUM_STATS; i++) { 
        m_buffer[i] = new double[k_NUM_TURNS]; 
        for (j=0; j<k_NUM_TURNS; j++) { 
            m_buffer[i][j] = 0.0; 
        } 
    } 


    
    
    
    FILE *fin; 
    fin = fopen ("oldaistats.txt", "r");
    if (fin == NULL) { 
        m_old_buffer = NULL; 
        return;
    } 

    m_old_buffer = new double*[k_NUM_STATS]; 
    for (i=0; i<k_NUM_STATS; i++) { 
        m_old_buffer[i] = new double[k_NUM_TURNS]; 
    } 

    double tmp;
    for (i=0; i<k_NUM_TURNS; i++) { 
        for (j=0; j<k_NUM_STATS; j++) { 
            fscanf(fin, "%lf", &tmp);
            m_old_buffer[j][i] = tmp;
        } 
    } 
    fclose (fin); 
}








AiStats::AiStats(CivArchive &archive)
{ 
    Serialize(archive); 
} 







AiStats::~AiStats()
{ 
    sint32 i; 

    for (i=0; i<k_NUM_STATS; i++) { 
        delete m_buffer[i];
        if (m_old_buffer) { 
            delete m_old_buffer[i]; 
        }
    } 
    delete m_buffer;
    m_buffer = NULL; 

    if (m_old_buffer) { 
        delete m_old_buffer; 
        m_old_buffer = NULL; 
    }

    delete m_tmp; 
    m_tmp = NULL; 
} 








void AiStatsPtr::Store (CivArchive &archive)
{
    sint32 i, j; 
    sint32 n ; 

    
    n = k_NUM_TURNS;
    for (i=0; i<k_NUM_STATS; i++) { 
         archive <<n;
        for (j=0; j<n; j++) { 
            archive << m_buffer[i][j]; 
        } 
    } 

    
    if (m_old_buffer == NULL) { 
        n = 0; 
        archive << n; 
    } else {         
        n = k_NUM_TURNS;
        archive << n; 
        for (i=0; i<k_NUM_STATS; i++) { 
             archive <<n;
            for (j=0; j<n; j++) { 
                archive << m_old_buffer[i][j]; 
            } 
        }
    }
}








void AiStatsPtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
    sint32 i, j; 
    sint32 n; 

    
    m_tmp = new double *[10]; 
    for (i=0; i<10; i++) { 
        m_tmp[i] = NULL; 
    } 

    
    m_buffer = new double*[k_NUM_STATS]; 
    for (i=0; i<k_NUM_STATS; i++) { 
        archive >> n;
        m_buffer[i] = new double[n]; 
        for (j=0; j<n; j++) { 
            archive >> m_buffer[i][j]; 
        } 
    } 

    archive >> n; 
    if (n == 0) { 
        m_old_buffer = NULL; 
    } else { 
       m_old_buffer = new double*[k_NUM_STATS]; 
       for (i=0; i<k_NUM_STATS; i++) { 
            archive >> n;
            m_old_buffer[i] = new double[n]; 
            for (j=0; j<n; j++) { 
                archive >> m_buffer[i][j]; 
            } 
       } 
    } 
}







void AiStats::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    AiStatsFlat::Serialize(archive); 
    AiStatsPtr::Serialize(archive); 
}








sint32 AiStats::GetNumFuzzyVariables()
{ 
    return k_NUM_STATS; 
} 










void AiStats::GetFuzzyGraph(
    sint32 idx_section, 
    sint32 idx_variable, 
    char **label, 
    double *minx, 
    double *maxx, 
    double *miny, 
    double *maxy, 
    sint32 *num_graphs, 
    sint32 *num_x,     
    double ***height, 
                        
                        
    double *defuzz_val 
) {

    Assert(0 <= idx_variable); 
    Assert(idx_variable < k_NUM_STATS); 

    
    *label = &g_label[idx_variable][0]; 
    *minx = 0.0; 
    *maxx = k_NUM_TURNS; 
    *miny = 0.0; 

    switch  (idx_variable) { 
    case AI_GRAPH_PROD: *maxy = 600.0; break; 
    case AI_GRAPH_GOLD: *maxy = 200.0; break; 
    case AI_GRAPH_FOOD: *maxy = 600.0; break; 
    case AI_GRAPH_UNIT: *maxy = 5000.0; break; 
    default:
        INSANE(FUZZY_GRAPH_UNKNOWN_VAR);
        *maxx = 1.0; 
    } 

    if (m_old_buffer == NULL) { 
        *num_graphs = 1;
    } else { 
        *num_graphs = 2; 
    } 

    *num_x = k_NUM_TURNS; 

   
    if (m_old_buffer) { 
        m_tmp[0] = m_old_buffer[idx_variable]; 
        m_tmp[1] = m_buffer[idx_variable]; 
    } else { 
        m_tmp[0] = m_buffer[idx_variable]; 
        m_tmp[1] = NULL; 
    }

    *height = m_tmp; 
    *defuzz_val = *num_x;     
}







void AiStats::AddTurn(AiMain *ai)

{ 
    sint32 r = ai->m_round_count->GetRound(); 

    if (k_NUM_TURNS <= r) { 
        return; 
    } 

    BSetID id; 
    CityAgent *ca; 
    sint32 p_tot = 0, g_tot = 0, f_tot = 0;
    sint32 p, g, f; 
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) { 
        ca->GetGross(p, g, f);
        p_tot += p; 
        g_tot += g; 
        f_tot += f; 
    }    

    m_buffer[0][r] = p_tot; 
    m_buffer[1][r] = g_tot; 
    m_buffer[2][r] = f_tot; 

   ArmyAgent *agent=NULL; 
   double m=0; 
   for (agent = ai->m_army_set->First(id); ai->m_army_set->Last(); agent = ai->m_army_set->Next(id)) { 
        m += agent->GetActualCost(ai); 
   } 

   m_buffer[3][r]= m; 
} 







void AiStats::Dump(AiMain *ai)

{
    sint32 r = ai->m_round_count->GetRound(); 
    sint32 i, j; 
    FILE *fout; 

    fout = fopen("logs\\aistats.txt", "w"); 
    for (i=0; i<r ; i++) { 
        for (j=0; j<k_NUM_STATS; j++) { 
           fprintf (fout, "%f\t", m_buffer[j][i]); 
        } 
        fprintf (fout, "%\n"); 
    } 

    for (i=r; i<k_NUM_TURNS; i++) { 
        for (j=0; j<k_NUM_STATS; j++) { 
           fprintf (fout, "0.0\t"); 
        } 
        fprintf (fout, "%\n"); 
    } 
    fclose (fout); 
}



