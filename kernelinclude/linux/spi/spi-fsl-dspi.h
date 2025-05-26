/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef SPI_FSL_DSPI_HEADER_H
#define SPI_FSL_DSPI_HEADER_H


struct fsl_dspi_platform_data {
	u32 cs_num;
	u32 bus_num;
	u32 sck_cs_delay;
	u32 cs_sck_delay;
};

#endif 
