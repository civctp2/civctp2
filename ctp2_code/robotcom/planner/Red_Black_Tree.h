













#ifndef __Red_Black_Tree_h__
#define __Red_Black_Tree_h__

#include "common.h"
#include "pointer_pool.h"

#define RED 1
#define BLACK 0

enum traversal_order
{
	FROM_ABOVE = 0,
	FROM_RIGHT,
	FROM_CENTER,
	FROM_LEFT
};







template <class DATA_TYPE, class KEY_TYPE>
class Red_Black_Tree_Node
{
	public:


		
		DATA_TYPE the_data;

		
		KEY_TYPE the_key;
	
		
		Red_Black_Tree_Node * left;
		Red_Black_Tree_Node * right;

		
		Red_Black_Tree_Node * parent;

		
		bool is_red_link;

		
		
		
		
		
		
		inline void Init
		(
			DATA_TYPE i_the_data,		
			KEY_TYPE i_the_key,			
			bool i_is_red,				
			Red_Black_Tree_Node * i_left,
			Red_Black_Tree_Node * i_right
		)
		{
			
			the_data = i_the_data;
			the_key = i_the_key;
			is_red_link = i_is_red;
			left = i_left;
			right = i_right;
		}
		
		
		
		
};



















template <class DATA_TYPE, class KEY_TYPE>
class Red_Black_Tree
{
	public:

		

		
		int count;

		
		
		
		
		
		
		Red_Black_Tree
		( 
			KEY_TYPE min_key,
			DATA_TYPE null_data,
			Pointer_Pool<Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> >  *
				i_Red_Black_Tree_Node_Pool	
		);


		
		
		
		
		
		
		~Red_Black_Tree();


		
		
		
		
		
		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * Insert
		(
			DATA_TYPE the_data,					
			KEY_TYPE the_key					
		);


		
		
		
		
		
		
		
		void Initialize_Reverse_Order_Tree_Traversal();


		
		
		
		
		
		
		DATA_TYPE Return_Next_Reverse_Order_Element();


		
		
		
		
		
		
		void Set_Traversal_Element
		(
			Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *new_current_traversal_node
		);


		
		
		
		
		
		
		
		void Choose_Highest_Traversal_Element
		(
			Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *new_current_traversal_node
		);


		
		
		
		
		
		
		bool Is_Traversal_Done()
		{
			if (count is 0) return true;

			return (head is traversal_node);
		}


		
		
		
		
		
		
		void Release_All_Nodes();


	protected:

		
		
		
		
		
		
		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * Rotate
		(
			KEY_TYPE the_key,
			Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *rotate_node
		);


		
		
		
		
		
		
		void Split
		(
			KEY_TYPE the_key
		);


		

		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * head;

		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * null_node;

		

		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * current_node;	

		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * parent_node;	

		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * grandparent_node;	
		
		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * great_grandparent_node;
		
		

		
		Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> * traversal_node;

		
		traversal_order how_we_got_here;

		
		Pointer_Pool<Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> >  *
			Red_Black_Tree_Node_Pool; 

};













template <class DATA_TYPE, class KEY_TYPE>
Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Red_Black_Tree
(
	KEY_TYPE min_key,
	DATA_TYPE null_data,
	Pointer_Pool<Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> >  *
		i_Red_Black_Tree_Node_Pool	
) 
{
	
	

	
	Red_Black_Tree_Node_Pool = i_Red_Black_Tree_Node_Pool;

	
	null_node = Red_Black_Tree_Node_Pool->Get_Next_Pointer();
	null_node->Init(null_data, min_key, BLACK, NULL, NULL);
	null_node->left = null_node;
	null_node->right = null_node;

	
	head = Red_Black_Tree_Node_Pool->Get_Next_Pointer();
	head->Init(null_data, min_key, BLACK, NULL, null_node);

	
	traversal_node = head;

	
	count = 0;

}













template <class DATA_TYPE, class KEY_TYPE>
Red_Black_Tree<DATA_TYPE, KEY_TYPE>::~Red_Black_Tree() 
{
	
	

	
	Release_All_Nodes();

	
	Red_Black_Tree_Node_Pool->Release_Pointer(head);
	Red_Black_Tree_Node_Pool->Release_Pointer(null_node);


}















template <class DATA_TYPE, class KEY_TYPE>
Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Insert
(
	DATA_TYPE the_data,					
	KEY_TYPE the_key					
) 
{
	
	Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *new_node;
	

	
	current_node = head;
	parent_node = head;
	grandparent_node = head;

	
	while (current_node != null_node)
	{
		
		great_grandparent_node = grandparent_node;
		grandparent_node = parent_node;
		parent_node = current_node;

		
		if (the_key < current_node->the_key) 
			current_node = current_node->left;
		else current_node = current_node->right;

		
		if (current_node->left->is_red_link and 
			current_node->right->is_red_link)
			Split(the_key);

	} 

	
	new_node = current_node = Red_Black_Tree_Node_Pool->Get_Next_Pointer();

	
	current_node->Init( the_data, the_key, RED, null_node, null_node );

	
	if (the_key < parent_node->the_key)
	{
		parent_node->left = current_node;
		current_node->parent = parent_node;
	}
	else
	{
		parent_node->right = current_node;
		current_node->parent = parent_node;
	}

	
	Split(the_key);

	
	head->right->is_red_link = BLACK;

	
	count++;

	
	return new_node;
}














