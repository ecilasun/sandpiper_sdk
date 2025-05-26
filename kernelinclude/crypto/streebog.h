/* SPDX-License-Identifier: GPL-2.0+ OR BSD-2-Clause */


#ifndef _CRYPTO_STREEBOG_H_
#define _CRYPTO_STREEBOG_H_

#include <linux/types.h>

#define STREEBOG256_DIGEST_SIZE	32
#define STREEBOG512_DIGEST_SIZE	64
#define STREEBOG_BLOCK_SIZE	64

struct streebog_uint512 {
	__le64 qword[8];
};

struct streebog_state {
	union {
		u8 buffer[STREEBOG_BLOCK_SIZE];
		struct streebog_uint512 m;
	};
	struct streebog_uint512 hash;
	struct streebog_uint512 h;
	struct streebog_uint512 N;
	struct streebog_uint512 Sigma;
	size_t fillsize;
};

#endif 
