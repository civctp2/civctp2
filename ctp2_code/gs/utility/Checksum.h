#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__ 1

typedef class CheckSum {

	uint32 m_buffer[16];
	uint8 *m_p1, *m_end_ptr, *m_pad_end_ptr;
    uint32 m_stateA, m_stateB, m_stateC, m_stateD;

  	sint32 m_total;

public:

	CheckSum();
	inline uint32 F(const uint32 x, const uint32 y, const uint32 z) const;
	inline uint32 G(const uint32 x, const uint32 y, const uint32 z) const;
	inline uint32 H(const uint32 x, const uint32 y, const uint32 z) const;
	inline uint32 I(const uint32 x, const uint32 y, const uint32 z) const;
	uint32 CircleShift (uint32 val, const uint32 s) const;
	void FF (uint32 &i, const uint32 j, const uint32 k, const uint32 l,
        const uint32 val, const uint32 s, const uint32 t) const;
	void GG(uint32 &i, const uint32 j, const uint32 k, const uint32 l,
        const uint32 val, const uint32 s, const uint32 t) const;
	void HH(uint32 &i, const uint32 j, const uint32 k, const uint32 l,
        const uint32 val, const uint32 s, const uint32 t) const;
	void II(uint32 &i, const uint32 j, const uint32 k, const uint32 l,
        const uint32 val, const uint32 s, const uint32 t) const;

	void Hash ();
	void AddData(void * val, const sint32 byte_count);
	void Done(uint32 &a, uint32 &b, uint32 &c, uint32 &d);
} Checksum;

#endif
