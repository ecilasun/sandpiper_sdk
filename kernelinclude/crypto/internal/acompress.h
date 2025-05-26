/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CRYPTO_ACOMP_INT_H
#define _CRYPTO_ACOMP_INT_H

#include <crypto/acompress.h>
#include <crypto/algapi.h>


struct acomp_alg {
	int (*compress)(struct acomp_req *req);
	int (*decompress)(struct acomp_req *req);
	void (*dst_free)(struct scatterlist *dst);
	int (*init)(struct crypto_acomp *tfm);
	void (*exit)(struct crypto_acomp *tfm);

	unsigned int reqsize;

	union {
		struct COMP_ALG_COMMON;
		struct comp_alg_common calg;
	};
};


static inline void *acomp_request_ctx(struct acomp_req *req)
{
	return req->__ctx;
}

static inline void *acomp_tfm_ctx(struct crypto_acomp *tfm)
{
	return tfm->base.__crt_ctx;
}

static inline void acomp_request_complete(struct acomp_req *req,
					  int err)
{
	crypto_request_complete(&req->base, err);
}

static inline struct acomp_req *__acomp_request_alloc(struct crypto_acomp *tfm)
{
	struct acomp_req *req;

	req = kzalloc(sizeof(*req) + crypto_acomp_reqsize(tfm), GFP_KERNEL);
	if (likely(req))
		acomp_request_set_tfm(req, tfm);
	return req;
}

static inline void __acomp_request_free(struct acomp_req *req)
{
	kfree_sensitive(req);
}


int crypto_register_acomp(struct acomp_alg *alg);


void crypto_unregister_acomp(struct acomp_alg *alg);

int crypto_register_acomps(struct acomp_alg *algs, int count);
void crypto_unregister_acomps(struct acomp_alg *algs, int count);

#endif
