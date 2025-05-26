/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_ARCH_PXA3XX_NAND_H
#define __ASM_ARCH_PXA3XX_NAND_H

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>


struct pxa3xx_nand_platform_data {
	
	bool keep_config;
	
	bool flash_bbt;
	
	int ecc_strength, ecc_step_size;
	
	const struct mtd_partition *parts;
	unsigned int nr_parts;
};

extern void pxa3xx_set_nand_info(struct pxa3xx_nand_platform_data *info);

#endif 
