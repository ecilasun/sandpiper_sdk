/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __INTEL_TCC_H__
#define __INTEL_TCC_H__

#include <linux/types.h>

int intel_tcc_get_tjmax(int cpu);
int intel_tcc_get_offset(int cpu);
int intel_tcc_set_offset(int cpu, int offset);
int intel_tcc_get_temp(int cpu, int *temp, bool pkg);

#endif 
