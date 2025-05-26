/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _CRYPTO_HASH_H
#define _CRYPTO_HASH_H

#include <linux/atomic.h>
#include <linux/crypto.h>
#include <linux/string.h>

struct crypto_ahash;




struct crypto_istat_hash {
	atomic64_t hash_cnt;
	atomic64_t hash_tlen;
	atomic64_t err_cnt;
};

#ifdef CONFIG_CRYPTO_STATS
#define HASH_ALG_COMMON_STAT struct crypto_istat_hash stat;
#else
#define HASH_ALG_COMMON_STAT
#endif


#define HASH_ALG_COMMON {		\
	HASH_ALG_COMMON_STAT		\
					\
	unsigned int digestsize;	\
	unsigned int statesize;		\
					\
	struct crypto_alg base;		\
}
struct hash_alg_common HASH_ALG_COMMON;

struct ahash_request {
	struct crypto_async_request base;

	unsigned int nbytes;
	struct scatterlist *src;
	u8 *result;

	
	void *priv;

	void *__ctx[] CRYPTO_MINALIGN_ATTR;
};


struct ahash_alg {
	int (*init)(struct ahash_request *req);
	int (*update)(struct ahash_request *req);
	int (*final)(struct ahash_request *req);
	int (*finup)(struct ahash_request *req);
	int (*digest)(struct ahash_request *req);
	int (*export)(struct ahash_request *req, void *out);
	int (*import)(struct ahash_request *req, const void *in);
	int (*setkey)(struct crypto_ahash *tfm, const u8 *key,
		      unsigned int keylen);
	int (*init_tfm)(struct crypto_ahash *tfm);
	void (*exit_tfm)(struct crypto_ahash *tfm);
	int (*clone_tfm)(struct crypto_ahash *dst, struct crypto_ahash *src);

	struct hash_alg_common halg;
};

struct shash_desc {
	struct crypto_shash *tfm;
	void *__ctx[] __aligned(ARCH_SLAB_MINALIGN);
};

#define HASH_MAX_DIGESTSIZE	 64


#define HASH_MAX_DESCSIZE	((sizeof(struct shash_desc) * 2) + 360)

#define SHASH_DESC_ON_STACK(shash, ctx)					     \
	char __##shash##_desc[sizeof(struct shash_desc) + HASH_MAX_DESCSIZE] \
		__aligned(__alignof__(struct shash_desc));		     \
	struct shash_desc *shash = (struct shash_desc *)__##shash##_desc


struct shash_alg {
	int (*init)(struct shash_desc *desc);
	int (*update)(struct shash_desc *desc, const u8 *data,
		      unsigned int len);
	int (*final)(struct shash_desc *desc, u8 *out);
	int (*finup)(struct shash_desc *desc, const u8 *data,
		     unsigned int len, u8 *out);
	int (*digest)(struct shash_desc *desc, const u8 *data,
		      unsigned int len, u8 *out);
	int (*export)(struct shash_desc *desc, void *out);
	int (*import)(struct shash_desc *desc, const void *in);
	int (*setkey)(struct crypto_shash *tfm, const u8 *key,
		      unsigned int keylen);
	int (*init_tfm)(struct crypto_shash *tfm);
	void (*exit_tfm)(struct crypto_shash *tfm);
	int (*clone_tfm)(struct crypto_shash *dst, struct crypto_shash *src);

	unsigned int descsize;

	union {
		struct HASH_ALG_COMMON;
		struct hash_alg_common halg;
	};
};
#undef HASH_ALG_COMMON
#undef HASH_ALG_COMMON_STAT

struct crypto_ahash {
	int (*init)(struct ahash_request *req);
	int (*update)(struct ahash_request *req);
	int (*final)(struct ahash_request *req);
	int (*finup)(struct ahash_request *req);
	int (*digest)(struct ahash_request *req);
	int (*export)(struct ahash_request *req, void *out);
	int (*import)(struct ahash_request *req, const void *in);
	int (*setkey)(struct crypto_ahash *tfm, const u8 *key,
		      unsigned int keylen);

	unsigned int statesize;
	unsigned int reqsize;
	struct crypto_tfm base;
};

struct crypto_shash {
	unsigned int descsize;
	struct crypto_tfm base;
};



static inline struct crypto_ahash *__crypto_ahash_cast(struct crypto_tfm *tfm)
{
	return container_of(tfm, struct crypto_ahash, base);
}


struct crypto_ahash *crypto_alloc_ahash(const char *alg_name, u32 type,
					u32 mask);

struct crypto_ahash *crypto_clone_ahash(struct crypto_ahash *tfm);

static inline struct crypto_tfm *crypto_ahash_tfm(struct crypto_ahash *tfm)
{
	return &tfm->base;
}


static inline void crypto_free_ahash(struct crypto_ahash *tfm)
{
	crypto_destroy_tfm(tfm, crypto_ahash_tfm(tfm));
}


