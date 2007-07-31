#pragma once
#ifndef _NET_UTIL_H_
#define _NET_UTIL_H_







#ifdef WIN32
#define NET_UTIL_LITTLE_ENDIAN
#endif

#ifdef NET_UTIL_LITTLE_ENDIAN
inline sint8 getbyte(uint8 *b)
{
	return *(sint8*)b;
}

inline sint16 getshort(uint8* b)
{
	return *(sint16*)b;
}

inline sint32 getlong(uint8* b)
{
	return *(sint32*)b;
}

inline sint64 getlong64(uint8* b)
{
	return *(sint64*)b;
}

inline double getdouble(uint8* b)
{
	return *(double*)b;
}

inline void putbyte(uint8* b, sint8 by)
{
	*b = by;
}

inline void putshort(uint8* b, sint16 s)
{
	*(sint16*)b = s;
}

inline void putlong(uint8* b, sint32 l)
{
	*(sint32*)b = l;
}

inline void putlong64(uint8* b, sint64 l)
{
	*(sint64*)b = l;
}
inline void putdouble(uint8* b, double l)
{
	*(double*)b = l;
}

#define NET_STRLEN strlen

#define PUSHBYTE(x)    {buf[size++] = (x);}
#define PUSHSHORT(x)  {putshort(&buf[size], (x)); size += sizeof(uint16);}
#define PUSHLONG(x)   {putlong(&buf[size], (x)); size += sizeof(uint32);}
#define PUSHLONG64(x) {putlong64(&buf[size], (x)); size += sizeof(uint64);}
#define PUSHDOUBLE(x) {putdouble(&buf[size], (x)); size += sizeof(double);}
#define PUSHID(x) {buf[size++] = ((x) >> 8); buf[size++] = ((x) & 0xff); }
#define PUSHSTRING(x) { uint16 l = ((x) ? (NET_STRLEN(x)) : 0); PUSHSHORT(l); if(l > 0) {memcpy(&buf[size], x, l); size += l;}}

#define PULLBYTE(x)   {x = buf[pos++];}
#define PULLBYTETYPE(x,ty) {x = (ty)buf[pos++];}
#define PULLSHORT(x)  {x = getshort(&buf[pos]); pos += sizeof(uint16);}
#define PULLSHORTTYPE(x, ty)  {x = (ty)getshort(&buf[pos]); pos += sizeof(uint16);}
#define PULLLONG(x)   {x = getlong(&buf[pos]); pos += sizeof(uint32);}
#define PULLLONGTYPE(x,ty) {x = (ty)getlong(&buf[pos]); pos += sizeof(uint32);}
#define PULLLONG64(x) {x = getlong64(&buf[pos]); pos += sizeof(uint64);}
#define PULLDOUBLE(x) {x = getdouble(&buf[pos]); pos += sizeof(double);}
#define PULLID(x) { x = (buf[pos] << 8) | (buf[pos+1]); pos += 2;}
#define PULLSTRING(x) { uint16 l; PULLSHORT(l); if(l > 0) { memcpy(x, &buf[pos], l); (x)[l] = 0; pos += l; }}
#define PULLNEWSTRING(x) { uint16 l; PULLSHORT(l); if(l > 0) {x = new char[l + 1]; memcpy(x, &buf[pos], l); (x)[l] = 0; pos += l;} else { x = NULL; }}

#else



#endif

#endif
