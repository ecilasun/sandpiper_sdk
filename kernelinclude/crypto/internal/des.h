/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __CRYPTO_INTERNAL_DES_H
#define __CRYPTO_INTERNAL_DES_H

#include <linux/crypto.h>
#include <linux/fips.h>
#include <crypto/des.h>
#include <crypto/aead.h>
#include <crypto/skcipher.h>


static inline int crypto_des_verify_key(struct crypto_tfm *tfm, const u8 *key)
{
	struct des_ctx tmp;
	int err;

	err = des_expand_key(&tmp, key, DES_KEY_SIZE);
	if (err == -ENOKEY) {
		if (crypto_tfm_get_flags(tfm) & CRYPTO_TFM_REQ_FORBID_WEAK_KEYS)
			err = -EINVAL;
		else
			err = 0;
	}
	memzero_explicit(&tmp, sizeof(tmp));
	return err;
}


static inline int des3_ede_verify_key(const u8 *key, unsigned int key_len,
				      bool check_weak)
{
	int ret = fips_enabled ? -EINVAL : -ENOKEY;
	u32 K[6];

	memcpy(K, key, DES3_EDE_KEY_SIZE);

	if ((!((K[0] ^ K[2]) | (K[1] ^ K[3])) ||
	     !((K[2] ^ K[4]) | (K[3] ^ K[5]))) &&
	    (fips_enabled || check_weak))
		goto bad;

	if ((!((K[0] ^ K[4]) | (K[1] ^ K[5]))) && fips_enabled)
		goto bad;

	ret = 0;
bad:
	memzero_explicit(K, DES3_EDE_KEY_SIZE);

	return ret;
}


static inline int crypto_des3_ede_verify_key(struct crypto_tfm *tfm,
					     const u8 *key)
{
	return des3_ede_verify_key(key, DES3_EDE_KEY_SIZE,
				   crypto_tfm_get_flags(tfm) &
				   CRYPTO_TFM_REQ_FORBID_WEAK_KEYS);
}

static inline int verify_skcipher_des_key(struct crypto_skcipher *tfm,
					  const u8 *key)
{
	return crypto_des_verify_key(crypto_skcipher_tfm(tfm), key);
}

static inline int verify_skcipher_des3_key(struct crypto_skcipher *tfm,
					   const u8 *key)
{
	return crypto_des3_ede_verify_key(crypto_skcipher_tfm(tfm), key);
}

static inline int verify_aead_des_key(struct crypto_aead *tfm, const u8 *key,
				      int keylen)
{
	if (keylen != DES_KEY_SIZE)
		return -EINVAL;
	return crypto_des_verify_key(crypto_aead_tfm(tfm), key);
}

static inline int verify_aead_des3_key(struct crypto_aead *tfm, const u8 *key,
				       int keylen)
{
	if (keylen != DES3_EDE_KEY_SIZE)
		return -EINVAL;
	return crypto_des3_ede_verify_key(crypto_aead_tfm(tfm), key);
}

#endif 
