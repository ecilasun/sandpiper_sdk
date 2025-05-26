/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __SHMEM_FS_H
#define __SHMEM_FS_H

#include <linux/file.h>
#include <linux/swap.h>
#include <linux/mempolicy.h>
#include <linux/pagemap.h>
#include <linux/percpu_counter.h>
#include <linux/xattr.h>
#include <linux/fs_parser.h>
#include <linux/userfaultfd_k.h>



#ifdef CONFIG_TMPFS_QUOTA
#define SHMEM_MAXQUOTAS 2
#endif

struct shmem_inode_info {
	spinlock_t		lock;
	unsigned int		seals;		
	unsigned long		flags;
	unsigned long		alloced;	
	unsigned long		swapped;	
	pgoff_t			fallocend;	
	struct list_head        shrinklist;     
	struct list_head	swaplist;	
	struct shared_policy	policy;		
	struct simple_xattrs	xattrs;		
	atomic_t		stop_eviction;	
	struct timespec64	i_crtime;	
	unsigned int		fsflags;	
#ifdef CONFIG_TMPFS_QUOTA
	struct dquot __rcu	*i_dquot[MAXQUOTAS];
#endif
	struct offset_ctx	dir_offsets;	
	struct inode		vfs_inode;
};

#define SHMEM_FL_USER_VISIBLE		FS_FL_USER_VISIBLE
#define SHMEM_FL_USER_MODIFIABLE \
	(FS_IMMUTABLE_FL | FS_APPEND_FL | FS_NODUMP_FL | FS_NOATIME_FL)
#define SHMEM_FL_INHERITED		(FS_NODUMP_FL | FS_NOATIME_FL)

struct shmem_quota_limits {
	qsize_t usrquota_bhardlimit; 
	qsize_t usrquota_ihardlimit; 
	qsize_t grpquota_bhardlimit; 
	qsize_t grpquota_ihardlimit; 
};

struct shmem_sb_info {
	unsigned long max_blocks;   
	struct percpu_counter used_blocks;  
	unsigned long max_inodes;   
	unsigned long free_ispace;  
	raw_spinlock_t stat_lock;   
	umode_t mode;		    
	unsigned char huge;	    
	kuid_t uid;		    
	kgid_t gid;		    
	bool full_inums;	    
	bool noswap;		    
	ino_t next_ino;		    
	ino_t __percpu *ino_batch;  
	struct mempolicy *mpol;     
	spinlock_t shrinklist_lock;   
	struct list_head shrinklist;  
	unsigned long shrinklist_len; 
	struct shmem_quota_limits qlimits; 
};

static inline struct shmem_inode_info *SHMEM_I(struct inode *inode)
{
	return container_of(inode, struct shmem_inode_info, vfs_inode);
}


extern const struct fs_parameter_spec shmem_fs_parameters[];
extern void shmem_init(void);
extern int shmem_init_fs_context(struct fs_context *fc);
extern struct file *shmem_file_setup(const char *name,
					loff_t size, unsigned long flags);
extern struct file *shmem_kernel_file_setup(const char *name, loff_t size,
					    unsigned long flags);
extern struct file *shmem_file_setup_with_mnt(struct vfsmount *mnt,
		const char *name, loff_t size, unsigned long flags);
extern int shmem_zero_setup(struct vm_area_struct *);
extern unsigned long shmem_get_unmapped_area(struct file *, unsigned long addr,
		unsigned long len, unsigned long pgoff, unsigned long flags);
extern int shmem_lock(struct file *file, int lock, struct ucounts *ucounts);
#ifdef CONFIG_SHMEM
extern const struct address_space_operations shmem_aops;
static inline bool shmem_mapping(struct address_space *mapping)
{
	return mapping->a_ops == &shmem_aops;
}
#else
static inline bool shmem_mapping(struct address_space *mapping)
{
	return false;
}
#endif 
extern void shmem_unlock_mapping(struct address_space *mapping);
extern struct page *shmem_read_mapping_page_gfp(struct address_space *mapping,
					pgoff_t index, gfp_t gfp_mask);
extern void shmem_truncate_range(struct inode *inode, loff_t start, loff_t end);
int shmem_unuse(unsigned int type);

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
extern bool shmem_is_huge(struct inode *inode, pgoff_t index, bool shmem_huge_force,
			  struct mm_struct *mm, unsigned long vm_flags);
#else
static __always_inline bool shmem_is_huge(struct inode *inode, pgoff_t index, bool shmem_huge_force,
					  struct mm_struct *mm, unsigned long vm_flags)
{
	return false;
}
#endif

#ifdef CONFIG_SHMEM
extern unsigned long shmem_swap_usage(struct vm_area_struct *vma);
#else
static inline unsigned long shmem_swap_usage(struct vm_area_struct *vma)
{
	return 0;
}
#endif
extern unsigned long shmem_partial_swap_usage(struct address_space *mapping,
						pgoff_t start, pgoff_t end);


enum sgp_type {
	SGP_READ,	
	SGP_NOALLOC,	
	SGP_CACHE,	
	SGP_WRITE,	
	SGP_FALLOC,	
};

int shmem_get_folio(struct inode *inode, pgoff_t index, struct folio **foliop,
		enum sgp_type sgp);
struct folio *shmem_read_folio_gfp(struct address_space *mapping,
		pgoff_t index, gfp_t gfp);

static inline struct folio *shmem_read_folio(struct address_space *mapping,
		pgoff_t index)
{
	return shmem_read_folio_gfp(mapping, index, mapping_gfp_mask(mapping));
}

static inline struct page *shmem_read_mapping_page(
				struct address_space *mapping, pgoff_t index)
{
	return shmem_read_mapping_page_gfp(mapping, index,
					mapping_gfp_mask(mapping));
}

static inline bool shmem_file(struct file *file)
{
	if (!IS_ENABLED(CONFIG_SHMEM))
		return false;
	if (!file || !file->f_mapping)
		return false;
	return shmem_mapping(file->f_mapping);
}


static inline pgoff_t shmem_fallocend(struct inode *inode, pgoff_t eof)
{
	return max(eof, SHMEM_I(inode)->fallocend);
}

extern bool shmem_charge(struct inode *inode, long pages);
extern void shmem_uncharge(struct inode *inode, long pages);

#ifdef CONFIG_USERFAULTFD
#ifdef CONFIG_SHMEM
extern int shmem_mfill_atomic_pte(pmd_t *dst_pmd,
				  struct vm_area_struct *dst_vma,
				  unsigned long dst_addr,
				  unsigned long src_addr,
				  uffd_flags_t flags,
				  struct folio **foliop);
#else 
#define shmem_mfill_atomic_pte(dst_pmd, dst_vma, dst_addr, \
			       src_addr, flags, foliop) ({ BUG(); 0; })
#endif 
#endif 


#define SHMEM_QUOTA_MAX_SPC_LIMIT 0x7fffffffffffffffLL 
#define SHMEM_QUOTA_MAX_INO_LIMIT 0x7fffffffffffffffLL

#ifdef CONFIG_TMPFS_QUOTA
extern const struct dquot_operations shmem_quota_operations;
extern struct quota_format_type shmem_quota_format;
#endif 

#endif
