/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_MTD_PLATNAND_H
#define __LINUX_MTD_PLATNAND_H

#include <linux/mtd/partitions.h>
#include <linux/mtd/rawnand.h>
#include <linux/platform_device.h>


struct platform_nand_chip {
	int nr_chips;
	int chip_offset;
	int nr_partitions;
	struct mtd_partition *partitions;
	int chip_delay;
	unsigned int options;
	unsigned int bbt_options;
	const char **part_probe_types;
};


struct platform_nand_ctrl {
	int (*probe)(struct platform_device *pdev);
	void (*remove)(struct platform_device *pdev);
	int (*dev_ready)(struct nand_chip *chip);
	void (*select_chip)(struct nand_chip *chip, int cs);
	void (*cmd_ctrl)(struct nand_chip *chip, int dat, unsigned int ctrl);
	void (*write_buf)(struct nand_chip *chip, const uint8_t *buf, int len);
	void (*read_buf)(struct nand_chip *chip, uint8_t *buf, int len);
	void *priv;
};


struct platform_nand_data {
	struct platform_nand_chip chip;
	struct platform_nand_ctrl ctrl;
};

#endif 
