/* SPDX-License-Identifier: GPL-2.0-only */


#define CACHE_TAUROS2_PREFETCH_ON	(1 << 0)
#define CACHE_TAUROS2_LINEFILL_BURST8	(1 << 1)

extern void __init tauros2_init(unsigned int features);
