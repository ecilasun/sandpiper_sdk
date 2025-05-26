/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef _LINUX_MAPLE_TREE_H
#define _LINUX_MAPLE_TREE_H


#include <linux/kernel.h>
#include <linux/rcupdate.h>
#include <linux/spinlock.h>



#if defined(CONFIG_64BIT) || defined(BUILD_VDSO32_64)

#define MAPLE_NODE_SLOTS	31	
#define MAPLE_RANGE64_SLOTS	16	
#define MAPLE_ARANGE64_SLOTS	10	
#define MAPLE_ALLOC_SLOTS	(MAPLE_NODE_SLOTS - 1)
#else

#define MAPLE_NODE_SLOTS	63	
#define MAPLE_RANGE64_SLOTS	32	
#define MAPLE_ARANGE64_SLOTS	21	
#define MAPLE_ALLOC_SLOTS	(MAPLE_NODE_SLOTS - 2)
#endif 

#define MAPLE_NODE_MASK		255UL




struct maple_metadata {
	unsigned char end;
	unsigned char gap;
};



struct maple_range_64 {
	struct maple_pnode *parent;
	unsigned long pivot[MAPLE_RANGE64_SLOTS - 1];
	union {
		void __rcu *slot[MAPLE_RANGE64_SLOTS];
		struct {
			void __rcu *pad[MAPLE_RANGE64_SLOTS - 1];
			struct maple_metadata meta;
		};
	};
};


struct maple_arange_64 {
	struct maple_pnode *parent;
	unsigned long pivot[MAPLE_ARANGE64_SLOTS - 1];
	void __rcu *slot[MAPLE_ARANGE64_SLOTS];
	unsigned long gap[MAPLE_ARANGE64_SLOTS];
	struct maple_metadata meta;
};

struct maple_alloc {
	unsigned long total;
	unsigned char node_count;
	unsigned int request_count;
	struct maple_alloc *slot[MAPLE_ALLOC_SLOTS];
};

struct maple_topiary {
	struct maple_pnode *parent;
	struct maple_enode *next; 
};

enum maple_type {
	maple_dense,
	maple_leaf_64,
	maple_range_64,
	maple_arange_64,
};



#define MT_FLAGS_ALLOC_RANGE	0x01
#define MT_FLAGS_USE_RCU	0x02
#define MT_FLAGS_HEIGHT_OFFSET	0x02
#define MT_FLAGS_HEIGHT_MASK	0x7C
#define MT_FLAGS_LOCK_MASK	0x300
#define MT_FLAGS_LOCK_IRQ	0x100
#define MT_FLAGS_LOCK_BH	0x200
#define MT_FLAGS_LOCK_EXTERN	0x300

#define MAPLE_HEIGHT_MAX	31


#define MAPLE_NODE_TYPE_MASK	0x0F
#define MAPLE_NODE_TYPE_SHIFT	0x03

#define MAPLE_RESERVED_RANGE	4096

#ifdef CONFIG_LOCKDEP
typedef struct lockdep_map *lockdep_map_p;
#define mt_lock_is_held(mt)                                             \
	(!(mt)->ma_external_lock || lock_is_held((mt)->ma_external_lock))

#define mt_write_lock_is_held(mt)					\
	(!(mt)->ma_external_lock ||					\
	 lock_is_held_type((mt)->ma_external_lock, 0))

#define mt_set_external_lock(mt, lock)					\
	(mt)->ma_external_lock = &(lock)->dep_map

#define mt_on_stack(mt)			(mt).ma_external_lock = NULL
#else
typedef struct {  } lockdep_map_p;
#define mt_lock_is_held(mt)		1
#define mt_write_lock_is_held(mt)	1
#define mt_set_external_lock(mt, lock)	do { } while (0)
#define mt_on_stack(mt)			do { } while (0)
#endif


