/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BPF_MEM_ALLOC_H
#define _BPF_MEM_ALLOC_H
#include <linux/compiler_types.h>
#include <linux/workqueue.h>

struct bpf_mem_cache;
struct bpf_mem_caches;

struct bpf_mem_alloc {
	struct bpf_mem_caches __percpu *caches;
	struct bpf_mem_cache __percpu *cache;
	bool percpu;
	struct work_struct work;
};


int bpf_mem_alloc_init(struct bpf_mem_alloc *ma, int size, bool percpu);
void bpf_mem_alloc_destroy(struct bpf_mem_alloc *ma);


void *bpf_mem_alloc(struct bpf_mem_alloc *ma, size_t size);
void bpf_mem_free(struct bpf_mem_alloc *ma, void *ptr);
void bpf_mem_free_rcu(struct bpf_mem_alloc *ma, void *ptr);


void *bpf_mem_cache_alloc(struct bpf_mem_alloc *ma);
void bpf_mem_cache_free(struct bpf_mem_alloc *ma, void *ptr);
void bpf_mem_cache_free_rcu(struct bpf_mem_alloc *ma, void *ptr);
void bpf_mem_cache_raw_free(void *ptr);
void *bpf_mem_cache_alloc_flags(struct bpf_mem_alloc *ma, gfp_t flags);

#endif 
