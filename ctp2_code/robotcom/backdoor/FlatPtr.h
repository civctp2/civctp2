

#ifndef __FLAT_PTR_H__
#define __FLAT_PTR_H__ 1

#define ZEROMEM(ptr_zero_me) \
ptr_zero_me::ptr_zero_me() { \
    memset(this, 0, sizeof(ptr_zero_me)); \
}; \

#define ZERODEL(ptr_zero_me) \
ptr_zero_me::~ptr_zero_me() { \
    memset(this, 0, sizeof(ptr_zero_me)); \
}; \


#define PTRSERIALIZE(ptr_serialize_class_name) \
void ptr_serialize_class_name::Serialize(CivArchive &archive) \
{\
    uint8 nmask[sizeof(*this)/4]; \
    sint32 pcount;\
    int *ptr;\
    sint32 i;\
    if(archive.IsStoring()) {\
        pcount=(sizeof(*this)/4);\
        ptr=(int*)this;\
        for(i=0;i<pcount;i++) { \
            if(*ptr == NULL){\
                nmask[i] = 0;\
            } else {\
                nmask[i] = 1; \
            } \
            ptr++; \
        } \
        archive << pcount; \
        archive.Store(nmask, pcount); \
        Store(archive); \
    } else  { \
        memset(this, 0, sizeof(*this)); \
        archive >> pcount; \
        Assert(pcount == (sizeof(*this)/4)); \
        archive.Load(nmask, pcount); \
\
        Load(archive, pcount, nmask); \
\
        ptr = (int*)this; \
        for (i=0; i<pcount; i++) { \
            if (nmask[i] == 0) {  \
                Assert(*ptr == NULL); \
            } else {  \
                Assert(*ptr != NULL); \
            }  \
            ptr++; \
        } \
    } \
}

     
#define FLATSERIALIZE(flat_serialize_class_name) \
void flat_serialize_class_name::Serialize(CivArchive &archive) \
{ \
    if (archive.IsStoring()) { \
        archive.Store((uint8*) this, sizeof(*this)); \
    } else { \
        archive.Load((uint8*) this, sizeof(*this)); \
    } \
}



#endif __FLAT_PTR_H__


































































































