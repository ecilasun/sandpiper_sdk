/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _CRYPTO_RNG_H
#define _CRYPTO_RNG_H

#include <linux/atomic.h>
#include <linux/container_of.h>
#include <linux/crypto.h>

struct crypto_rng;


struct crypto_istat_rng {
	atomic64_t generate_cnt;
	atomic64_t generate_tlen;
	atomic64_t seed_cnt;
	atomic64_t err_cnt;
};


struct rng_alg {
	int (*generate)(struct crypto_rng *tfm,
			const u8 *src, unsigned int slen,
			u8 *dst, unsigned int dlen);
	int (*seed)(struct crypto_rng *tfm, const u8 *seed, unsigned int slen);
	void (*set_ent)(struct crypto_rng *tfm, const u8 *data,
			unsigned int len);

#ifdef CONFIG_CRYPTO_STATS
	struct crypto_istat_rng stat;
#endif

	unsigned int seedsize;

	struct crypto_alg base;
};

struct crypto_rng {
	struct crypto_tfm base;
};

extern struct crypto_rng *crypto_default_rng;

int crypto_get_default_rng(void);
void crypto_put_default_rng(void);




struct crypto_rng *crypto_alloc_rng(const char *alg_name, u32 type, u32 mask);

static inline struct crypto_tfm *crypto_rng_tfm(struct crypto_rng *tfm)
{
	return &tfm->base;
}

static inline struct rng_alg *__crypto_rng_alg(struct crypto_alg *alg)
{
	return container_of(alg, struct rng_alg, base);
}


static inline struct rng_alg *crypto_rng_alg(struct crypto_rng *tfm)
{
	return __crypto_rng_alg(crypto_rng_tfm(tfm)->__crt_alg);
}


static inline void crypto_free_rng(struct crypto_rng *tfm)
{
	crypto_destroy_tfm(tfm, crypto_rng_tfm(tfm));
}

static inline struct crypto_istat_rng *rng_get_stat(struct rng_alg *alg)
{
#ifdef CONFIG_CRYPTO_STATS
	return &alg->stat;
#else
	return NULL;
#endif
}

static inline int crypto_rng_errstat(struct rng_alg *alg, int err)
{
	if (!IS_ENABLED(CONFIG_CRYPTO_STATS))
		return err;

	if (err && err != -EINPROGRESS && err != -EBUSY)
		atomic64_inc(&rng_get_stat(alg)->err_cnt);

	return err;
}


static inline int crypto_rng_generate(struct crypto_rng *tfm,
				      const u8 *src, unsigned int slen,
				      u8 *dst, unsigned int dlen)
{
	struct rng_alg *alg = crypto_rng_alg(tfm);

	if (IS_ENABLED(CONFIG_CRYPTO_STATS)) {
		struct crypto_istat_rng *istat = rng_get_stat(alg);

		atomic64_inc(&istat->generate_cnt);
		atomic64_add(dlen, &istat->generate_tlen);
	}

	return crypto_rng_errstat(alg,
				  alg->generate(tfm, src, slen, dst, dlen));
}


static inline int crypto_rng_get_bytes(struct crypto_rng *tfm,
				       u8 *rdata, unsigned int dlen)
{
	return crypto_rng_generate(tfm, NULL, 0, rdata, dlen);
}


int crypto_rng_reset(struct crypto_rng *tfm, const u8 *seed,
		     unsigned int slen);


static inline int crypto_rng_seedsize(struct crypto_rng *tfm)
{
	return crypto_rng_alg(tfm)->seedsize;
}

#endif
