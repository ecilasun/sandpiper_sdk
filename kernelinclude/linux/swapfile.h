/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SWAPFILE_H
#define _LINUX_SWAPFILE_H

extern unsigned long generic_max_swapfile_size(void);
unsigned long arch_max_swapfile_size(void);


extern unsigned long swapfile_maximum_size;

extern bool swap_migration_ad_supported;

#endif 
