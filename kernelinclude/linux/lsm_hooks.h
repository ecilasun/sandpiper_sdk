

#ifndef __LINUX_LSM_HOOKS_H
#define __LINUX_LSM_HOOKS_H

#include <linux/security.h>
#include <linux/init.h>
#include <linux/rculist.h>
#include <linux/xattr.h>

union security_list_options {
	#define LSM_HOOK(RET, DEFAULT, NAME, ...) RET (*NAME)(__VA_ARGS__);
	#include "lsm_hook_defs.h"
	#undef LSM_HOOK
};

struct security_hook_heads {
	#define LSM_HOOK(RET, DEFAULT, NAME, ...) struct hlist_head NAME;
	#include "lsm_hook_defs.h"
	#undef LSM_HOOK
} __randomize_layout;


struct security_hook_list {
	struct hlist_node		list;
	struct hlist_head		*head;
	union security_list_options	hook;
	const char			*lsm;
} __randomize_layout;


struct lsm_blob_sizes {
	int	lbs_cred;
	int	lbs_file;
	int	lbs_inode;
	int	lbs_superblock;
	int	lbs_ipc;
	int	lbs_msg_msg;
	int	lbs_task;
	int	lbs_xattr_count; 
};


static inline struct xattr *lsm_get_xattr_slot(struct xattr *xattrs,
					       int *xattr_count)
{
	if (unlikely(!xattrs))
		return NULL;
	return &xattrs[(*xattr_count)++];
}


#define LSM_RET_VOID ((void) 0)


#define LSM_HOOK_INIT(HEAD, HOOK) \
	{ .head = &security_hook_heads.HEAD, .hook = { .HEAD = HOOK } }

extern struct security_hook_heads security_hook_heads;
extern char *lsm_names;

extern void security_add_hooks(struct security_hook_list *hooks, int count,
				const char *lsm);

#define LSM_FLAG_LEGACY_MAJOR	BIT(0)
#define LSM_FLAG_EXCLUSIVE	BIT(1)

enum lsm_order {
	LSM_ORDER_FIRST = -1,	
	LSM_ORDER_MUTABLE = 0,
	LSM_ORDER_LAST = 1,	
};

struct lsm_info {
	const char *name;	
	enum lsm_order order;	
	unsigned long flags;	
	int *enabled;		
	int (*init)(void);	
	struct lsm_blob_sizes *blobs; 
};

extern struct lsm_info __start_lsm_info[], __end_lsm_info[];
extern struct lsm_info __start_early_lsm_info[], __end_early_lsm_info[];

#define DEFINE_LSM(lsm)							\
	static struct lsm_info __lsm_##lsm				\
		__used __section(".lsm_info.init")			\
		__aligned(sizeof(unsigned long))

#define DEFINE_EARLY_LSM(lsm)						\
	static struct lsm_info __early_lsm_##lsm			\
		__used __section(".early_lsm_info.init")		\
		__aligned(sizeof(unsigned long))

extern int lsm_inode_alloc(struct inode *inode);

#endif 
