/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_CRYPTO_H
#define _LINUX_CRYPTO_H

#include <linux/completion.h>
#include <linux/refcount.h>
#include <linux/slab.h>
#include <linux/types.h>


#define CRYPTO_ALG_TYPE_MASK		0x0000000f
#define CRYPTO_ALG_TYPE_CIPHER		0x00000001
#define CRYPTO_ALG_TYPE_COMPRESS	0x00000002
#define CRYPTO_ALG_TYPE_AEAD		0x00000003
#define CRYPTO_ALG_TYPE_SKCIPHER	0x00000005
#define CRYPTO_ALG_TYPE_AKCIPHER	0x00000006
#define CRYPTO_ALG_TYPE_SIG		0x00000007
#define CRYPTO_ALG_TYPE_KPP		0x00000008
#define CRYPTO_ALG_TYPE_ACOMPRESS	0x0000000a
#define CRYPTO_ALG_TYPE_SCOMPRESS	0x0000000b
#define CRYPTO_ALG_TYPE_RNG		0x0000000c
#define CRYPTO_ALG_TYPE_HASH		0x0000000e
#define CRYPTO_ALG_TYPE_SHASH		0x0000000e
#define CRYPTO_ALG_TYPE_AHASH		0x0000000f

#define CRYPTO_ALG_TYPE_HASH_MASK	0x0000000e
#define CRYPTO_ALG_TYPE_AHASH_MASK	0x0000000e
#define CRYPTO_ALG_TYPE_ACOMPRESS_MASK	0x0000000e

#define CRYPTO_ALG_LARVAL		0x00000010
#define CRYPTO_ALG_DEAD			0x00000020
#define CRYPTO_ALG_DYING		0x00000040
#define CRYPTO_ALG_ASYNC		0x00000080


#define CRYPTO_ALG_NEED_FALLBACK	0x00000100



#define CRYPTO_ALG_TESTED		0x00000400


#define CRYPTO_ALG_INSTANCE		0x00000800


#define CRYPTO_ALG_KERN_DRIVER_ONLY	0x00001000


#define CRYPTO_ALG_INTERNAL		0x00002000


#define CRYPTO_ALG_OPTIONAL_KEY		0x00004000


#define CRYPTO_NOLOAD			0x00008000


#define CRYPTO_ALG_ALLOCATES_MEMORY	0x00010000


#define CRYPTO_ALG_FIPS_INTERNAL	0x00020000


#define CRYPTO_TFM_NEED_KEY		0x00000001

#define CRYPTO_TFM_REQ_MASK		0x000fff00
#define CRYPTO_TFM_REQ_FORBID_WEAK_KEYS	0x00000100
#define CRYPTO_TFM_REQ_MAY_SLEEP	0x00000200
#define CRYPTO_TFM_REQ_MAY_BACKLOG	0x00000400


#define CRYPTO_MAX_ALG_NAME		128


#define CRYPTO_MINALIGN ARCH_KMALLOC_MINALIGN

#define CRYPTO_MINALIGN_ATTR __attribute__ ((__aligned__(CRYPTO_MINALIGN)))

struct crypto_tfm;
struct crypto_type;
struct module;

typedef void (*crypto_completion_t)(void *req, int err);



struct crypto_async_request {
	struct list_head list;
	crypto_completion_t complete;
	void *data;
	struct crypto_tfm *tfm;

	u32 flags;
};




struct cipher_alg {
	unsigned int cia_min_keysize;
	unsigned int cia_max_keysize;
	int (*cia_setkey)(struct crypto_tfm *tfm, const u8 *key,
	                  unsigned int keylen);
	void (*cia_encrypt)(struct crypto_tfm *tfm, u8 *dst, const u8 *src);
	void (*cia_decrypt)(struct crypto_tfm *tfm, u8 *dst, const u8 *src);
};


struct compress_alg {
	int (*coa_compress)(struct crypto_tfm *tfm, const u8 *src,
			    unsigned int slen, u8 *dst, unsigned int *dlen);
	int (*coa_decompress)(struct crypto_tfm *tfm, const u8 *src,
			      unsigned int slen, u8 *dst, unsigned int *dlen);
};

#define cra_cipher	cra_u.cipher
#define cra_compress	cra_u.compress


struct crypto_alg {
	struct list_head cra_list;
	struct list_head cra_users;

	u32 cra_flags;
	unsigned int cra_blocksize;
	unsigned int cra_ctxsize;
	unsigned int cra_alignmask;

	int cra_priority;
	refcount_t cra_refcnt;

	char cra_name[CRYPTO_MAX_ALG_NAME];
	char cra_driver_name[CRYPTO_MAX_ALG_NAME];

	const struct crypto_type *cra_type;

