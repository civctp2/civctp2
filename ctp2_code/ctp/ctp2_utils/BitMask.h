
#ifndef BIT_MASK_H__
#define BIT_MASK_H__

#include "CivArchive.h"

#define SIZE_IN_BYTES(bitSize) ((bitSize + 7) / 8)

class BitMask {
  private:
	uint8* m_bytes;
	sint32 m_sizeInBits;

  public:
	BitMask(sint32 bitSize) {
		m_bytes = new uint8[SIZE_IN_BYTES(bitSize)];
		m_sizeInBits = bitSize;

		
		memset(m_bytes, 0, SIZE_IN_BYTES(m_sizeInBits) * sizeof(uint8));
	}

	BitMask(CivArchive &archive) {
		Serialize(archive);
	}

	~BitMask() {
		delete [] m_bytes;
	}

	void Serialize(CivArchive &archive) {
		if(archive.IsStoring()) {
			archive << m_sizeInBits;
			archive.Store(m_bytes, SIZE_IN_BYTES(m_sizeInBits));
		} else {
			archive >> m_sizeInBits;
			m_bytes = new uint8[SIZE_IN_BYTES(m_sizeInBits)];
			archive.Load(m_bytes, SIZE_IN_BYTES(m_sizeInBits));
		}
	}

	void SetBit(sint32 bit) {
		Assert(bit >= 0);
		Assert(bit < m_sizeInBits);
		if(bit < 0 || bit >= m_sizeInBits)
			return;

		sint32 byte = bit / 8;
		m_bytes[byte] |= (1 << (bit % 8));
	}
	
	void ClearBit(sint32 bit) {
		Assert(bit >= 0);
		Assert(bit < m_sizeInBits);
		if(bit < 0 || bit >= m_sizeInBits)
			return;

		sint32 byte = bit / 8;

		m_bytes[byte] &= ~(1 << (bit % 8));
	}

	bool GetBit(sint32 bit) {
		Assert(bit >= 0);
		Assert(bit < m_sizeInBits);
		if(bit < 0 || bit >= m_sizeInBits)
			return false;

		sint32 byte = bit / 8;

		return (m_bytes[byte] & (1 << (bit % 8))) != 0;
	}

	bool AllBitsSet() {
		sint32 i;
		for(i = 0; i < SIZE_IN_BYTES(m_sizeInBits) - 1; i++) {
			if(m_bytes[i] != 0xff)
				return false;
		}

		if(!(m_sizeInBits % 8))
			return m_bytes[SIZE_IN_BYTES(m_sizeInBits) - 1] == 0xff;

		
		if(m_bytes[SIZE_IN_BYTES(m_sizeInBits) - 1] == (0xff >> (8 - (m_sizeInBits % 8))))
			return true;
		return false;
	}
		   
};


#endif
