/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause) */


#ifndef __DT_BINDINGS_RZV2M_PINCTRL_H
#define __DT_BINDINGS_RZV2M_PINCTRL_H

#define RZV2M_PINS_PER_PORT	16


#define RZV2M_PORT_PINMUX(b, p, f)	((b) * RZV2M_PINS_PER_PORT + (p) | ((f) << 16))


#define RZV2M_GPIO(port, pin)	((port) * RZV2M_PINS_PER_PORT + (pin))

#endif 
