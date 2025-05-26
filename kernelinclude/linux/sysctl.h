/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_SYSCTL_H
#define _LINUX_SYSCTL_H

#include <linux/list.h>
#include <linux/rcupdate.h>
#include <linux/wait.h>
#include <linux/rbtree.h>
#include <linux/uidgid.h>
#include <uapi/linux/sysctl.h>


struct completion;
struct ctl_table;
struct nsproxy;
struct ctl_table_root;
struct ctl_table_header;
struct ctl_dir;


#define SYSCTL_ZERO			((void *)&sysctl_vals[0])
#define SYSCTL_ONE			((void *)&sysctl_vals[1])
#define SYSCTL_TWO			((void *)&sysctl_vals[2])
#define SYSCTL_THREE			((void *)&sysctl_vals[3])
#define SYSCTL_FOUR			((void *)&sysctl_vals[4])
#define SYSCTL_ONE_HUNDRED		((void *)&sysctl_vals[5])
#define SYSCTL_TWO_HUNDRED		((void *)&sysctl_vals[6])
#define SYSCTL_ONE_THOUSAND		((void *)&sysctl_vals[7])
#define SYSCTL_THREE_THOUSAND		((void *)&sysctl_vals[8])
#define SYSCTL_INT_MAX			((void *)&sysctl_vals[9])


#define SYSCTL_MAXOLDUID		((void *)&sysctl_vals[10])
#define SYSCTL_NEG_ONE			((void *)&sysctl_vals[11])

extern const int sysctl_vals[];

#define SYSCTL_LONG_ZERO	((void *)&sysctl_long_vals[0])
#define SYSCTL_LONG_ONE		((void *)&sysctl_long_vals[1])
#define SYSCTL_LONG_MAX		((void *)&sysctl_long_vals[2])

extern const unsigned long sysctl_long_vals[];

typedef int proc_handler(struct ctl_table *ctl, int write, void *buffer,
		size_t *lenp, loff_t *ppos);

int proc_dostring(struct ctl_table *, int, void *, size_t *, loff_t *);
int proc_dobool(struct ctl_table *table, int write, void *buffer,
		size_t *lenp, loff_t *ppos);
int proc_dointvec(struct ctl_table *, int, void *, size_t *, loff_t *);
int proc_douintvec(struct ctl_table *, int, void *, size_t *, loff_t *);
int proc_dointvec_minmax(struct ctl_table *, int, void *, size_t *, loff_t *);
int proc_douintvec_minmax(struct ctl_table *table, int write, void *buffer,
		size_t *lenp, loff_t *ppos);
int proc_dou8vec_minmax(struct ctl_table *table, int write, void *buffer,
			size_t *lenp, loff_t *ppos);
int proc_dointvec_jiffies(struct ctl_table *, int, void *, size_t *, loff_t *);
int proc_dointvec_ms_jiffies_minmax(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);
int proc_dointvec_userhz_jiffies(struct ctl_table *, int, void *, size_t *,
		loff_t *);
int proc_dointvec_ms_jiffies(struct ctl_table *, int, void *, size_t *,
		loff_t *);
int proc_doulongvec_minmax(struct ctl_table *, int, void *, size_t *, loff_t *);
int proc_doulongvec_ms_jiffies_minmax(struct ctl_table *table, int, void *,
		size_t *, loff_t *);
int proc_do_large_bitmap(struct ctl_table *, int, void *, size_t *, loff_t *);
int proc_do_static_key(struct ctl_table *table, int write, void *buffer,
		size_t *lenp, loff_t *ppos);




struct ctl_table_poll {
	atomic_t event;
	wait_queue_head_t wait;
};

static inline void *proc_sys_poll_event(struct ctl_table_poll *poll)
{
	return (void *)(unsigned long)atomic_read(&poll->event);
}

#define __CTL_TABLE_POLL_INITIALIZER(name) {				\
	.event = ATOMIC_INIT(0),					\
	.wait = __WAIT_QUEUE_HEAD_INITIALIZER(name.wait) }

#define DEFINE_CTL_TABLE_POLL(name)					\
	struct ctl_table_poll name = __CTL_TABLE_POLL_INITIALIZER(name)


