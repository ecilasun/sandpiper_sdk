// SPDX-License-Identifier: GPL-2.0

#ifndef _RSLIB_H_
#define _RSLIB_H_

#include <linux/list.h>
#include <linux/types.h>	
#include <linux/gfp.h>		


struct rs_codec {
	int		mm;
	int		nn;
	uint16_t	*alpha_to;
	uint16_t	*index_of;
	uint16_t	*genpoly;
	int		nroots;
	int		fcr;
	int		prim;
	int		iprim;
	int		gfpoly;
	int		(*gffunc)(int);
	int		users;
	struct list_head list;
};


struct rs_control {
	struct rs_codec	*codec;
	uint16_t	buffers[];
};


#ifdef CONFIG_REED_SOLOMON_ENC8
int encode_rs8(struct rs_control *rs, uint8_t *data, int len, uint16_t *par,
	       uint16_t invmsk);
#endif
#ifdef CONFIG_REED_SOLOMON_DEC8
int decode_rs8(struct rs_control *rs, uint8_t *data, uint16_t *par, int len,
		uint16_t *s, int no_eras, int *eras_pos, uint16_t invmsk,
	       uint16_t *corr);
#endif


#ifdef CONFIG_REED_SOLOMON_ENC16
int encode_rs16(struct rs_control *rs, uint16_t *data, int len, uint16_t *par,
		uint16_t invmsk);
#endif
#ifdef CONFIG_REED_SOLOMON_DEC16
int decode_rs16(struct rs_control *rs, uint16_t *data, uint16_t *par, int len,
		uint16_t *s, int no_eras, int *eras_pos, uint16_t invmsk,
		uint16_t *corr);
#endif

struct rs_control *init_rs_gfp(int symsize, int gfpoly, int fcr, int prim,
			       int nroots, gfp_t gfp);


static inline struct rs_control *init_rs(int symsize, int gfpoly, int fcr,
					 int prim, int nroots)
{
	return init_rs_gfp(symsize, gfpoly, fcr, prim, nroots, GFP_KERNEL);
}

struct rs_control *init_rs_non_canonical(int symsize, int (*func)(int),
					 int fcr, int prim, int nroots);


void free_rs(struct rs_control *rs);


static inline int rs_modnn(struct rs_codec *rs, int x)
{
	while (x >= rs->nn) {
		x -= rs->nn;
		x = (x >> rs->mm) + (x & rs->nn);
	}
	return x;
}

#endif
