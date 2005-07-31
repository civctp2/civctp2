#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __AVL_H__
#define __AVL_H__ 1



typedef enum {AVL_LEFT = 0, AVL_RIGHT = 1, AVL_MID = 2} AVL_INDEX_TYPE;

class MapPoint; 

template <class T> class AVL { 

 

    AVL_INDEX_TYPE m_balance; 

    sint32 rotate_remove_avl(AVL<T> ** node, 
                AVL_INDEX_TYPE a, AVL_INDEX_TYPE b);

    sint32 balance_remove_avl(AVL<T> **node, AVL_INDEX_TYPE a, AVL_INDEX_TYPE b);

    sint32 promote_rightmost_avl(AVL<T> **c, AVL<T> **p, 
                           AVL<T> **dead,  sint32 is_top);

    void rotate_insert_avl(AVL<T> ** node,  
                       AVL_INDEX_TYPE a, AVL_INDEX_TYPE b);

    sint32 balance_insert_avl(AVL<T> **node, AVL_INDEX_TYPE a, AVL_INDEX_TYPE b);

public: 
    
    AVL *m_ptr[2]; 

#ifdef _DEBUG
    sint32 m_queue_idx; 
#endif

    T m_data; 

    friend class AstarQueue; 
    friend class Astar; 
    friend class AVLHeap; 
    
    friend sint32 count_point(AVL<T> *node, const MapPoint  &pos);
    friend sint32 check_for_dups(AVL<T> *cost_tree, AVL<T> *node);

    AVL();
    void InitAVL(); 

    T * GetData() { return &m_data; } 

    sint32 IsLeaf() const { return (m_ptr[0] == NULL) && (m_ptr[1] == NULL) ; } 
    sint32 Insert (AVL<T> *addme, AVL<T> **node);

    sint32 Verify(AVL<T> *node, sint32 &depth);

    sint32 Remove(AVL<T> *delme, AVL<T> **dead,
                AVL<T> **node);


    sint32 RemoveSmallest(AVL<T> **smallest, 
       AVL<T> **node);

    sint32 GetBiggest(AVL<T> **biggest, AVL<T> **node); 

    
    sint32 Search(AVL<T> *matchme, AVL<T> *node, AVL<T> **visited);
    void Clear(); 

#ifdef _DEBUG
    inline sint32 GetPriorityQueueIndex() const { return m_queue_idx; } 
    inline void SetPriorityQueueIndex(const sint32 new_idx) { m_queue_idx = new_idx; } 
#endif
    void GetPos(MapPoint &pos); 

    BOOL operator< (AVL<T> & compare_me) const; 
};


template <class T> AVL<T>::AVL() { 

    InitAVL(); 
}


template <class T> void AVL<T>::InitAVL() { 
    
    m_balance = AVL_MID;
    m_ptr[0] = NULL; 
    m_ptr[1] = NULL; 
#ifdef _DEBUG
    m_queue_idx = -1; 
#endif
}














template <class T>  sint32 AVL<T>::Insert (AVL<T> *addme, AVL<T> **node)

{  
  
   if(*node != NULL) { 
        Assert(!addme->m_data.Identical((*node)->m_data)); 
   }


   if (*node == NULL) { 
        addme->m_balance = AVL_MID; 
        addme->m_ptr[0] = NULL; 
        addme->m_ptr[1] = NULL; 
        *node = addme;  
        return TRUE;  
   } else if (addme->m_data < (*node)->m_data){

        Assert((*node)->m_ptr[AVL_LEFT]  != (*node));
        Assert((*node)->m_ptr[AVL_RIGHT]  != (*node));
      

      if (Insert(addme, &((*node)->m_ptr[AVL_LEFT])) == FALSE) {
         return FALSE;  
      
      }else{  
         return balance_insert_avl(node, AVL_LEFT, AVL_RIGHT);
      }

   }else{

      Assert((*node)->m_ptr[AVL_LEFT] != (*node));
      Assert((*node)->m_ptr[AVL_RIGHT] != (*node));
      
    
    if (Insert(addme, &((*node)->m_ptr[AVL_RIGHT]))== FALSE) {
         return FALSE;  
      }else{  
          return balance_insert_avl(node, AVL_RIGHT, AVL_LEFT);
      }
      

   }
   return 0;
}






































































































