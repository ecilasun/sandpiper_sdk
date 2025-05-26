/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_PR_H
#define LINUX_PR_H

#include <uapi/linux/pr.h>

struct pr_keys {
	u32	generation;
	u32	num_keys;
	u64	keys[];
};

struct pr_held_reservation {
	u64		key;
	u32		generation;
	enum pr_type	type;
};

struct pr_ops {
	int (*pr_register)(struct block_device *bdev, u64 old_key, u64 new_key,
			u32 flags);
	int (*pr_reserve)(struct block_device *bdev, u64 key,
			enum pr_type type, u32 flags);
	int (*pr_release)(struct block_device *bdev, u64 key,
			enum pr_type type);
	int (*pr_preempt)(struct block_device *bdev, u64 old_key, u64 new_key,
			enum pr_type type, bool abort);
	int (*pr_clear)(struct block_device *bdev, u64 key);
	
	int (*pr_read_keys)(struct block_device *bdev,
			struct pr_keys *keys_info);
	int (*pr_read_reservation)(struct block_device *bdev,
			struct pr_held_reservation *rsv);
};

#endif 
