





#include "c3.h"
  
#include "CityList.h"
#include "FlatPtr.h"
#include "civarchive.h"

#include "dynarr.h"
#include "bset.h"

#include "ic3GameState.h"
#include "aimain.h"

#include "CityAgent.h"
#include "FzOut.h"

ZEROMEM(CityListFlat); 
ZEROMEM(CityListPtr); 
ZERODEL(CityListPtr); 
FLATSERIALIZE(CityListFlat); 

int CityVal::operator< (CityVal &c)
{
    return GetVal() < c.GetVal(); 
}

CityList::CityList()
{ 
    m_list = new DynamicArray<CityVal *>; 
}

CityList::CityList(BSet<CityAgent> *city_set, CivArchive &archive)
{ 
    Serialize(city_set, archive); 
} 

CityList::~CityList()
{ 
    delete m_list; 
    m_list = NULL; 
} 


void CityListPtr::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        Store(city_set, archive); 
    } else { 
        Load(city_set, archive); 
    } 
}

void CityListPtr::Store (BSet<CityAgent> *city_set, CivArchive &archive)
{


    








}


void CityListPtr::Load(BSet<CityAgent> *city_set, CivArchive &archive)
{
    


















}



void CityList::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE 

    CityListFlat::Serialize(archive); 
    CityListPtr::Serialize(city_set, archive); 
}




void CityList::HeapSort() 

{ 
    sint32 l=-1, j=-1, ir=-1, i=-1; 
    CityVal* tmp; 

    l = ((m_list->Num()-1) >> 1) + 1; 
    ir = m_list->Num()-1; 

    if (ir < 1) { 
        return; 
    }

    
    
    
    
    for (;;) { 

        if (l > 0) {  
            tmp = m_list->Access(--l); 
        } else { 
            tmp = m_list->Access(ir); 
            m_list->Access(ir) = m_list->Access(0);  
            if (--ir == 0) {                        
                m_list->Access(0)=tmp;                      
                return;                             
            }                                       
        } 


        i = l;              
        if (l) { 
            j = l << 1;         
        } else { 
           j = 1;
        } 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

        while (j <= ir) { 
Assert(++finite_loop < 100000); 
            if ((j < ir) && (*(m_list->Access(j)) < *(m_list->Access(j+1)))) { 
                ++j;                        
            } 

            if (*tmp < *(m_list->Access(j))) {
                m_list->Access(i) = m_list->Access(j); 
                i = j; 
                j = j << 1; 
            }  else { 
                j = ir + 1;  
            }
        }
        m_list->Access(i) = tmp; 
    }
}

void CityList::FindMedianOnlyHappy(sint32 &median, const double fuzzy_horizontal, 
                          const double fuzzy_vertical)

{
    sint32 n; 
    n = m_list->Num(); 

    if (n < 1) {
        m_median = 0; 
        median = m_median; 
        return; 
    } 

    sint32 idx_city; 
    double val_min, val_max = -1.0;

    val_min = m_list->Get(0)->GetVal();

    
    if (k_BIG_TIME <= val_min) { 
        m_median = 0; 
        median = m_median; 
        return; 
    } 

    double v; 
    sint32 idx_max; 
    sint32 m; 
    sint32 happy_cities = 0; 

    
    for (idx_city=0; idx_city<n; idx_city++) { 
        v = m_list->Get(idx_city)->GetVal();

         m = m_list->Access(idx_city)->GetAgent()->GetNumMusicians();
         if (m < 1) {  
             happy_cities++; 
            if (val_max < v) { 
                idx_max = idx_city; 
                val_max = v; 
            }
        }
    } 

    
    if (happy_cities < 2)  { 
        m_median = 0;
        median = m_median; 
        return; 
    }

    
    val_max -= val_min; 

    
    sint32 city_ratio, target_city; 
    double y, diff, dy_min; 
    dy_min = 1000.0; 
    for (idx_city=0; idx_city<n; idx_city++) { 
        y = double(m_list->Get(idx_city)->GetVal() - val_min)/val_max; 
        diff =  fabs(fuzzy_horizontal - y);
        if (diff < dy_min) { 
            dy_min = diff; 
            city_ratio = idx_city; 
        } 
    } 

    
    n = m_list->Num()-1;
    if ((double(city_ratio)/double(n)) < fuzzy_vertical) { 

        
        target_city = sint32(fuzzy_vertical * double(n)); 

        for (idx_city = city_ratio; (idx_city < target_city); idx_city++) { 
            m = m_list->Access(idx_city+1)->GetAgent()->GetNumMusicians();
            if (0 < m) { 
                break; 
            }
 
        }
        m_median = idx_city; 
    } else { 
        m_median = city_ratio; 
    } 
 
    median = m_median; 
}


