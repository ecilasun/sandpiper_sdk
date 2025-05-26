/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_NAMEI_H
#define _LINUX_NAMEI_H

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/path.h>
#include <linux/fcntl.h>
#include <linux/errno.h>

enum { MAX_NESTED_LINKS = 8 };

#define MAXSYMLINKS 40


enum {LAST_NORM, LAST_ROOT, LAST_DOT, LAST_DOTDOT};


#define LOOKUP_FOLLOW		0x0001	
#define LOOKUP_DIRECTORY	0x0002	
#define LOOKUP_AUTOMOUNT	0x0004  
#define LOOKUP_EMPTY		0x4000	
#define LOOKUP_DOWN		0x8000	
#define LOOKUP_MOUNTPOINT	0x0080	

#define LOOKUP_REVAL		0x0020	
#define LOOKUP_RCU		0x0040	


#define LOOKUP_OPEN		0x0100	
#define LOOKUP_CREATE		0x0200	
#define LOOKUP_EXCL		0x0400	
#define LOOKUP_RENAME_TARGET	0x0800	


#define LOOKUP_PARENT		0x0010


#define LOOKUP_NO_SYMLINKS	0x010000 
#define LOOKUP_NO_MAGICLINKS	0x020000 
#define LOOKUP_NO_XDEV		0x040000 
#define LOOKUP_BENEATH		0x080000 
#define LOOKUP_IN_ROOT		0x100000 
#define LOOKUP_CACHED		0x200000 

#define LOOKUP_IS_SCOPED (LOOKUP_BENEATH | LOOKUP_IN_ROOT)

extern int path_pts(struct path *path);

extern int user_path_at_empty(int, const char __user *, unsigned, struct path *, int *empty);

static inline int user_path_at(int dfd, const char __user *name, unsigned flags,
		 struct path *path)
{
	return user_path_at_empty(dfd, name, flags, path, NULL);
}

struct dentry *lookup_one_qstr_excl(const struct qstr *name,
				    struct dentry *base,
				    unsigned int flags);
extern int kern_path(const char *, unsigned, struct path *);

extern struct dentry *kern_path_create(int, const char *, struct path *, unsigned int);
extern struct dentry *user_path_create(int, const char __user *, struct path *, unsigned int);
extern void done_path_create(struct path *, struct dentry *);
extern struct dentry *kern_path_locked(const char *, struct path *);
int vfs_path_parent_lookup(struct filename *filename, unsigned int flags,
			   struct path *parent, struct qstr *last, int *type,
			   const struct path *root);
int vfs_path_lookup(struct dentry *, struct vfsmount *, const char *,
		    unsigned int, struct path *);

extern struct dentry *try_lookup_one_len(const char *, struct dentry *, int);
extern struct dentry *lookup_one_len(const char *, struct dentry *, int);
extern struct dentry *lookup_one_len_unlocked(const char *, struct dentry *, int);
extern struct dentry *lookup_positive_unlocked(const char *, struct dentry *, int);
struct dentry *lookup_one(struct mnt_idmap *, const char *, struct dentry *, int);
struct dentry *lookup_one_unlocked(struct mnt_idmap *idmap,
				   const char *name, struct dentry *base,
				   int len);
struct dentry *lookup_one_positive_unlocked(struct mnt_idmap *idmap,
					    const char *name,
					    struct dentry *base, int len);

extern int follow_down_one(struct path *);
extern int follow_down(struct path *path, unsigned int flags);
extern int follow_up(struct path *);

extern struct dentry *lock_rename(struct dentry *, struct dentry *);
extern struct dentry *lock_rename_child(struct dentry *, struct dentry *);
extern void unlock_rename(struct dentry *, struct dentry *);

extern int __must_check nd_jump_link(const struct path *path);

static inline void nd_terminate_link(void *name, size_t len, size_t maxlen)
{
	((char *) name)[min(len, maxlen)] = '\0';
}


static inline bool
retry_estale(const long error, const unsigned int flags)
{
	return error == -ESTALE && !(flags & LOOKUP_REVAL);
}

#endif 
