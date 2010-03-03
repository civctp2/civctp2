//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Bit table
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented memory leak when loading from file.
// - Corrected handling of "no data" when reading from file
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once 
#endif

#ifndef BIT_TABLE_H_
#define BIT_TABLE_H_

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <algorithm>    // std::copy, std::fill_n

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

	void Cleanup()
	{
		delete[] m_data;
		m_data      = NULL;
		y_col_len   = 0;
		m_total_len = 0;
		max_x       = 0;
		max_y       = 0;
	}

	void Resize(sint32 mx, sint32 my, bool start_val)
	{
		y_col_len = 1 + (my>>5);
		m_total_len = mx * y_col_len;

		delete [] m_data;
		if (m_total_len > 0)
			m_data = new sint32[m_total_len];
		else
			m_data = NULL;

		max_x = mx; 
		max_y = my; 

		Reset(start_val);
	}

	void Reset(bool start_val)
	{
		if (m_total_len > 0)
		{
			std::fill_n(m_data, m_total_len, (start_val) ? 0xffff : 0);
		}
	}

	void Serialize (CivArchive &archive)
	{
		if (archive.IsStoring())
		{
			archive << y_col_len;
			archive << m_total_len;
			archive << max_x;
			archive << max_y;

			if (m_total_len > 0)
			{
				archive.Store((uint8*)m_data, m_total_len *sizeof(sint32));
			}
		}
		else
		{
			archive >> y_col_len;
			archive >> m_total_len;
			archive >> max_x;
			archive >> max_y;

			delete [] m_data;
			if (m_total_len > 0)
			{
				m_data = new sint32[m_total_len]; 
				archive.Load((uint8*)m_data, m_total_len * sizeof(sint32));
			}
			else
			{
				m_data = NULL;
			}
		}
	}

	BOOL Get(sint32 x, sint32 y) const
	{
		Assert(0 <= x);
		Assert(x < max_x);
		Assert(0 <= y);
		Assert(y < max_y);

		return m_data[y_col_len * x + (y>>5)] &  (0x1<<(y & 0x1f)); // @ToDo: Check wether this is used outside as a bool
	}

	void Set(sint32 x, sint32 y, BOOL v)
	{
		Assert(0 <= x);
		Assert(x < max_x);
		Assert(0 <= y);
		Assert(y < max_y);

		if (v)
		{
			m_data[y_col_len * x + (y>>5)] |=  (0x1<<(y & 0x1f));
		}
		else
		{
			m_data[y_col_len * x + (y>>5)] &=  ~(0x1<<(y & 0x1f));
		}
	}

	Bit_Table& operator=(const Bit_Table& rhs) 
	{
		if (this != &rhs)
		{
			/// @todo Add delete+new for m_data when this assert fails
			Assert(m_total_len == rhs.m_total_len); 
			Assert(y_col_len == rhs.y_col_len);
			if (m_total_len > 0)
			{
				std::copy(rhs.m_data, rhs.m_data + static_cast<size_t>(m_total_len), m_data);
			}
		}

		return *this; 
	}
};

#endif // __BIT_TABLE_H__