struct maple_tree {
	union {
		spinlock_t	ma_lock;
		lockdep_map_p	ma_external_lock;
	};
	unsigned int	ma_flags;
	void __rcu      *ma_root;
};


#define MTREE_INIT(name, __flags) {					\
	.ma_lock = __SPIN_LOCK_UNLOCKED((name).ma_lock),		\
	.ma_flags = __flags,						\
	.ma_root = NULL,						\
}


#ifdef CONFIG_LOCKDEP
#define MTREE_INIT_EXT(name, __flags, __lock) {				\
	.ma_external_lock = &(__lock).dep_map,				\
	.ma_flags = (__flags),						\
	.ma_root = NULL,						\
}
#else
#define MTREE_INIT_EXT(name, __flags, __lock)	MTREE_INIT(name, __flags)
#endif

#define DEFINE_MTREE(name)						\
	struct maple_tree name = MTREE_INIT(name, 0)

#define mtree_lock(mt)		spin_lock((&(mt)->ma_lock))
#define mtree_unlock(mt)	spin_unlock((&(mt)->ma_lock))



struct maple_node {
	union {
		struct {
			struct maple_pnode *parent;
			void __rcu *slot[MAPLE_NODE_SLOTS];
		};
		struct {
			void *pad;
			struct rcu_head rcu;
			struct maple_enode *piv_parent;
			unsigned char parent_slot;
			enum maple_type type;
			unsigned char slot_len;
			unsigned int ma_flags;
		};
		struct maple_range_64 mr64;
		struct maple_arange_64 ma64;
		struct maple_alloc alloc;
	};
};


struct ma_topiary {
	struct maple_enode *head;
	struct maple_enode *tail;
	struct maple_tree *mtree;
};

void *mtree_load(struct maple_tree *mt, unsigned long index);

int mtree_insert(struct maple_tree *mt, unsigned long index,
		void *entry, gfp_t gfp);
int mtree_insert_range(struct maple_tree *mt, unsigned long first,
		unsigned long last, void *entry, gfp_t gfp);
int mtree_alloc_range(struct maple_tree *mt, unsigned long *startp,
		void *entry, unsigned long size, unsigned long min,
		unsigned long max, gfp_t gfp);
int mtree_alloc_rrange(struct maple_tree *mt, unsigned long *startp,
		void *entry, unsigned long size, unsigned long min,
		unsigned long max, gfp_t gfp);

int mtree_store_range(struct maple_tree *mt, unsigned long first,
		      unsigned long last, void *entry, gfp_t gfp);
int mtree_store(struct maple_tree *mt, unsigned long index,
		void *entry, gfp_t gfp);
void *mtree_erase(struct maple_tree *mt, unsigned long index);

void mtree_destroy(struct maple_tree *mt);
void __mt_destroy(struct maple_tree *mt);


static inline bool mtree_empty(const struct maple_tree *mt)
{
	return mt->ma_root == NULL;
}




struct ma_state {
	struct maple_tree *tree;	
	unsigned long index;		
	unsigned long last;		
	struct maple_enode *node;	
	unsigned long min;		
	unsigned long max;		
	struct maple_alloc *alloc;	
	unsigned char depth;		
	unsigned char offset;
	unsigned char mas_flags;
};

struct ma_wr_state {
	struct ma_state *mas;
	struct maple_node *node;	
	unsigned long r_min;		
	unsigned long r_max;		
	enum maple_type type;		
	unsigned char offset_end;	
	unsigned char node_end;		
	unsigned long *pivots;		
	unsigned long end_piv;		
	void __rcu **slots;		
	void *entry;			
	void *content;			
};

#define mas_lock(mas)           spin_lock(&((mas)->tree->ma_lock))
#define mas_unlock(mas)         spin_unlock(&((mas)->tree->ma_lock))



