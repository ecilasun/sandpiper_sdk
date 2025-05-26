/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
#ifndef FREELIST_H
#define FREELIST_H

#include <linux/atomic.h>



struct freelist_node {
	atomic_t		refs;
	struct freelist_node	*next;
};

struct freelist_head {
	struct freelist_node	*head;
};

#define REFS_ON_FREELIST 0x80000000
#define REFS_MASK	 0x7FFFFFFF

static inline void __freelist_add(struct freelist_node *node, struct freelist_head *list)
{
	
	struct freelist_node *head = READ_ONCE(list->head);

	for (;;) {
		WRITE_ONCE(node->next, head);
		atomic_set_release(&node->refs, 1);

		if (!try_cmpxchg_release(&list->head, &head, node)) {
			
			if (atomic_fetch_add_release(REFS_ON_FREELIST - 1, &node->refs) == 1)
				continue;
		}
		return;
	}
}

static inline void freelist_add(struct freelist_node *node, struct freelist_head *list)
{
	
	if (!atomic_fetch_add_release(REFS_ON_FREELIST, &node->refs)) {
		
		__freelist_add(node, list);
	}
}

static inline struct freelist_node *freelist_try_get(struct freelist_head *list)
{
	struct freelist_node *prev, *next, *head = smp_load_acquire(&list->head);
	unsigned int refs;

	while (head) {
		prev = head;
		refs = atomic_read(&head->refs);
		if ((refs & REFS_MASK) == 0 ||
		    !atomic_try_cmpxchg_acquire(&head->refs, &refs, refs+1)) {
			head = smp_load_acquire(&list->head);
			continue;
		}

		
		next = READ_ONCE(head->next);
		if (try_cmpxchg_acquire(&list->head, &head, next)) {
			
			WARN_ON_ONCE(atomic_read(&head->refs) & REFS_ON_FREELIST);

			
			atomic_fetch_add(-2, &head->refs);

			return head;
		}

		
		refs = atomic_fetch_add(-1, &prev->refs);
		if (refs == REFS_ON_FREELIST + 1)
			__freelist_add(prev, list);
	}

	return NULL;
}

#endif 
