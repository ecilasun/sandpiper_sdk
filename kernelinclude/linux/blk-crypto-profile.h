/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_BLK_CRYPTO_PROFILE_H
#define __LINUX_BLK_CRYPTO_PROFILE_H

#include <linux/bio.h>
#include <linux/blk-crypto.h>

struct blk_crypto_profile;


struct blk_crypto_ll_ops {

	
	int (*keyslot_program)(struct blk_crypto_profile *profile,
			       const struct blk_crypto_key *key,
			       unsigned int slot);

	
	int (*keyslot_evict)(struct blk_crypto_profile *profile,
			     const struct blk_crypto_key *key,
			     unsigned int slot);
};


struct blk_crypto_profile {

	

	
	struct blk_crypto_ll_ops ll_ops;

	
	unsigned int max_dun_bytes_supported;

	
	unsigned int modes_supported[BLK_ENCRYPTION_MODE_MAX];

	
	struct device *dev;

	

	
	unsigned int num_slots;

	
	struct rw_semaphore lock;
	struct lock_class_key lockdep_key;

	
	wait_queue_head_t idle_slots_wait_queue;
	struct list_head idle_slots;
	spinlock_t idle_slots_lock;

	
	struct hlist_head *slot_hashtable;
	unsigned int log_slot_ht_size;

	
	struct blk_crypto_keyslot *slots;
};

int blk_crypto_profile_init(struct blk_crypto_profile *profile,
			    unsigned int num_slots);

int devm_blk_crypto_profile_init(struct device *dev,
				 struct blk_crypto_profile *profile,
				 unsigned int num_slots);

unsigned int blk_crypto_keyslot_index(struct blk_crypto_keyslot *slot);

void blk_crypto_reprogram_all_keys(struct blk_crypto_profile *profile);

void blk_crypto_profile_destroy(struct blk_crypto_profile *profile);

void blk_crypto_intersect_capabilities(struct blk_crypto_profile *parent,
				       const struct blk_crypto_profile *child);

bool blk_crypto_has_capabilities(const struct blk_crypto_profile *target,
				 const struct blk_crypto_profile *reference);

void blk_crypto_update_capabilities(struct blk_crypto_profile *dst,
				    const struct blk_crypto_profile *src);

#endif 
