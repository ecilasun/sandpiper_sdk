/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_FSVERITY_H
#define _LINUX_FSVERITY_H

#include <linux/fs.h>
#include <linux/mm.h>
#include <crypto/hash_info.h>
#include <crypto/sha2.h>
#include <uapi/linux/fsverity.h>


#define FS_VERITY_MAX_DIGEST_SIZE	SHA512_DIGEST_SIZE


#define FS_VERITY_MAX_DESCRIPTOR_SIZE	16384


struct fsverity_operations {

	
	int (*begin_enable_verity)(struct file *filp);

	
	int (*end_enable_verity)(struct file *filp, const void *desc,
				 size_t desc_size, u64 merkle_tree_size);

	
	int (*get_verity_descriptor)(struct inode *inode, void *buf,
				     size_t bufsize);

	
	struct page *(*read_merkle_tree_page)(struct inode *inode,
					      pgoff_t index,
					      unsigned long num_ra_pages);

	
	int (*write_merkle_tree_block)(struct inode *inode, const void *buf,
				       u64 pos, unsigned int size);
};

#ifdef CONFIG_FS_VERITY

static inline struct fsverity_info *fsverity_get_info(const struct inode *inode)
{
	
	return smp_load_acquire(&inode->i_verity_info);
}



int fsverity_ioctl_enable(struct file *filp, const void __user *arg);



int fsverity_ioctl_measure(struct file *filp, void __user *arg);
int fsverity_get_digest(struct inode *inode,
			u8 raw_digest[FS_VERITY_MAX_DIGEST_SIZE],
			u8 *alg, enum hash_algo *halg);



int __fsverity_file_open(struct inode *inode, struct file *filp);
int __fsverity_prepare_setattr(struct dentry *dentry, struct iattr *attr);
void __fsverity_cleanup_inode(struct inode *inode);


static inline void fsverity_cleanup_inode(struct inode *inode)
{
	if (inode->i_verity_info)
		__fsverity_cleanup_inode(inode);
}



int fsverity_ioctl_read_metadata(struct file *filp, const void __user *uarg);



bool fsverity_verify_blocks(struct folio *folio, size_t len, size_t offset);
void fsverity_verify_bio(struct bio *bio);
void fsverity_enqueue_verify_work(struct work_struct *work);

#else 

static inline struct fsverity_info *fsverity_get_info(const struct inode *inode)
{
	return NULL;
}



static inline int fsverity_ioctl_enable(struct file *filp,
					const void __user *arg)
{
	return -EOPNOTSUPP;
}



static inline int fsverity_ioctl_measure(struct file *filp, void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fsverity_get_digest(struct inode *inode,
				      u8 raw_digest[FS_VERITY_MAX_DIGEST_SIZE],
				      u8 *alg, enum hash_algo *halg)
{
	
	return 0;
}



static inline int __fsverity_file_open(struct inode *inode, struct file *filp)
{
	return -EOPNOTSUPP;
}

static inline int __fsverity_prepare_setattr(struct dentry *dentry,
					     struct iattr *attr)
{
	return -EOPNOTSUPP;
}

static inline void fsverity_cleanup_inode(struct inode *inode)
{
}



static inline int fsverity_ioctl_read_metadata(struct file *filp,
					       const void __user *uarg)
{
	return -EOPNOTSUPP;
}



static inline bool fsverity_verify_blocks(struct folio *folio, size_t len,
					  size_t offset)
{
	WARN_ON_ONCE(1);
	return false;
}

static inline void fsverity_verify_bio(struct bio *bio)
{
	WARN_ON_ONCE(1);
}

static inline void fsverity_enqueue_verify_work(struct work_struct *work)
{
	WARN_ON_ONCE(1);
}

#endif	

static inline bool fsverity_verify_folio(struct folio *folio)
{
	return fsverity_verify_blocks(folio, folio_size(folio), 0);
}

static inline bool fsverity_verify_page(struct page *page)
{
	return fsverity_verify_blocks(page_folio(page), PAGE_SIZE, 0);
}


static inline bool fsverity_active(const struct inode *inode)
{
	return fsverity_get_info(inode) != NULL;
}


static inline int fsverity_file_open(struct inode *inode, struct file *filp)
{
	if (IS_VERITY(inode))
		return __fsverity_file_open(inode, filp);
	return 0;
}


static inline int fsverity_prepare_setattr(struct dentry *dentry,
					   struct iattr *attr)
{
	if (IS_VERITY(d_inode(dentry)))
		return __fsverity_prepare_setattr(dentry, attr);
	return 0;
}

#endif	
