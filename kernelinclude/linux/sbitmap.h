/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_SCALE_BITMAP_H
#define __LINUX_SCALE_BITMAP_H

#include <linux/atomic.h>
#include <linux/bitops.h>
#include <linux/cache.h>
#include <linux/list.h>
#include <linux/log2.h>
#include <linux/minmax.h>
#include <linux/percpu.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <linux/types.h>
#include <linux/wait.h>

struct seq_file;


struct sbitmap_word {
	
	unsigned long word;

	
	unsigned long cleared ____cacheline_aligned_in_smp;
} ____cacheline_aligned_in_smp;


struct sbitmap {
	
	unsigned int depth;

	
	unsigned int shift;

	
	unsigned int map_nr;

	
	bool round_robin;

	
	struct sbitmap_word *map;

	
	unsigned int __percpu *alloc_hint;
};

#define SBQ_WAIT_QUEUES 8
#define SBQ_WAKE_BATCH 8


struct sbq_wait_state {
	
	wait_queue_head_t wait;
} ____cacheline_aligned_in_smp;


struct sbitmap_queue {
	
	struct sbitmap sb;

	
	unsigned int wake_batch;

	
	atomic_t wake_index;

	
	struct sbq_wait_state *ws;

	
	atomic_t ws_active;

	
	unsigned int min_shallow_depth;

	
	atomic_t completion_cnt;

	
	atomic_t wakeup_cnt;
};


int sbitmap_init_node(struct sbitmap *sb, unsigned int depth, int shift,
		      gfp_t flags, int node, bool round_robin, bool alloc_hint);


static inline unsigned int __map_depth(const struct sbitmap *sb, int index)
{
	if (index == sb->map_nr - 1)
		return sb->depth - (index << sb->shift);
	return 1U << sb->shift;
}


static inline void sbitmap_free(struct sbitmap *sb)
{
	free_percpu(sb->alloc_hint);
	kvfree(sb->map);
	sb->map = NULL;
}


void sbitmap_resize(struct sbitmap *sb, unsigned int depth);


int sbitmap_get(struct sbitmap *sb);


int sbitmap_get_shallow(struct sbitmap *sb, unsigned long shallow_depth);


bool sbitmap_any_bit_set(const struct sbitmap *sb);

#define SB_NR_TO_INDEX(sb, bitnr) ((bitnr) >> (sb)->shift)
#define SB_NR_TO_BIT(sb, bitnr) ((bitnr) & ((1U << (sb)->shift) - 1U))

typedef bool (*sb_for_each_fn)(struct sbitmap *, unsigned int, void *);


static inline void __sbitmap_for_each_set(struct sbitmap *sb,
					  unsigned int start,
					  sb_for_each_fn fn, void *data)
{
	unsigned int index;
	unsigned int nr;
	unsigned int scanned = 0;

	if (start >= sb->depth)
		start = 0;
	index = SB_NR_TO_INDEX(sb, start);
	nr = SB_NR_TO_BIT(sb, start);

	while (scanned < sb->depth) {
		unsigned long word;
		unsigned int depth = min_t(unsigned int,
					   __map_depth(sb, index) - nr,
					   sb->depth - scanned);

		scanned += depth;
		word = sb->map[index].word & ~sb->map[index].cleared;
		if (!word)
			goto next;

		
		depth += nr;
		while (1) {
			nr = find_next_bit(&word, depth, nr);
			if (nr >= depth)
				break;
			if (!fn(sb, (index << sb->shift) + nr, data))
				return;

			nr++;
		}
next:
		nr = 0;
		if (++index >= sb->map_nr)
			index = 0;
	}
}


static inline void sbitmap_for_each_set(struct sbitmap *sb, sb_for_each_fn fn,
					void *data)
{
	__sbitmap_for_each_set(sb, 0, fn, data);
}

static inline unsigned long *__sbitmap_word(struct sbitmap *sb,
					    unsigned int bitnr)
{
	return &sb->map[SB_NR_TO_INDEX(sb, bitnr)].word;
}



static inline void sbitmap_set_bit(struct sbitmap *sb, unsigned int bitnr)
{
	set_bit(SB_NR_TO_BIT(sb, bitnr), __sbitmap_word(sb, bitnr));
}

static inline void sbitmap_clear_bit(struct sbitmap *sb, unsigned int bitnr)
{
	clear_bit(SB_NR_TO_BIT(sb, bitnr), __sbitmap_word(sb, bitnr));
}


