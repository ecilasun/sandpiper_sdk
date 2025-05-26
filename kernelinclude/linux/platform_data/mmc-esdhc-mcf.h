/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_PLATFORM_DATA_MCF_ESDHC_H__
#define __LINUX_PLATFORM_DATA_MCF_ESDHC_H__

enum cd_types {
	ESDHC_CD_NONE,		
	ESDHC_CD_CONTROLLER,	
	ESDHC_CD_PERMANENT,	
};

struct mcf_esdhc_platform_data {
	int max_bus_width;
	int cd_type;
};

#endif 
