#ifndef __LIST_H__
#define __LIST_H__


#include <stddef.h>

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)


// list node to link the *real* node into list
struct list_head {
	struct list_head *next_ptr, *prev_ptr;
};

// check whether the list is empty (contains only one pseudo list node)
#define list_empty(list) ((list)->next_ptr == (list))

// get the *real* node from the list node
#define list_entry(ptr, type, member) \
		(type *)((char *)ptr - offsetof(type, member))

#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next_ptr, type, member)

#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev_ptr, type, member)

#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev_ptr, typeof(*(pos)), member)

#define list_next_entry(pos, member) \
	list_entry((pos)->member.next_ptr, typeof(*(pos)), member)

// iterate the list 
#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next_ptr, typeof(*pos), member); \
        	&pos->member != (head); \
        	pos = list_entry(pos->member.next_ptr, typeof(*pos), member)) 

// iterate the list safely, during which node could be added or removed in the list
#define list_for_each_entry_safe(pos, q, head, member) \
    for (pos = list_entry((head)->next_ptr, typeof(*pos), member), \
	        q = list_entry(pos->member.next_ptr, typeof(*pos), member); \
	        &pos->member != (head); \
	        pos = q, q = list_entry(pos->member.next_ptr, typeof(*q), member))


#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_last_entry(head, typeof(*pos), member);		\
	     &pos->member != (head); 					\
	     pos = list_prev_entry(pos, member))



/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) 		\
	for (pos = list_next_entry(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = list_prev_entry(pos, member);			\
	     &pos->member != (head);					\
	     pos = list_prev_entry(pos, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = list_next_entry(pos, member), 				\
		n = list_next_entry(pos, member);				\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = list_next_entry(pos, member);					\
	     &pos->member != (head);						\
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_last_entry(head, typeof(*pos), member),		\
		n = list_prev_entry(pos, member);			\
	     &pos->member != (head); 					\
	     pos = n, n = list_prev_entry(n, member))

// initialize the list head
static inline void init_list_head(struct list_head *list)
{
	list->next_ptr = list->prev_ptr = list;
}

// insert a new_ptr node between prev_ptr and next_ptr
static inline void list_insert(struct list_head *new_ptr,
			      struct list_head *prev_ptr,
			      struct list_head *next_ptr)
{
	next_ptr->prev_ptr = new_ptr;
	prev_ptr->next_ptr = new_ptr;
	new_ptr->next_ptr = next_ptr;
	new_ptr->prev_ptr = prev_ptr;
}

// add a list node at the head of the list
static inline void list_add_head(struct list_head *new_ptr, struct list_head *head)
{
	list_insert(new_ptr, head, head->next_ptr);
}

// add a list node at the tail of the list 
static inline void list_add_tail(struct list_head *new_ptr, struct list_head *head)
{
	list_insert(new_ptr, head->prev_ptr, head);
}

// delete the node from the list (note that it only remove the entry from 
// list, but not free allocated memory)
static inline void list_delete_entry(struct list_head *entry)
{
	entry->next_ptr->prev_ptr = entry->prev_ptr;
	entry->prev_ptr->next_ptr = entry->next_ptr;
}


#endif /* __LIST_H__ */