/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CRYPTO_SIG_H
#define _CRYPTO_SIG_H

#include <linux/crypto.h>


struct crypto_sig {
	struct crypto_tfm base;
};




struct crypto_sig *crypto_alloc_sig(const char *alg_name, u32 type, u32 mask);

static inline struct crypto_tfm *crypto_sig_tfm(struct crypto_sig *tfm)
{
	return &tfm->base;
}


static inline void crypto_free_sig(struct crypto_sig *tfm)
{
	crypto_destroy_tfm(tfm, crypto_sig_tfm(tfm));
}


int crypto_sig_maxsize(struct crypto_sig *tfm);


int crypto_sig_sign(struct crypto_sig *tfm,
		    const void *src, unsigned int slen,
		    void *dst, unsigned int dlen);


int crypto_sig_verify(struct crypto_sig *tfm,
		      const void *src, unsigned int slen,
		      const void *digest, unsigned int dlen);


int crypto_sig_set_pubkey(struct crypto_sig *tfm,
			  const void *key, unsigned int keylen);


int crypto_sig_set_privkey(struct crypto_sig *tfm,
			   const void *key, unsigned int keylen);
#endif
