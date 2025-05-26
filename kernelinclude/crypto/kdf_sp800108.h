/* SPDX-License-Identifier: GPL-2.0 */



#ifndef _CRYPTO_KDF108_H
#define _CRYPTO_KDF108_H

#include <crypto/hash.h>
#include <linux/uio.h>


int crypto_kdf108_ctr_generate(struct crypto_shash *kmd,
			       const struct kvec *info, unsigned int info_nvec,
			       u8 *dst, unsigned int dlen);


int crypto_kdf108_setkey(struct crypto_shash *kmd,
			 const u8 *key, size_t keylen,
			 const u8 *ikm, size_t ikmlen);

#endif 
