/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_INTERVAL_TREE_H
#define _LINUX_INTERVAL_TREE_H

#include <linux/rbtree.h>

struct interval_tree_node {
	struct rb_node rb;
	unsigned long start;	
	unsigned long last;	
	unsigned long __subtree_last;
};

extern void
interval_tree_insert(struct interval_tree_node *node,
		     struct rb_root_cached *root);

extern void
interval_tree_remove(struct interval_tree_node *node,
		     struct rb_root_cached *root);

extern struct interval_tree_node *
interval_tree_iter_first(struct rb_root_cached *root,
			 unsigned long start, unsigned long last);

extern struct interval_tree_node *
interval_tree_iter_next(struct interval_tree_node *node,
			unsigned long start, unsigned long last);


struct interval_tree_span_iter {
	
	struct interval_tree_node *nodes[2];
	unsigned long first_index;
	unsigned long last_index;

	
	union {
		unsigned long start_hole;
		unsigned long start_used;
	};
	union {
		unsigned long last_hole;
		unsigned long last_used;
	};
	int is_hole;
};

void interval_tree_span_iter_first(struct interval_tree_span_iter *state,
				   struct rb_root_cached *itree,
				   unsigned long first_index,
				   unsigned long last_index);
void interval_tree_span_iter_advance(struct interval_tree_span_iter *iter,
				     struct rb_root_cached *itree,
				     unsigned long new_index);
void interval_tree_span_iter_next(struct interval_tree_span_iter *state);

static inline bool
interval_tree_span_iter_done(struct interval_tree_span_iter *state)
{
	return state->is_hole == -1;
}

#define interval_tree_for_each_span(span, itree, first_index, last_index)      \
	for (interval_tree_span_iter_first(span, itree,                        \
					   first_index, last_index);           \
	     !interval_tree_span_iter_done(span);                              \
	     interval_tree_span_iter_next(span))

#endif	