template <class T> sint32 AVL<T>::rotate_remove_avl(AVL<T> ** node, 
                AVL_INDEX_TYPE a, AVL_INDEX_TYPE b)

{  AVL *tmp1, *tmp2;
   sint32 flag = TRUE;

   
   tmp1 = (*node)->m_ptr[a];
   if ((tmp1->m_balance == a) || (tmp1->m_balance == AVL_MID)){  
      
      (*node)->m_ptr[a] = tmp1->m_ptr[b];
      tmp1->m_ptr[b] = *node;

      
      if (tmp1->m_balance == AVL_MID){
        (*node)->m_balance = a;
        tmp1->m_balance = b;
        flag = FALSE;   
      }else{
         (*node)->m_balance = AVL_MID;
         tmp1->m_balance = AVL_MID;
      }

      *node = tmp1;


   } else {
      

      
      tmp2 = tmp1->m_ptr[b];
      tmp1->m_ptr[b] = tmp2->m_ptr[a];
      tmp2->m_ptr[a] = tmp1;

      
      (*node)->m_ptr[a] = tmp2->m_ptr[b];
      tmp2->m_ptr[b] = *node;

      
      if (tmp2->m_balance == a){
         (*node)->m_balance = b;
      }else {
         (*node)->m_balance = AVL_MID;
      }

      if (tmp2->m_balance == b){
         tmp1->m_balance = a;
      }else{
         tmp1->m_balance = AVL_MID;
      }
      *node = tmp2;
      tmp2->m_balance = AVL_MID;
   }   
   return flag;  
                  
                  
}

  












template <class T> sint32 AVL<T>::balance_remove_avl(AVL<T> **node, 
                                  AVL_INDEX_TYPE a, AVL_INDEX_TYPE b)
   
{  
   AVL_INDEX_TYPE *m_balance = &((*node)->m_balance);
         
   if (*m_balance == b) {  
      *m_balance = AVL_MID; 
      return TRUE;            
   } else if (*m_balance ==  AVL_MID){
      *m_balance = a;  
      return FALSE;   
   } else if (*m_balance == a) {
      return rotate_remove_avl(node, a, b); 
                     
   }
	_ASSERTE(0);  
   return FALSE;
}


















template <class T> sint32 AVL<T>::promote_rightmost_avl(AVL<T> **c, AVL<T> **p, 
                           AVL<T>  **dead, sint32 is_top)
{
 
   _ASSERTE (c != NULL);
   _ASSERTE (*c != NULL);
   _ASSERTE (p != NULL);
   _ASSERTE (*p != NULL);

   if ((*c)->m_ptr[AVL_RIGHT] == NULL) {    
      *dead = *p;
      *p = *c;
      *c = (*c)->m_ptr[AVL_LEFT]; 
      (*p)->m_balance = (*dead)->m_balance;
      (*p)->m_ptr[AVL_LEFT] = (*dead)->m_ptr[AVL_LEFT];
      (*p)->m_ptr[AVL_RIGHT] = (*dead)->m_ptr[AVL_RIGHT];
      return TRUE;  
   } else {  
      if (promote_rightmost_avl(&(*c)->m_ptr[AVL_RIGHT], 
               p, dead, FALSE) == TRUE){
            if (is_top){
              return balance_remove_avl (&((*p)->m_ptr[AVL_LEFT]), AVL_LEFT, AVL_RIGHT);
            } else {
              return balance_remove_avl (c, AVL_LEFT, AVL_RIGHT);
            }
      } else { 
         return FALSE; 
      }
   }
}

template <class T> sint32 AVL<T>::Verify(AVL<T> *node, sint32 &depth) 

