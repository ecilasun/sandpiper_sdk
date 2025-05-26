/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef SPI_INTEL_PDATA_H
#define SPI_INTEL_PDATA_H

enum intel_spi_type {
	INTEL_SPI_BYT = 1,
	INTEL_SPI_LPT,
	INTEL_SPI_BXT,
	INTEL_SPI_CNL,
};


struct intel_spi_boardinfo {
	enum intel_spi_type type;
	bool (*set_writeable)(void __iomem *base, void *data);
	void *data;
};

#endif 
