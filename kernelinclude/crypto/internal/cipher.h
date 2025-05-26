/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _CRYPTO_INTERNAL_CIPHER_H
#define _CRYPTO_INTERNAL_CIPHER_H

#include <crypto/algapi.h>

struct crypto_cipher {
	struct crypto_tfm base;
};



static inline struct crypto_cipher *__crypto_cipher_cast(struct crypto_tfm *tfm)
{
	return (struct crypto_cipher *)tfm;
}


static inline struct crypto_cipher *crypto_alloc_cipher(const char *alg_name,
							u32 type, u32 mask)
{
	type &= ~CRYPTO_ALG_TYPE_MASK;
	type |= CRYPTO_ALG_TYPE_CIPHER;
	mask |= CRYPTO_ALG_TYPE_MASK;

	return __crypto_cipher_cast(crypto_alloc_base(alg_name, type, mask));
}

static inline struct crypto_tfm *crypto_cipher_tfm(struct crypto_cipher *tfm)
{
	return &tfm->base;
}


static inline void crypto_free_cipher(struct crypto_cipher *tfm)
{
	crypto_free_tfm(crypto_cipher_tfm(tfm));
}


static inline int crypto_has_cipher(const char *alg_name, u32 type, u32 mask)
{
	type &= ~CRYPTO_ALG_TYPE_MASK;
	type |= CRYPTO_ALG_TYPE_CIPHER;
	mask |= CRYPTO_ALG_TYPE_MASK;

	return crypto_has_alg(alg_name, type, mask);
}


static inline unsigned int crypto_cipher_blocksize(struct crypto_cipher *tfm)
{
	return crypto_tfm_alg_blocksize(crypto_cipher_tfm(tfm));
}

static inline unsigned int crypto_cipher_alignmask(struct crypto_cipher *tfm)
{
	return crypto_tfm_alg_alignmask(crypto_cipher_tfm(tfm));
}

static inline u32 crypto_cipher_get_flags(struct crypto_cipher *tfm)
{
	return crypto_tfm_get_flags(crypto_cipher_tfm(tfm));
}

static inline void crypto_cipher_set_flags(struct crypto_cipher *tfm,
					   u32 flags)
{
	crypto_tfm_set_flags(crypto_cipher_tfm(tfm), flags);
}

static inline void crypto_cipher_clear_flags(struct crypto_cipher *tfm,
					     u32 flags)
{
	crypto_tfm_clear_flags(crypto_cipher_tfm(tfm), flags);
}


int crypto_cipher_setkey(struct crypto_cipher *tfm,
			 const u8 *key, unsigned int keylen);


void crypto_cipher_encrypt_one(struct crypto_cipher *tfm,
			       u8 *dst, const u8 *src);


void crypto_cipher_decrypt_one(struct crypto_cipher *tfm,
			       u8 *dst, const u8 *src);

struct crypto_cipher *crypto_clone_cipher(struct crypto_cipher *cipher);

struct crypto_cipher_spawn {
	struct crypto_spawn base;
};

static inline int crypto_grab_cipher(struct crypto_cipher_spawn *spawn,
				     struct crypto_instance *inst,
				     const char *name, u32 type, u32 mask)
{
	type &= ~CRYPTO_ALG_TYPE_MASK;
	type |= CRYPTO_ALG_TYPE_CIPHER;
	mask |= CRYPTO_ALG_TYPE_MASK;
	return crypto_grab_spawn(&spawn->base, inst, name, type, mask);
}

static inline void crypto_drop_cipher(struct crypto_cipher_spawn *spawn)
{
	crypto_drop_spawn(&spawn->base);
}

static inline struct crypto_alg *crypto_spawn_cipher_alg(
       struct crypto_cipher_spawn *spawn)
{
	return spawn->base.alg;
}

static inline struct crypto_cipher *crypto_spawn_cipher(
	struct crypto_cipher_spawn *spawn)
{
	u32 type = CRYPTO_ALG_TYPE_CIPHER;
	u32 mask = CRYPTO_ALG_TYPE_MASK;

	return __crypto_cipher_cast(crypto_spawn_tfm(&spawn->base, type, mask));
}

static inline struct cipher_alg *crypto_cipher_alg(struct crypto_cipher *tfm)
{
	return &crypto_cipher_tfm(tfm)->__crt_alg->cra_cipher;
}

#endif
