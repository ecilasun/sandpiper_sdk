/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_LOCKDEP_TYPES_H
#define __LINUX_LOCKDEP_TYPES_H

#include <linux/types.h>

#define MAX_LOCKDEP_SUBCLASSES		8UL

enum lockdep_wait_type {
	LD_WAIT_INV = 0,	

	LD_WAIT_FREE,		
	LD_WAIT_SPIN,		

#ifdef CONFIG_PROVE_RAW_LOCK_NESTING
	LD_WAIT_CONFIG,		
#else
	LD_WAIT_CONFIG = LD_WAIT_SPIN,
#endif
	LD_WAIT_SLEEP,		

	LD_WAIT_MAX,		
};

enum lockdep_lock_type {
	LD_LOCK_NORMAL = 0,	
	LD_LOCK_PERCPU,		
	LD_LOCK_WAIT_OVERRIDE,	
	LD_LOCK_MAX,
};

#ifdef CONFIG_LOCKDEP


#define XXX_LOCK_USAGE_STATES		2
#define LOCK_TRACE_STATES		(XXX_LOCK_USAGE_STATES*4 + 2)


#define NR_LOCKDEP_CACHING_CLASSES	2


struct lockdep_subclass_key {
	char __one_byte;
} __attribute__ ((__packed__));


struct lock_class_key {
	union {
		struct hlist_node		hash_entry;
		struct lockdep_subclass_key	subkeys[MAX_LOCKDEP_SUBCLASSES];
	};
};

extern struct lock_class_key __lockdep_no_validate__;

struct lock_trace;

#define LOCKSTAT_POINTS		4

struct lockdep_map;
typedef int (*lock_cmp_fn)(const struct lockdep_map *a,
			   const struct lockdep_map *b);
typedef void (*lock_print_fn)(const struct lockdep_map *map);


struct lock_class {
	
	struct hlist_node		hash_entry;

	
	struct list_head		lock_entry;

	
	struct list_head		locks_after, locks_before;

	const struct lockdep_subclass_key *key;
	lock_cmp_fn			cmp_fn;
	lock_print_fn			print_fn;

	unsigned int			subclass;
	unsigned int			dep_gen_id;

	
	unsigned long			usage_mask;
	const struct lock_trace		*usage_traces[LOCK_TRACE_STATES];

	
	int				name_version;
	const char			*name;

	u8				wait_type_inner;
	u8				wait_type_outer;
	u8				lock_type;
	

#ifdef CONFIG_LOCK_STAT
	unsigned long			contention_point[LOCKSTAT_POINTS];
	unsigned long			contending_point[LOCKSTAT_POINTS];
#endif
} __no_randomize_layout;

#ifdef CONFIG_LOCK_STAT
struct lock_time {
	s64				min;
	s64				max;
	s64				total;
	unsigned long			nr;
};

enum bounce_type {
	bounce_acquired_write,
	bounce_acquired_read,
	bounce_contended_write,
	bounce_contended_read,
	nr_bounce_types,

	bounce_acquired = bounce_acquired_write,
	bounce_contended = bounce_contended_write,
};

struct lock_class_stats {
	unsigned long			contention_point[LOCKSTAT_POINTS];
	unsigned long			contending_point[LOCKSTAT_POINTS];
	struct lock_time		read_waittime;
	struct lock_time		write_waittime;
	struct lock_time		read_holdtime;
	struct lock_time		write_holdtime;
	unsigned long			bounces[nr_bounce_types];
};

struct lock_class_stats lock_stats(struct lock_class *class);
void clear_lock_stats(struct lock_class *class);
#endif


struct lockdep_map {
	struct lock_class_key		*key;
	struct lock_class		*class_cache[NR_LOCKDEP_CACHING_CLASSES];
	const char			*name;
	u8				wait_type_outer; 
	u8				wait_type_inner; 
	u8				lock_type;
	
#ifdef CONFIG_LOCK_STAT
	int				cpu;
	unsigned long			ip;
#endif
};

struct pin_cookie { unsigned int val; };

#else 


struct lock_class_key { };


struct lockdep_map { };

struct pin_cookie { };

#endif 

#endif 