static inline void sbitmap_deferred_clear_bit(struct sbitmap *sb, unsigned int bitnr)
{
	unsigned long *addr = &sb->map[SB_NR_TO_INDEX(sb, bitnr)].cleared;

	set_bit(SB_NR_TO_BIT(sb, bitnr), addr);
}


static inline void sbitmap_put(struct sbitmap *sb, unsigned int bitnr)
{
	sbitmap_deferred_clear_bit(sb, bitnr);

	if (likely(sb->alloc_hint && !sb->round_robin && bitnr < sb->depth))
		*raw_cpu_ptr(sb->alloc_hint) = bitnr;
}

static inline int sbitmap_test_bit(struct sbitmap *sb, unsigned int bitnr)
{
	return test_bit(SB_NR_TO_BIT(sb, bitnr), __sbitmap_word(sb, bitnr));
}

static inline int sbitmap_calculate_shift(unsigned int depth)
{
	int	shift = ilog2(BITS_PER_LONG);

	
	if (depth >= 4) {
		while ((4U << shift) > depth)
			shift--;
	}

	return shift;
}


void sbitmap_show(struct sbitmap *sb, struct seq_file *m);



unsigned int sbitmap_weight(const struct sbitmap *sb);


void sbitmap_bitmap_show(struct sbitmap *sb, struct seq_file *m);


int sbitmap_queue_init_node(struct sbitmap_queue *sbq, unsigned int depth,
			    int shift, bool round_robin, gfp_t flags, int node);


static inline void sbitmap_queue_free(struct sbitmap_queue *sbq)
{
	kfree(sbq->ws);
	sbitmap_free(&sbq->sb);
}


void sbitmap_queue_recalculate_wake_batch(struct sbitmap_queue *sbq,
					    unsigned int users);


void sbitmap_queue_resize(struct sbitmap_queue *sbq, unsigned int depth);


int __sbitmap_queue_get(struct sbitmap_queue *sbq);


unsigned long __sbitmap_queue_get_batch(struct sbitmap_queue *sbq, int nr_tags,
					unsigned int *offset);


int sbitmap_queue_get_shallow(struct sbitmap_queue *sbq,
			      unsigned int shallow_depth);


static inline int sbitmap_queue_get(struct sbitmap_queue *sbq,
				    unsigned int *cpu)
{
	int nr;

	*cpu = get_cpu();
	nr = __sbitmap_queue_get(sbq);
	put_cpu();
	return nr;
}


void sbitmap_queue_min_shallow_depth(struct sbitmap_queue *sbq,
				     unsigned int min_shallow_depth);


void sbitmap_queue_clear(struct sbitmap_queue *sbq, unsigned int nr,
			 unsigned int cpu);


void sbitmap_queue_clear_batch(struct sbitmap_queue *sbq, int offset,
				int *tags, int nr_tags);

static inline int sbq_index_inc(int index)
{
	return (index + 1) & (SBQ_WAIT_QUEUES - 1);
}

static inline void sbq_index_atomic_inc(atomic_t *index)
{
	int old = atomic_read(index);
	int new = sbq_index_inc(old);
	atomic_cmpxchg(index, old, new);
}


static inline struct sbq_wait_state *sbq_wait_ptr(struct sbitmap_queue *sbq,
						  atomic_t *wait_index)
{
	struct sbq_wait_state *ws;

	ws = &sbq->ws[atomic_read(wait_index)];
	sbq_index_atomic_inc(wait_index);
	return ws;
}


void sbitmap_queue_wake_all(struct sbitmap_queue *sbq);


void sbitmap_queue_wake_up(struct sbitmap_queue *sbq, int nr);


void sbitmap_queue_show(struct sbitmap_queue *sbq, struct seq_file *m);

struct sbq_wait {
	struct sbitmap_queue *sbq;	
	struct wait_queue_entry wait;
};

#define DEFINE_SBQ_WAIT(name)							\
	struct sbq_wait name = {						\
		.sbq = NULL,							\
		.wait = {							\
			.private	= current,				\
			.func		= autoremove_wake_function,		\
			.entry		= LIST_HEAD_INIT((name).wait.entry),	\
		}								\
	}


void sbitmap_prepare_to_wait(struct sbitmap_queue *sbq,
				struct sbq_wait_state *ws,
				struct sbq_wait *sbq_wait, int state);


void sbitmap_finish_wait(struct sbitmap_queue *sbq, struct sbq_wait_state *ws,
				struct sbq_wait *sbq_wait);


void sbitmap_add_wait_queue(struct sbitmap_queue *sbq,
			    struct sbq_wait_state *ws,
			    struct sbq_wait *sbq_wait);


void sbitmap_del_wait_queue(struct sbq_wait *sbq_wait);

#endif 
