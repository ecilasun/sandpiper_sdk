/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_SPI_MMC_SPI_H
#define __LINUX_SPI_MMC_SPI_H

#include <linux/spi/spi.h>
#include <linux/interrupt.h>

struct device;
struct mmc_host;


struct mmc_spi_platform_data {
	
	int (*init)(struct device *,
		irqreturn_t (*)(int, void *),
		void *);
	void (*exit)(struct device *, void *);

	
	unsigned long caps;
	unsigned long caps2;

	
	u16 detect_delay;

	
	u16 powerup_msecs;		
	u32 ocr_mask;			
	void (*setpower)(struct device *, unsigned int maskval);
};

extern struct mmc_spi_platform_data *mmc_spi_get_pdata(struct spi_device *spi);
extern void mmc_spi_put_pdata(struct spi_device *spi);

#endif 
