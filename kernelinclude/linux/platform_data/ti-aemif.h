/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __TI_DAVINCI_AEMIF_DATA_H__
#define __TI_DAVINCI_AEMIF_DATA_H__

#include <linux/of_platform.h>


struct aemif_abus_data {
	u32 cs;
};


struct aemif_platform_data {
	struct of_dev_auxdata *dev_lookup;
	u32 cs_offset;
	struct aemif_abus_data *abus_data;
	size_t num_abus_data;
	struct platform_device *sub_devices;
	size_t num_sub_devices;
};

#endif 