#define MAS_START	((struct maple_enode *)1UL)
#define MAS_ROOT	((struct maple_enode *)5UL)
#define MAS_NONE	((struct maple_enode *)9UL)
#define MAS_PAUSE	((struct maple_enode *)17UL)
#define MAS_OVERFLOW	((struct maple_enode *)33UL)
#define MAS_UNDERFLOW	((struct maple_enode *)65UL)
#define MA_ERROR(err) \
		((struct maple_enode *)(((unsigned long)err << 2) | 2UL))

#define MA_STATE(name, mt, first, end)					\
	struct ma_state name = {					\
		.tree = mt,						\
		.index = first,						\
		.last = end,						\
		.node = MAS_START,					\
		.min = 0,						\
		.max = ULONG_MAX,					\
		.alloc = NULL,						\
		.mas_flags = 0,						\
	}

#define MA_WR_STATE(name, ma_state, wr_entry)				\
	struct ma_wr_state name = {					\
		.mas = ma_state,					\
		.content = NULL,					\
		.entry = wr_entry,					\
	}

#define MA_TOPIARY(name, tree)						\
	struct ma_topiary name = {					\
		.head = NULL,						\
		.tail = NULL,						\
		.mtree = tree,						\
	}

void *mas_walk(struct ma_state *mas);
void *mas_store(struct ma_state *mas, void *entry);
void *mas_erase(struct ma_state *mas);
int mas_store_gfp(struct ma_state *mas, void *entry, gfp_t gfp);
void mas_store_prealloc(struct ma_state *mas, void *entry);
void *mas_find(struct ma_state *mas, unsigned long max);
void *mas_find_range(struct ma_state *mas, unsigned long max);
void *mas_find_rev(struct ma_state *mas, unsigned long min);
void *mas_find_range_rev(struct ma_state *mas, unsigned long max);
int mas_preallocate(struct ma_state *mas, void *entry, gfp_t gfp);
bool mas_is_err(struct ma_state *mas);

bool mas_nomem(struct ma_state *mas, gfp_t gfp);
void mas_pause(struct ma_state *mas);
void maple_tree_init(void);
void mas_destroy(struct ma_state *mas);
int mas_expected_entries(struct ma_state *mas, unsigned long nr_entries);

void *mas_prev(struct ma_state *mas, unsigned long min);
void *mas_prev_range(struct ma_state *mas, unsigned long max);
void *mas_next(struct ma_state *mas, unsigned long max);
void *mas_next_range(struct ma_state *mas, unsigned long max);

int mas_empty_area(struct ma_state *mas, unsigned long min, unsigned long max,
		   unsigned long size);

int mas_empty_area_rev(struct ma_state *mas, unsigned long min,
		       unsigned long max, unsigned long size);

static inline void mas_init(struct ma_state *mas, struct maple_tree *tree,
			    unsigned long addr)
{
	memset(mas, 0, sizeof(struct ma_state));
	mas->tree = tree;
	mas->index = mas->last = addr;
	mas->max = ULONG_MAX;
	mas->node = MAS_START;
}


static inline bool mas_is_none(const struct ma_state *mas)
{
	return mas->node == MAS_NONE;
}


static inline bool mas_is_paused(const struct ma_state *mas)
{
	return mas->node == MAS_PAUSE;
}


static inline bool mas_is_active(struct ma_state *mas)
{
	if ((unsigned long)mas->node >= MAPLE_RESERVED_RANGE)
		return true;

	return false;
}


static inline void mas_reset(struct ma_state *mas)
{
	mas->node = MAS_START;
}


#define mas_for_each(__mas, __entry, __max) \
	while (((__entry) = mas_find((__mas), (__max))) != NULL)

static inline void __mas_set_range(struct ma_state *mas, unsigned long start,
		unsigned long last)
{
	mas->index = start;
	mas->last = last;
}


static inline
void mas_set_range(struct ma_state *mas, unsigned long start, unsigned long last)
{
	__mas_set_range(mas, start, last);
	mas->node = MAS_START;
}


static inline void mas_set(struct ma_state *mas, unsigned long index)
{

	mas_set_range(mas, index, index);
}

