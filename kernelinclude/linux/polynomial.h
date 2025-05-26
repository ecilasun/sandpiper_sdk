/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _POLYNOMIAL_H
#define _POLYNOMIAL_H


struct polynomial_term {
	unsigned int deg;
	long coef;
	long divider;
	long divider_leftover;
};


struct polynomial {
	long total_divider;
	struct polynomial_term terms[];
};

long polynomial_calc(const struct polynomial *poly, long data);

#endif
