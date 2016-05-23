//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Civilization archive for storing and loading the information to/from savegames
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
// _DEBUG
// - Generate debug version when set.
//
// ARCHIVE_TYPE_CHECK
// linux // Why lower case?
//
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added put and get methods for MBCHAR* (Aug 24th 2005 Martin Gühmann)
// - Removed DoubleUp method. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3types.h"

#include "c3errors.h"

#include "civarchive.h"

#ifndef USE_COM_REPLACEMENT
STDMETHODIMP CivArchive::QueryInterface(REFIID riid, void **obj)
{
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CivArchive::AddRef()
#else
uint32 CivArchive::AddRef()
#endif
{
	return ++m_refCount;
}

#ifndef USE_COM_REPLACEMENT
STDMETHODIMP_(ULONG) CivArchive::Release()
#else
uint32 CivArchive::Release()
#endif
{
	--m_refCount;
	if(m_refCount)
		return m_refCount;

	delete this;
	return 0;
}

#define k_ALLOC_SIZE		((uint32)8096)

CivArchive::CivArchive()
{
	Assert (0 < k_ALLOC_SIZE);
	m_refCount = 0;
	m_pbBaseMemory = new uint8[k_ALLOC_SIZE];
	m_ulAllocated = k_ALLOC_SIZE;
	m_pbInsert = m_pbBaseMemory;
	m_ulLength = 0;
	m_bIsStoring = true;
}

CivArchive::CivArchive(uint32 ulSize)
{
	Assert(0<ulSize);
	m_refCount = 0;
	m_pbBaseMemory = new uint8[ulSize];
	m_ulAllocated = ulSize;
	m_pbInsert = m_pbBaseMemory;
	m_ulLength = ulSize;
	m_bIsStoring = false;
}

CivArchive::~CivArchive()
{
	delete m_pbBaseMemory;
	m_pbBaseMemory = NULL;
}

void CivArchive::SetSize(uint32 ulSize)
{
	delete m_pbBaseMemory ;
	m_pbBaseMemory = NULL;

	Assert(0<ulSize);
	m_pbBaseMemory = new uint8[ulSize] ;
	m_ulAllocated = ulSize ;
	m_pbInsert = m_pbBaseMemory ;
	m_ulLength = ulSize ;
	m_bIsStoring = false ;
}

void CivArchive::DoubleExpand(uint32 ulAmount)
{

	Assert(0 < m_ulAllocated);
	uint32 ulSize = m_ulAllocated;

#ifdef _DEBUG
	sint32 finite_loop=0;
#endif

	sint32 count = 0;

	do
	{
		ulSize *= 2;
		count++;
		Assert(++finite_loop < 100000);
	}
	while ((ulSize <= (m_ulAllocated + ulAmount)) && (count < 31));

	Assert(count < 31);
	if (31 <= count)
	{
		exit(0);
	}
	Assert(0<ulSize);

	uint8* pbData = new uint8[ulSize];
	memcpy(pbData, m_pbBaseMemory, m_ulLength);
	m_pbInsert = pbData + (m_pbInsert - m_pbBaseMemory);
	m_ulAllocated = ulSize;
	delete m_pbBaseMemory;
	m_pbBaseMemory = pbData;
}

void CivArchive::Store(uint8 *pbData, uint32 ulLen)
{
#ifdef ARCHIVE_TYPE_CHECK
TypeCheck(TYPE_CHECK_ARRAY);
#endif ARCHIVE_TYPE_CHECK

	Assert(m_bIsStoring);

	if ((m_ulLength + ulLen) >= m_ulAllocated)
		DoubleExpand(ulLen);

	memcpy(m_pbInsert, pbData, ulLen);
	m_pbInsert += ulLen;
	m_ulLength += ulLen;
}

void CivArchive::Load(uint8 *pbData, uint32 ulLen)
{
#ifdef ARCHIVE_TYPE_CHECK
	TypeCheck(TYPE_CHECK_ARRAY);
#endif ARCHIVE_TYPE_CHECK

#ifdef _DEBUG
	Assert(!m_bIsStoring);
	if (((m_pbInsert - m_pbBaseMemory) + ulLen) > m_ulLength)
	{
		{
			BOOL ARCHIVE_LOAD_MEMORY_ERROR=0;
			Assert(ARCHIVE_LOAD_MEMORY_ERROR);
		}
		exit(0);
	}
#endif
	Assert(pbData);
	memcpy(pbData, m_pbInsert, ulLen) ;
	m_pbInsert += ulLen ;
}

#ifndef HUNT_SERIALIZE

void CivArchive::StoreChunk(uint8 *start, uint8 *end)
{
	Store(start, (end-start));
}

void CivArchive::LoadChunk(uint8 *start, uint8 *end)
{
	Load(start, (end-start));
}

#endif // HUNT_SERIALIZE

enum
{
	TYPE_CHECK_UNKNOWN,
	TYPE_CHECK_SINT8,
	TYPE_CHECK_UINT8,
	TYPE_CHECK_SINT16,
	TYPE_CHECK_UINT16,
	TYPE_CHECK_SINT32,
	TYPE_CHECK_UINT32,
	TYPE_CHECK_SINT64,
	TYPE_CHECK_UINT64,
	TYPE_CHECK_DOUBLE,
	TYPE_CHECK_ARRAY
};

void CivArchive::TypeCheck(const uint8 archive_type_check)
{
	if (m_bIsStoring)
	{
		if ((m_ulLength + 1) >= m_ulAllocated)
			DoubleExpand(1);

		*m_pbInsert = uint8(archive_type_check);
		m_pbInsert++;
		m_ulLength++;
	}
	else
	{
		uint8 val;
		val = *m_pbInsert;
		m_pbInsert++;

		Assert(archive_type_check == val);
	}
}

void CivArchive::PerformMagic(uint32 id)
{
	PutUINT32(k_ARCHIVE_MAGIC_VALUE_1);
	PutUINT32(k_ARCHIVE_MAGIC_VALUE_2);
	PutUINT32(id);
}

void CivArchive::TestMagic(uint32 id)
{
	if (GetUINT32() == k_ARCHIVE_MAGIC_VALUE_1)
		if (GetUINT32() == k_ARCHIVE_MAGIC_VALUE_2)
			if (GetUINT32() == id)
				return;

	{
		BOOL ARCHIVE_MAGIC_DUD=0;
		Assert(ARCHIVE_MAGIC_DUD);
	}

	exit(0);
}

void CivArchive::PutDoubleString(const double &val)
{
	char buf[256];
	sprintf(buf, "%8.8le", val);

#ifndef linux
	if(buf[0] != '-')
	{
		memmove(&buf[12], &buf[13], 3);
	}
	else
	{
		memmove(&buf[13], &buf[14], 3);
	}
#endif
	Store((uint8*)buf, strlen(buf));
}

void CivArchive::ResetForLoad()
{
	m_pbInsert = m_pbBaseMemory;
	SetLoad();
}
