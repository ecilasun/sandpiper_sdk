/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_STATFS_H
#define _LINUX_STATFS_H

#include <linux/types.h>
#include <asm/statfs.h>
#include <asm/byteorder.h>

struct kstatfs {
	long f_type;
	long f_bsize;
	u64 f_blocks;
	u64 f_bfree;
	u64 f_bavail;
	u64 f_files;
	u64 f_ffree;
	__kernel_fsid_t f_fsid;
	long f_namelen;
	long f_frsize;
	long f_flags;
	long f_spare[4];
};


#define ST_RDONLY	0x0001	
#define ST_NOSUID	0x0002	
#define ST_NODEV	0x0004	
#define ST_NOEXEC	0x0008	
#define ST_SYNCHRONOUS	0x0010	
#define ST_VALID	0x0020	
#define ST_MANDLOCK	0x0040	



#define ST_NOATIME	0x0400	
#define ST_NODIRATIME	0x0800	
#define ST_RELATIME	0x1000	
#define ST_NOSYMFOLLOW	0x2000	

struct dentry;
extern int vfs_get_fsid(struct dentry *dentry, __kernel_fsid_t *fsid);

static inline __kernel_fsid_t u64_to_fsid(u64 v)
{
	return (__kernel_fsid_t){.val = {(u32)v, (u32)(v>>32)}};
}


static inline __kernel_fsid_t uuid_to_fsid(__u8 *uuid)
{
	return u64_to_fsid(le64_to_cpup((void *)uuid) ^
		le64_to_cpup((void *)(uuid + sizeof(u64))));
}

#endif
