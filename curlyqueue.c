/**
 * @file curlyqueue.c
 * Implementations of functions declared in curlyqueue.h.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curlyqueue.h"

/**
 * Creates new list node.
 * @pre		Calling fn has determined prev and next pointers
 * @post	Mem has been allocated for new node and
 * 		and node field values have been assigned
 * @note	Calling fn must free mem!
 */
curlyqueue_node_t* curlyqueue_create_node( void* value, curlyqueue_node_t* prev, curlyqueue_node_t* next ){
	
	curlyqueue_node_t* node = ( curlyqueue_node_t* )malloc( sizeof( curlyqueue_node_t ) );
	
	node->value	= value;
	node->prev	= prev;
	node->next	= next;
	
	return node;
}

/**
 * Initializes the linked list manager struct.
 * @post	Mem has been allocated for queuea and 
 * 		field valies have been initialized to 
 * 		null (if pointers) or zero (if count)
 * @note	Calling fn must free mem!
 */
curlyqueue_t* curlyqueue_create_queue(){
	
	curlyqueue_t* queue = malloc( sizeof( struct curlyqueue_t ) );
	
	queue->back	= NULL;
	queue->front	= NULL;
	queue->iterator	= NULL;
	queue->count	= 0;
	
	return queue;
}

/** 
 * Rolls through list, freeing resources.
 * @param	queue	the q to destroy
 * @pre		The queue pointer is not NULL
 * @post	All nodes in the list are freed
 * @post	The list manager's pointers are set to null
 * @post	The list manager's count is decremented for each freed node
 */
void curlyqueue_destroy_queue( curlyqueue_t* queue ){
	
	curlyqueue_node_t* curr = queue->back;	/* The current node. */
		
	curlyqueue_node_t* next; /* The node after current node. */
	
	/* while the current node isn't the end of the list*/
	while( curr != NULL ){
		
		next = curr->next;
		
		free( curr );
		
		curr = next;
		
		queue->count--;
	}
	
	
	free( queue );
	
	/* dedangle pointer */
	queue = NULL;
}


/**
 * Creates a node for a given value and adds it to the back of a queue
 * @param	queue	The queue to add the value to
 * @param	value	The value to push onto the q
 * @post	The queue's count is incremented
 * @post	Iterator is set to queue->back (to remove the need for
 * 			remembering if the iter was initalized or not,ie to avoid
 * 			the possibility of a null iter if there are items in list)
 */
void curlyqueue_enqueue( curlyqueue_t* queue, void* value ){
	
	curlyqueue_node_t* prev = NULL;
	curlyqueue_node_t* next = queue->back;
	
	curlyqueue_node_t* node = curlyqueue_create_node( value, prev, next );
	
	/* if this is the first node, the tail = the head*/
	if( curlyqueue_is_empty( queue ) ){
		
		queue->front = node;
		
	}else{
		
		queue->back->prev = node;
		
	}
	
	queue->back = node;
	
	/* increment the list's internal count of nodes */
	queue->count++;	
	
	/* set iter to back with each enqueue */
	curlyqueue_iterator_jump_to_back( queue );
}

/**
 * Pops value of front of q and returns a void pointer to it
 * @throw	empty_q	exception if q is empty
 */
void* curlyqueue_dequeue( curlyqueue_t* queue, except_t* e ){
	
	if( curlyqueue_is_empty( queue ) ){
		/* throw exception */
		e->thrown = 1;
		memcpy( e->type, "empty_q", 8 );
		return NULL;
	}

	/* store the value of the node currently at the front of the q */
	void* value = queue->front->value;

	if( 1 == queue->count ){

		free( queue->front );

		/* dedangle pointers */
		queue->front	= NULL;
		queue->back		= NULL;

	}else{
		curlyqueue_node_t* old_front	= queue->front;
		curlyqueue_node_t* new_front	= queue->front->prev;

		queue->front = new_front;
		new_front->next = NULL;

		free( old_front );		
	}

	queue->count--;

	return value;
}

/**
 * A simple function that checks if a queue
 * is empty or not.
 * 
 * @param	queue The queue to check.
 * 
 * @return	1, if the queue is empty
 * @return 	0 if not empty
 */
