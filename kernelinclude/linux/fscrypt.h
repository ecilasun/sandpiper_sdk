/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_FSCRYPT_H
#define _LINUX_FSCRYPT_H

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <uapi/linux/fscrypt.h>


#define FSCRYPT_CONTENTS_ALIGNMENT 16

union fscrypt_policy;
struct fscrypt_info;
struct fs_parameter;
struct seq_file;

struct fscrypt_str {
	unsigned char *name;
	u32 len;
};

struct fscrypt_name {
	const struct qstr *usr_fname;
	struct fscrypt_str disk_name;
	u32 hash;
	u32 minor_hash;
	struct fscrypt_str crypto_buf;
	bool is_nokey_name;
};

#define FSTR_INIT(n, l)		{ .name = n, .len = l }
#define FSTR_TO_QSTR(f)		QSTR_INIT((f)->name, (f)->len)
#define fname_name(p)		((p)->disk_name.name)
#define fname_len(p)		((p)->disk_name.len)


#define FSCRYPT_SET_CONTEXT_MAX_SIZE	40

#ifdef CONFIG_FS_ENCRYPTION


#define FS_CFLG_OWN_PAGES (1U << 1)


struct fscrypt_operations {

	
	unsigned int flags;

	
	const char *key_prefix;

	
	int (*get_context)(struct inode *inode, void *ctx, size_t len);

	
	int (*set_context)(struct inode *inode, const void *ctx, size_t len,
			   void *fs_data);

	
	const union fscrypt_policy *(*get_dummy_policy)(struct super_block *sb);

	
	bool (*empty_dir)(struct inode *inode);

	
	bool (*has_stable_inodes)(struct super_block *sb);

	
	void (*get_ino_and_lblk_bits)(struct super_block *sb,
				      int *ino_bits_ret, int *lblk_bits_ret);

	
	struct block_device **(*get_devices)(struct super_block *sb,
					     unsigned int *num_devs);
};

static inline struct fscrypt_info *fscrypt_get_info(const struct inode *inode)
{
	
	return smp_load_acquire(&inode->i_crypt_info);
}


static inline bool fscrypt_needs_contents_encryption(const struct inode *inode)
{
	return IS_ENCRYPTED(inode) && S_ISREG(inode->i_mode);
}


static inline void fscrypt_handle_d_move(struct dentry *dentry)
{
	dentry->d_flags &= ~DCACHE_NOKEY_NAME;
}


static inline bool fscrypt_is_nokey_name(const struct dentry *dentry)
{
	return dentry->d_flags & DCACHE_NOKEY_NAME;
}


void fscrypt_enqueue_decrypt_work(struct work_struct *);

struct page *fscrypt_encrypt_pagecache_blocks(struct page *page,
					      unsigned int len,
					      unsigned int offs,
					      gfp_t gfp_flags);
int fscrypt_encrypt_block_inplace(const struct inode *inode, struct page *page,
				  unsigned int len, unsigned int offs,
				  u64 lblk_num, gfp_t gfp_flags);

int fscrypt_decrypt_pagecache_blocks(struct folio *folio, size_t len,
				     size_t offs);
int fscrypt_decrypt_block_inplace(const struct inode *inode, struct page *page,
				  unsigned int len, unsigned int offs,
				  u64 lblk_num);

static inline bool fscrypt_is_bounce_page(struct page *page)
{
	return page->mapping == NULL;
}

static inline struct page *fscrypt_pagecache_page(struct page *bounce_page)
{
	return (struct page *)page_private(bounce_page);
}

static inline bool fscrypt_is_bounce_folio(struct folio *folio)
{
	return folio->mapping == NULL;
}

static inline struct folio *fscrypt_pagecache_folio(struct folio *bounce_folio)
{
	return bounce_folio->private;
}

void fscrypt_free_bounce_page(struct page *bounce_page);


int fscrypt_ioctl_set_policy(struct file *filp, const void __user *arg);
int fscrypt_ioctl_get_policy(struct file *filp, void __user *arg);
int fscrypt_ioctl_get_policy_ex(struct file *filp, void __user *arg);
int fscrypt_ioctl_get_nonce(struct file *filp, void __user *arg);
int fscrypt_has_permitted_context(struct inode *parent, struct inode *child);
int fscrypt_context_for_new_inode(void *ctx, struct inode *inode);
int fscrypt_set_context(struct inode *inode, void *fs_data);

