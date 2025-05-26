/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _CRYPTO_SKCIPHER_H
#define _CRYPTO_SKCIPHER_H

#include <linux/atomic.h>
#include <linux/container_of.h>
#include <linux/crypto.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>

struct scatterlist;


struct skcipher_request {
	unsigned int cryptlen;

	u8 *iv;

	struct scatterlist *src;
	struct scatterlist *dst;

	struct crypto_async_request base;

	void *__ctx[] CRYPTO_MINALIGN_ATTR;
};

struct crypto_skcipher {
	unsigned int reqsize;

	struct crypto_tfm base;
};

struct crypto_sync_skcipher {
	struct crypto_skcipher base;
};


struct crypto_istat_cipher {
	atomic64_t encrypt_cnt;
	atomic64_t encrypt_tlen;
	atomic64_t decrypt_cnt;
	atomic64_t decrypt_tlen;
	atomic64_t err_cnt;
};


struct skcipher_alg {
	int (*setkey)(struct crypto_skcipher *tfm, const u8 *key,
	              unsigned int keylen);
	int (*encrypt)(struct skcipher_request *req);
	int (*decrypt)(struct skcipher_request *req);
	int (*init)(struct crypto_skcipher *tfm);
	void (*exit)(struct crypto_skcipher *tfm);

	unsigned int min_keysize;
	unsigned int max_keysize;
	unsigned int ivsize;
	unsigned int chunksize;
	unsigned int walksize;

#ifdef CONFIG_CRYPTO_STATS
	struct crypto_istat_cipher stat;
#endif

	struct crypto_alg base;
};

#define MAX_SYNC_SKCIPHER_REQSIZE      384

#define SYNC_SKCIPHER_REQUEST_ON_STACK(name, tfm) \
	char __##name##_desc[sizeof(struct skcipher_request) + \
			     MAX_SYNC_SKCIPHER_REQSIZE + \
			     (!(sizeof((struct crypto_sync_skcipher *)1 == \
				       (typeof(tfm))1))) \
			    ] CRYPTO_MINALIGN_ATTR; \
	struct skcipher_request *name = (void *)__##name##_desc



static inline struct crypto_skcipher *__crypto_skcipher_cast(
	struct crypto_tfm *tfm)
{
	return container_of(tfm, struct crypto_skcipher, base);
}


struct crypto_skcipher *crypto_alloc_skcipher(const char *alg_name,
					      u32 type, u32 mask);

struct crypto_sync_skcipher *crypto_alloc_sync_skcipher(const char *alg_name,
					      u32 type, u32 mask);

static inline struct crypto_tfm *crypto_skcipher_tfm(
	struct crypto_skcipher *tfm)
{
	return &tfm->base;
}


static inline void crypto_free_skcipher(struct crypto_skcipher *tfm)
{
	crypto_destroy_tfm(tfm, crypto_skcipher_tfm(tfm));
}

static inline void crypto_free_sync_skcipher(struct crypto_sync_skcipher *tfm)
{
	crypto_free_skcipher(&tfm->base);
}


int crypto_has_skcipher(const char *alg_name, u32 type, u32 mask);

static inline const char *crypto_skcipher_driver_name(
	struct crypto_skcipher *tfm)
{
	return crypto_tfm_alg_driver_name(crypto_skcipher_tfm(tfm));
}

static inline struct skcipher_alg *crypto_skcipher_alg(
	struct crypto_skcipher *tfm)
{
	return container_of(crypto_skcipher_tfm(tfm)->__crt_alg,
			    struct skcipher_alg, base);
}

static inline unsigned int crypto_skcipher_alg_ivsize(struct skcipher_alg *alg)
{
	return alg->ivsize;
}


static inline unsigned int crypto_skcipher_ivsize(struct crypto_skcipher *tfm)
{
	return crypto_skcipher_alg(tfm)->ivsize;
}

static inline unsigned int crypto_sync_skcipher_ivsize(
	struct crypto_sync_skcipher *tfm)
{
	return crypto_skcipher_ivsize(&tfm->base);
}


static inline unsigned int crypto_skcipher_blocksize(
	struct crypto_skcipher *tfm)
{
	return crypto_tfm_alg_blocksize(crypto_skcipher_tfm(tfm));
}

static inline unsigned int crypto_skcipher_alg_chunksize(
	struct skcipher_alg *alg)
{
	return alg->chunksize;
}


static inline unsigned int crypto_skcipher_chunksize(
	struct crypto_skcipher *tfm)
{
	return crypto_skcipher_alg_chunksize(crypto_skcipher_alg(tfm));
}