int curlyqueue_is_empty( curlyqueue_t* queue ){
	if( 0 == queue->count ){
		return 1;
	}else{
		return 0;
	}
}

/**
 * Point the iterator at the back of the list
 */
void curlyqueue_iterator_jump_to_back( curlyqueue_t* queue ){
	queue->iterator = queue->back;
}

/**
 * Point the iterator at the front of the list
 */
void curlyqueue_iterator_jump_to_front( curlyqueue_t* queue ){
	queue->iterator = queue->front;
}

/**
 * Advances the queue iterator forward one element
 * @throw	null_iter	if iterator is currently 
 * 				pointing at NULL, i.e.
 * 				trying to access iter->next 
 * 				would be bad
 * 
 */
void curlyqueue_iterator_step_forward( curlyqueue_t* queue, except_t* e ){
	
	if( NULL == queue->iterator ){
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return;
	}
	
	queue->iterator = queue->iterator->next; 
}

/**
 * Moves the iterator backward one item
 * @throws	null_iter	if q iterator is null, which can happen
 * 						if it has not yet been init'd, or if
 * 						q is empty
 */
void curlyqueue_iterator_step_backward( curlyqueue_t* queue, except_t* e ){
	
	/* BEGIN: case: iter is null */
	if( NULL == queue->iterator ){
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return;
	}
	/* END: case: iter is null */
	
	/* BEGIN: case: iter not null */
	
	queue->iterator = queue->iterator->prev; 
	
	/* END: case: iter not null */
}

/**
 * Gets the value at the point in the queue indicated by the iterator
 */
void* curlyqueue_get_value_at_iterator( curlyqueue_t* queue, except_t* e ){
	if( NULL == queue->iterator ){
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return NULL;
	}
	
	return queue->iterator->value;
}

/**
 * Tests if the current list element pointed at by the iterator has 
 * an element after it
 * @throws	empty_q		if queue is empty
 * @throws	null_iter	if iterator is null (use iter_reset to fix if q 
 * 						is non-empty)
 * @return	0	if queue does not have next
 * @return	1	if queue does have next
 * @see	curlyqueue_iterator_has_prev
 */
int curlyqueue_iterator_has_next( curlyqueue_t* queue, except_t* e ) {
	if( curlyqueue_is_empty( queue ) ){
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "empty_q", 10 );
		return -1;
	} else if( NULL == queue->iterator ){
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return -1;
	}
	
	if( NULL == queue->iterator->next ) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * Tests if the current list element pointed at by the iterator has 
 * an element before it
 * @throws	empty_q		if queue is empty
 * @throws	null_iter	if iterator is null (use iter_reset to fix if q 
 * 						is non-empty)
 * @return	0	if queue does not have prev
 * @return	1	if queue does have prev
 * @see	curlyqueue_iterator_has_next
 */
int curlyqueue_iterator_has_prev( curlyqueue_t* queue, except_t* e ) {
	if( curlyqueue_is_empty( queue ) ){
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "empty_q", 10 );
		return -1;
	} else if( NULL == queue->iterator ){
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return -1;
	}
	
	if( NULL == queue->iterator->prev ) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * Inserts a value into the q at the position before the node pointed 
 * at by the iter.
 * @post	Mem has been allocated for a new node
 * @post	a node was created,and the value is now in queue 
 * @post	the q iter points at the enew node
 * @note	The calling function is responsible for freeing the memory allocated
 * @throws	null_iter	if iter is null
 */
void curlyqueue_insert_value_before_iterator( curlyqueue_t* queue, void* value, except_t* e ){
	
	/* BEGIN: case - q empty */
	if ( NULL == queue->iterator ) {
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return;
	}
	
	/* BEGIN: case - count == 1 or iter == back */
	if ( ( 1 == queue->count ) || ( queue->iterator == queue->back ) ) {
		/* remember, enq adds element & sets queue->back */
		curlyqueue_enqueue( queue, value );
	}
	/* END: case - count == 1 or iter == back */
	
	/* BEGIN: case - iter points to middle or front */
	else {
		
		/* create node */
		/* if iter != back, & count > 1, iter must have prev */
		curlyqueue_node_t* prev = queue->iterator->prev;	
		curlyqueue_node_t* next = queue->iterator;				
		curlyqueue_node_t* node = curlyqueue_create_node( value, prev, next );
		
		/* insert node in list */
		prev->next = node;
		next->prev = node;
		
		queue->count++;
	}
}

