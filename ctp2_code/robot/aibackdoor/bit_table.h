













#pragma once 

#ifndef __BIT_TABLE_H__
#define __BIT_TABLE_H__ 1

class CivArchive; 

class Bit_Table
{ 
	sint32 y_col_len; 
	sint32 m_total_len; 
    sint32 max_x; 
    sint32 max_y; 

	sint32 *m_data;

public:

	Bit_Table()
		{
			m_data = NULL;
		}

	Bit_Table(const Bit_Table & rhs)
		{
			m_data = NULL;
			Resize(rhs.max_x, rhs.max_y,0);
			*this = rhs;
		}

	void Resize(const sint32 mx, const sint32 my, const BOOL start_val)
	{
        y_col_len = 1 + (my>>5); 
        m_total_len = mx * y_col_len; 

		if (m_data != NULL)
			delete m_data;

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

    Bit_Table(CivArchive &archive) { 
        Serialize (archive); 
    } 

    ~Bit_Table() { 
		if (m_data != NULL)
			delete[] m_data; 
        m_data = NULL; 
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


#endif __BIT_TABLE_H__