struct fscrypt_dummy_policy {
	const union fscrypt_policy *policy;
};

int fscrypt_parse_test_dummy_encryption(const struct fs_parameter *param,
				    struct fscrypt_dummy_policy *dummy_policy);
bool fscrypt_dummy_policies_equal(const struct fscrypt_dummy_policy *p1,
				  const struct fscrypt_dummy_policy *p2);
void fscrypt_show_test_dummy_encryption(struct seq_file *seq, char sep,
					struct super_block *sb);
static inline bool
fscrypt_is_dummy_policy_set(const struct fscrypt_dummy_policy *dummy_policy)
{
	return dummy_policy->policy != NULL;
}
static inline void
fscrypt_free_dummy_policy(struct fscrypt_dummy_policy *dummy_policy)
{
	kfree(dummy_policy->policy);
	dummy_policy->policy = NULL;
}


void fscrypt_destroy_keyring(struct super_block *sb);
int fscrypt_ioctl_add_key(struct file *filp, void __user *arg);
int fscrypt_ioctl_remove_key(struct file *filp, void __user *arg);
int fscrypt_ioctl_remove_key_all_users(struct file *filp, void __user *arg);
int fscrypt_ioctl_get_key_status(struct file *filp, void __user *arg);


int fscrypt_prepare_new_inode(struct inode *dir, struct inode *inode,
			      bool *encrypt_ret);
void fscrypt_put_encryption_info(struct inode *inode);
void fscrypt_free_inode(struct inode *inode);
int fscrypt_drop_inode(struct inode *inode);


int fscrypt_fname_encrypt(const struct inode *inode, const struct qstr *iname,
			  u8 *out, unsigned int olen);
bool fscrypt_fname_encrypted_size(const struct inode *inode, u32 orig_len,
				  u32 max_len, u32 *encrypted_len_ret);
int fscrypt_setup_filename(struct inode *inode, const struct qstr *iname,
			   int lookup, struct fscrypt_name *fname);

static inline void fscrypt_free_filename(struct fscrypt_name *fname)
{
	kfree(fname->crypto_buf.name);
}

int fscrypt_fname_alloc_buffer(u32 max_encrypted_len,
			       struct fscrypt_str *crypto_str);
void fscrypt_fname_free_buffer(struct fscrypt_str *crypto_str);
int fscrypt_fname_disk_to_usr(const struct inode *inode,
			      u32 hash, u32 minor_hash,
			      const struct fscrypt_str *iname,
			      struct fscrypt_str *oname);
bool fscrypt_match_name(const struct fscrypt_name *fname,
			const u8 *de_name, u32 de_name_len);
u64 fscrypt_fname_siphash(const struct inode *dir, const struct qstr *name);
int fscrypt_d_revalidate(struct dentry *dentry, unsigned int flags);


bool fscrypt_decrypt_bio(struct bio *bio);
int fscrypt_zeroout_range(const struct inode *inode, pgoff_t lblk,
			  sector_t pblk, unsigned int len);


int fscrypt_file_open(struct inode *inode, struct file *filp);
int __fscrypt_prepare_link(struct inode *inode, struct inode *dir,
			   struct dentry *dentry);
int __fscrypt_prepare_rename(struct inode *old_dir, struct dentry *old_dentry,
			     struct inode *new_dir, struct dentry *new_dentry,
			     unsigned int flags);
int __fscrypt_prepare_lookup(struct inode *dir, struct dentry *dentry,
			     struct fscrypt_name *fname);
int fscrypt_prepare_lookup_partial(struct inode *dir, struct dentry *dentry);
int __fscrypt_prepare_readdir(struct inode *dir);
int __fscrypt_prepare_setattr(struct dentry *dentry, struct iattr *attr);
int fscrypt_prepare_setflags(struct inode *inode,
			     unsigned int oldflags, unsigned int flags);
int fscrypt_prepare_symlink(struct inode *dir, const char *target,
			    unsigned int len, unsigned int max_len,
			    struct fscrypt_str *disk_link);
int __fscrypt_encrypt_symlink(struct inode *inode, const char *target,
			      unsigned int len, struct fscrypt_str *disk_link);
const char *fscrypt_get_symlink(struct inode *inode, const void *caddr,
				unsigned int max_size,
				struct delayed_call *done);
int fscrypt_symlink_getattr(const struct path *path, struct kstat *stat);
static inline void fscrypt_set_ops(struct super_block *sb,
				   const struct fscrypt_operations *s_cop)
{
	sb->s_cop = s_cop;
}
#else  

