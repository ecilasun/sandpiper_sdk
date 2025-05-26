/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */


#ifndef __DT_BINDINGS_PINCTRL_SPPCTL_H__
#define __DT_BINDINGS_PINCTRL_SPPCTL_H__

#define IOP_G_MASTE		(0x01 << 0)
#define IOP_G_FIRST		(0x01 << 1)

#define SPPCTL_PCTL_G_PMUX	(0x00        | IOP_G_MASTE)
#define SPPCTL_PCTL_G_GPIO	(IOP_G_FIRST | IOP_G_MASTE)
#define SPPCTL_PCTL_G_IOPP	(IOP_G_FIRST | 0x00)

#define SPPCTL_PCTL_L_OUT	(0x01 << 0)	
#define SPPCTL_PCTL_L_OU1	(0x01 << 1)	
#define SPPCTL_PCTL_L_INV	(0x01 << 2)	
#define SPPCTL_PCTL_L_ONV	(0x01 << 3)	
#define SPPCTL_PCTL_L_ODR	(0x01 << 4)	


#define SPPCTL_IOPAD(pin, typ, fun, flg)	(((pin) << 24) | ((typ) << 16) | \
						((fun) << 8) | (flg))

#endif
