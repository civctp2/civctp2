
#pragma once

#ifndef  MEMORY_MANAGER
#define  MEMORY_MANAGER 1


#define MM_HASH_TABLE_SIZE 1024 

class Watch_Table; 

class Pointer_Watch {
    uint32 *m_ptr; 
    sint32 m_count; 
    Pointer_Watch *m_next; 

public:

    Pointer_Watch(uint32 *p); 
    ~Pointer_Watch();
    friend Watch_Table; 

}; 

class Watch_Table { 

    Pointer_Watch *m_hash_table[MM_HASH_TABLE_SIZE]; 

    sint32 Key(uint32 *p); 
    Pointer_Watch* GetWatch(uint32 *p);

public:
    Watch_Table();
    ~Watch_Table(); 

    void WatchAnotherPtr(uint32 *p); 
    void EventAlloc(uint32 *p);
    void EventDealloc(uint32 *p);
};

#define MM_BLOCK_SIZE 1024  

struct MM_Block_Node { 
	uint32* m_block; 
	MM_Block_Node* m_next;
};

#define MM_BULLET_PROOF 1

class Memory_Manager { 

#ifdef MM_BULLET_PROOF
    Watch_Table m_watch; 
#endif MM_BULLET_PROOF

    sint32 m_ref_count; 

    sint32 m_obj_bytes;
    sint32 m_num_lwords;
    sint32 m_num_obj_per_block;
    sint32 m_ready_mem_count;
    sint32 m_block_count;
    sint32 m_total_obj_count; 

    MM_Block_Node *m_block_head; 
    uint32 *m_ready_mem; 

public:

	Memory_Manager(const sint32 o_bytes); 
	~Memory_Manager(); 
    void AddRef(); 
    BOOL Release(); 

    void GetAnotherMemoryBlock(); 

	void * Alloc(); 
	void Dealloc(void *ptr); 

}; 



#endif  MEMORY_MANAGER
