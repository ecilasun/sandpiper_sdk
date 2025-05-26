/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _BPF_LOCAL_STORAGE_H
#define _BPF_LOCAL_STORAGE_H

#include <linux/bpf.h>
#include <linux/filter.h>
#include <linux/rculist.h>
#include <linux/list.h>
#include <linux/hash.h>
#include <linux/types.h>
#include <linux/bpf_mem_alloc.h>
#include <uapi/linux/btf.h>

#define BPF_LOCAL_STORAGE_CACHE_SIZE	16

#define bpf_rcu_lock_held()                                                    \
	(rcu_read_lock_held() || rcu_read_lock_trace_held() ||                 \
	 rcu_read_lock_bh_held())
struct bpf_local_storage_map_bucket {
	struct hlist_head list;
	raw_spinlock_t lock;
};


struct bpf_local_storage_map {
	struct bpf_map map;
	
	struct bpf_local_storage_map_bucket *buckets;
	u32 bucket_log;
	u16 elem_size;
	u16 cache_idx;
	struct bpf_mem_alloc selem_ma;
	struct bpf_mem_alloc storage_ma;
	bool bpf_ma;
};

struct bpf_local_storage_data {
	
	struct bpf_local_storage_map __rcu *smap;
	u8 data[] __aligned(8);
};


struct bpf_local_storage_elem {
	struct hlist_node map_node;	
	struct hlist_node snode;	
	struct bpf_local_storage __rcu *local_storage;
	struct rcu_head rcu;
	
	
	struct bpf_local_storage_data sdata ____cacheline_aligned;
};

struct bpf_local_storage {
	struct bpf_local_storage_data __rcu *cache[BPF_LOCAL_STORAGE_CACHE_SIZE];
	struct bpf_local_storage_map __rcu *smap;
	struct hlist_head list; 
	void *owner;		
	struct rcu_head rcu;
	raw_spinlock_t lock;	
};


#define BPF_LOCAL_STORAGE_MAX_VALUE_SIZE				       \
	min_t(u32,                                                             \
	      (KMALLOC_MAX_SIZE - MAX_BPF_STACK -                              \
	       sizeof(struct bpf_local_storage_elem)),                         \
	      (U16_MAX - sizeof(struct bpf_local_storage_elem)))

#define SELEM(_SDATA)                                                          \
	container_of((_SDATA), struct bpf_local_storage_elem, sdata)
#define SDATA(_SELEM) (&(_SELEM)->sdata)

#define BPF_LOCAL_STORAGE_CACHE_SIZE	16

struct bpf_local_storage_cache {
	spinlock_t idx_lock;
	u64 idx_usage_counts[BPF_LOCAL_STORAGE_CACHE_SIZE];
};

#define DEFINE_BPF_STORAGE_CACHE(name)				\
static struct bpf_local_storage_cache name = {			\
	.idx_lock = __SPIN_LOCK_UNLOCKED(name.idx_lock),	\
}


int bpf_local_storage_map_alloc_check(union bpf_attr *attr);

struct bpf_map *
bpf_local_storage_map_alloc(union bpf_attr *attr,
			    struct bpf_local_storage_cache *cache,
			    bool bpf_ma);

struct bpf_local_storage_data *
bpf_local_storage_lookup(struct bpf_local_storage *local_storage,
			 struct bpf_local_storage_map *smap,
			 bool cacheit_lockit);

void bpf_local_storage_destroy(struct bpf_local_storage *local_storage);

void bpf_local_storage_map_free(struct bpf_map *map,
				struct bpf_local_storage_cache *cache,
				int __percpu *busy_counter);

int bpf_local_storage_map_check_btf(const struct bpf_map *map,
				    const struct btf *btf,
				    const struct btf_type *key_type,
				    const struct btf_type *value_type);

void bpf_selem_link_storage_nolock(struct bpf_local_storage *local_storage,
				   struct bpf_local_storage_elem *selem);

void bpf_selem_unlink(struct bpf_local_storage_elem *selem, bool reuse_now);

void bpf_selem_link_map(struct bpf_local_storage_map *smap,
			struct bpf_local_storage_elem *selem);

struct bpf_local_storage_elem *
bpf_selem_alloc(struct bpf_local_storage_map *smap, void *owner, void *value,
		bool charge_mem, gfp_t gfp_flags);

void bpf_selem_free(struct bpf_local_storage_elem *selem,
		    struct bpf_local_storage_map *smap,
		    bool reuse_now);

int
bpf_local_storage_alloc(void *owner,
			struct bpf_local_storage_map *smap,
			struct bpf_local_storage_elem *first_selem,
			gfp_t gfp_flags);

struct bpf_local_storage_data *
bpf_local_storage_update(void *owner, struct bpf_local_storage_map *smap,
			 void *value, u64 map_flags, gfp_t gfp_flags);

u64 bpf_local_storage_map_mem_usage(const struct bpf_map *map);

#endif 
