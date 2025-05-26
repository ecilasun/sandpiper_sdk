/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _CRYPTO_SM2_H
#define _CRYPTO_SM2_H

struct shash_desc;

#if IS_REACHABLE(CONFIG_CRYPTO_SM2)
int sm2_compute_z_digest(struct shash_desc *desc,
			 const void *key, unsigned int keylen, void *dgst);
#else
static inline int sm2_compute_z_digest(struct shash_desc *desc,
				       const void *key, unsigned int keylen,
				       void *dgst)
{
	return -ENOTSUPP;
}
#endif

#endif 
