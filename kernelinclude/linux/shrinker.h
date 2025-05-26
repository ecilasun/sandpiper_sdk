/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SHRINKER_H
#define _LINUX_SHRINKER_H

#include <linux/atomic.h>
#include <linux/types.h>


struct shrink_control {
	gfp_t gfp_mask;

	
	int nid;

	
	unsigned long nr_to_scan;

	
	unsigned long nr_scanned;

	
	struct mem_cgroup *memcg;
};

#define SHRINK_STOP (~0UL)
#define SHRINK_EMPTY (~0UL - 1)

struct shrinker {
	unsigned long (*count_objects)(struct shrinker *,
				       struct shrink_control *sc);
	unsigned long (*scan_objects)(struct shrinker *,
				      struct shrink_control *sc);

	long batch;	
	int seeks;	
	unsigned flags;

	
	struct list_head list;
#ifdef CONFIG_MEMCG
	
	int id;
#endif
#ifdef CONFIG_SHRINKER_DEBUG
	int debugfs_id;
	const char *name;
	struct dentry *debugfs_entry;
#endif
	
	atomic_long_t *nr_deferred;
};
#define DEFAULT_SEEKS 2 


#define SHRINKER_REGISTERED	(1 << 0)
#define SHRINKER_NUMA_AWARE	(1 << 1)
#define SHRINKER_MEMCG_AWARE	(1 << 2)

#define SHRINKER_NONSLAB	(1 << 3)

extern int __printf(2, 3) prealloc_shrinker(struct shrinker *shrinker,
					    const char *fmt, ...);
extern void register_shrinker_prepared(struct shrinker *shrinker);
extern int __printf(2, 3) register_shrinker(struct shrinker *shrinker,
					    const char *fmt, ...);
extern void unregister_shrinker(struct shrinker *shrinker);
extern void free_prealloced_shrinker(struct shrinker *shrinker);
extern void synchronize_shrinkers(void);

#ifdef CONFIG_SHRINKER_DEBUG
extern int shrinker_debugfs_add(struct shrinker *shrinker);
extern struct dentry *shrinker_debugfs_detach(struct shrinker *shrinker,
					      int *debugfs_id);
extern void shrinker_debugfs_remove(struct dentry *debugfs_entry,
				    int debugfs_id);
extern int __printf(2, 3) shrinker_debugfs_rename(struct shrinker *shrinker,
						  const char *fmt, ...);
#else 
static inline int shrinker_debugfs_add(struct shrinker *shrinker)
{
	return 0;
}
static inline struct dentry *shrinker_debugfs_detach(struct shrinker *shrinker,
						     int *debugfs_id)
{
	*debugfs_id = -1;
	return NULL;
}
static inline void shrinker_debugfs_remove(struct dentry *debugfs_entry,
					   int debugfs_id)
{
}
static inline __printf(2, 3)
int shrinker_debugfs_rename(struct shrinker *shrinker, const char *fmt, ...)
{
	return 0;
}
#endif 
#endif 
