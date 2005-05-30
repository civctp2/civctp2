 
#include "c3.h"
#include "bset.h"
#include "Ic3CivArchive.h"
#include "civarchive.h"

BSetID::BSetID ()

{ 
    val = 0; 
} 

#ifdef WIN32
BSetID::BSetID (const int &copy_me)
{ 
    val = uint32(copy_me); 
}
#endif

BSetID::BSetID (const uint32 &copy_me)
{ 
    val = copy_me; 
} 

BSetID::BSetID (const sint32 &copy_me)
{ 
    val = uint32(copy_me); 
} 

BSetID::BSetID (const BSetID &copy_me)
{ 
    val = copy_me.val;
}

#ifdef WIN32
BSetID& BSetID::operator = (const int &copy_me)
{
    val = uint32(copy_me); 
    return *this;
}
#endif

BSetID& BSetID::operator= (const uint32 &copy_me)
{
    val = copy_me; 
    return *this; 
}

BSetID& BSetID::operator= (const sint32 &copy_me)
{ 
    val = uint32(copy_me); 
    return *this; 

} 

BSetID& BSetID::operator= (const BSetID &copy_me)
{
    val = copy_me.val; 
    return *this; 

}

BSetID& BSetID::operator-- ()

{ 
    val--; 
    return *this;
}

BSetID& BSetID::operator++ ()
{ 
    val++;
    return *this;

}

BOOL BSetID::operator== (const BSetID &test_me) const
{
    return val == test_me.val; 
}

BOOL BSetID::operator!= (const BSetID &test_me) const
{
    return val != test_me.val; 
}

BOOL BSetID::operator< (const BSetID &test_me) const
{ 
    return val < test_me.val; 
} 

BOOL BSetID::operator<= (const BSetID &test_me) const
{ 
    return val <= test_me.val;
} 

BOOL BSetID::operator> (const BSetID &test_me) const
{ 
    return val > test_me.val; 
}

BOOL BSetID::operator>= (const BSetID &test_me) const
{ 
    return val >= test_me.val; 
} 

void BSetID::Castrate()
{ 
    return ; 
} 

void BSetID::DelPointers()
{
    return; 
}

BSetID::BSetID (IC3CivArchive *a)
{
    Serialize(a); 
}


void BSetID::Serialize(CivArchive &archive)
{ 
    if (archive.IsStoring()) { 
        archive << val; 
    } else { 
        archive >> val; 
    }
} 


void BSetID::Serialize(IC3CivArchive *archive)
{ 
    if (archive->IsStoring()) { 
        archive->Store((uint8*)&val, sizeof(val)); 
    } else { 
        archive->Load((uint8*)&val, sizeof(val)); 
    }
} 




