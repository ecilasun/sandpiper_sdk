/* SPDX-License-Identifier: GPL-2.0 */



#ifndef _CRYPTO_SM4_H
#define _CRYPTO_SM4_H

#include <linux/types.h>
#include <linux/crypto.h>

#define SM4_KEY_SIZE	16
#define SM4_BLOCK_SIZE	16
#define SM4_RKEY_WORDS	32

struct sm4_ctx {
	u32 rkey_enc[SM4_RKEY_WORDS];
	u32 rkey_dec[SM4_RKEY_WORDS];
};

extern const u32 crypto_sm4_fk[];
extern const u32 crypto_sm4_ck[];
extern const u8 crypto_sm4_sbox[];


int sm4_expandkey(struct sm4_ctx *ctx, const u8 *in_key,
			  unsigned int key_len);


void sm4_crypt_block(const u32 *rk, u8 *out, const u8 *in);

#endif
