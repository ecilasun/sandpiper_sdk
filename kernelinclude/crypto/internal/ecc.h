
#ifndef _CRYPTO_ECC_H
#define _CRYPTO_ECC_H

#include <crypto/ecc_curve.h>
#include <asm/unaligned.h>


#define ECC_CURVE_NIST_P192_DIGITS  3
#define ECC_CURVE_NIST_P256_DIGITS  4
#define ECC_CURVE_NIST_P384_DIGITS  6
#define ECC_CURVE_NIST_P521_DIGITS  9
#define ECC_MAX_DIGITS              DIV_ROUND_UP(521, 64) 

#define ECC_DIGITS_TO_BYTES_SHIFT 3

#define ECC_MAX_BYTES (ECC_MAX_DIGITS << ECC_DIGITS_TO_BYTES_SHIFT)

#define ECC_POINT_INIT(x, y, ndigits)	(struct ecc_point) { x, y, ndigits }


static inline void ecc_swap_digits(const void *in, u64 *out, unsigned int ndigits)
{
	const __be64 *src = (__force __be64 *)in;
	int i;

	for (i = 0; i < ndigits; i++)
		out[i] = get_unaligned_be64(&src[ndigits - 1 - i]);
}


static inline void ecc_digits_from_bytes(const u8 *in, unsigned int nbytes,
					 u64 *out, unsigned int ndigits)
{
	unsigned int o = nbytes & 7;
	__be64 msd = 0;

	if (o) {
		memcpy((u8 *)&msd + sizeof(msd) - o, in, o);
		out[--ndigits] = be64_to_cpu(msd);
		in += o;
	}
	ecc_swap_digits(in, out, ndigits);
}


int ecc_is_key_valid(unsigned int curve_id, unsigned int ndigits,
		     const u64 *private_key, unsigned int private_key_len);


int ecc_gen_privkey(unsigned int curve_id, unsigned int ndigits, u64 *privkey);


int ecc_make_pub_key(const unsigned int curve_id, unsigned int ndigits,
		     const u64 *private_key, u64 *public_key);


int crypto_ecdh_shared_secret(unsigned int curve_id, unsigned int ndigits,
			      const u64 *private_key, const u64 *public_key,
			      u64 *secret);


int ecc_is_pubkey_valid_partial(const struct ecc_curve *curve,
				struct ecc_point *pk);


int ecc_is_pubkey_valid_full(const struct ecc_curve *curve,
			     struct ecc_point *pk);


bool vli_is_zero(const u64 *vli, unsigned int ndigits);


int vli_cmp(const u64 *left, const u64 *right, unsigned int ndigits);


u64 vli_sub(u64 *result, const u64 *left, const u64 *right,
	    unsigned int ndigits);


void vli_from_be64(u64 *dest, const void *src, unsigned int ndigits);


void vli_from_le64(u64 *dest, const void *src, unsigned int ndigits);


void vli_mod_inv(u64 *result, const u64 *input, const u64 *mod,
		 unsigned int ndigits);


void vli_mod_mult_slow(u64 *result, const u64 *left, const u64 *right,
		       const u64 *mod, unsigned int ndigits);


unsigned int vli_num_bits(const u64 *vli, unsigned int ndigits);


struct ecc_point *ecc_alloc_point(unsigned int ndigits);


void ecc_free_point(struct ecc_point *p);


bool ecc_point_is_zero(const struct ecc_point *point);


void ecc_point_mult_shamir(const struct ecc_point *result,
			   const u64 *x, const struct ecc_point *p,
			   const u64 *y, const struct ecc_point *q,
			   const struct ecc_curve *curve);

#endif
