/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CRYPTO_ACOMP_H
#define _CRYPTO_ACOMP_H

#include <linux/atomic.h>
#include <linux/container_of.h>
#include <linux/crypto.h>

#define CRYPTO_ACOMP_ALLOC_OUTPUT	0x00000001
#define CRYPTO_ACOMP_DST_MAX		131072


struct acomp_req {
	struct crypto_async_request base;
	struct scatterlist *src;
	struct scatterlist *dst;
	unsigned int slen;
	unsigned int dlen;
	u32 flags;
	void *__ctx[] CRYPTO_MINALIGN_ATTR;
};


struct crypto_acomp {
	int (*compress)(struct acomp_req *req);
	int (*decompress)(struct acomp_req *req);
	void (*dst_free)(struct scatterlist *dst);
	unsigned int reqsize;
	struct crypto_tfm base;
};


struct crypto_istat_compress {
	atomic64_t compress_cnt;
	atomic64_t compress_tlen;
	atomic64_t decompress_cnt;
	atomic64_t decompress_tlen;
	atomic64_t err_cnt;
};

#ifdef CONFIG_CRYPTO_STATS
#define COMP_ALG_COMMON_STATS struct crypto_istat_compress stat;
#else
#define COMP_ALG_COMMON_STATS
#endif

#define COMP_ALG_COMMON {			\
	COMP_ALG_COMMON_STATS			\
						\
	struct crypto_alg base;			\
}
struct comp_alg_common COMP_ALG_COMMON;




struct crypto_acomp *crypto_alloc_acomp(const char *alg_name, u32 type,
					u32 mask);

struct crypto_acomp *crypto_alloc_acomp_node(const char *alg_name, u32 type,
					u32 mask, int node);

static inline struct crypto_tfm *crypto_acomp_tfm(struct crypto_acomp *tfm)
{
	return &tfm->base;
}

static inline struct comp_alg_common *__crypto_comp_alg_common(
	struct crypto_alg *alg)
{
	return container_of(alg, struct comp_alg_common, base);
}

static inline struct crypto_acomp *__crypto_acomp_tfm(struct crypto_tfm *tfm)
{
	return container_of(tfm, struct crypto_acomp, base);
}

static inline struct comp_alg_common *crypto_comp_alg_common(
	struct crypto_acomp *tfm)
{
	return __crypto_comp_alg_common(crypto_acomp_tfm(tfm)->__crt_alg);
}

static inline unsigned int crypto_acomp_reqsize(struct crypto_acomp *tfm)
{
	return tfm->reqsize;
}

static inline void acomp_request_set_tfm(struct acomp_req *req,
					 struct crypto_acomp *tfm)
{
	req->base.tfm = crypto_acomp_tfm(tfm);
}

static inline struct crypto_acomp *crypto_acomp_reqtfm(struct acomp_req *req)
{
	return __crypto_acomp_tfm(req->base.tfm);
}


static inline void crypto_free_acomp(struct crypto_acomp *tfm)
{
	crypto_destroy_tfm(tfm, crypto_acomp_tfm(tfm));
}

static inline int crypto_has_acomp(const char *alg_name, u32 type, u32 mask)
{
	type &= ~CRYPTO_ALG_TYPE_MASK;
	type |= CRYPTO_ALG_TYPE_ACOMPRESS;
	mask |= CRYPTO_ALG_TYPE_ACOMPRESS_MASK;

	return crypto_has_alg(alg_name, type, mask);
}


struct acomp_req *acomp_request_alloc(struct crypto_acomp *tfm);


void acomp_request_free(struct acomp_req *req);


static inline void acomp_request_set_callback(struct acomp_req *req,
					      u32 flgs,
					      crypto_completion_t cmpl,
					      void *data)
{
	req->base.complete = cmpl;
	req->base.data = data;
	req->base.flags &= CRYPTO_ACOMP_ALLOC_OUTPUT;
	req->base.flags |= flgs & ~CRYPTO_ACOMP_ALLOC_OUTPUT;
}


static inline void acomp_request_set_params(struct acomp_req *req,
					    struct scatterlist *src,
					    struct scatterlist *dst,
					    unsigned int slen,
					    unsigned int dlen)
{
	req->src = src;
	req->dst = dst;
	req->slen = slen;
	req->dlen = dlen;

	req->flags &= ~CRYPTO_ACOMP_ALLOC_OUTPUT;
	if (!req->dst)
		req->flags |= CRYPTO_ACOMP_ALLOC_OUTPUT;
}

static inline struct crypto_istat_compress *comp_get_stat(
	struct comp_alg_common *alg)
{
#ifdef CONFIG_CRYPTO_STATS
	return &alg->stat;
#else
	return NULL;
#endif
}

static inline int crypto_comp_errstat(struct comp_alg_common *alg, int err)
{
	if (!IS_ENABLED(CONFIG_CRYPTO_STATS))
		return err;

	if (err && err != -EINPROGRESS && err != -EBUSY)
		atomic64_inc(&comp_get_stat(alg)->err_cnt);

	return err;
}


static inline int crypto_acomp_compress(struct acomp_req *req)
{
	struct crypto_acomp *tfm = crypto_acomp_reqtfm(req);
	struct comp_alg_common *alg;

	alg = crypto_comp_alg_common(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS)) {
		struct crypto_istat_compress *istat = comp_get_stat(alg);

		atomic64_inc(&istat->compress_cnt);
		atomic64_add(req->slen, &istat->compress_tlen);
	}

	return crypto_comp_errstat(alg, tfm->compress(req));
}


static inline int crypto_acomp_decompress(struct acomp_req *req)
{
	struct crypto_acomp *tfm = crypto_acomp_reqtfm(req);
	struct comp_alg_common *alg;

	alg = crypto_comp_alg_common(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS)) {
		struct crypto_istat_compress *istat = comp_get_stat(alg);

		atomic64_inc(&istat->decompress_cnt);
		atomic64_add(req->slen, &istat->decompress_tlen);
	}

	return crypto_comp_errstat(alg, tfm->decompress(req));
}

#endif
