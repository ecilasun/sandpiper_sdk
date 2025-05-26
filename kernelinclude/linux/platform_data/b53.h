

#ifndef __B53_H
#define __B53_H

#include <linux/types.h>
#include <linux/platform_data/dsa.h>

struct b53_platform_data {
	
	struct dsa_chip_data cd;

	u32 chip_id;
	u16 enabled_ports;

	
	unsigned big_endian:1;
	void __iomem *regs;
};

#endif