uint32  g_bf_table[256] = {
0x00000000,
0x00000080,
0x00000040,
0x000000c0,
0x00000020,
0x000000a0,
0x00000060,
0x000000e0,
0x00000010,
0x00000090,
0x00000050,
0x000000d0,
0x00000030,
0x000000b0,
0x00000070,
0x000000f0,
0x00000008,
0x00000088,
0x00000048,
0x000000c8,
0x00000028,
0x000000a8,
0x00000068,
0x000000e8,
0x00000018,
0x00000098,
0x00000058,
0x000000d8,
0x00000038,
0x000000b8,
0x00000078,
0x000000f8,
0x00000004,
0x00000084,
0x00000044,
0x000000c4,
0x00000024,
0x000000a4,
0x00000064,
0x000000e4,
0x00000014,
0x00000094,
0x00000054,
0x000000d4,
0x00000034,
0x000000b4,
0x00000074,
0x000000f4,
0x0000000c,
0x0000008c,
0x0000004c,
0x000000cc,
0x0000002c,
0x000000ac,
0x0000006c,
0x000000ec,
0x0000001c,
0x0000009c,
0x0000005c,
0x000000dc,
0x0000003c,
0x000000bc,
0x0000007c,
0x000000fc,
0x00000002,
0x00000082,
0x00000042,
0x000000c2,
0x00000022,
0x000000a2,
0x00000062,
0x000000e2,
0x00000012,
0x00000092,
0x00000052,
0x000000d2,
0x00000032,
0x000000b2,
0x00000072,
0x000000f2,
0x0000000a,
0x0000008a,
0x0000004a,
0x000000ca,
0x0000002a,
0x000000aa,
0x0000006a,
0x000000ea,
0x0000001a,
0x0000009a,
0x0000005a,
0x000000da,
0x0000003a,
0x000000ba,
0x0000007a,
0x000000fa,
0x00000006,
0x00000086,
0x00000046,
0x000000c6,
0x00000026,
0x000000a6,
0x00000066,
0x000000e6,
0x00000016,
0x00000096,
0x00000056,
0x000000d6,
0x00000036,
0x000000b6,
0x00000076,
0x000000f6,
0x0000000e,
0x0000008e,
0x0000004e,
0x000000ce,
0x0000002e,
0x000000ae,
0x0000006e,
0x000000ee,
0x0000001e,
0x0000009e,
0x0000005e,
0x000000de,
0x0000003e,
0x000000be,
0x0000007e,
0x000000fe,
0x00000001,
0x00000081,
0x00000041,
0x000000c1,
0x00000021,
0x000000a1,
0x00000061,
0x000000e1,
0x00000011,
0x00000091,
0x00000051,
0x000000d1,
0x00000031,
0x000000b1,
0x00000071,
0x000000f1,
0x00000009,
0x00000089,
0x00000049,
0x000000c9,
0x00000029,
0x000000a9,
0x00000069,
0x000000e9,
0x00000019,
0x00000099,
0x00000059,
0x000000d9,
0x00000039,
0x000000b9,
0x00000079,
0x000000f9,
0x00000005,
0x00000085,
0x00000045,
0x000000c5,
0x00000025,
0x000000a5,
0x00000065,
0x000000e5,
0x00000015,
0x00000095,
0x00000055,
0x000000d5,
0x00000035,
0x000000b5,
0x00000075,
0x000000f5,
0x0000000d,
0x0000008d,
0x0000004d,
0x000000cd,
0x0000002d,
0x000000ad,
0x0000006d,
0x000000ed,
0x0000001d,
0x0000009d,
0x0000005d,
0x000000dd,
0x0000003d,
0x000000bd,
0x0000007d,
0x000000fd,
0x00000003,
0x00000083,
0x00000043,
0x000000c3,
0x00000023,
0x000000a3,
0x00000063,
0x000000e3,
0x00000013,
0x00000093,
0x00000053,
0x000000d3,
0x00000033,
0x000000b3,
0x00000073,
0x000000f3,
0x0000000b,
0x0000008b,
0x0000004b,
0x000000cb,
0x0000002b,
0x000000ab,
0x0000006b,
0x000000eb,
0x0000001b,
0x0000009b,
0x0000005b,
0x000000db,
0x0000003b,
0x000000bb,
0x0000007b,
0x000000fb,
0x00000007,
0x00000087,
0x00000047,
0x000000c7,
0x00000027,
0x000000a7,
0x00000067,
0x000000e7,
0x00000017,
0x00000097,
0x00000057,
0x000000d7,
0x00000037,
0x000000b7,
0x00000077,
0x000000f7,
0x0000000f,
0x0000008f,
0x0000004f,
0x000000cf,
0x0000002f,
0x000000af,
0x0000006f,
0x000000ef,
0x0000001f,
0x0000009f,
0x0000005f,
0x000000df,
0x0000003f,
0x000000bf,
0x0000007f,
0x000000ff
};

uint32 BitsFlip(uint32 val)

{ 
	uint32 tmp = 0; 

	tmp = g_bf_table[val & 0xff] << 24;
	tmp |= g_bf_table[(val >> 8) & 0xff] << 16; 
	tmp |= g_bf_table[(val >> 16) & 0xff] << 8; 
	tmp |= g_bf_table[(val >> 24) & 0xff]; 

	return tmp;  
}