/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _PINCTRL_SINGLE_H
#define _PINCTRL_SINGLE_H


struct pcs_pdata {
	int irq;
	void (*rearm)(void);
};

#endif 
