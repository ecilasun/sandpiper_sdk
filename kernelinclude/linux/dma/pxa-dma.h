/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _PXA_DMA_H_
#define _PXA_DMA_H_

enum pxad_chan_prio {
	PXAD_PRIO_HIGHEST = 0,
	PXAD_PRIO_NORMAL,
	PXAD_PRIO_LOW,
	PXAD_PRIO_LOWEST,
};


struct pxad_param {
	unsigned int drcmr;
	enum pxad_chan_prio prio;
};

#endif 