int crypto_has_ahash(const char *alg_name, u32 type, u32 mask);

static inline const char *crypto_ahash_alg_name(struct crypto_ahash *tfm)
{
	return crypto_tfm_alg_name(crypto_ahash_tfm(tfm));
}

static inline const char *crypto_ahash_driver_name(struct crypto_ahash *tfm)
{
	return crypto_tfm_alg_driver_name(crypto_ahash_tfm(tfm));
}

static inline unsigned int crypto_ahash_alignmask(
	struct crypto_ahash *tfm)
{
	return crypto_tfm_alg_alignmask(crypto_ahash_tfm(tfm));
}


static inline unsigned int crypto_ahash_blocksize(struct crypto_ahash *tfm)
{
	return crypto_tfm_alg_blocksize(crypto_ahash_tfm(tfm));
}

static inline struct hash_alg_common *__crypto_hash_alg_common(
	struct crypto_alg *alg)
{
	return container_of(alg, struct hash_alg_common, base);
}

static inline struct hash_alg_common *crypto_hash_alg_common(
	struct crypto_ahash *tfm)
{
	return __crypto_hash_alg_common(crypto_ahash_tfm(tfm)->__crt_alg);
}


static inline unsigned int crypto_ahash_digestsize(struct crypto_ahash *tfm)
{
	return crypto_hash_alg_common(tfm)->digestsize;
}


static inline unsigned int crypto_ahash_statesize(struct crypto_ahash *tfm)
{
	return tfm->statesize;
}

static inline u32 crypto_ahash_get_flags(struct crypto_ahash *tfm)
{
	return crypto_tfm_get_flags(crypto_ahash_tfm(tfm));
}

static inline void crypto_ahash_set_flags(struct crypto_ahash *tfm, u32 flags)
{
	crypto_tfm_set_flags(crypto_ahash_tfm(tfm), flags);
}

static inline void crypto_ahash_clear_flags(struct crypto_ahash *tfm, u32 flags)
{
	crypto_tfm_clear_flags(crypto_ahash_tfm(tfm), flags);
}


static inline struct crypto_ahash *crypto_ahash_reqtfm(
	struct ahash_request *req)
{
	return __crypto_ahash_cast(req->base.tfm);
}


static inline unsigned int crypto_ahash_reqsize(struct crypto_ahash *tfm)
{
	return tfm->reqsize;
}

static inline void *ahash_request_ctx(struct ahash_request *req)
{
	return req->__ctx;
}


int crypto_ahash_setkey(struct crypto_ahash *tfm, const u8 *key,
			unsigned int keylen);


int crypto_ahash_finup(struct ahash_request *req);


int crypto_ahash_final(struct ahash_request *req);


int crypto_ahash_digest(struct ahash_request *req);


static inline int crypto_ahash_export(struct ahash_request *req, void *out)
{
	return crypto_ahash_reqtfm(req)->export(req, out);
}


static inline int crypto_ahash_import(struct ahash_request *req, const void *in)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);

	if (crypto_ahash_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		return -ENOKEY;

	return tfm->import(req, in);
}


static inline int crypto_ahash_init(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);

	if (crypto_ahash_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		return -ENOKEY;

	return tfm->init(req);
}

static inline struct crypto_istat_hash *hash_get_stat(
	struct hash_alg_common *alg)
{
#ifdef CONFIG_CRYPTO_STATS
	return &alg->stat;
#else
	return NULL;
#endif
}

static inline int crypto_hash_errstat(struct hash_alg_common *alg, int err)
{
	if (!IS_ENABLED(CONFIG_CRYPTO_STATS))
		return err;

	if (err && err != -EINPROGRESS && err != -EBUSY)
		atomic64_inc(&hash_get_stat(alg)->err_cnt);

	return err;
}


static inline int crypto_ahash_update(struct ahash_request *req)
{
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct hash_alg_common *alg = crypto_hash_alg_common(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS))
		atomic64_add(req->nbytes, &hash_get_stat(alg)->hash_tlen);

	return crypto_hash_errstat(alg, tfm->update(req));
}




static inline void ahash_request_set_tfm(struct ahash_request *req,
					 struct crypto_ahash *tfm)
{
	req->base.tfm = crypto_ahash_tfm(tfm);
}


static inline struct ahash_request *ahash_request_alloc(
	struct crypto_ahash *tfm, gfp_t gfp)
{
	struct ahash_request *req;

	req = kmalloc(sizeof(struct ahash_request) +
		      crypto_ahash_reqsize(tfm), gfp);

	if (likely(req))
		ahash_request_set_tfm(req, tfm);

	return req;
}


static inline void ahash_request_free(struct ahash_request *req)
{
	kfree_sensitive(req);
}

static inline void ahash_request_zero(struct ahash_request *req)
{
	memzero_explicit(req, sizeof(*req) +
			      crypto_ahash_reqsize(crypto_ahash_reqtfm(req)));
}

