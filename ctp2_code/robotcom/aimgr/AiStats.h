


#ifndef __AiStats_H__
#define __AiStats_H__ 1


class CivArchive; 

#define k_NUM_STATS 4
#define k_NUM_TURNS 100


    
class AiStatsFlat{ 
protected:
    
    sint32 m_num_turns; 
   
public:
	AiStatsFlat(); 
	void Serialize(CivArchive &archive); 
};

class AiStatsPtr { 
protected:

    double **m_buffer; 
    double **m_old_buffer; 
    double **m_tmp; 

public:

	AiStatsPtr(); 
    ~AiStatsPtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};


class AiMain; 

class AiStats : public AiStatsFlat, public AiStatsPtr  { 

public:


    AiStats(); 
    AiStats(CivArchive &archive); 
    ~AiStats(); 
    void Serialize(CivArchive &archive); 



    sint32 GetNumFuzzyVariables();

    void GetFuzzyGraph(sint32 idx_section, sint32 idx_variable, 
        char **label, double *minx, double *maxx, double *miny, double *maxy, 
        sint32 *num_graphs, sint32 *num_x, double ***height, double *defuzz_val);

    void AddTurn(AiMain *ai);
    void Dump(AiMain *ai); 

};

#endif __AiStats_H__