void CityList::FindMedian(sint32 &median, const double fuzzy_horizontal, 
                          const double fuzzy_vertical)

{
    sint32 n; 
    n = m_list->Num(); 

    if (n <= 1) {
        m_median = 0; 
        median = m_median; 
        return; 
    } 

    sint32 idx_city; 
    double val_min, val_max = -1.0;

    val_min = m_list->Get(0)->GetVal();

    
    if (k_BIG_TIME <= val_min) { 
        m_median = 0; 
        median = m_median; 
        return; 
    } 

    
    val_max = m_list->Get(n-1)->GetVal(); 
    val_max -= val_min; 

    
    sint32 city_ratio; 
    double y, diff, dy_min; 
    dy_min = 1000.0; 
    for (idx_city=0; idx_city<n; idx_city++) { 
		
		
        
		
		
		y = double(m_list->Get(idx_city)->GetVal() - val_min)/
			(val_max + 0.000000001); 


        
		
		
		diff =  fabs(fuzzy_horizontal - y);
		
		
        if (diff < dy_min) { 
            dy_min = diff; 
            city_ratio = idx_city; 
        } 
    } 

    
    n = m_list->Num()-1;
	
	
	
	
    if ((double(city_ratio)/double(n)) < fuzzy_vertical) { 
        
        m_median = sint32(fuzzy_vertical * double(n)); 
    } else { 
        m_median = city_ratio; 
    } 
 
    median = m_median; 
}


sint32 CityList::GetNumCities() const
{ 
    return m_list->Num(); 
} 

CityVal* CityList::GetCity(const sint32 idx_city) const
{
    return m_list->Get(idx_city);
}

void CityList::Clear()
{ 
    sint32 idx_city, num_city; 

    if (m_list) { 
        num_city = m_list->Num(); 
        for (idx_city=0; idx_city<num_city; idx_city++) { 
            delete m_list->Access(idx_city); 
            m_list->Access(idx_city) = NULL; 
        }
        m_list->Clear(); 
    }
}

BOOL CityList::IsAboveMedian(const BSetID &city_id) const
{
    sint32 idx_city, num_city; 
    num_city = m_list->Num(); 

    for(idx_city=m_median+1; idx_city<num_city; idx_city++) { 
        if (city_id == m_list->Get(idx_city)->m_id) { 
            return TRUE; 
        }
    } 
    return FALSE; 
}


BOOL CityList::GetCityIndex(const BSetID &city_id, sint32 &the_index) const
{
    sint32 idx_city, num_city; 
    num_city = m_list->Num(); 

    for(idx_city=0; idx_city<num_city; idx_city++) { 
        if (city_id == m_list->Get(idx_city)->m_id) { 
            the_index = idx_city;
            return TRUE; 
        }
    } 
    the_index = -1;
    return FALSE; 
}

void CityList::CalcAverage()
{
    sint32 idx_city, num_city; 
    num_city = m_list->Num(); 


    double sum = 0.0; 
    for(idx_city=0; idx_city<num_city; idx_city++) { 
        sum += m_list->Get(idx_city)->GetVal(); 
    }
    if (num_city < 1) { 
        m_average = 0.0; 
    } else { 
        m_average = sum / num_city; 
    } 
}

double CityList::GetMin() const
{ 
    return m_list->Get(0)->GetVal(); 
} 