{
   sint32 l=0, r=0; 
   
   if (node == NULL) 
       return TRUE; 

   if (node->m_ptr[0] == NULL) { 
      l = 0; 
   } else { 
      if (Verify(node->m_ptr[0], l) == FALSE) { 
         return FALSE; 
      } 
   }
   
   if (node->m_ptr[1] == NULL) { 
      r = 0; 
   } else { 
      if (Verify(node->m_ptr[1], r) == FALSE) { 
         return FALSE; 
      } 
   }
   
   if (l < r) { 
      if (1 < (r - l)) { 
         Assert(0); 
         return FALSE; 
      } 
      
      if (node->m_balance != AVL_RIGHT) { 
         Assert(0); 

         return FALSE; 
      } 
      
      depth = r + 1; 
      return TRUE;
   } else if (r < l) { 
      
      if (1 < (l- r)){ 
         Assert(0); 

         return FALSE; 
      } 
      
      if (node->m_balance != AVL_LEFT) { 
         Assert(0); 

         return FALSE; 
      } 
      
      depth = l + 1; 
      return TRUE;
   } else { 
      if (node->m_balance != AVL_MID) { 
          Assert(0); 

          return FALSE; 
      }
      
      depth = r + 1; 
      return TRUE; 
   }
}
















template <class T> sint32 AVL<T>::Remove(AVL<T> *delme, 
     AVL<T> **dead, AVL<T> **node)

{
   if ((*node) == NULL) {


      _ASSERTE(0);
		return FALSE;
   } else if (delme->m_data < (*node)->m_data) {
      if (Remove (delme, dead, 
         &((*node)->m_ptr[AVL_LEFT]))){
         return balance_remove_avl(node, AVL_RIGHT, AVL_LEFT);
      }
   } else if ((*node)->m_data < delme->m_data ) {
      if (Remove (delme, dead, 
         &((*node)->m_ptr[AVL_RIGHT]))){
         return balance_remove_avl(node, AVL_LEFT, AVL_RIGHT);
      }
   } else {  

      
      
      if ((*node)->m_ptr[AVL_RIGHT] == NULL){
         *dead = *node;
         *node = (*node)->m_ptr[AVL_LEFT];
         return TRUE;
      } else if ((*node)->m_ptr[AVL_LEFT] == NULL){
         *dead = *node;
         *node = (*node)->m_ptr[AVL_RIGHT];
         return TRUE;
      }  else {
         
         
         
         
         
         if (promote_rightmost_avl(&(*node)->m_ptr[AVL_LEFT], 
             node, dead, TRUE) == TRUE){
             return balance_remove_avl(node, AVL_RIGHT, AVL_LEFT);
         }else{
             return FALSE;
         }     
      }
   }
	return FALSE;
}





















template <class T>  void AVL<T>::rotate_insert_avl(AVL<T> ** node,  
                       AVL_INDEX_TYPE a, AVL_INDEX_TYPE b)
{  AVL *tmp1, *tmp2;

   
   tmp1 = (*node)->m_ptr[a];
   if (tmp1->m_balance == a) {  
      
      (*node)->m_ptr[a] = tmp1->m_ptr[b];
      tmp1->m_ptr[b] = *node;
      (*node)->m_balance = AVL_MID;
      *node = tmp1;
   } else {
      
      

      
      tmp2 = tmp1->m_ptr[b];
      tmp1->m_ptr[b] = tmp2->m_ptr[a];
      tmp2->m_ptr[a] = tmp1;

      
      (*node)->m_ptr[a] = tmp2->m_ptr[b];
      tmp2->m_ptr[b] = *node;
      if (tmp2->m_balance == a){
         (*node)->m_balance = b;
      }else {
         (*node)->m_balance = AVL_MID;
      }

      if (tmp2->m_balance == b){
         tmp1->m_balance = a;
      }else{
         tmp1->m_balance = AVL_MID;
      }
      *node = tmp2;
   }
   (*node)->m_balance = AVL_MID;
}

  
















template <class T> sint32 AVL<T>::balance_insert_avl(AVL<T> **node, 
         AVL_INDEX_TYPE a, AVL_INDEX_TYPE b)
   
{ 
   AVL_INDEX_TYPE *m_balance = &((*node)->m_balance);
         
   if (*m_balance == b) {  
      *m_balance = AVL_MID; 
      return FALSE;            
   } else if (*m_balance ==  AVL_MID){
      *m_balance = a;  
      return TRUE;   
   } else if (*m_balance == a) {
      rotate_insert_avl(node, a, b); 
      return FALSE;                
   }
   _ASSERTE(0);  
	return FALSE;
} 














