

#pragma once
#ifndef __TAX_RATE_H__
#define __TAX_RATE_H__ 1

#include "CivArchive.h"


#define k_TAXRATE_VERSION_MAJOR	0									
#define k_TAXRATE_VERSION_MINOR	0									

class TaxRate { 

    
	
	double m_science;
	double m_science_before_anarchy;
	
	

	
	

	
	
public: 
	TaxRate () { 
		m_science = 0.0; 
		m_science_before_anarchy = 0.0;
	}

	void SetTaxRateBeforeAnarchy(double s, sint32 owner) { m_science_before_anarchy = s; }
	double GetScienceBeforeAnarchy() { return m_science_before_anarchy; }
	void SetTaxRates(double s, sint32 owner);
	void InitTaxRates(double s, sint32 owner);
	void GetScienceTaxRate(double &s) 
	{ 	s = m_science; 
		Assert(0.0 <= s); 
        Assert(s <= 1.0); 

	}

	void Serialize(CivArchive &archive) ;
}; 

uint32 TaxRate_TaxRate_GetVersion(void) ;
#endif
