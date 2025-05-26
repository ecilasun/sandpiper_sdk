/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __DT_BINDINGS_PINCTRL_RENESAS_RZA2_H
#define __DT_BINDINGS_PINCTRL_RENESAS_RZA2_H

#define RZA2_PINS_PER_PORT	8


#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3
#define PORT4 4
#define PORT5 5
#define PORT6 6
#define PORT7 7
#define PORT8 8
#define PORT9 9
#define PORTA 10
#define PORTB 11
#define PORTC 12
#define PORTD 13
#define PORTE 14
#define PORTF 15
#define PORTG 16
#define PORTH 17

#define PORTJ 18
#define PORTK 19
#define PORTL 20
#define PORTM 21	


#define RZA2_PINMUX(b, p, f)	((b) * RZA2_PINS_PER_PORT + (p) | (f << 16))


#define RZA2_PIN(port, pin)	((port) * RZA2_PINS_PER_PORT + (pin))

#endif 
