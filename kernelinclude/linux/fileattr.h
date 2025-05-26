/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_FILEATTR_H
#define _LINUX_FILEATTR_H


#define FS_COMMON_FL \
	(FS_SYNC_FL | FS_IMMUTABLE_FL | FS_APPEND_FL | \
	 FS_NODUMP_FL |	FS_NOATIME_FL | FS_DAX_FL | \
	 FS_PROJINHERIT_FL)

#define FS_XFLAG_COMMON \
	(FS_XFLAG_SYNC | FS_XFLAG_IMMUTABLE | FS_XFLAG_APPEND | \
	 FS_XFLAG_NODUMP | FS_XFLAG_NOATIME | FS_XFLAG_DAX | \
	 FS_XFLAG_PROJINHERIT)


struct fileattr {
	u32	flags;		
	
	u32	fsx_xflags;	
	u32	fsx_extsize;	
	u32	fsx_nextents;	
	u32	fsx_projid;	
	u32	fsx_cowextsize;	
	
	bool	flags_valid:1;
	bool	fsx_valid:1;
};

int copy_fsxattr_to_user(const struct fileattr *fa, struct fsxattr __user *ufa);

void fileattr_fill_xflags(struct fileattr *fa, u32 xflags);
void fileattr_fill_flags(struct fileattr *fa, u32 flags);


static inline bool fileattr_has_fsx(const struct fileattr *fa)
{
	return fa->fsx_valid &&
		((fa->fsx_xflags & ~FS_XFLAG_COMMON) || fa->fsx_extsize != 0 ||
		 fa->fsx_projid != 0 ||	fa->fsx_cowextsize != 0);
}

int vfs_fileattr_get(struct dentry *dentry, struct fileattr *fa);
int vfs_fileattr_set(struct mnt_idmap *idmap, struct dentry *dentry,
		     struct fileattr *fa);

#endif 
