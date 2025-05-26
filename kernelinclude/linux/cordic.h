
#ifndef __CORDIC_H_
#define __CORDIC_H_

#include <linux/types.h>

#define CORDIC_ANGLE_GEN	39797
#define CORDIC_PRECISION_SHIFT	16
#define CORDIC_NUM_ITER	(CORDIC_PRECISION_SHIFT + 2)

#define CORDIC_FIXED(X)	((s32)((X) << CORDIC_PRECISION_SHIFT))
#define CORDIC_FLOAT(X)	(((X) >= 0) \
		? ((((X) >> (CORDIC_PRECISION_SHIFT - 1)) + 1) >> 1) \
		: -((((-(X)) >> (CORDIC_PRECISION_SHIFT - 1)) + 1) >> 1))


struct cordic_iq {
	s32 i;
	s32 q;
};


struct cordic_iq cordic_calc_iq(s32 theta);

#endif 
