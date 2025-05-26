/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_IVERSION_H
#define _LINUX_IVERSION_H

#include <linux/fs.h>




#define I_VERSION_QUERIED_SHIFT	(1)
#define I_VERSION_QUERIED	(1ULL << (I_VERSION_QUERIED_SHIFT - 1))
#define I_VERSION_INCREMENT	(1ULL << I_VERSION_QUERIED_SHIFT)


static inline void
inode_set_iversion_raw(struct inode *inode, u64 val)
{
	atomic64_set(&inode->i_version, val);
}


static inline u64
inode_peek_iversion_raw(const struct inode *inode)
{
	return atomic64_read(&inode->i_version);
}


static inline void
inode_set_max_iversion_raw(struct inode *inode, u64 val)
{
	u64 cur = inode_peek_iversion_raw(inode);

	do {
		if (cur > val)
			break;
	} while (!atomic64_try_cmpxchg(&inode->i_version, &cur, val));
}


static inline void
inode_set_iversion(struct inode *inode, u64 val)
{
	inode_set_iversion_raw(inode, val << I_VERSION_QUERIED_SHIFT);
}


static inline void
inode_set_iversion_queried(struct inode *inode, u64 val)
{
	inode_set_iversion_raw(inode, (val << I_VERSION_QUERIED_SHIFT) |
				I_VERSION_QUERIED);
}

bool inode_maybe_inc_iversion(struct inode *inode, bool force);


static inline void
inode_inc_iversion(struct inode *inode)
{
	inode_maybe_inc_iversion(inode, true);
}


static inline bool
inode_iversion_need_inc(struct inode *inode)
{
	return inode_peek_iversion_raw(inode) & I_VERSION_QUERIED;
}


static inline void
inode_inc_iversion_raw(struct inode *inode)
{
	atomic64_inc(&inode->i_version);
}


static inline u64
inode_peek_iversion(const struct inode *inode)
{
	return inode_peek_iversion_raw(inode) >> I_VERSION_QUERIED_SHIFT;
}


static inline u64 time_to_chattr(struct timespec64 *t)
{
	u64 chattr = t->tv_sec;

	chattr <<= 32;
	chattr += t->tv_nsec;
	return chattr;
}

u64 inode_query_iversion(struct inode *inode);


static inline bool
inode_eq_iversion_raw(const struct inode *inode, u64 old)
{
	return inode_peek_iversion_raw(inode) == old;
}


static inline bool
inode_eq_iversion(const struct inode *inode, u64 old)
{
	return inode_peek_iversion(inode) == old;
}
#endif
