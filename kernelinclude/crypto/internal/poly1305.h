/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _CRYPTO_INTERNAL_POLY1305_H
#define _CRYPTO_INTERNAL_POLY1305_H

#include <asm/unaligned.h>
#include <linux/types.h>
#include <crypto/poly1305.h>



void poly1305_core_setkey(struct poly1305_core_key *key,
			  const u8 raw_key[POLY1305_BLOCK_SIZE]);
static inline void poly1305_core_init(struct poly1305_state *state)
{
	*state = (struct poly1305_state){};
}

void poly1305_core_blocks(struct poly1305_state *state,
			  const struct poly1305_core_key *key, const void *src,
			  unsigned int nblocks, u32 hibit);
void poly1305_core_emit(const struct poly1305_state *state, const u32 nonce[4],
			void *dst);

#endif