static inline bool mt_external_lock(const struct maple_tree *mt)
{
	return (mt->ma_flags & MT_FLAGS_LOCK_MASK) == MT_FLAGS_LOCK_EXTERN;
}


static inline void mt_init_flags(struct maple_tree *mt, unsigned int flags)
{
	mt->ma_flags = flags;
	if (!mt_external_lock(mt))
		spin_lock_init(&mt->ma_lock);
	rcu_assign_pointer(mt->ma_root, NULL);
}


static inline void mt_init(struct maple_tree *mt)
{
	mt_init_flags(mt, 0);
}

static inline bool mt_in_rcu(struct maple_tree *mt)
{
#ifdef CONFIG_MAPLE_RCU_DISABLED
	return false;
#endif
	return mt->ma_flags & MT_FLAGS_USE_RCU;
}


static inline void mt_clear_in_rcu(struct maple_tree *mt)
{
	if (!mt_in_rcu(mt))
		return;

	if (mt_external_lock(mt)) {
		WARN_ON(!mt_lock_is_held(mt));
		mt->ma_flags &= ~MT_FLAGS_USE_RCU;
	} else {
		mtree_lock(mt);
		mt->ma_flags &= ~MT_FLAGS_USE_RCU;
		mtree_unlock(mt);
	}
}


static inline void mt_set_in_rcu(struct maple_tree *mt)
{
	if (mt_in_rcu(mt))
		return;

	if (mt_external_lock(mt)) {
		WARN_ON(!mt_lock_is_held(mt));
		mt->ma_flags |= MT_FLAGS_USE_RCU;
	} else {
		mtree_lock(mt);
		mt->ma_flags |= MT_FLAGS_USE_RCU;
		mtree_unlock(mt);
	}
}

static inline unsigned int mt_height(const struct maple_tree *mt)
{
	return (mt->ma_flags & MT_FLAGS_HEIGHT_MASK) >> MT_FLAGS_HEIGHT_OFFSET;
}

void *mt_find(struct maple_tree *mt, unsigned long *index, unsigned long max);
void *mt_find_after(struct maple_tree *mt, unsigned long *index,
		    unsigned long max);
void *mt_prev(struct maple_tree *mt, unsigned long index,  unsigned long min);
void *mt_next(struct maple_tree *mt, unsigned long index, unsigned long max);


#define mt_for_each(__tree, __entry, __index, __max) \
	for (__entry = mt_find(__tree, &(__index), __max); \
		__entry; __entry = mt_find_after(__tree, &(__index), __max))


#ifdef CONFIG_DEBUG_MAPLE_TREE
enum mt_dump_format {
	mt_dump_dec,
	mt_dump_hex,
};

extern atomic_t maple_tree_tests_run;
extern atomic_t maple_tree_tests_passed;

void mt_dump(const struct maple_tree *mt, enum mt_dump_format format);
void mas_dump(const struct ma_state *mas);
void mas_wr_dump(const struct ma_wr_state *wr_mas);
void mt_validate(struct maple_tree *mt);
void mt_cache_shrink(void);
#define MT_BUG_ON(__tree, __x) do {					\
	atomic_inc(&maple_tree_tests_run);				\
	if (__x) {							\
		pr_info("BUG at %s:%d (%u)\n",				\
		__func__, __LINE__, __x);				\
		mt_dump(__tree, mt_dump_hex);				\
		pr_info("Pass: %u Run:%u\n",				\
			atomic_read(&maple_tree_tests_passed),		\
			atomic_read(&maple_tree_tests_run));		\
		dump_stack();						\
	} else {							\
		atomic_inc(&maple_tree_tests_passed);			\
	}								\
} while (0)