static inline struct fscrypt_info *fscrypt_get_info(const struct inode *inode)
{
	return NULL;
}

static inline bool fscrypt_needs_contents_encryption(const struct inode *inode)
{
	return false;
}

static inline void fscrypt_handle_d_move(struct dentry *dentry)
{
}

static inline bool fscrypt_is_nokey_name(const struct dentry *dentry)
{
	return false;
}


static inline void fscrypt_enqueue_decrypt_work(struct work_struct *work)
{
}

static inline struct page *fscrypt_encrypt_pagecache_blocks(struct page *page,
							    unsigned int len,
							    unsigned int offs,
							    gfp_t gfp_flags)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline int fscrypt_encrypt_block_inplace(const struct inode *inode,
						struct page *page,
						unsigned int len,
						unsigned int offs, u64 lblk_num,
						gfp_t gfp_flags)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_decrypt_pagecache_blocks(struct folio *folio,
						   size_t len, size_t offs)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_decrypt_block_inplace(const struct inode *inode,
						struct page *page,
						unsigned int len,
						unsigned int offs, u64 lblk_num)
{
	return -EOPNOTSUPP;
}

static inline bool fscrypt_is_bounce_page(struct page *page)
{
	return false;
}

static inline struct page *fscrypt_pagecache_page(struct page *bounce_page)
{
	WARN_ON_ONCE(1);
	return ERR_PTR(-EINVAL);
}

static inline bool fscrypt_is_bounce_folio(struct folio *folio)
{
	return false;
}

static inline struct folio *fscrypt_pagecache_folio(struct folio *bounce_folio)
{
	WARN_ON_ONCE(1);
	return ERR_PTR(-EINVAL);
}

static inline void fscrypt_free_bounce_page(struct page *bounce_page)
{
}