	union {
		struct cipher_alg cipher;
		struct compress_alg compress;
	} cra_u;

	int (*cra_init)(struct crypto_tfm *tfm);
	void (*cra_exit)(struct crypto_tfm *tfm);
	void (*cra_destroy)(struct crypto_alg *alg);
	
	struct module *cra_module;
} CRYPTO_MINALIGN_ATTR;


struct crypto_wait {
	struct completion completion;
	int err;
};


#define DECLARE_CRYPTO_WAIT(_wait) \
	struct crypto_wait _wait = { \
		COMPLETION_INITIALIZER_ONSTACK((_wait).completion), 0 }


void crypto_req_done(void *req, int err);

static inline int crypto_wait_req(int err, struct crypto_wait *wait)
{
	switch (err) {
	case -EINPROGRESS:
	case -EBUSY:
		wait_for_completion(&wait->completion);
		reinit_completion(&wait->completion);
		err = wait->err;
		break;
	}

	return err;
}

static inline void crypto_init_wait(struct crypto_wait *wait)
{
	init_completion(&wait->completion);
}


int crypto_has_alg(const char *name, u32 type, u32 mask);



struct crypto_tfm {
	refcount_t refcnt;

	u32 crt_flags;

	int node;
	
	void (*exit)(struct crypto_tfm *tfm);
	
	struct crypto_alg *__crt_alg;

	void *__crt_ctx[] CRYPTO_MINALIGN_ATTR;
};

struct crypto_comp {
	struct crypto_tfm base;
};


 
struct crypto_tfm *crypto_alloc_base(const char *alg_name, u32 type, u32 mask);
void crypto_destroy_tfm(void *mem, struct crypto_tfm *tfm);

static inline void crypto_free_tfm(struct crypto_tfm *tfm)
{
	return crypto_destroy_tfm(tfm, tfm);
}


static inline const char *crypto_tfm_alg_name(struct crypto_tfm *tfm)
{
	return tfm->__crt_alg->cra_name;
}

static inline const char *crypto_tfm_alg_driver_name(struct crypto_tfm *tfm)
{
	return tfm->__crt_alg->cra_driver_name;
}

static inline unsigned int crypto_tfm_alg_blocksize(struct crypto_tfm *tfm)
{
	return tfm->__crt_alg->cra_blocksize;
}

static inline unsigned int crypto_tfm_alg_alignmask(struct crypto_tfm *tfm)
{
	return tfm->__crt_alg->cra_alignmask;
}

static inline u32 crypto_tfm_get_flags(struct crypto_tfm *tfm)
{
	return tfm->crt_flags;
}

static inline void crypto_tfm_set_flags(struct crypto_tfm *tfm, u32 flags)
{
	tfm->crt_flags |= flags;
}

static inline void crypto_tfm_clear_flags(struct crypto_tfm *tfm, u32 flags)
{
	tfm->crt_flags &= ~flags;
}

static inline unsigned int crypto_tfm_ctx_alignment(void)
{
	struct crypto_tfm *tfm;
	return __alignof__(tfm->__crt_ctx);
}

static inline struct crypto_comp *__crypto_comp_cast(struct crypto_tfm *tfm)
{
	return (struct crypto_comp *)tfm;
}

static inline struct crypto_comp *crypto_alloc_comp(const char *alg_name,
						    u32 type, u32 mask)
{
	type &= ~CRYPTO_ALG_TYPE_MASK;
	type |= CRYPTO_ALG_TYPE_COMPRESS;
	mask |= CRYPTO_ALG_TYPE_MASK;

	return __crypto_comp_cast(crypto_alloc_base(alg_name, type, mask));
}

static inline struct crypto_tfm *crypto_comp_tfm(struct crypto_comp *tfm)
{
	return &tfm->base;
}

static inline void crypto_free_comp(struct crypto_comp *tfm)
{
	crypto_free_tfm(crypto_comp_tfm(tfm));
}

static inline int crypto_has_comp(const char *alg_name, u32 type, u32 mask)
{
	type &= ~CRYPTO_ALG_TYPE_MASK;
	type |= CRYPTO_ALG_TYPE_COMPRESS;
	mask |= CRYPTO_ALG_TYPE_MASK;

	return crypto_has_alg(alg_name, type, mask);
}

static inline const char *crypto_comp_name(struct crypto_comp *tfm)
{
	return crypto_tfm_alg_name(crypto_comp_tfm(tfm));
}

int crypto_comp_compress(struct crypto_comp *tfm,
			 const u8 *src, unsigned int slen,
			 u8 *dst, unsigned int *dlen);

int crypto_comp_decompress(struct crypto_comp *tfm,
			   const u8 *src, unsigned int slen,
			   u8 *dst, unsigned int *dlen);

#endif	

