/**
 * @file curlyqueue.h
 * Declares functions and data structures implemented and used, respectively, in curlyqueue.c.
 */
#ifndef CURLYQUEUE_H_
#define CURLYQUEUE_H_

#ifndef NULL
#define NULL	0
#endif

typedef struct	except_t			except_t;
typedef struct	curlyqueue_node_t 	curlyqueue_node_t;
typedef	struct	curlyqueue_t		curlyqueue_t;

/**
 * A datatype to help implement exception handling.
 */
struct except_t {
	int thrown;	/**< Setting to one indicates exception was thrown*/
	char type[10];	/**< A brief description of exception */
};

/**
 * The head, tail, and count of a queue.
 */
struct curlyqueue_t {
	curlyqueue_node_t*	back;		/**< Ptr to back of queue */
	curlyqueue_node_t*	front;		/**< Ptr to front of queue */
	curlyqueue_node_t*	iterator;	/**< Ptr to a node in queue */
	int count;				/**< The number of items in queue*/
};

/**
 * A node in a linked list. 
 */
struct curlyqueue_node_t {
	void*			value;	/**< The value of the node item */
	curlyqueue_node_t* 	prev;	/**< The previos node in the list */
	curlyqueue_node_t* 	next;	/**< The next node in the list */
};

/* BEGIN: method list */

/* BEGIN: queue management */
curlyqueue_t*	curlyqueue_create_queue();
void		curlyqueue_destroy_queue( curlyqueue_t* );

void	curlyqueue_enqueue( curlyqueue_t* queue, void* value );
void*	curlyqueue_dequeue( curlyqueue_t* queue, except_t* e );

int	curlyqueue_is_empty( curlyqueue_t* );
/* END: queue management */

/* BEGIN: iterator */
int curlyqueue_iterator_has_next( curlyqueue_t* queue, except_t* e );
int curlyqueue_iterator_has_prev( curlyqueue_t* queue, except_t* e );

void curlyqueue_iterator_step_forward( curlyqueue_t* queue, except_t* e );
void curlyqueue_iterator_step_backward( curlyqueue_t* queue, except_t* e );

void curlyqueue_iterator_jump_to_front( curlyqueue_t* queue );
void curlyqueue_iterator_jump_to_back( curlyqueue_t* queue );
/* END: iterator */

/* BEGIN: insert, delete, and get value */
void*	curlyqueue_get_value_at_iterator( curlyqueue_t* queue, except_t* e );
void	curlyqueue_delete_value_at_iterator( curlyqueue_t* queue, except_t* e );
void	curlyqueue_insert_value_before_iterator( curlyqueue_t* queue, void* value, except_t* e );
void	curlyqueue_insert_value_after_iterator( curlyqueue_t* queue, void* value, except_t* e );
/* END: insert, delete, and get value */

/* END: method list */

#endif /*CURLYQUEUE_H_*/