#if 0

void print_avl(FILE *fout, AVL<T> *node, sint32 c)

{ sint32 i;
   
   if (c == 0)
      fprintf (fout, "------------------------------------\n");

   for (i=0; i<c; i++)
      fprintf (fout, "  ");

   if (t) {
      fprintf (fout, "%6.4f %4d (%3d %3d)",  node->total_cost, 
      node->id, node->pos.x, node->pos.y);
   }  else {
      fprintf (fout, "%4d (%3d %3d) %6.4f ", 
         node->id,  node->pos.x, node->pos.y, node->total_cost);
   }  

   switch (node->m_balance){
   case AVL_LEFT: fprintf (fout, "L\n"); break;
   case AVL_RIGHT: fprintf (fout, "R\n"); break;
   case AVL_MID: fprintf (fout, "-\n"); break ;
   default : fprintf (fout, "m_balance out of bounds\n"); break;
   }

  
   if (node->m_ptr[AVL_RIGHT] == NULL) {
      if (node->m_balance == AVL_RIGHT) {
         fprintf (stderr, "BALANCE ERROR\n");
      }
   }else {
      print_avl(fout, node->m_ptr[AVL_RIGHT], t, c + 1);
   }

   if (node->m_ptr[AVL_LEFT] == NULL) {
      if (node->m_balance == AVL_LEFT) {
         fprintf (stderr, "BALANCE ERROR\n");
      }
   }else {
      print_avl(fout, node->m_ptr[AVL_LEFT], t, c + 1);
   }
}
#endif

template <class T> sint32 AVL<T>::Search(AVL<T> *matchme, 
               AVL<T> *node, AVL<T> **visited)

{  if (node == NULL){
      *visited = NULL;
      return FALSE;
   }else if (matchme->m_data < node->m_data){
      return Search(matchme, node->m_ptr[AVL_LEFT], visited);
   } else if (node->m_data < matchme->m_data){
      return Search(matchme, node->m_ptr[AVL_RIGHT], visited);
   } else {
      *visited = node;
      return TRUE;
   }
}















template <class T> sint32 AVL<T>::RemoveSmallest(AVL<T> **smallest, 
                                    AVL<T> **node)

{  _ASSERTE(node != NULL);

   if ((*node)->m_ptr[AVL_LEFT] == NULL){

      
      *smallest = *node;
      *node = (*node)->m_ptr[AVL_RIGHT];
      return TRUE;  
   } else {

      
      if (RemoveSmallest(smallest,
         &(*node)->m_ptr[AVL_LEFT])){
         
         return balance_remove_avl(node, AVL_RIGHT, AVL_LEFT);
      } else {
         return FALSE;  
      }
   }
}









template <class T> sint32 AVL<T>::GetBiggest(
    AVL<T> **biggest, 
    AVL<T> **node 
    )
{  
    if (NULL == (*node)) { 
        *biggest = NULL; 
        return FALSE; 
    } else if ((*node)->m_ptr[AVL_RIGHT] == NULL){

      
      *biggest = *node;
       return TRUE; 
    } else {

      
      return GetBiggest(biggest,&(*node)->m_ptr[AVL_RIGHT]);
   }
}

template <class T> void AVL<T>::Clear() 
{
    m_data.Clear();
#ifdef _MSC_VER
    m_balance = (AVL_INDEX_TYPE)0xcdcdcdcd; 
    m_ptr[0] = (AVL *)0xcdcdcdcd; 
    m_ptr[1] = (AVL *)0xcdcdcdcd;
#else
    m_balance = AVL_MID;
    m_ptr[0] = 0;
    m_ptr[1] = 0;
#endif
}

template <class T> BOOL AVL<T>::operator< (AVL<T> & compare_me) const
{
    return m_data < (compare_me.m_data); 
} 

template <class T> void AVL<T>::GetPos(MapPoint &pos)
{
    m_data.GetPos(pos); 
}


#endif
