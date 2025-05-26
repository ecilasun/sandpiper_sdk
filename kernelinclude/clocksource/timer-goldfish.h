/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _CLOCKSOURCE_TIMER_GOLDFISH_H
#define _CLOCKSOURCE_TIMER_GOLDFISH_H


#define TIMER_TIME_LOW		0x00
#define TIMER_TIME_HIGH		0x04
#define TIMER_ALARM_LOW		0x08
#define TIMER_ALARM_HIGH	0x0c
#define TIMER_IRQ_ENABLED	0x10
#define TIMER_CLEAR_ALARM	0x14
#define TIMER_ALARM_STATUS	0x18
#define TIMER_CLEAR_INTERRUPT	0x1c

extern int goldfish_timer_init(int irq, void __iomem *base);

#endif 
