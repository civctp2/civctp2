#include "C3.h"
#include "Memory_Manager.h"




Pointer_Watch::Pointer_Watch(uint32 *p)
{
    m_ptr = p; 
    m_count=0; 
    m_next=NULL; 
}

Pointer_Watch::~Pointer_Watch()
{
    return; 
} 

Watch_Table::Watch_Table()
{
    sint32 i; 

    for (i=0; i<MM_HASH_TABLE_SIZE; i++) { 
        m_hash_table[i] = NULL;
    } 
}

Watch_Table::~Watch_Table()
{
}



sint32 Watch_Table::Key(uint32 *p)
{
    uint32 bits32 = (uint32)p; 
    uint32 bits16 = bits32 ^ (bits32 >> 16); 
    uint32 k =  0x000003ff & (bits16  ^ (bits16  >> 6)); 
    Assert(0<= k ); 
    Assert(k < MM_HASH_TABLE_SIZE);
    return sint32(k);
}

Pointer_Watch* Watch_Table::GetWatch(uint32 *p)
{ 
    sint32 k = Key(p); 

    Pointer_Watch *w = m_hash_table[k]; 

    while (w) { 
        if (w->m_ptr == p) return w; 
        w = w->m_next; 
    } 
    Assert(0); 
    return NULL;
}
 
void Watch_Table::WatchAnotherPtr(uint32 *p)
{ 
    sint32 k = Key(p); 
    Pointer_Watch *w = new Pointer_Watch(p);     
    w->m_next = m_hash_table[k]; 
    m_hash_table[k] = w; 
} 

void Watch_Table::EventAlloc(uint32 *p)
{
    Pointer_Watch *w = GetWatch(p); 
    if (!w) return; 
    Assert(w->m_count == 0); 
    w->m_count++;  
}

void Watch_Table::EventDealloc(uint32 *p)
{
    Pointer_Watch *w = GetWatch(p); 
    if (!w) return; 
    Assert(w->m_count == 1); 
    w->m_count--;  
}



Memory_Manager::Memory_Manager(const sint32 o_bytes)
{
    m_obj_bytes = o_bytes; 
    Assert(m_obj_bytes  >=4); 
    Assert(m_obj_bytes < (MM_BLOCK_SIZE*4)); 

    m_num_lwords = ((m_obj_bytes % 4) + m_obj_bytes )/4; 

    m_num_obj_per_block = sint32(MM_BLOCK_SIZE / m_num_lwords); 

    m_ready_mem_count=0; 
    m_block_count = 0; 
    m_total_obj_count = 0; 
    m_block_head = NULL;
    m_ready_mem = NULL; 

    m_ref_count = 1; 
    
}

Memory_Manager::~Memory_Manager()
{
    MM_Block_Node *p, *del_me; 

    p = m_block_head; 
#ifdef _DEBUG
    sint32 finite_count_delete_memory_manager=0; 
#endif _DEBUG

    while (p) {  
        Assert(finite_count_delete_memory_manager++ < 100000); 

        del_me = p; 
        p = p->m_next;
        delete[] del_me->m_block; 
        delete del_me; 
        del_me = NULL; 
    } 
}

void Memory_Manager::AddRef()
{ 
    m_ref_count++; 
}

BOOL Memory_Manager::Release()
{
    m_ref_count--; 
    if (m_ref_count<1) { 
        delete this; 
        return TRUE; 
    }  else { 
        return FALSE; 
    } 
}


void Memory_Manager::GetAnotherMemoryBlock()
{
    m_block_count++;
    MM_Block_Node *tmp_block_node = new MM_Block_Node;     
    tmp_block_node->m_block = new uint32[MM_BLOCK_SIZE];
    tmp_block_node->m_next = m_block_head; 
    m_block_head = tmp_block_node; 
   
    
    uint32 *p = m_block_head->m_block; 
    uint32 *old = NULL; 
    sint32 i; 
    for (i=0; i < m_num_obj_per_block; i++) { 
        *p = (uint32)old; 
        old = p; 

#ifdef MM_BULLET_PROOF
        m_watch.WatchAnotherPtr(p); 
#endif MM_BULLET_PROOF
        p += m_num_lwords;
    } 

    Assert(old); 
    m_ready_mem_count += m_num_obj_per_block; 
    m_total_obj_count += m_num_obj_per_block; 
    m_ready_mem = old; 
}

void * Memory_Manager::Alloc()
{
    if (m_ready_mem == NULL) {
        GetAnotherMemoryBlock();
    } 

    m_ready_mem_count--; 
    Assert(0 <= m_ready_mem_count); 
    uint32 * old_ptr = m_ready_mem;
        
    m_ready_mem = (uint32*)(*m_ready_mem);  
    Assert(m_ready_mem != (uint32*)0xcdcdcdcd); 
    Assert(m_ready_mem != (uint32*)0xdfdfdfdf); 

#ifdef _DEBUG
    memset(old_ptr, 0xcd, m_obj_bytes); 
#endif _DEBUG

    Assert(old_ptr); 
#ifdef MM_BULLET_PROOF
    m_watch.EventAlloc(old_ptr); 
#endif

    return old_ptr; 

}

void Memory_Manager::Dealloc(void *del_ptr)
{
    Assert(del_ptr);
#ifdef _DEBUG
    memset(del_ptr, 0xdf, m_obj_bytes); 
#endif _DEBUG


    uint32 *addme = (uint32*)del_ptr;

#ifdef MM_BULLET_PROOF
    m_watch.EventDealloc(addme); 
#endif

    *addme = (uint32)m_ready_mem; 
    m_ready_mem = addme; 

    Assert(m_ready_mem != (uint32*)0xcdcdcdcd); 
    Assert(m_ready_mem != (uint32*)0xdfdfdfdf); 

    m_ready_mem_count++; 
}
