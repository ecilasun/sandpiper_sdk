/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_MEMORY_H_
#define _LINUX_MEMORY_H_

#include <linux/node.h>
#include <linux/compiler.h>
#include <linux/mutex.h>

#define MIN_MEMORY_BLOCK_SIZE     (1UL << SECTION_SIZE_BITS)


struct memory_group {
	int nid;
	struct list_head memory_blocks;
	unsigned long present_kernel_pages;
	unsigned long present_movable_pages;
	bool is_dynamic;
	union {
		struct {
			unsigned long max_pages;
		} s;
		struct {
			unsigned long unit_pages;
		} d;
	};
};

struct memory_block {
	unsigned long start_section_nr;
	unsigned long state;		
	int online_type;		
	int nid;			
	
	struct zone *zone;
	struct device dev;
	struct vmem_altmap *altmap;
	struct memory_group *group;	
	struct list_head group_next;	
#if defined(CONFIG_MEMORY_FAILURE) && defined(CONFIG_MEMORY_HOTPLUG)
	atomic_long_t nr_hwpoison;
#endif
};

int arch_get_memory_phys_device(unsigned long start_pfn);
unsigned long memory_block_size_bytes(void);
int set_memory_block_size_order(unsigned int order);


#define	MEM_ONLINE		(1<<0) 
#define	MEM_GOING_OFFLINE	(1<<1) 
#define	MEM_OFFLINE		(1<<2) 
#define	MEM_GOING_ONLINE	(1<<3)
#define	MEM_CANCEL_ONLINE	(1<<4)
#define	MEM_CANCEL_OFFLINE	(1<<5)

struct memory_notify {
	unsigned long start_pfn;
	unsigned long nr_pages;
	int status_change_nid_normal;
	int status_change_nid;
};

struct notifier_block;
struct mem_section;


#define DEFAULT_CALLBACK_PRI	0
#define SLAB_CALLBACK_PRI	1
#define HMAT_CALLBACK_PRI	2
#define MM_COMPUTE_BATCH_PRI	10
#define CPUSET_CALLBACK_PRI	10
#define MEMTIER_HOTPLUG_PRI	100
#define KSM_CALLBACK_PRI	100

#ifndef CONFIG_MEMORY_HOTPLUG
static inline void memory_dev_init(void)
{
	return;
}
static inline int register_memory_notifier(struct notifier_block *nb)
{
	return 0;
}
static inline void unregister_memory_notifier(struct notifier_block *nb)
{
}
static inline int memory_notify(unsigned long val, void *v)
{
	return 0;
}
static inline int hotplug_memory_notifier(notifier_fn_t fn, int pri)
{
	return 0;
}
#else 
extern int register_memory_notifier(struct notifier_block *nb);
extern void unregister_memory_notifier(struct notifier_block *nb);
int create_memory_block_devices(unsigned long start, unsigned long size,
				struct vmem_altmap *altmap,
				struct memory_group *group);
void remove_memory_block_devices(unsigned long start, unsigned long size);
extern void memory_dev_init(void);
extern int memory_notify(unsigned long val, void *v);
extern struct memory_block *find_memory_block(unsigned long section_nr);
typedef int (*walk_memory_blocks_func_t)(struct memory_block *, void *);
extern int walk_memory_blocks(unsigned long start, unsigned long size,
			      void *arg, walk_memory_blocks_func_t func);
extern int for_each_memory_block(void *arg, walk_memory_blocks_func_t func);

extern int memory_group_register_static(int nid, unsigned long max_pages);
extern int memory_group_register_dynamic(int nid, unsigned long unit_pages);
extern int memory_group_unregister(int mgid);
struct memory_group *memory_group_find_by_id(int mgid);
typedef int (*walk_memory_groups_func_t)(struct memory_group *, void *);
int walk_dynamic_memory_groups(int nid, walk_memory_groups_func_t func,
			       struct memory_group *excluded, void *arg);
#define hotplug_memory_notifier(fn, pri) ({		\
	static __meminitdata struct notifier_block fn##_mem_nb =\
		{ .notifier_call = fn, .priority = pri };\
	register_memory_notifier(&fn##_mem_nb);			\
})

#ifdef CONFIG_NUMA
void memory_block_add_nid(struct memory_block *mem, int nid,
			  enum meminit_context context);
#endif 
#endif	


extern struct mutex text_mutex;

#endif 
