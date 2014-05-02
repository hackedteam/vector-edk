#include "list.h"

/**
 * __ntfs_list_add - Insert a new entry between two known consecutive entries.
 * @new:
 * @prev:
 * @next:
 *
 * This is only for internal list manipulation where we know the prev/next
 * entries already!
 */
void __ntfs_list_add(struct ntfs_list_head * new,
		struct ntfs_list_head * prev, struct ntfs_list_head * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * ntfs_list_add - add a new entry
 * @new:	new entry to be added
 * @head:	list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
void ntfs_list_add(struct ntfs_list_head *new,
		struct ntfs_list_head *head)
{
	__ntfs_list_add(new, head, head->next);
}

/**
 * ntfs_list_add_tail - add a new entry
 * @new:	new entry to be added
 * @head:	list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
void ntfs_list_add_tail(struct ntfs_list_head *new,
		struct ntfs_list_head *head)
{
	__ntfs_list_add(new, head->prev, head);
}

/**
 * __ntfs_list_del -
 * @prev:
 * @next:
 *
 * Delete a list entry by making the prev/next entries point to each other.
 *
 * This is only for internal list manipulation where we know the prev/next
 * entries already!
 */
void __ntfs_list_del(struct ntfs_list_head * prev,
		struct ntfs_list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * ntfs_list_del - deletes entry from list.
 * @entry:	the element to delete from the list.
 *
 * Note: ntfs_list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
void ntfs_list_del(struct ntfs_list_head *entry)
{
	__ntfs_list_del(entry->prev, entry->next);
}

/**
 * ntfs_list_del_init - deletes entry from list and reinitialize it.
 * @entry:	the element to delete from the list.
 */
void ntfs_list_del_init(struct ntfs_list_head *entry)
{
	__ntfs_list_del(entry->prev, entry->next);
	NTFS_INIT_LIST_HEAD(entry);
}

/**
 * ntfs_list_empty - tests whether a list is empty
 * @head:	the list to test.
 */
int ntfs_list_empty(struct ntfs_list_head *head)
{
	return head->next == head;
}

/**
 * ntfs_list_splice - join two lists
 * @list:	the new list to add.
 * @head:	the place to add it in the first list.
 */
void ntfs_list_splice(struct ntfs_list_head *list,
		struct ntfs_list_head *head)
{
	struct ntfs_list_head *first = list->next;

	if (first != list) {
		struct ntfs_list_head *last = list->prev;
		struct ntfs_list_head *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}