/**
 * Insert the given value after the item pointed at by the q iterator
 */
void curlyqueue_insert_value_after_iterator( curlyqueue_t* queue, void* value, except_t* e ){

	/* BEGIN: case - q empty */
	if ( NULL == queue->iterator ) {
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return;
	}
	/* END: case - q empty */
	
	/* BEGIN: case - count == 1 or iter points to front */
	if ( ( 1 == queue->count ) || ( queue->iterator == queue->front ) ) {
		
		/* create node */
		curlyqueue_node_t* prev = queue->iterator;
		curlyqueue_node_t* next = queue->iterator->next;
		curlyqueue_node_t* node = curlyqueue_create_node( value, prev, next );	
		
		/* insert it and update q front ptr */
		prev->next		= node;	
		
		queue->front	= node;
		
		queue->count++;		
	}
	/* END: case - count == 1 or iter points to front */
	
	/* BEGIN: case - count > 1 & iter does not point to front */
	else {
		/* create node */
		curlyqueue_node_t* prev = queue->iterator;
		curlyqueue_node_t* next = queue->iterator->next;
		curlyqueue_node_t* node = curlyqueue_create_node( value, prev, next );
		
		/* insert it */
		prev->next = node;
		next->prev = node;
		
		queue->count++;		
	}
}

/**
 * Deletes the value pointed at by the iterator, removes
 * the assoc node from the list, and deallocates memory.
 * @throws	null_iter	if iterator is not set to an element
 */
void curlyqueue_delete_value_at_iterator( curlyqueue_t* queue, except_t* e ) {
	
	/* BEGIN: case - iter is uninitialized */
	if ( NULL == queue->iterator ) {
		/* throw */
		e->thrown = 1;
		memcpy( e->type, "null_iter", 10 );
		return;
	}
	/* END: case - iter is uninitialized */
	
	/* BEGIN: case - q count == 1 */
	if ( 1 == queue->count ) {
		{except_t dequeue_exception;dequeue_exception.thrown=0;
			curlyqueue_dequeue( queue, &dequeue_exception );
		}
		
		/* dedangle iterator */
		curlyqueue_iterator_jump_to_back( queue );
		
		return;
	}
	/* END: case - q count == 1 */
	
	/* BEGIN: case - iter points to front */
	else if ( queue->iterator == queue->front ) {
		curlyqueue_node_t *old_front = queue->front;
		curlyqueue_node_t *new_front = queue->front->prev;
		
		queue->front	= new_front;
		new_front->next = NULL;
		
		free( old_front );
		
		queue->count--;
		
		/* bump iterator back to new front */
	    {except_t e2;e2.thrown=0;
	    	curlyqueue_iterator_step_backward( queue, &e2 );
	    	
	    	if( e2.thrown ){
	    		/* throw */
	    		e->thrown = 1;
	    		memcpy( e->type, e2.type, 10 );
	    		return;
	    	}
	    }
	}
	/* END: case - iter points to front */
	
	/* BEGIN: case - iter points to back */
	else if ( queue->iterator == queue->back ) {
		{except_t dequeue_exception;dequeue_exception.thrown=0;
			curlyqueue_dequeue( queue, &dequeue_exception );
		}
		
		/* dedangle iterator */
		curlyqueue_iterator_jump_to_back( queue );
	}
	/* END: case - iter points to back */
	
	/* BEGIN: case - iter points to middle value */
	else {
		curlyqueue_node_t *prev = queue->iterator->prev;
		curlyqueue_node_t *next = queue->iterator->next;
		
		free( queue->iterator );
		
		prev->next = next;
		next->prev = prev;
		
	    {except_t step_back_e;step_back_e.thrown=0;
	    	curlyqueue_iterator_step_backward( queue, &step_back_e );
	    	
	    	if( step_back_e.thrown ){
	    		/* throw */
	    		e->thrown = 1;
	    		memcpy( e->type, step_back_e.type, 10 );
	    		return;
	    	}
	    }
	}
	/* END: case - iter points to middle value */
	
}






