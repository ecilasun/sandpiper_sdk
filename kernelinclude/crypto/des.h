/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __CRYPTO_DES_H
#define __CRYPTO_DES_H

#include <linux/types.h>

#define DES_KEY_SIZE		8
#define DES_EXPKEY_WORDS	32
#define DES_BLOCK_SIZE		8

#define DES3_EDE_KEY_SIZE	(3 * DES_KEY_SIZE)
#define DES3_EDE_EXPKEY_WORDS	(3 * DES_EXPKEY_WORDS)
#define DES3_EDE_BLOCK_SIZE	DES_BLOCK_SIZE

struct des_ctx {
	u32 expkey[DES_EXPKEY_WORDS];
};

struct des3_ede_ctx {
	u32 expkey[DES3_EDE_EXPKEY_WORDS];
};

void des_encrypt(const struct des_ctx *ctx, u8 *dst, const u8 *src);
void des_decrypt(const struct des_ctx *ctx, u8 *dst, const u8 *src);

void des3_ede_encrypt(const struct des3_ede_ctx *dctx, u8 *dst, const u8 *src);
void des3_ede_decrypt(const struct des3_ede_ctx *dctx, u8 *dst, const u8 *src);


int des_expand_key(struct des_ctx *ctx, const u8 *key, unsigned int keylen);


int des3_ede_expand_key(struct des3_ede_ctx *ctx, const u8 *key,
			unsigned int keylen);

#endif 
