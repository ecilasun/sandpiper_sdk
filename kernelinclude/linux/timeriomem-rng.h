/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_TIMERIOMEM_RNG_H
#define _LINUX_TIMERIOMEM_RNG_H

struct timeriomem_rng_data {
	void __iomem		*address;

	
	unsigned int		period;

	
	unsigned int		quality;
};

#endif 