static inline unsigned int crypto_sync_skcipher_blocksize(
	struct crypto_sync_skcipher *tfm)
{
	return crypto_skcipher_blocksize(&tfm->base);
}

static inline unsigned int crypto_skcipher_alignmask(
	struct crypto_skcipher *tfm)
{
	return crypto_tfm_alg_alignmask(crypto_skcipher_tfm(tfm));
}

static inline u32 crypto_skcipher_get_flags(struct crypto_skcipher *tfm)
{
	return crypto_tfm_get_flags(crypto_skcipher_tfm(tfm));
}

static inline void crypto_skcipher_set_flags(struct crypto_skcipher *tfm,
					       u32 flags)
{
	crypto_tfm_set_flags(crypto_skcipher_tfm(tfm), flags);
}

static inline void crypto_skcipher_clear_flags(struct crypto_skcipher *tfm,
						 u32 flags)
{
	crypto_tfm_clear_flags(crypto_skcipher_tfm(tfm), flags);
}

static inline u32 crypto_sync_skcipher_get_flags(
	struct crypto_sync_skcipher *tfm)
{
	return crypto_skcipher_get_flags(&tfm->base);
}

static inline void crypto_sync_skcipher_set_flags(
	struct crypto_sync_skcipher *tfm, u32 flags)
{
	crypto_skcipher_set_flags(&tfm->base, flags);
}

static inline void crypto_sync_skcipher_clear_flags(
	struct crypto_sync_skcipher *tfm, u32 flags)
{
	crypto_skcipher_clear_flags(&tfm->base, flags);
}


int crypto_skcipher_setkey(struct crypto_skcipher *tfm,
			   const u8 *key, unsigned int keylen);

static inline int crypto_sync_skcipher_setkey(struct crypto_sync_skcipher *tfm,
					 const u8 *key, unsigned int keylen)
{
	return crypto_skcipher_setkey(&tfm->base, key, keylen);
}

static inline unsigned int crypto_skcipher_min_keysize(
	struct crypto_skcipher *tfm)
{
	return crypto_skcipher_alg(tfm)->min_keysize;
}

static inline unsigned int crypto_skcipher_max_keysize(
	struct crypto_skcipher *tfm)
{
	return crypto_skcipher_alg(tfm)->max_keysize;
}


static inline struct crypto_skcipher *crypto_skcipher_reqtfm(
	struct skcipher_request *req)
{
	return __crypto_skcipher_cast(req->base.tfm);
}

static inline struct crypto_sync_skcipher *crypto_sync_skcipher_reqtfm(
	struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);

	return container_of(tfm, struct crypto_sync_skcipher, base);
}


int crypto_skcipher_encrypt(struct skcipher_request *req);


int crypto_skcipher_decrypt(struct skcipher_request *req);




static inline unsigned int crypto_skcipher_reqsize(struct crypto_skcipher *tfm)
{
	return tfm->reqsize;
}


static inline void skcipher_request_set_tfm(struct skcipher_request *req,
					    struct crypto_skcipher *tfm)
{
	req->base.tfm = crypto_skcipher_tfm(tfm);
}

static inline void skcipher_request_set_sync_tfm(struct skcipher_request *req,
					    struct crypto_sync_skcipher *tfm)
{
	skcipher_request_set_tfm(req, &tfm->base);
}

static inline struct skcipher_request *skcipher_request_cast(
	struct crypto_async_request *req)
{
	return container_of(req, struct skcipher_request, base);
}


static inline struct skcipher_request *skcipher_request_alloc(
	struct crypto_skcipher *tfm, gfp_t gfp)
{
	struct skcipher_request *req;

	req = kmalloc(sizeof(struct skcipher_request) +
		      crypto_skcipher_reqsize(tfm), gfp);

	if (likely(req))
		skcipher_request_set_tfm(req, tfm);

	return req;
}


static inline void skcipher_request_free(struct skcipher_request *req)
{
	kfree_sensitive(req);
}

static inline void skcipher_request_zero(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);

	memzero_explicit(req, sizeof(*req) + crypto_skcipher_reqsize(tfm));
}


static inline void skcipher_request_set_callback(struct skcipher_request *req,
						 u32 flags,
						 crypto_completion_t compl,
						 void *data)
{
	req->base.complete = compl;
	req->base.data = data;
	req->base.flags = flags;
}


static inline void skcipher_request_set_crypt(
	struct skcipher_request *req,
	struct scatterlist *src, struct scatterlist *dst,
	unsigned int cryptlen, void *iv)
{
	req->src = src;
	req->dst = dst;
	req->cryptlen = cryptlen;
	req->iv = iv;
}

#endif	