template <class DATA_TYPE, class KEY_TYPE>
void Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Initialize_Reverse_Order_Tree_Traversal() 
{
	
	

	
	traversal_node = head->right;

	
	while (traversal_node->right != null_node)
	{
		
		traversal_node = traversal_node->right;

	} 

	
	how_we_got_here = FROM_ABOVE;
}













template <class DATA_TYPE, class KEY_TYPE>
DATA_TYPE Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Return_Next_Reverse_Order_Element() 
{
	
	bool next_node_found = false;		
	

	
	while (not next_node_found)
	{

		
		if (how_we_got_here is FROM_ABOVE)
		{
			
			if (traversal_node->right isnt null_node)
			{
				
				traversal_node = traversal_node->right;

				
				continue;

			} 

			
			how_we_got_here = FROM_RIGHT;

		} 

		
		if (how_we_got_here is FROM_RIGHT)
		{
			
			next_node_found = true;

			
			how_we_got_here = FROM_CENTER;

			
			break;

		} 

		
		if (how_we_got_here is FROM_CENTER)
		{

			
			if (traversal_node->left isnt null_node)
			{
				
				traversal_node = traversal_node->left;

				
				how_we_got_here = FROM_ABOVE;

				
				continue;

			} 

		} 

		
		if (traversal_node->parent->right is traversal_node)
		{
			
			how_we_got_here = FROM_RIGHT;

		} 
		else
		{
			
			how_we_got_here = FROM_LEFT;

		} 

		
		traversal_node = traversal_node->parent;


	} 

	
	return traversal_node->the_data;
}













template <class DATA_TYPE, class KEY_TYPE>
void Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Set_Traversal_Element
(
	Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *new_current_traversal_node
) 
{
	
	

	
	traversal_node = new_current_traversal_node;

	
	how_we_got_here = FROM_RIGHT;

}














template <class DATA_TYPE, class KEY_TYPE>
void Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Choose_Highest_Traversal_Element
(
	Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *new_current_traversal_node
) 
{
	
	

	
	if (new_current_traversal_node->the_key > traversal_node->the_key)
	{
		
		traversal_node = new_current_traversal_node;

		
		how_we_got_here = FROM_RIGHT;

	} 

}














template <class DATA_TYPE, class KEY_TYPE>
void Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Release_All_Nodes()
{
	
	Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *node_to_free;
	

	
	Initialize_Reverse_Order_Tree_Traversal();

	
	while ((traversal_node isnt head) and (traversal_node isnt null_node))
	{

		
		if (how_we_got_here is FROM_ABOVE)
		{
			
			if (traversal_node->right isnt null_node)
			{
				
				traversal_node = traversal_node->right;

				
				continue;

			} 

			
			how_we_got_here = FROM_RIGHT;

		} 

		
		if (how_we_got_here is FROM_RIGHT)
		{
			
			how_we_got_here = FROM_CENTER;

		} 

		
		if (how_we_got_here is FROM_CENTER)
		{

			
			if (traversal_node->left isnt null_node)
			{
				
				traversal_node = traversal_node->left;

				
				how_we_got_here = FROM_ABOVE;

				
				continue;

			} 

		} 

		
		if (traversal_node->parent->right is traversal_node)
		{
			
			how_we_got_here = FROM_RIGHT;

		} 
		else
		{
			
			how_we_got_here = FROM_LEFT;

		} 

		
		node_to_free = traversal_node;

		
		traversal_node = traversal_node->parent;

		
		if (node_to_free isnt null_node)
		{
			
			Red_Black_Tree_Node_Pool->Release_Pointer(node_to_free);

			
			count--;

		} 

	} 

	
	head->right = null_node;
	head->left = NULL;

}



















template <class DATA_TYPE, class KEY_TYPE>
Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Rotate
(
	KEY_TYPE the_key,
	Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *rotate_node
) 
{
	
	Red_Black_Tree_Node<DATA_TYPE, KEY_TYPE> *child, *grandchild;
	

	
	if (the_key < rotate_node->the_key)
		child = rotate_node->left;
	else 
		child = rotate_node->right;

	
	if (the_key < child->the_key)
	{
		
		grandchild = child->left;

		
		child->left = grandchild->right;
		grandchild->right->parent = child;

		grandchild->right = child;
		child->parent = grandchild;

	} 
	else
	{
		
		grandchild = child->right;

		
		child->right = grandchild->left;
		grandchild->left->parent = child;

		grandchild->left = child;
		child->parent = grandchild;

	} 

	
	if (the_key < rotate_node->the_key)
	{
		rotate_node->left = grandchild;
		grandchild->parent = rotate_node;
	}
	else 
	{
		rotate_node->right = grandchild;
		grandchild->parent = rotate_node;
	}

	
	return grandchild;
}













template <class DATA_TYPE, class KEY_TYPE>
void Red_Black_Tree<DATA_TYPE, KEY_TYPE>::Split
(
	KEY_TYPE the_key
) 
{
	
	

	
	current_node->is_red_link = RED;
	current_node->left->is_red_link = BLACK;
	current_node->right->is_red_link = BLACK;

	
	if (parent_node->is_red_link == RED)
	{
		
		grandparent_node->is_red_link = RED;
		
		
		
		if ((the_key < grandparent_node->the_key)
			!=
			(the_key < parent_node->the_key))
			parent_node = Rotate(the_key, grandparent_node);

		
		current_node = Rotate(the_key, great_grandparent_node);

		
		current_node->is_red_link = BLACK;

	} 

}

























































#endif 