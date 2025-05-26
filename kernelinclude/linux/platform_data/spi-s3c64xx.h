/* SPDX-License-Identifier: GPL-2.0 */



#ifndef __SPI_S3C64XX_H
#define __SPI_S3C64XX_H

#include <linux/dmaengine.h>

struct platform_device;


struct s3c64xx_spi_csinfo {
	u8 fb_delay;
};


struct s3c64xx_spi_info {
	int src_clk_nr;
	int num_cs;
	bool no_cs;
	bool polling;
	int (*cfg_gpio)(void);
};


extern void s3c64xx_spi0_set_platdata(int src_clk_nr, int num_cs);


extern int s3c64xx_spi0_cfg_gpio(void);

extern struct s3c64xx_spi_info s3c64xx_spi0_pdata;
#endif 