static inline int fscrypt_ioctl_set_policy(struct file *filp,
					   const void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_ioctl_get_policy(struct file *filp, void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_ioctl_get_policy_ex(struct file *filp,
					      void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_ioctl_get_nonce(struct file *filp, void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_has_permitted_context(struct inode *parent,
						struct inode *child)
{
	return 0;
}

static inline int fscrypt_set_context(struct inode *inode, void *fs_data)
{
	return -EOPNOTSUPP;
}

struct fscrypt_dummy_policy {
};

static inline int
fscrypt_parse_test_dummy_encryption(const struct fs_parameter *param,
				    struct fscrypt_dummy_policy *dummy_policy)
{
	return -EINVAL;
}

static inline bool
fscrypt_dummy_policies_equal(const struct fscrypt_dummy_policy *p1,
			     const struct fscrypt_dummy_policy *p2)
{
	return true;
}

static inline void fscrypt_show_test_dummy_encryption(struct seq_file *seq,
						      char sep,
						      struct super_block *sb)
{
}

static inline bool
fscrypt_is_dummy_policy_set(const struct fscrypt_dummy_policy *dummy_policy)
{
	return false;
}

static inline void
fscrypt_free_dummy_policy(struct fscrypt_dummy_policy *dummy_policy)
{
}


static inline void fscrypt_destroy_keyring(struct super_block *sb)
{
}

static inline int fscrypt_ioctl_add_key(struct file *filp, void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_ioctl_remove_key(struct file *filp, void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_ioctl_remove_key_all_users(struct file *filp,
						     void __user *arg)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_ioctl_get_key_status(struct file *filp,
					       void __user *arg)
{
	return -EOPNOTSUPP;
}



static inline int fscrypt_prepare_new_inode(struct inode *dir,
					    struct inode *inode,
					    bool *encrypt_ret)
{
	if (IS_ENCRYPTED(dir))
		return -EOPNOTSUPP;
	return 0;
}

static inline void fscrypt_put_encryption_info(struct inode *inode)
{
	return;
}

static inline void fscrypt_free_inode(struct inode *inode)
{
}

static inline int fscrypt_drop_inode(struct inode *inode)
{
	return 0;
}

 
static inline int fscrypt_setup_filename(struct inode *dir,
					 const struct qstr *iname,
					 int lookup, struct fscrypt_name *fname)
{
	if (IS_ENCRYPTED(dir))
		return -EOPNOTSUPP;

	memset(fname, 0, sizeof(*fname));
	fname->usr_fname = iname;
	fname->disk_name.name = (unsigned char *)iname->name;
	fname->disk_name.len = iname->len;
	return 0;
}

static inline void fscrypt_free_filename(struct fscrypt_name *fname)
{
	return;
}

static inline int fscrypt_fname_alloc_buffer(u32 max_encrypted_len,
					     struct fscrypt_str *crypto_str)
{
	return -EOPNOTSUPP;
}

static inline void fscrypt_fname_free_buffer(struct fscrypt_str *crypto_str)
{
	return;
}

static inline int fscrypt_fname_disk_to_usr(const struct inode *inode,
					    u32 hash, u32 minor_hash,
					    const struct fscrypt_str *iname,
					    struct fscrypt_str *oname)
{
	return -EOPNOTSUPP;
}

static inline bool fscrypt_match_name(const struct fscrypt_name *fname,
				      const u8 *de_name, u32 de_name_len)
{
	
	if (de_name_len != fname->disk_name.len)
		return false;
	return !memcmp(de_name, fname->disk_name.name, fname->disk_name.len);
}

static inline u64 fscrypt_fname_siphash(const struct inode *dir,
					const struct qstr *name)
{
	WARN_ON_ONCE(1);
	return 0;
}

static inline int fscrypt_d_revalidate(struct dentry *dentry,
				       unsigned int flags)
{
	return 1;
}


static inline bool fscrypt_decrypt_bio(struct bio *bio)
{
	return true;
}

static inline int fscrypt_zeroout_range(const struct inode *inode, pgoff_t lblk,
					sector_t pblk, unsigned int len)
{
	return -EOPNOTSUPP;
}



static inline int fscrypt_file_open(struct inode *inode, struct file *filp)
{
	if (IS_ENCRYPTED(inode))
		return -EOPNOTSUPP;
	return 0;
}

static inline int __fscrypt_prepare_link(struct inode *inode, struct inode *dir,
					 struct dentry *dentry)
{
	return -EOPNOTSUPP;
}

static inline int __fscrypt_prepare_rename(struct inode *old_dir,
					   struct dentry *old_dentry,
					   struct inode *new_dir,
					   struct dentry *new_dentry,
					   unsigned int flags)
{
	return -EOPNOTSUPP;
}

static inline int __fscrypt_prepare_lookup(struct inode *dir,
					   struct dentry *dentry,
					   struct fscrypt_name *fname)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_prepare_lookup_partial(struct inode *dir,
						 struct dentry *dentry)
{
	return -EOPNOTSUPP;
}

static inline int __fscrypt_prepare_readdir(struct inode *dir)
{
	return -EOPNOTSUPP;
}

static inline int __fscrypt_prepare_setattr(struct dentry *dentry,
					    struct iattr *attr)
{
	return -EOPNOTSUPP;
}

static inline int fscrypt_prepare_setflags(struct inode *inode,
					   unsigned int oldflags,
					   unsigned int flags)
{
	return 0;
}

static inline int fscrypt_prepare_symlink(struct inode *dir,
					  const char *target,
					  unsigned int len,
					  unsigned int max_len,
					  struct fscrypt_str *disk_link)
{
	if (IS_ENCRYPTED(dir))
		return -EOPNOTSUPP;
	disk_link->name = (unsigned char *)target;
	disk_link->len = len + 1;
	if (disk_link->len > max_len)
		return -ENAMETOOLONG;
	return 0;
}

static inline int __fscrypt_encrypt_symlink(struct inode *inode,
					    const char *target,
					    unsigned int len,
					    struct fscrypt_str *disk_link)
{
	return -EOPNOTSUPP;
}

static inline const char *fscrypt_get_symlink(struct inode *inode,
					      const void *caddr,
					      unsigned int max_size,
					      struct delayed_call *done)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline int fscrypt_symlink_getattr(const struct path *path,
					  struct kstat *stat)
{
	return -EOPNOTSUPP;
}

static inline void fscrypt_set_ops(struct super_block *sb,
				   const struct fscrypt_operations *s_cop)
{
}

#endif	


#ifdef CONFIG_FS_ENCRYPTION_INLINE_CRYPT

bool __fscrypt_inode_uses_inline_crypto(const struct inode *inode);

void fscrypt_set_bio_crypt_ctx(struct bio *bio,
			       const struct inode *inode, u64 first_lblk,
			       gfp_t gfp_mask);

void fscrypt_set_bio_crypt_ctx_bh(struct bio *bio,
				  const struct buffer_head *first_bh,
				  gfp_t gfp_mask);

bool fscrypt_mergeable_bio(struct bio *bio, const struct inode *inode,
			   u64 next_lblk);

bool fscrypt_mergeable_bio_bh(struct bio *bio,
			      const struct buffer_head *next_bh);

bool fscrypt_dio_supported(struct inode *inode);

u64 fscrypt_limit_io_blocks(const struct inode *inode, u64 lblk, u64 nr_blocks);

#else 

static inline bool __fscrypt_inode_uses_inline_crypto(const struct inode *inode)
{
	return false;
}

static inline void fscrypt_set_bio_crypt_ctx(struct bio *bio,
					     const struct inode *inode,
					     u64 first_lblk, gfp_t gfp_mask) { }

static inline void fscrypt_set_bio_crypt_ctx_bh(
					 struct bio *bio,
					 const struct buffer_head *first_bh,
					 gfp_t gfp_mask) { }

static inline bool fscrypt_mergeable_bio(struct bio *bio,
					 const struct inode *inode,
					 u64 next_lblk)
{
	return true;
}

static inline bool fscrypt_mergeable_bio_bh(struct bio *bio,
					    const struct buffer_head *next_bh)
{
	return true;
}

static inline bool fscrypt_dio_supported(struct inode *inode)
{
	return !fscrypt_needs_contents_encryption(inode);
}

static inline u64 fscrypt_limit_io_blocks(const struct inode *inode, u64 lblk,
					  u64 nr_blocks)
{
	return nr_blocks;
}
#endif 


static inline bool fscrypt_inode_uses_inline_crypto(const struct inode *inode)
{
	return fscrypt_needs_contents_encryption(inode) &&
	       __fscrypt_inode_uses_inline_crypto(inode);
}


static inline bool fscrypt_inode_uses_fs_layer_crypto(const struct inode *inode)
{
	return fscrypt_needs_contents_encryption(inode) &&
	       !__fscrypt_inode_uses_inline_crypto(inode);
}


static inline bool fscrypt_has_encryption_key(const struct inode *inode)
{
	return fscrypt_get_info(inode) != NULL;
}


static inline int fscrypt_prepare_link(struct dentry *old_dentry,
				       struct inode *dir,
				       struct dentry *dentry)
{
	if (IS_ENCRYPTED(dir))
		return __fscrypt_prepare_link(d_inode(old_dentry), dir, dentry);
	return 0;
}


static inline int fscrypt_prepare_rename(struct inode *old_dir,
					 struct dentry *old_dentry,
					 struct inode *new_dir,
					 struct dentry *new_dentry,
					 unsigned int flags)
{
	if (IS_ENCRYPTED(old_dir) || IS_ENCRYPTED(new_dir))
		return __fscrypt_prepare_rename(old_dir, old_dentry,
						new_dir, new_dentry, flags);
	return 0;
}


static inline int fscrypt_prepare_lookup(struct inode *dir,
					 struct dentry *dentry,
					 struct fscrypt_name *fname)
{
	if (IS_ENCRYPTED(dir))
		return __fscrypt_prepare_lookup(dir, dentry, fname);

	memset(fname, 0, sizeof(*fname));
	fname->usr_fname = &dentry->d_name;
	fname->disk_name.name = (unsigned char *)dentry->d_name.name;
	fname->disk_name.len = dentry->d_name.len;
	return 0;
}


static inline int fscrypt_prepare_readdir(struct inode *dir)
{
	if (IS_ENCRYPTED(dir))
		return __fscrypt_prepare_readdir(dir);
	return 0;
}


static inline int fscrypt_prepare_setattr(struct dentry *dentry,
					  struct iattr *attr)
{
	if (IS_ENCRYPTED(d_inode(dentry)))
		return __fscrypt_prepare_setattr(dentry, attr);
	return 0;
}


static inline int fscrypt_encrypt_symlink(struct inode *inode,
					  const char *target,
					  unsigned int len,
					  struct fscrypt_str *disk_link)
{
	if (IS_ENCRYPTED(inode))
		return __fscrypt_encrypt_symlink(inode, target, len, disk_link);
	return 0;
}


static inline void fscrypt_finalize_bounce_page(struct page **pagep)
{
	struct page *page = *pagep;

	if (fscrypt_is_bounce_page(page)) {
		*pagep = fscrypt_pagecache_page(page);
		fscrypt_free_bounce_page(page);
	}
}

#endif	
