/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _CRYPTO_INTERNAL_SIMD_H
#define _CRYPTO_INTERNAL_SIMD_H

#include <linux/percpu.h>
#include <linux/types.h>



struct simd_skcipher_alg;
struct skcipher_alg;

struct simd_skcipher_alg *simd_skcipher_create_compat(const char *algname,
						      const char *drvname,
						      const char *basename);
struct simd_skcipher_alg *simd_skcipher_create(const char *algname,
					       const char *basename);
void simd_skcipher_free(struct simd_skcipher_alg *alg);

int simd_register_skciphers_compat(struct skcipher_alg *algs, int count,
				   struct simd_skcipher_alg **simd_algs);

void simd_unregister_skciphers(struct skcipher_alg *algs, int count,
			       struct simd_skcipher_alg **simd_algs);



struct simd_aead_alg;
struct aead_alg;

struct simd_aead_alg *simd_aead_create_compat(const char *algname,
					      const char *drvname,
					      const char *basename);
struct simd_aead_alg *simd_aead_create(const char *algname,
				       const char *basename);
void simd_aead_free(struct simd_aead_alg *alg);

int simd_register_aeads_compat(struct aead_alg *algs, int count,
			       struct simd_aead_alg **simd_algs);

void simd_unregister_aeads(struct aead_alg *algs, int count,
			   struct simd_aead_alg **simd_algs);


#ifdef CONFIG_CRYPTO_MANAGER_EXTRA_TESTS
DECLARE_PER_CPU(bool, crypto_simd_disabled_for_test);
#define crypto_simd_usable() \
	(may_use_simd() && !this_cpu_read(crypto_simd_disabled_for_test))
#else
#define crypto_simd_usable() may_use_simd()
#endif

#endif 
