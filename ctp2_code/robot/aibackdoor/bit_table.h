//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Bit table
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented memory leak when loading from file.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once 
#endif

#ifndef __BIT_TABLE_H__
#define __BIT_TABLE_H__ 1

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <algorithm>    // std::copy

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class Bit_Table;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "civarchive.h" // CivArchive

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class Bit_Table
{ 
	sint32 y_col_len; 
	sint32 m_total_len; 
    sint32 max_x; 
    sint32 max_y; 

	sint32 *m_data;

public:

	Bit_Table()
    :   y_col_len   (0),
        m_total_len (0),
        max_x       (0),
        max_y       (0),
        m_data      (NULL)
    { ; };

	Bit_Table(const Bit_Table & rhs)    // copy constructor
    :   y_col_len   (rhs.y_col_len),
        m_total_len (rhs.m_total_len),
        max_x       (rhs.max_x),
        max_y       (rhs.max_y),
        m_data      (NULL)
	{
		if (rhs.m_data)
        {
            m_data = new sint32[m_total_len];
            std::copy(rhs.m_data, rhs.m_data + static_cast<size_t>(m_total_len), m_data);
        }
	};

    Bit_Table(CivArchive &archive) 
    :   y_col_len   (0),
        m_total_len (0),
        max_x       (0),
        max_y       (0),
        m_data      (NULL)
    { 
        Serialize(archive); 
    };

    ~Bit_Table() 
    { 
		delete [] m_data; 
    } 

	void Resize(const sint32 mx, const sint32 my, const BOOL start_val)
	{
        y_col_len = 1 + (my>>5); 
        m_total_len = mx * y_col_len; 

		delete [] m_data;
		if (m_total_len)
			m_data = new sint32[m_total_len]; 
		else
			m_data = NULL;

        max_x = mx; 
        max_y = my; 

		Reset(start_val);
    } 

	void Reset(const BOOL start_val)
	{
        if (start_val) {
            memset(m_data, 0xffff, m_total_len * sizeof(sint32));
        } else {   
            memset(m_data, 0x0, m_total_len * sizeof(sint32));
        }
	}

    void Serialize (CivArchive &archive) { 
        if (archive.IsStoring()) { 
            archive << y_col_len; 
	        archive << m_total_len; 
            archive << max_x; 
            archive << max_y; 

	        archive.Store((uint8*)m_data, m_total_len *sizeof(sint32)); 
        } else { 
            archive >> y_col_len; 
	        archive >> m_total_len; 
            archive >> max_x; 
            archive >> max_y; 
            
            delete [] m_data;
            m_data = new sint32[m_total_len]; 

	        archive.Load((uint8*)m_data, m_total_len *sizeof(sint32)); 
        } 
    } 

	
	BOOL Get(const sint32 x, const sint32 y) const 
	{ 
        Assert(0 <= x); 
        Assert(x < max_x); 
        Assert(0 <= y); 
        Assert(y < max_y); 
		return m_data[y_col_len * x + (y>>5)] &  (0x1<<(y & 0x1f)); 
	}

	void Set(const sint32 x, const sint32 y, const BOOL v) 
	{ 
            Assert(0 <= x); 
            Assert(x < max_x); 
            Assert(0 <= y); 
            Assert(y < max_y); 
			if (v) { 
				m_data[y_col_len * x + (y>>5)] |=  (0x1<<(y & 0x1f));
			} else {
				m_data[y_col_len * x + (y>>5)] &=  ~(0x1<<(y & 0x1f));
			}
	}

	Bit_Table& operator=(const Bit_Table& rhs) 
	{
        Assert(m_total_len == rhs.m_total_len); 
        Assert(y_col_len == rhs.y_col_len); 
		memcpy (m_data, rhs.m_data, m_total_len * sizeof(sint32));  
        return *this; 
	} 

};


#endif // __BIT_TABLE_H__
