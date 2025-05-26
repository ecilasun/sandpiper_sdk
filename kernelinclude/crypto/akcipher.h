/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CRYPTO_AKCIPHER_H
#define _CRYPTO_AKCIPHER_H

#include <linux/atomic.h>
#include <linux/crypto.h>


struct akcipher_request {
	struct crypto_async_request base;
	struct scatterlist *src;
	struct scatterlist *dst;
	unsigned int src_len;
	unsigned int dst_len;
	void *__ctx[] CRYPTO_MINALIGN_ATTR;
};


struct crypto_akcipher {
	unsigned int reqsize;

	struct crypto_tfm base;
};


struct crypto_istat_akcipher {
	atomic64_t encrypt_cnt;
	atomic64_t encrypt_tlen;
	atomic64_t decrypt_cnt;
	atomic64_t decrypt_tlen;
	atomic64_t verify_cnt;
	atomic64_t sign_cnt;
	atomic64_t err_cnt;
};


struct akcipher_alg {
	int (*sign)(struct akcipher_request *req);
	int (*verify)(struct akcipher_request *req);
	int (*encrypt)(struct akcipher_request *req);
	int (*decrypt)(struct akcipher_request *req);
	int (*set_pub_key)(struct crypto_akcipher *tfm, const void *key,
			   unsigned int keylen);
	int (*set_priv_key)(struct crypto_akcipher *tfm, const void *key,
			    unsigned int keylen);
	unsigned int (*max_size)(struct crypto_akcipher *tfm);
	int (*init)(struct crypto_akcipher *tfm);
	void (*exit)(struct crypto_akcipher *tfm);

#ifdef CONFIG_CRYPTO_STATS
	struct crypto_istat_akcipher stat;
#endif

	struct crypto_alg base;
};




struct crypto_akcipher *crypto_alloc_akcipher(const char *alg_name, u32 type,
					      u32 mask);

static inline struct crypto_tfm *crypto_akcipher_tfm(
	struct crypto_akcipher *tfm)
{
	return &tfm->base;
}

static inline struct akcipher_alg *__crypto_akcipher_alg(struct crypto_alg *alg)
{
	return container_of(alg, struct akcipher_alg, base);
}

static inline struct crypto_akcipher *__crypto_akcipher_tfm(
	struct crypto_tfm *tfm)
{
	return container_of(tfm, struct crypto_akcipher, base);
}

static inline struct akcipher_alg *crypto_akcipher_alg(
	struct crypto_akcipher *tfm)
{
	return __crypto_akcipher_alg(crypto_akcipher_tfm(tfm)->__crt_alg);
}

static inline unsigned int crypto_akcipher_reqsize(struct crypto_akcipher *tfm)
{
	return tfm->reqsize;
}

static inline void akcipher_request_set_tfm(struct akcipher_request *req,
					    struct crypto_akcipher *tfm)
{
	req->base.tfm = crypto_akcipher_tfm(tfm);
}

static inline struct crypto_akcipher *crypto_akcipher_reqtfm(
	struct akcipher_request *req)
{
	return __crypto_akcipher_tfm(req->base.tfm);
}


static inline void crypto_free_akcipher(struct crypto_akcipher *tfm)
{
	crypto_destroy_tfm(tfm, crypto_akcipher_tfm(tfm));
}


static inline struct akcipher_request *akcipher_request_alloc(
	struct crypto_akcipher *tfm, gfp_t gfp)
{
	struct akcipher_request *req;

	req = kmalloc(sizeof(*req) + crypto_akcipher_reqsize(tfm), gfp);
	if (likely(req))
		akcipher_request_set_tfm(req, tfm);

	return req;
}


static inline void akcipher_request_free(struct akcipher_request *req)
{
	kfree_sensitive(req);
}


static inline void akcipher_request_set_callback(struct akcipher_request *req,
						 u32 flgs,
						 crypto_completion_t cmpl,
						 void *data)
{
	req->base.complete = cmpl;
	req->base.data = data;
	req->base.flags = flgs;
}


static inline void akcipher_request_set_crypt(struct akcipher_request *req,
					      struct scatterlist *src,
					      struct scatterlist *dst,
					      unsigned int src_len,
					      unsigned int dst_len)
{
	req->src = src;
	req->dst = dst;
	req->src_len = src_len;
	req->dst_len = dst_len;
}


static inline unsigned int crypto_akcipher_maxsize(struct crypto_akcipher *tfm)
{
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);

	return alg->max_size(tfm);
}

static inline struct crypto_istat_akcipher *akcipher_get_stat(
	struct akcipher_alg *alg)
{
#ifdef CONFIG_CRYPTO_STATS
	return &alg->stat;
#else
	return NULL;
#endif
}

static inline int crypto_akcipher_errstat(struct akcipher_alg *alg, int err)
{
	if (!IS_ENABLED(CONFIG_CRYPTO_STATS))
		return err;

	if (err && err != -EINPROGRESS && err != -EBUSY)
		atomic64_inc(&akcipher_get_stat(alg)->err_cnt);

	return err;
}


static inline int crypto_akcipher_encrypt(struct akcipher_request *req)
{
	struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS)) {
		struct crypto_istat_akcipher *istat = akcipher_get_stat(alg);

		atomic64_inc(&istat->encrypt_cnt);
		atomic64_add(req->src_len, &istat->encrypt_tlen);
	}

	return crypto_akcipher_errstat(alg, alg->encrypt(req));
}


static inline int crypto_akcipher_decrypt(struct akcipher_request *req)
{
	struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS)) {
		struct crypto_istat_akcipher *istat = akcipher_get_stat(alg);

		atomic64_inc(&istat->decrypt_cnt);
		atomic64_add(req->src_len, &istat->decrypt_tlen);
	}

	return crypto_akcipher_errstat(alg, alg->decrypt(req));
}


int crypto_akcipher_sync_encrypt(struct crypto_akcipher *tfm,
				 const void *src, unsigned int slen,
				 void *dst, unsigned int dlen);


int crypto_akcipher_sync_decrypt(struct crypto_akcipher *tfm,
				 const void *src, unsigned int slen,
				 void *dst, unsigned int dlen);


static inline int crypto_akcipher_sign(struct akcipher_request *req)
{
	struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS))
		atomic64_inc(&akcipher_get_stat(alg)->sign_cnt);

	return crypto_akcipher_errstat(alg, alg->sign(req));
}


static inline int crypto_akcipher_verify(struct akcipher_request *req)
{
	struct crypto_akcipher *tfm = crypto_akcipher_reqtfm(req);
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS))
		atomic64_inc(&akcipher_get_stat(alg)->verify_cnt);

	return crypto_akcipher_errstat(alg, alg->verify(req));
}


static inline int crypto_akcipher_set_pub_key(struct crypto_akcipher *tfm,
					      const void *key,
					      unsigned int keylen)
{
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);

	return alg->set_pub_key(tfm, key, keylen);
}


static inline int crypto_akcipher_set_priv_key(struct crypto_akcipher *tfm,
					       const void *key,
					       unsigned int keylen)
{
	struct akcipher_alg *alg = crypto_akcipher_alg(tfm);

	return alg->set_priv_key(tfm, key, keylen);
}
#endif