struct ctl_table {
	const char *procname;		
	void *data;
	int maxlen;
	umode_t mode;
	
	enum {
		SYSCTL_TABLE_TYPE_DEFAULT,
		SYSCTL_TABLE_TYPE_PERMANENTLY_EMPTY
	} type;
	proc_handler *proc_handler;	
	struct ctl_table_poll *poll;
	void *extra1;
	void *extra2;
} __randomize_layout;

struct ctl_node {
	struct rb_node node;
	struct ctl_table_header *header;
};


struct ctl_table_header {
	union {
		struct {
			struct ctl_table *ctl_table;
			int ctl_table_size;
			int used;
			int count;
			int nreg;
		};
		struct rcu_head rcu;
	};
	struct completion *unregistering;
	struct ctl_table *ctl_table_arg;
	struct ctl_table_root *root;
	struct ctl_table_set *set;
	struct ctl_dir *parent;
	struct ctl_node *node;
	struct hlist_head inodes; 
};

struct ctl_dir {
	
	struct ctl_table_header header;
	struct rb_root root;
};

struct ctl_table_set {
	int (*is_seen)(struct ctl_table_set *);
	struct ctl_dir dir;
};

struct ctl_table_root {
	struct ctl_table_set default_set;
	struct ctl_table_set *(*lookup)(struct ctl_table_root *root);
	void (*set_ownership)(struct ctl_table_header *head,
			      struct ctl_table *table,
			      kuid_t *uid, kgid_t *gid);
	int (*permissions)(struct ctl_table_header *head, struct ctl_table *table);
};


struct ctl_path {
	const char *procname;
};

#define register_sysctl(path, table)	\
	register_sysctl_sz(path, table, ARRAY_SIZE(table))

#ifdef CONFIG_SYSCTL

void proc_sys_poll_notify(struct ctl_table_poll *poll);

extern void setup_sysctl_set(struct ctl_table_set *p,
	struct ctl_table_root *root,
	int (*is_seen)(struct ctl_table_set *));
extern void retire_sysctl_set(struct ctl_table_set *set);

struct ctl_table_header *__register_sysctl_table(
	struct ctl_table_set *set,
	const char *path, struct ctl_table *table, size_t table_size);
struct ctl_table_header *register_sysctl_sz(const char *path, struct ctl_table *table,
					    size_t table_size);
void unregister_sysctl_table(struct ctl_table_header * table);

extern int sysctl_init_bases(void);
extern void __register_sysctl_init(const char *path, struct ctl_table *table,
				 const char *table_name, size_t table_size);
#define register_sysctl_init(path, table)	\
	__register_sysctl_init(path, table, #table, ARRAY_SIZE(table))
extern struct ctl_table_header *register_sysctl_mount_point(const char *path);

void do_sysctl_args(void);
bool sysctl_is_alias(char *param);
int do_proc_douintvec(struct ctl_table *table, int write,
		      void *buffer, size_t *lenp, loff_t *ppos,
		      int (*conv)(unsigned long *lvalp,
				  unsigned int *valp,
				  int write, void *data),
		      void *data);

extern int pwrsw_enabled;
extern int unaligned_enabled;
extern int unaligned_dump_stack;
extern int no_unaligned_warning;

#define SYSCTL_PERM_EMPTY_DIR	(1 << 0)

#else 

static inline void register_sysctl_init(const char *path, struct ctl_table *table)
{
}

static inline struct ctl_table_header *register_sysctl_mount_point(const char *path)
{
	return NULL;
}

static inline struct ctl_table_header *register_sysctl_sz(const char *path,
							  struct ctl_table *table,
							  size_t table_size)
{
	return NULL;
}

static inline void unregister_sysctl_table(struct ctl_table_header * table)
{
}

static inline void setup_sysctl_set(struct ctl_table_set *p,
	struct ctl_table_root *root,
	int (*is_seen)(struct ctl_table_set *))
{
}

static inline void do_sysctl_args(void)
{
}

static inline bool sysctl_is_alias(char *param)
{
	return false;
}
#endif 

int sysctl_max_threads(struct ctl_table *table, int write, void *buffer,
		size_t *lenp, loff_t *ppos);

#endif 