#define MAS_BUG_ON(__mas, __x) do {					\
	atomic_inc(&maple_tree_tests_run);				\
	if (__x) {							\
		pr_info("BUG at %s:%d (%u)\n",				\
		__func__, __LINE__, __x);				\
		mas_dump(__mas);					\
		mt_dump((__mas)->tree, mt_dump_hex);			\
		pr_info("Pass: %u Run:%u\n",				\
			atomic_read(&maple_tree_tests_passed),		\
			atomic_read(&maple_tree_tests_run));		\
		dump_stack();						\
	} else {							\
		atomic_inc(&maple_tree_tests_passed);			\
	}								\
} while (0)

#define MAS_WR_BUG_ON(__wrmas, __x) do {				\
	atomic_inc(&maple_tree_tests_run);				\
	if (__x) {							\
		pr_info("BUG at %s:%d (%u)\n",				\
		__func__, __LINE__, __x);				\
		mas_wr_dump(__wrmas);					\
		mas_dump((__wrmas)->mas);				\
		mt_dump((__wrmas)->mas->tree, mt_dump_hex);		\
		pr_info("Pass: %u Run:%u\n",				\
			atomic_read(&maple_tree_tests_passed),		\
			atomic_read(&maple_tree_tests_run));		\
		dump_stack();						\
	} else {							\
		atomic_inc(&maple_tree_tests_passed);			\
	}								\
} while (0)

#define MT_WARN_ON(__tree, __x)  ({					\
	int ret = !!(__x);						\
	atomic_inc(&maple_tree_tests_run);				\
	if (ret) {							\
		pr_info("WARN at %s:%d (%u)\n",				\
		__func__, __LINE__, __x);				\
		mt_dump(__tree, mt_dump_hex);				\
		pr_info("Pass: %u Run:%u\n",				\
			atomic_read(&maple_tree_tests_passed),		\
			atomic_read(&maple_tree_tests_run));		\
		dump_stack();						\
	} else {							\
		atomic_inc(&maple_tree_tests_passed);			\
	}								\
	unlikely(ret);							\
})

#define MAS_WARN_ON(__mas, __x) ({					\
	int ret = !!(__x);						\
	atomic_inc(&maple_tree_tests_run);				\
	if (ret) {							\
		pr_info("WARN at %s:%d (%u)\n",				\
		__func__, __LINE__, __x);				\
		mas_dump(__mas);					\
		mt_dump((__mas)->tree, mt_dump_hex);			\
		pr_info("Pass: %u Run:%u\n",				\
			atomic_read(&maple_tree_tests_passed),		\
			atomic_read(&maple_tree_tests_run));		\
		dump_stack();						\
	} else {							\
		atomic_inc(&maple_tree_tests_passed);			\
	}								\
	unlikely(ret);							\
})

#define MAS_WR_WARN_ON(__wrmas, __x) ({					\
	int ret = !!(__x);						\
	atomic_inc(&maple_tree_tests_run);				\
	if (ret) {							\
		pr_info("WARN at %s:%d (%u)\n",				\
		__func__, __LINE__, __x);				\
		mas_wr_dump(__wrmas);					\
		mas_dump((__wrmas)->mas);				\
		mt_dump((__wrmas)->mas->tree, mt_dump_hex);		\
		pr_info("Pass: %u Run:%u\n",				\
			atomic_read(&maple_tree_tests_passed),		\
			atomic_read(&maple_tree_tests_run));		\
		dump_stack();						\
	} else {							\
		atomic_inc(&maple_tree_tests_passed);			\
	}								\
	unlikely(ret);							\
})
#else
#define MT_BUG_ON(__tree, __x)		BUG_ON(__x)
#define MAS_BUG_ON(__mas, __x)		BUG_ON(__x)
#define MAS_WR_BUG_ON(__mas, __x)	BUG_ON(__x)
#define MT_WARN_ON(__tree, __x)		WARN_ON(__x)
#define MAS_WARN_ON(__mas, __x)		WARN_ON(__x)
#define MAS_WR_WARN_ON(__mas, __x)	WARN_ON(__x)
#endif 

#endif 
