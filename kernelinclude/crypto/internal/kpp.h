/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CRYPTO_KPP_INT_H
#define _CRYPTO_KPP_INT_H
#include <crypto/kpp.h>
#include <crypto/algapi.h>


struct kpp_instance {
	void (*free)(struct kpp_instance *inst);
	union {
		struct {
			char head[offsetof(struct kpp_alg, base)];
			struct crypto_instance base;
		} s;
		struct kpp_alg alg;
	};
};


struct crypto_kpp_spawn {
	struct crypto_spawn base;
};


static inline void *kpp_request_ctx(struct kpp_request *req)
{
	return req->__ctx;
}

static inline void *kpp_request_ctx_dma(struct kpp_request *req)
{
	unsigned int align = crypto_dma_align();

	if (align <= crypto_tfm_ctx_alignment())
		align = 1;

	return PTR_ALIGN(kpp_request_ctx(req), align);
}

static inline void kpp_set_reqsize(struct crypto_kpp *kpp,
				   unsigned int reqsize)
{
	kpp->reqsize = reqsize;
}

static inline void kpp_set_reqsize_dma(struct crypto_kpp *kpp,
				       unsigned int reqsize)
{
	reqsize += crypto_dma_align() & ~(crypto_tfm_ctx_alignment() - 1);
	kpp->reqsize = reqsize;
}

static inline void *kpp_tfm_ctx(struct crypto_kpp *tfm)
{
	return crypto_tfm_ctx(&tfm->base);
}

static inline void *kpp_tfm_ctx_dma(struct crypto_kpp *tfm)
{
	return crypto_tfm_ctx_dma(&tfm->base);
}

static inline void kpp_request_complete(struct kpp_request *req, int err)
{
	crypto_request_complete(&req->base, err);
}

static inline const char *kpp_alg_name(struct crypto_kpp *tfm)
{
	return crypto_kpp_tfm(tfm)->__crt_alg->cra_name;
}



static inline struct crypto_instance *kpp_crypto_instance(
	struct kpp_instance *inst)
{
	return &inst->s.base;
}


static inline struct kpp_instance *kpp_instance(struct crypto_instance *inst)
{
	return container_of(inst, struct kpp_instance, s.base);
}


static inline struct kpp_instance *kpp_alg_instance(struct crypto_kpp *kpp)
{
	return kpp_instance(crypto_tfm_alg_instance(&kpp->base));
}


static inline void *kpp_instance_ctx(struct kpp_instance *inst)
{
	return crypto_instance_ctx(kpp_crypto_instance(inst));
}



int crypto_register_kpp(struct kpp_alg *alg);


void crypto_unregister_kpp(struct kpp_alg *alg);


int kpp_register_instance(struct crypto_template *tmpl,
			  struct kpp_instance *inst);



int crypto_grab_kpp(struct crypto_kpp_spawn *spawn,
		    struct crypto_instance *inst,
		    const char *name, u32 type, u32 mask);


static inline void crypto_drop_kpp(struct crypto_kpp_spawn *spawn)
{
	crypto_drop_spawn(&spawn->base);
}


static inline struct kpp_alg *crypto_spawn_kpp_alg(
	struct crypto_kpp_spawn *spawn)
{
	return container_of(spawn->base.alg, struct kpp_alg, base);
}


static inline struct crypto_kpp *crypto_spawn_kpp(
	struct crypto_kpp_spawn *spawn)
{
	return crypto_spawn_tfm2(&spawn->base);
}

#endif
