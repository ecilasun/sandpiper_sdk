/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _CRYTO_ECC_CURVE_H
#define _CRYTO_ECC_CURVE_H

#include <linux/types.h>


struct ecc_point {
	u64 *x;
	u64 *y;
	u8 ndigits;
};


struct ecc_curve {
	char *name;
	u32 nbits;
	struct ecc_point g;
	u64 *p;
	u64 *n;
	u64 *a;
	u64 *b;
};


const struct ecc_curve *ecc_get_curve(unsigned int curve_id);


const struct ecc_curve *ecc_get_curve25519(void);

#endif
