/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */


#ifndef __DT_BINDINGS_RZG2L_PINCTRL_H
#define __DT_BINDINGS_RZG2L_PINCTRL_H

#define RZG2L_PINS_PER_PORT	8


#define RZG2L_PORT_PINMUX(b, p, f)	((b) * RZG2L_PINS_PER_PORT + (p) | ((f) << 16))


#define RZG2L_GPIO(port, pin)	((port) * RZG2L_PINS_PER_PORT + (pin))

#endif 
