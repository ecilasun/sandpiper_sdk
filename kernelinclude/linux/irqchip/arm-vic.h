/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_ARM_HARDWARE_VIC_H
#define __ASM_ARM_HARDWARE_VIC_H

#include <linux/types.h>

void vic_init(void __iomem *base, unsigned int irq_start, u32 vic_sources, u32 resume_sources);

#endif
