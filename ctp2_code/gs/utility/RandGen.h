#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __RAND_GEN_H__
#define __RAND_GEN_H__ 1

class CivArchive;

#define k_RAND_MBIG 1000000000
#define k_RAND_MSEED 161803398
#define k_RAND_MZ 0
#define k_RAND_FAC (1.0/double(k_RAND_MBIG))

class RandomGenerator;
extern RandomGenerator *g_rand;

class RandomGenerator

{

    sint32 m_start_seed;
	sint32 m_buffer[56];
    sint32 *m_firstp, *m_secondp, *m_endp;
	sint32 m_callCount;

	friend class NetRand;

public:
    RandomGenerator(sint32 seed);
    RandomGenerator(CivArchive &archive);
	RandomGenerator(RandomGenerator &copyme);
    void Initialize(sint32 seed);

	sint32 CallCount() { return m_callCount; }

    sint32 Next();
    sint32 Next(sint32 r) {
		Assert(0 <r);
#ifdef LOG_RAND
		if(this == g_rand) {
			sint32 res = Next() % r;
			DPRINTF(k_DBG_GAMESTATE, ("RandomGenerator::Next(%d) = %d\n",
									  r, res));
			return res;
		}
#endif
		return Next() % r;
	}

    double NextF() { return Next() * k_RAND_FAC; }
    double NextB()
    {   sint32 val = Next();
        return 0x00000001 & (val ^ (val >> 5) ^ (val >> 13) ^ (val >> 19));
    }

    sint32 GetSeed() const { return m_start_seed; }
    void Serialize (CivArchive &archive);
};

extern RandomGenerator *g_rand;

#else

class RandomGenerator;

#endif
