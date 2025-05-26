/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_ADXL_H
#define _LINUX_ADXL_H

const char * const *adxl_get_component_names(void);
int adxl_decode(u64 addr, u64 component_values[]);

#endif 
