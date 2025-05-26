/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_BLK_CRYPTO_H
#define __LINUX_BLK_CRYPTO_H

#include <linux/types.h>

enum blk_crypto_mode_num {
	BLK_ENCRYPTION_MODE_INVALID,
	BLK_ENCRYPTION_MODE_AES_256_XTS,
	BLK_ENCRYPTION_MODE_AES_128_CBC_ESSIV,
	BLK_ENCRYPTION_MODE_ADIANTUM,
	BLK_ENCRYPTION_MODE_SM4_XTS,
	BLK_ENCRYPTION_MODE_MAX,
};

#define BLK_CRYPTO_MAX_KEY_SIZE		64

struct blk_crypto_config {
	enum blk_crypto_mode_num crypto_mode;
	unsigned int data_unit_size;
	unsigned int dun_bytes;
};


struct blk_crypto_key {
	struct blk_crypto_config crypto_cfg;
	unsigned int data_unit_size_bits;
	unsigned int size;
	u8 raw[BLK_CRYPTO_MAX_KEY_SIZE];
};

#define BLK_CRYPTO_MAX_IV_SIZE		32
#define BLK_CRYPTO_DUN_ARRAY_SIZE	(BLK_CRYPTO_MAX_IV_SIZE / sizeof(u64))


struct bio_crypt_ctx {
	const struct blk_crypto_key	*bc_key;
	u64				bc_dun[BLK_CRYPTO_DUN_ARRAY_SIZE];
};

#include <linux/blk_types.h>
#include <linux/blkdev.h>

#ifdef CONFIG_BLK_INLINE_ENCRYPTION

static inline bool bio_has_crypt_ctx(struct bio *bio)
{
	return bio->bi_crypt_context;
}

void bio_crypt_set_ctx(struct bio *bio, const struct blk_crypto_key *key,
		       const u64 dun[BLK_CRYPTO_DUN_ARRAY_SIZE],
		       gfp_t gfp_mask);

bool bio_crypt_dun_is_contiguous(const struct bio_crypt_ctx *bc,
				 unsigned int bytes,
				 const u64 next_dun[BLK_CRYPTO_DUN_ARRAY_SIZE]);

int blk_crypto_init_key(struct blk_crypto_key *blk_key, const u8 *raw_key,
			enum blk_crypto_mode_num crypto_mode,
			unsigned int dun_bytes,
			unsigned int data_unit_size);

int blk_crypto_start_using_key(struct block_device *bdev,
			       const struct blk_crypto_key *key);

void blk_crypto_evict_key(struct block_device *bdev,
			  const struct blk_crypto_key *key);

bool blk_crypto_config_supported_natively(struct block_device *bdev,
					  const struct blk_crypto_config *cfg);
bool blk_crypto_config_supported(struct block_device *bdev,
				 const struct blk_crypto_config *cfg);

#else 

static inline bool bio_has_crypt_ctx(struct bio *bio)
{
	return false;
}

#endif 

int __bio_crypt_clone(struct bio *dst, struct bio *src, gfp_t gfp_mask);

static inline int bio_crypt_clone(struct bio *dst, struct bio *src,
				  gfp_t gfp_mask)
{
	if (bio_has_crypt_ctx(src))
		return __bio_crypt_clone(dst, src, gfp_mask);
	return 0;
}

#endif 
