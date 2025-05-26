


#ifndef _CRYPTO_B128OPS_H
#define _CRYPTO_B128OPS_H

#include <linux/types.h>

typedef struct {
	__be64 a, b;
} be128;

typedef struct {
	__le64 b, a;
} le128;

static inline void be128_xor(be128 *r, const be128 *p, const be128 *q)
{
	r->a = p->a ^ q->a;
	r->b = p->b ^ q->b;
}

static inline void le128_xor(le128 *r, const le128 *p, const le128 *q)
{
	r->a = p->a ^ q->a;
	r->b = p->b ^ q->b;
}

#endif 