static inline struct ahash_request *ahash_request_cast(
	struct crypto_async_request *req)
{
	return container_of(req, struct ahash_request, base);
}


static inline void ahash_request_set_callback(struct ahash_request *req,
					      u32 flags,
					      crypto_completion_t compl,
					      void *data)
{
	req->base.complete = compl;
	req->base.data = data;
	req->base.flags = flags;
}


static inline void ahash_request_set_crypt(struct ahash_request *req,
					   struct scatterlist *src, u8 *result,
					   unsigned int nbytes)
{
	req->src = src;
	req->nbytes = nbytes;
	req->result = result;
}




struct crypto_shash *crypto_alloc_shash(const char *alg_name, u32 type,
					u32 mask);

struct crypto_shash *crypto_clone_shash(struct crypto_shash *tfm);

int crypto_has_shash(const char *alg_name, u32 type, u32 mask);

static inline struct crypto_tfm *crypto_shash_tfm(struct crypto_shash *tfm)
{
	return &tfm->base;
}


static inline void crypto_free_shash(struct crypto_shash *tfm)
{
	crypto_destroy_tfm(tfm, crypto_shash_tfm(tfm));
}

static inline const char *crypto_shash_alg_name(struct crypto_shash *tfm)
{
	return crypto_tfm_alg_name(crypto_shash_tfm(tfm));
}

static inline const char *crypto_shash_driver_name(struct crypto_shash *tfm)
{
	return crypto_tfm_alg_driver_name(crypto_shash_tfm(tfm));
}

static inline unsigned int crypto_shash_alignmask(
	struct crypto_shash *tfm)
{
	return crypto_tfm_alg_alignmask(crypto_shash_tfm(tfm));
}


static inline unsigned int crypto_shash_blocksize(struct crypto_shash *tfm)
{
	return crypto_tfm_alg_blocksize(crypto_shash_tfm(tfm));
}

static inline struct shash_alg *__crypto_shash_alg(struct crypto_alg *alg)
{
	return container_of(alg, struct shash_alg, base);
}

static inline struct shash_alg *crypto_shash_alg(struct crypto_shash *tfm)
{
	return __crypto_shash_alg(crypto_shash_tfm(tfm)->__crt_alg);
}


static inline unsigned int crypto_shash_digestsize(struct crypto_shash *tfm)
{
	return crypto_shash_alg(tfm)->digestsize;
}

static inline unsigned int crypto_shash_statesize(struct crypto_shash *tfm)
{
	return crypto_shash_alg(tfm)->statesize;
}

static inline u32 crypto_shash_get_flags(struct crypto_shash *tfm)
{
	return crypto_tfm_get_flags(crypto_shash_tfm(tfm));
}

static inline void crypto_shash_set_flags(struct crypto_shash *tfm, u32 flags)
{
	crypto_tfm_set_flags(crypto_shash_tfm(tfm), flags);
}

static inline void crypto_shash_clear_flags(struct crypto_shash *tfm, u32 flags)
{
	crypto_tfm_clear_flags(crypto_shash_tfm(tfm), flags);
}


static inline unsigned int crypto_shash_descsize(struct crypto_shash *tfm)
{
	return tfm->descsize;
}

static inline void *shash_desc_ctx(struct shash_desc *desc)
{
	return desc->__ctx;
}


int crypto_shash_setkey(struct crypto_shash *tfm, const u8 *key,
			unsigned int keylen);


int crypto_shash_digest(struct shash_desc *desc, const u8 *data,
			unsigned int len, u8 *out);


int crypto_shash_tfm_digest(struct crypto_shash *tfm, const u8 *data,
			    unsigned int len, u8 *out);


static inline int crypto_shash_export(struct shash_desc *desc, void *out)
{
	return crypto_shash_alg(desc->tfm)->export(desc, out);
}


static inline int crypto_shash_import(struct shash_desc *desc, const void *in)
{
	struct crypto_shash *tfm = desc->tfm;

	if (crypto_shash_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		return -ENOKEY;

	return crypto_shash_alg(tfm)->import(desc, in);
}


static inline int crypto_shash_init(struct shash_desc *desc)
{
	struct crypto_shash *tfm = desc->tfm;

	if (crypto_shash_get_flags(tfm) & CRYPTO_TFM_NEED_KEY)
		return -ENOKEY;

	return crypto_shash_alg(tfm)->init(desc);
}


int crypto_shash_update(struct shash_desc *desc, const u8 *data,
			unsigned int len);


int crypto_shash_final(struct shash_desc *desc, u8 *out);


int crypto_shash_finup(struct shash_desc *desc, const u8 *data,
		       unsigned int len, u8 *out);

static inline void shash_desc_zero(struct shash_desc *desc)
{
	memzero_explicit(desc,
			 sizeof(*desc) + crypto_shash_descsize(desc->tfm));
}

#endif	
