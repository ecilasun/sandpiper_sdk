/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MIN_HEAP_H
#define _LINUX_MIN_HEAP_H

#include <linux/bug.h>
#include <linux/string.h>
#include <linux/types.h>


struct min_heap {
	void *data;
	int nr;
	int size;
};


struct min_heap_callbacks {
	int elem_size;
	bool (*less)(const void *lhs, const void *rhs);
	void (*swp)(void *lhs, void *rhs);
};


static __always_inline
void min_heapify(struct min_heap *heap, int pos,
		const struct min_heap_callbacks *func)
{
	void *left, *right, *parent, *smallest;
	void *data = heap->data;

	for (;;) {
		if (pos * 2 + 1 >= heap->nr)
			break;

		left = data + ((pos * 2 + 1) * func->elem_size);
		parent = data + (pos * func->elem_size);
		smallest = parent;
		if (func->less(left, smallest))
			smallest = left;

		if (pos * 2 + 2 < heap->nr) {
			right = data + ((pos * 2 + 2) * func->elem_size);
			if (func->less(right, smallest))
				smallest = right;
		}
		if (smallest == parent)
			break;
		func->swp(smallest, parent);
		if (smallest == left)
			pos = (pos * 2) + 1;
		else
			pos = (pos * 2) + 2;
	}
}


static __always_inline
void min_heapify_all(struct min_heap *heap,
		const struct min_heap_callbacks *func)
{
	int i;

	for (i = heap->nr / 2; i >= 0; i--)
		min_heapify(heap, i, func);
}


static __always_inline
void min_heap_pop(struct min_heap *heap,
		const struct min_heap_callbacks *func)
{
	void *data = heap->data;

	if (WARN_ONCE(heap->nr <= 0, "Popping an empty heap"))
		return;

	
	heap->nr--;
	memcpy(data, data + (heap->nr * func->elem_size), func->elem_size);
	min_heapify(heap, 0, func);
}


static __always_inline
void min_heap_pop_push(struct min_heap *heap,
		const void *element,
		const struct min_heap_callbacks *func)
{
	memcpy(heap->data, element, func->elem_size);
	min_heapify(heap, 0, func);
}


static __always_inline
void min_heap_push(struct min_heap *heap, const void *element,
		const struct min_heap_callbacks *func)
{
	void *data = heap->data;
	void *child, *parent;
	int pos;

	if (WARN_ONCE(heap->nr >= heap->size, "Pushing on a full heap"))
		return;

	
	pos = heap->nr;
	memcpy(data + (pos * func->elem_size), element, func->elem_size);
	heap->nr++;

	
	for (; pos > 0; pos = (pos - 1) / 2) {
		child = data + (pos * func->elem_size);
		parent = data + ((pos - 1) / 2) * func->elem_size;
		if (func->less(parent, child))
			break;
		func->swp(parent, child);
	}
}

#endif 
