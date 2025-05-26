/* SPDX-License-Identifier: (GPL-2.0 OR MIT) */


#ifndef _HIRSCHMANN_HELLCREEK_H_
#define _HIRSCHMANN_HELLCREEK_H_

#include <linux/types.h>

struct hellcreek_platform_data {
	const char *name;	
	int num_ports;		
	int is_100_mbits;	
	int qbv_support;	
	int qbv_on_cpu_port;	
	int qbu_support;	
	u16 module_id;		
};

#endif 
